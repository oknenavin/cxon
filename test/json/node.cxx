// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/json.hxx"
#include "cxon/lang/json/node.hxx"

#include "cxon/lib/std/list.hxx"

#include "../pretty.hxx"
#include "node.pretty.hxx"

#include "node.ordered.hxx"

#include <fstream>
#include <memory>
#include <chrono>
#include <cstring>
#include <cstdio>
#include <cassert>

///////////////////////////////////////////////////////////////////////////////

using node = cxon::json::ordered_node;

struct test_time {
    double base = 0;
    double read = 0;
    double write = 0;
    double pretty_string = 0;
    double pretty = 0;
};

struct test_case {
    std::string source;
    test_time time;
    std::string error;
};

template <typename B>
    static double measure(unsigned c, B block) {
        CXON_ASSERT(c > 0, "unexpected");
        using clock = std::chrono::steady_clock;
        using std::chrono::duration_cast;
        using std::chrono::nanoseconds;
        double t = std::numeric_limits<double>::max();
            do {
                auto const s = clock::now();
                block();
                t = std::min(t, duration_cast<nanoseconds>(clock::now() - s).count() / 1e6);
            }   while (--c);
        return t;
    }

template <typename R, typename I>
    static std::string format_error(const R& r, I b) {
        return std::string()
            .append(r.ec.category().name())
            .append(":")
            .append(std::to_string(std::distance(b, r.end)))
            .append(": ")
            .append(r.ec.message())
        ;
    }

constexpr unsigned cxjson_repeat = 3;

static void cxjson_test_time(test_case& test) {
    std::ifstream is(test.source, std::ifstream::binary);
        if (!is) return test.error = "cannot be opened", void();
    std::string const json = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
    {   // base
        std::vector<std::unique_ptr<char[]>> vs;
        test.time.base = measure(cxjson_repeat, [&] { // something like strdup
            vs.emplace_back(std::unique_ptr<char[]>(new char[strlen(json.c_str()) + 1]));
            std::memcpy(vs.back().get(), json.c_str(), json.size());
        });
    }
    {   // cxon
        std::vector<node> vj;
        test.time.read = measure(cxjson_repeat, [&] {
            vj.emplace_back();
            auto const r = cxon::from_bytes(vj.back(), json);
            if (!r) test.error = format_error(r, json.begin());
        });
        node j = vj.back(); vj.clear();
        test.time.write = measure(cxjson_repeat, [&] {
            std::string s; cxon::to_bytes(s, j);
        });
        {   std::string s;
            test.time.pretty_string = measure(cxjson_repeat, [&] {
                s = cxon::test::pretty(json);
            });
        }
        {   std::string s;
            test.time.pretty = measure(cxjson_repeat, [&] {
                cxon::to_bytes(cxon::test::make_indenter(s), j);
            });
        }
    }
}

using cases = std::vector<test_case>;

