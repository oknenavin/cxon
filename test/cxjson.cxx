// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "../cxon.hxx"
#include "../pretty.hxx"
#include "../cxjson/cxjson.hxx"
#include "../cxjson/pretty.hxx"

#include <fstream>
#include <memory>
#include <chrono>
#include <cstring>
#include <cstdio>
#include <cassert>

///////////////////////////////////////////////////////////////////////////////

using node = cxjson::ordered_node;
using JSON = cxon::JSON<>;

struct test_time {
    double base = 0;
    double read = 0;
    double write = 0;
    double pretty = 0;
    double pretty_native = 0;
};

struct test_case {
    std::string source;
    test_time time;
    std::string error;
};

template <typename B>
    static double measure(B block) {
        using clock = std::chrono::steady_clock;
        using std::chrono::duration_cast;
        using std::chrono::nanoseconds;
        auto const s = clock::now();
            block();
        return duration_cast<nanoseconds>(clock::now() - s).count() / 1e6;
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

static void cxjson_test_time(test_case& test) {
    std::ifstream is(test.source, std::ifstream::binary);
        if (!is) return test.error = "cannot be opened", void();
    std::string const json = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
    {   // base
        std::unique_ptr<char[]> s;
        test.time.base = measure([&] { // something like strdup
            s = std::unique_ptr<char[]>(new char[strlen(json.c_str()) + 1]);
            std::memcpy(s.get(), json.c_str(), json.size());
        });
    }
    {   // cxon
        node j;
        test.time.read = measure([&] {
            auto const r = cxon::from_chars(j, json);
            if (!r) test.error = format_error(r, json.begin());
        });
        test.time.write = measure([&] {
            std::string s; cxon::to_chars(s, j);
        });
        {   std::string s;
            test.time.pretty = measure([&] {
                std::string s;
                //cxon::to_chars(cxon::make_indenter(std::back_inserter(s)), j);
                cxon::to_chars(cxon::make_indenter(s), j);
            });
            test.time.pretty_native = measure([&] {
                s = cxjson::pretty(j);
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

struct my_type {
    std::vector<int> even;
    std::list<int> odd;
    bool operator ==(const my_type& v) const { return even == v.even && odd == v.odd; }
};
CXON_STRUCT(my_type,
    CXON_STRUCT_FIELD_ASIS(even),
    CXON_STRUCT_FIELD_ASIS(odd)
)

static unsigned self() {
    unsigned a_ = 0;
    unsigned f_ = 0;
#   define CHECK(c) ++a_; if (!(c))\
        fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__), fflush(stderr), ++f_;\
        CXON_ASSERT((c), "check failed");
    {   //using node = cxjson::node;

        char const s[] = "{\"array0\":[{\"1\":1},{\"2\":2}],\"array1\":[\"string\",3,{\"bool\":true,\"null\":null},null],\"number\":4}";
        node jns;
        {   // from/to string
            cxon::from_chars(jns, s);
            std::string r; cxon::to_chars(std::back_inserter(r), jns);
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
            std::string r; cxon::to_chars(r, jno);
            CHECK(r == s);
        }
        {   // pretty
            std::string s0; cxon::to_chars(cxon::make_indenter(s0), jns);
            auto const s = cxjson::pretty<cxon::JSON<>, std::vector<char>>(jno);
            std::string const s1 = cxjson::pretty(jno);
            CHECK(s0 == s1);
        }
        {   node jn;
            auto const r = cxon::from_chars(jn, "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[");
            CHECK(!r && r.ec == cxjson::error::recursion_depth_exceeded);
        }
        {   node jn;
            auto const r = cxon::from_chars(jn, "~");
            CHECK(!r && r.ec == cxjson::error::invalid);
        }
        {   using namespace cxjson;
            std::error_condition ec;
            ec = error::ok;
                CXON_ASSERT(ec.category() == error_category::value, "check failed");
                CXON_ASSERT(std::strcmp(ec.category().name(), "cxjson") == 0, "check failed");
                CXON_ASSERT(ec.message() == "no error", "check failed");
            ec = error::invalid;
                CXON_ASSERT(ec.message() == "invalid json", "check failed");
            ec = error::recursion_depth_exceeded;
                CXON_ASSERT(ec.message() == "recursion depth limit exceeded", "check failed");
#           ifdef NDEBUG
                ec = error(255);
                    CXON_ASSERT(ec.message() == "unknown error", "check failed");
#           endif
        }
    }
    {   // ex1
        using my_type = std::map<std::string, std::vector<int>>;

        my_type mv1 = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} },
                mv2;
        std::string json;
            cxon::to_chars(json, mv1);
            cxon::from_chars(mv2, json);
        CHECK(mv1 == mv2);

        std::string const pretty_json = cxon::pretty(json);
    }
    {   // ex2
        my_type mv1 = { {2, 4, 6}, {1, 3, 5} },
                mv2;
        std::string json;
            cxon::to_chars(json, mv1);
            cxon::from_chars(mv2, json);
        CHECK(mv1 == mv2);
    }
    {   // ex3
        using node = cxjson::node;
    
        char const s0[] = "{\"even\":[2,4,6],\"odd\":[1,3,5]}";
        node const n0 = node::object {
            { "even", node::array { 2, 4, 6 } },
            { "odd", node::array { 1, 3, 5 } }
        };

        node n1; // read
            cxon::from_chars(n1, s0);
        CHECK(n1 == n0);

        std::string s1; // write
            cxon::to_chars(s1, n0);
        CHECK(s1 == s0);
    }
    {   // ex4
        using node = cxjson::node;

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
            CHECK(n2.is<node::null>()); // default node type is node_type::null
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
            cxon::to_chars(s1, n1);
        std::string s2;
            cxon::to_chars(s2, n2);
        CHECK(s1 == s2);

        std::string const pretty_json = cxon::pretty(s1);
    }
    {   // ex5
        using node = cxjson::node;
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
            cxjson::node n = "string";
            cxjson::node::string& v = n.imbue<cxjson::node::string>(); CHECK(v == "string");
        }
        {   // T is not the same
            cxjson::node n = "string";
            cxjson::node::number& v = n.imbue<cxjson::node::number>(); CHECK(v == 0.0);
        }
    }
    {   // ex7
        cxjson::node n = "one";
            n.get<cxjson::node::string>() = "another";
        CHECK(n.get<cxjson::node::string>() == "another");
    }
    {   // ex8
        cxjson::node n = "one";
            auto *v = n.get_if<cxjson::node::string>(); CHECK(v != nullptr);
        CHECK(n.get_if<cxjson::node::array>() == nullptr);
    }
    {   // ex9
        cxjson::node const n; CHECK(n.type() == cxjson::node_type::null);
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
        using node = cxjson::node;
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
        fprintf(stdout, "cxjson/self: %u of %u failed\n", f_, a_) :
        fprintf(stdout, "cxjson/self: %u of %u passed\n", a_, a_)
    ;
    return f_;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        return self();
    }
    cases pass, fail, time, diff;
    if (!cl_parse(argc, argv, pass, fail, time, diff)) {
        return fprintf(stderr, "usage: cxjson ((pass|fail|diff|time) (file|@file)+)+\n"), 1;
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
            node result; auto const r = cxon::from_chars(result, s);
            if (!r) {
                ++err, c.error += "must pass: '" + c.source + "' (failed with '" + format_error(r, s.begin()) + "')";
            }
        }
        size_t fc = 0;
            for (auto& c : pass) {
                if (!c.error.empty()) {
                    ++fc, fprintf(stderr, "%s: %s\n", c.source.c_str(), c.error.c_str());
                }
            }
        fc ?
            fprintf(stdout, "cxjson/pass: %zu of %zu failed\n", fc, pass.size()) :
            fprintf(stdout, "cxjson/pass: %zu of %zu passed\n", pass.size(), pass.size())
        ;
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
            auto r = cxon::from_chars(result, std::istreambuf_iterator<char>(is), e);
            if (r) {
                cxon::io::consume<cxon::JSON<>>(r.end, e);
                if (r.end != e) continue; // trailing chars
                std::string tluser; cxon::to_chars(tluser, result);
                ++err, c.error += "must fail: '" + c.source + "' (passed as '" + tluser + "')";
            }
        }
        size_t fc = 0;
            for (auto& c : fail) {
                if (!c.error.empty()) {
                    ++fc, fprintf(stderr, "%s: %s\n", c.source.c_str(), c.error.c_str());
                }
            }
        fc ?
            fprintf(stdout, "cxjson/fail: %zu of %zu failed\n", fc, fail.size()) :
            fprintf(stdout, "cxjson/fail: %zu of %zu passed\n", fail.size(), fail.size())
        ;
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
            node result;
            {   // from
                std::ofstream os(name(c.source) + ".0.json", std::ofstream::binary);
                    if (!os) {
                        ++err, c.error = name(c.source) + ".0.json" + ": cannot be opened";
                        continue;
                    }
                auto o = cxon::make_indenter(std::ostreambuf_iterator<char>(os));
                    for (auto c : json)  *o = c;
                auto const r = cxon::from_chars(result, json);
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
                auto o = cxon::make_indenter(std::ostreambuf_iterator<char>(os));
                auto const w = cxon::to_chars(o, result);
                    if (!w) {
                        //++err, c.error = format_error(w, o);
                        ++err, c.error += w.ec.category().name(),
                        c.error += ": " + w.ec.message();
                        continue;
                        CXON_ASSERT(w, "unexpected");
                        fprintf(stdout, "%s %s ", (name(c.source) + ".0.json").c_str(), (name(c.source) + ".1.json").c_str());
                    }
            }
        }
        size_t fc = 0;
            for (auto& c : diff) {
                if (!c.error.empty()) {
                    ++fc, fprintf(stderr, "%s:\n\tfailed: %s\n", c.source.c_str(), c.error.c_str());
                }
            }
        if (!fc) {
            for (auto& c : diff) {
                fprintf(stdout, "%s %s ", (name(c.source) + ".0.json").c_str(), (name(c.source) + ".1.json").c_str());
            }
        }
        else {
            fprintf(stdout, "cxjson/diff: %zu of %zu failed\n", fc, diff.size());
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
                fprintf(stdout, "\tcxon::pretty  :\t%8.2f\tx %6.2f\n", c.time.pretty, c.time.pretty / c.time.base);
                fprintf(stdout, "\tcxjson::pretty:\t%8.2f\tx %6.2f\n", c.time.pretty_native, c.time.pretty_native / c.time.base);
                total.base += c.time.base,
                total.read += c.time.read,
                total.write += c.time.write,
                total.pretty += c.time.pretty,
                total.pretty_native += c.time.pretty_native;
            }
            else {
                fprintf(stdout, "\tfailed: %s\n", c.error.c_str());
            }
        }
        fprintf(stdout, "------------------------------------------------\n");
        fprintf(stdout, "\tread          :                 x %6.2f\n", total.read / total.base);
        fprintf(stdout, "\twrite         :                 x %6.2f\n", total.write / total.base);
        fprintf(stdout, "\tcxon::pretty  :                 x %6.2f\n", total.pretty / total.base);
        fprintf(stdout, "\tcxjson::pretty:                 x %6.2f\n", total.pretty_native / total.base);
    }
    return err;
}