static bool cl_parse(int argc, char *argv[], cases& pass, cases& fail, cases& time, cases& diff) {
    if (argc < 2) return false;
    static auto const key = [](const char* v) {
        return  std::strcmp("pass", v) == 0 || std::strcmp("fail", v) == 0 ||
                std::strcmp("time", v) == 0 || std::strcmp("diff", v) == 0
        ;
    };
    static auto const add = [](const char* v, cases& c) {
        if (*v == '@') {
            std::ifstream is(v + 1);
            if (is) {
                    std::string l;
                    while (std::getline(is, l)) {
                        auto const f = l.find_first_not_of(" \t");
                        if (f != std::string::npos && l[f] != '#') {
                            c.push_back({l, {}, {}});
                        }
                    }
            }
            else    fprintf(stderr, "%s: cannot be opened\n", v);
        }
        else {
            c.push_back({v, {}, {}});
        }
    };
    for (int i = 1; i < argc; ++i) {
        restart:
#       define CXJSON_PROC(k)\
            if (std::strcmp(#k, argv[i]) == 0) {\
                if (++i == argc || key(argv[i])) return false;\
                do {\
                    if (key(argv[i])) goto restart;\
                    add(argv[i], k);\
                }   while (++i < argc);\
                continue;\
            }
        CXJSON_PROC(pass)
        CXJSON_PROC(fail)
        CXJSON_PROC(diff)
        CXJSON_PROC(time)
#       undef CXJSON_PROC
    }
    return true;
}

struct my_traits : cxon::json::node_traits {
    using                               string_type = std::u16string;
    template <class T> using            array_type = std::list<T>;
    template <class K, class V> using   object_type = std::multimap<K, V>;
};

struct my_type {
    std::vector<int> even;
    std::list<int> odd;
    bool operator ==(const my_type& v) const { return even == v.even && odd == v.odd; }
};
CXON_STRUCT(my_type,
    CXON_JSON_STRUCT_FIELD_ASIS(even),
    CXON_JSON_STRUCT_FIELD_ASIS(odd)
)

static unsigned self() {
    unsigned a_ = 0;
    unsigned f_ = 0;
#   define CHECK(c) ++a_; if (!(c))\
        fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__), fflush(stderr), ++f_;\
        CXON_ASSERT((c), "check failed");
    {   // custom type binding + equal keys
        using node = cxon::json::basic_node<my_traits>;
        node n;
        cxon::from_bytes(n, "{\"k\": 42, \"k\": 43}");
        CHECK(n.is<node::object>() && n.get<node::object>().count(u"k") == 2);
    }
    {   //using node = cxon::json::node;

        char const s[] = "{\"array0\":[{\"1\":1},{\"2\":2}],\"array1\":[\"string\",3,{\"bool\":true,\"null\":null},null],\"number\":4}";
        node jns;
        {   // from/to string
            cxon::from_bytes(jns, s);
            std::string r; cxon::to_bytes(std::back_inserter(r), jns);
            CHECK(r == s);
        }
        node jno;
        {   // from object
            jno = node::object {
                {"array0", node::array {
                    node::object {{"1", 1}},
                    node::object {{"2", 2}}
                }},
                {"array1", node::array {
                    "string",
                    3,
                    node::object {
                        {"bool", true},
                        {"null", nullptr}
                    },
                    nullptr
                }},
                {"number", 4}
            };
            std::string r; cxon::to_bytes(r, jno);
            CHECK(r == s);
        }
        {   std::string s1;
                cxon::to_bytes(cxon::test::make_indenter(s1), jns);
            std::string s2;
                cxon::to_bytes(cxon::test::make_indenter(s2), jno);
            std::string const s0 =
                cxon::test::pretty(s1);
            CHECK(s1 == s0);
            CHECK(s2 == s0);
        }
        {   node n;
                cxon::from_bytes(n, "[3.1415926, 3.1415926, 3.1415926]");
            std::string s1;
#           if !defined(__GNUG__) || defined(__clang__)
                cxon::to_bytes(cxon::test::make_indenter(s1, 4, ' '), n, cxon::json::fp_precision::set<4>());
#           else
                cxon::to_bytes<cxon::JSON<>, cxon::json::ordered_node_traits> // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                    (cxon::test::make_indenter(s1, 4, ' '), n, cxon::json::fp_precision::set<4>());
#           endif
            std::string const s0 =
                cxon::test::pretty(s1, 4, ' ');
            CHECK(s1 == s0);
        }
        {   node n;
                cxon::from_bytes(n, "[[3.1415926, 3.1415926, [3.1415926, 3.1415926]], [3.1415926]]");
            std::string s1;
                cxon::to_bytes(cxon::test::make_indenter(s1, 2, ' '), n);
            std::string const s0 =
                cxon::test::pretty(s1, 2, ' ');
            CHECK(s1 == s0);
        }
        {   std::vector<node> v;
                cxon::from_bytes(v, "[[3.1415926, 3.1415926, [3.1415926, 3.1415926]], [3.1415926]]");
            std::string s1;
                cxon::to_bytes(cxon::test::make_indenter(s1, 2, ' '), v);
            std::string const s0 =
                cxon::test::pretty(s1, 2, ' ');
            CHECK(s1 == s0);
        }
        {   std::vector<node> v;
                cxon::from_bytes(v, "[{\"even\": [2, 4, 6]}, {\"odd\": [1, 3, 5]}]");
            std::string s1;
                cxon::to_bytes(cxon::test::make_indenter(s1, 2, ' '), v);
            std::string const s0 =
                cxon::test::pretty(s1, 2, ' ');
            CHECK(s1 == s0);
        }
        {   std::map<std::string, node> m;
                cxon::from_bytes(m, "{\"even\": [2, 4, 6], \"odd\": [1, 3, 5]}");
            std::string s1;
                cxon::to_bytes(cxon::test::make_indenter(s1, 2, ' '), m);
            std::string const s0 =
                cxon::test::pretty(s1, 2, ' ');
            CHECK(s1 == s0);
        }
        {   node n;
                cxon::from_bytes(n, "[[[[42]]]]");
            std::string s;
#           if !defined(__GNUG__) || defined(__clang__)
                auto const r = cxon::to_bytes(cxon::test::make_indenter(s), n, cxon::json::recursion_depth::set<4>());
#           else
                auto const r = cxon::to_bytes<cxon::JSON<>, cxon::json::ordered_node_traits> // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                                    (cxon::test::make_indenter(s), n, cxon::json::recursion_depth::set<4>());
#           endif
            CHECK(!r && r.ec == cxon::json::error::recursion_depth_exceeded);
        }
        {   node jn;
            auto const r = cxon::from_bytes(jn, "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[");
            CHECK(!r && r.ec == cxon::json::error::recursion_depth_exceeded);
        }
        {   node jn;
#           if !defined(__GNUG__) || defined(__clang__)
                auto const r = cxon::from_bytes(jn, "[[[[", cxon::json::recursion_depth::set<4>());
#           else
                auto const r = cxon::from_bytes<cxon::JSON<>, cxon::json::ordered_node_traits> // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                                    (jn, "[[[[", cxon::json::recursion_depth::set<4>());
#           endif
            CHECK(!r && r.ec == cxon::json::error::recursion_depth_exceeded);
        }
        {   node jn;
            auto const r = cxon::from_bytes(jn, "~");
            CHECK(!r && r.ec == cxon::json::error::invalid);
        }
        {   using namespace cxon::json;
            std::error_condition ec;
            ec = error::ok;
                CXON_ASSERT(ec.category() == error_category::value(), "check failed");
                CXON_ASSERT(std::strcmp(ec.category().name(), "cxon/json/node") == 0, "check failed");
                CXON_ASSERT(ec.message() == "no error", "check failed");
            ec = error::invalid;
                CXON_ASSERT(ec.message() == "invalid json", "check failed");
            ec = error::recursion_depth_exceeded;
                CXON_ASSERT(ec.message() == "recursion depth limit exceeded", "check failed");
            ec = error(255);
                CXON_ASSERT(ec.message() == "unknown error", "check failed");
        }
    }
    {   // ex1
        using my_type = std::map<std::string, std::vector<int>>;

        my_type mv1 = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} },
                mv2;
        std::string json;
            cxon::to_bytes(json, mv1);
            cxon::from_bytes(mv2, json);
        CHECK(mv1 == mv2);

        std::string const pretty_json = cxon::test::pretty(json);
    }
    {   // ex2
        my_type mv1 = { {2, 4, 6}, {1, 3, 5} },
                mv2;
        std::string json;
            cxon::to_bytes(json, mv1);
            cxon::from_bytes(mv2, json);
        CHECK(mv1 == mv2);
    }
    {   // ex3
        using node = cxon::json::node;
    
        char const s0[] = "{\"even\":[2,4,6],\"odd\":[1,3,5]}";
        node const n0 = node::object {
            { "even", node::array { 2, 4, 6 } },
            { "odd", node::array { 1, 3, 5 } }
        };

        node n1; // read
            cxon::from_bytes(n1, s0);
        CHECK(n1 == n0);

        std::string s1; // write
            cxon::to_bytes(s1, n0);
        CHECK(s1 == s0);
    }
    {   // ex4
        using node = cxon::json::node;

        // build using initializer lists
        node n1 = node::object {
            { "object", node::object { {"object", 0} } },
            { "array", node::array {
                    node::object { {"object", 0} }, // objects and
                    node::array { 1, 2, 3 },        // arrays must be explicit
                    "4",        // string
                    5,          // number
                    true,       // boolean
                    nullptr     // null
                }
            },
            { "string", "string" }, // "key": value
            { "number", 3.14 },
            { "boolean", false },
            { "null", nullptr }
        };

        // build using node's methods
        node n2;
            CHECK(n2.is<node::null>()); // default node type is node_kind::null
            auto& o = n2.imbue<node::object>(); // change the type and return its value
                CHECK(n2.is<node::object>());
                o["object"] = node::object {};      CHECK(o["object"].is<node::object>());
                o["array"] = node::array {};        CHECK(o["array"].is<node::array>());
                o["string"] = "string";             CHECK(o["string"].is<node::string>());
                o["number"] = 3.14;                 CHECK(o["number"].is<node::number>());
                o["boolean"] = false;               CHECK(o["boolean"].is<node::boolean>());
                o["null"] = nullptr;                CHECK(o["null"].is<node::null>());
            auto& o1 = o["object"].get<node::object>(); // get value reference, the type is known
                o1["object"] = 0;
            auto& a = o["array"].get<node::array>();    // get value reference, the type is known
                a.push_back(node::object {});       CHECK(a.back().is<node::object>());
                a.push_back(node::array {1, 2, 3}); CHECK(a.back().is<node::array>());
                a.push_back("4");                   CHECK(a.back().is<node::string>());
                a.push_back(5);                     CHECK(a.back().is<node::number>());
                a.push_back(true);                  CHECK(a.back().is<node::boolean>());
                a.push_back(nullptr);               CHECK(a.back().is<node::null>());
            auto* o2 = a[0].get_if<node::object>(); // get value pointer if the type match
                (*o2)["object"] = 0;
        CHECK(n1 == n2);

        std::string s1;
            cxon::to_bytes(s1, n1);
        std::string s2;
            cxon::to_bytes(s2, n2);
        CHECK(s1 == s2);

        std::string const pretty_json = cxon::test::pretty(s1);
    }
    {   // ex5
        using node = cxon::json::node;
        {   // (1)
            node n; CHECK(n.is<node::null>());
        }
        {   // (2)
            node o = true; CHECK(o.is<node::boolean>());
            node n(o); CHECK(n.is<node::boolean>() && n.get<node::boolean>());
        }
        {   // (3)
            node n(42.0); CHECK(n.is<node::number>() && n.get<node::number>() == 42.0);
        }
        {   // (3)
            node::object const o = { {"key", "value"} };
            node n(o); CHECK(n.is<node::object>() && n.get<node::object>() == o);
        }
        {   // (3)
            node::array const a = { 1, "string" };
            node n(a); CHECK(n.is<node::array>() && n.get<node::array>() == a);
        }
        {   // (4)
            node n(42); CHECK(n.is<node::number>() && n.get<node::number>() == 42);
        }
        {   // (4)
            node n("string"); CHECK(n.is<node::string>() && n.get<node::string>() == "string");
        }
    }
    {   // ex6
        {   // T is the same
            cxon::json::node n = "string";
            cxon::json::node::string& v = n.imbue<cxon::json::node::string>(); CHECK(v == "string");
        }
        {   // T is not the same
            cxon::json::node n = "string";
            cxon::json::node::number& v = n.imbue<cxon::json::node::number>(); CHECK(v == 0.0);
        }
    }
    {   // ex7
        cxon::json::node n = "one";
            n.get<cxon::json::node::string>() = "another";
        CHECK(n.get<cxon::json::node::string>() == "another");
    }
    {   // ex8
        cxon::json::node n = "one";
            auto *v = n.get_if<cxon::json::node::string>(); CHECK(v != nullptr);
        CHECK(n.get_if<cxon::json::node::array>() == nullptr);
    }
    {   // ex9
        cxon::json::node const n; CHECK(n.kind() == cxon::json::node_kind::null);
    }
    {   // ex10
        {   node a = nullptr, b;
            b = a; CHECK(a == b);
        }
        {   node a = true, b;
            b = a; CHECK(a == b);
        }
        {   node a = 42, b;
            b = a; CHECK(a == b);
        }
        {   node a = "blah", b;
            b = a; CHECK(a == b);
        }
        {   node a = node::array {4, 2}, b;
            b = a; CHECK(a == b);
        }
        {   node a = node::object {{"q", "?"}, {"a", 42}}, b;
            b = a; CHECK(a == b);
        }
    }
    {   // ex11
        using node = cxon::json::node;
        {   node a = nullptr, b;
            b = a; CHECK(a == b);
        }
        {   node a = true, b;
            b = a; CHECK(a == b);
        }
        {   node a = 42, b;
            b = a; CHECK(a == b);
        }
        {   node a = "blah", b;
            b = a; CHECK(a == b);
        }
        {   node a = node::array {4, 2}, b;
            b = a; CHECK(a == b);
        }
        {   node a = node::object {{"q", "?"}, {"a", 42}}, b;
            b = a; CHECK(a == b);
        }
    }
#   undef CHECK
    f_ ?
        fprintf(stdout, "cxon/json/node/self: %u of %u failed\n", f_, a_) :
        fprintf(stdout, "cxon/json/node/self: %u of %u passed\n", a_, a_)
    ;
    return f_;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        return self();
    }
    cases pass, fail, time, diff;
    if (!cl_parse(argc, argv, pass, fail, time, diff)) {
        return fprintf(stderr, "usage: cxon/json/node ((pass|fail|diff|time) (file|@file)+)+\n"), fflush(stderr), 1;
    }
    int err = 0;
    if (!pass.empty()) {
        for (auto& c : pass) {
            std::ifstream is(c.source, std::ifstream::binary);
                if (!is) {
                    ++err, c.error = "cannot be opened";
                    continue;
                }
            std::string const s = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            node result; auto const r = cxon::from_bytes(result, s);
            if (!r) {
                ++err, c.error += "must pass: '" + c.source + "' (failed with '" + format_error(r, s.begin()) + "')";
            }
        }
        size_t fc = 0;
            for (auto& c : pass) {
                if (!c.error.empty()) {
                    ++fc, fprintf(stderr, "%s: %s\n", c.source.c_str(), c.error.c_str()), fflush(stderr);
                }
            }
        fc ?
            fprintf(stdout, "cxon/json/node/pass: %zu of %zu failed\n", fc, pass.size()) :
            fprintf(stdout, "cxon/json/node/pass: %zu of %zu passed\n", pass.size(), pass.size())
        ;   fflush(stdout);
    }
    if (!fail.empty()) {
        for (auto& c : fail) {
            std::ifstream is(c.source, std::ifstream::binary);
                if (!is) {
                    ++err, c.error = "cannot be opened";
                    continue;
                }
            node result;
            auto const e = std::istreambuf_iterator<char>();
            if (auto r = cxon::from_bytes(result, std::istreambuf_iterator<char>(is), e)) {
                cxon::chio::consume<cxon::JSON<>>(r.end, e);
                    if (r.end != e) continue; // trailing chars
                std::string pass; cxon::to_bytes(pass, result);
                ++err, c.error += "must fail: '" + c.source + "' (passed as '" + pass + "')";
            }
        }
        size_t fc = 0;
            for (auto& c : fail) {
                if (!c.error.empty()) {
                    ++fc, fprintf(stderr, "%s: %s\n", c.source.c_str(), c.error.c_str()), fflush(stderr);
                }
            }
        fc ?
            fprintf(stdout, "cxon/json/node/fail: %zu of %zu failed\n", fc, fail.size()) :
            fprintf(stdout, "cxon/json/node/fail: %zu of %zu passed\n", fail.size(), fail.size())
        ;   fflush(stdout);
    }
    if (!diff.empty()) {
        static auto const name = [](const std::string& p) {
            auto f = p.find_last_of("/\\"); f = f != p.npos ? f + 1 : 0;
            auto l = p.rfind('.'); if (l == p.npos) l = p.size();
            return std::string(p, f, l - f);
        };
        for (auto& c : diff) {
            std::string json;
            {   // read
                std::ifstream is(c.source, std::ifstream::binary);
                    if (!is) {
                        ++err, c.error = "cannot be opened";
                        continue;
                    }
                json.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            }
            {   // pretty
                std::ofstream os(name(c.source) + ".0.json", std::ofstream::binary);
                    if (!os) {
                        ++err, c.error = name(c.source) + ".0.json" + ": cannot be opened";
                        continue;
                    }
                cxon::test::pretty(std::ostreambuf_iterator<char>(os), json);
            }
            node result;
            {   // from
                auto const r = cxon::from_bytes(result, json);
                    if (!r) {
                        ++err, c.error = format_error(r, json.cbegin());
                        continue;
                    }
            }
            {   // to
                std::ofstream os(name(c.source) + ".1.json", std::ofstream::binary);
                    if (!os) {
                        ++err, c.error = name(c.source) + ".1.json" + "cannot be opened";
                        continue;
                    }
                auto const w = cxon::to_bytes(cxon::test::make_indenter(std::ostreambuf_iterator<char>(os)), result);
                    if (!w) {
                        ++err, c.error += w.ec.category().name(),
                        c.error += ": " + w.ec.message();
                    }
            }
        }
        size_t fc = 0;
            for (auto& c : diff) {
                if (!c.error.empty()) {
                    ++fc, fprintf(stderr, "%s:\n\tfailed: %s\n", c.source.c_str(), c.error.c_str()), fflush(stderr);
                }
            }
        if (!fc) {
            for (auto& c : diff) {
                fprintf(stdout, "%s %s ", (name(c.source) + ".0.json").c_str(), (name(c.source) + ".1.json").c_str()), fflush(stdout);
            }
        }
        else {
            fprintf(stdout, "cxon/json/node/diff: %zu of %zu failed\n", fc, diff.size()), fflush(stdout);
        }
    }
    if (!time.empty()) {
        for (auto& c : time) {
            cxjson_test_time(c);
        }
        test_time total;
        for (auto& c : time) {
            fprintf(stdout, "%s:\n", c.source.c_str());
            if (c.error.empty()) {
                fprintf(stdout, "\tbase          :\t%8.2f\n", c.time.base);
                fprintf(stdout, "\tread          :\t%8.2f\tx %6.2f\n", c.time.read, c.time.read / c.time.base);
                fprintf(stdout, "\twrite         :\t%8.2f\tx %6.2f\n", c.time.write, c.time.write / c.time.base);
                fprintf(stdout, "\tpretty/string :\t%8.2f\tx %6.2f\n", c.time.pretty_string, c.time.pretty_string / c.time.base);
                fprintf(stdout, "\tpretty        :\t%8.2f\tx %6.2f\n", c.time.pretty, c.time.pretty / c.time.base);
                total.base += c.time.base,
                total.read += c.time.read,
                total.write += c.time.write,
                total.pretty_string += c.time.pretty_string,
                total.pretty += c.time.pretty;
            }
            else {
                fprintf(stdout, "\tfailed: %s\n", c.error.c_str());
            }
        }
        fprintf(stdout, "------------------------------------------------\n");
        fprintf(stdout, "\tread          :                 x %6.2f\n", total.read / total.base);
        fprintf(stdout, "\twrite         :                 x %6.2f\n", total.write / total.base);
        fprintf(stdout, "\tpretty/string :                 x %6.2f\n", total.pretty_string / total.base);
        fprintf(stdout, "\tpretty        :                 x %6.2f\n", total.pretty / total.base);
    }
    return err;
}