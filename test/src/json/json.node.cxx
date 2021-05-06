// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/json.hxx"
#include "cxon/cbor.hxx"

#include "cxon/lib/node.ordered.hxx"
#include "cxon/lib/std/list.hxx"

#include "cxon/lang/json/tidy.hxx"

#ifdef COMPARE_WITH_BOOST_JSON
#   define BOOST_JSON_STANDALONE
#   include <boost/json/src.hpp>
#endif

#ifdef COMPARE_WITH_RAPIDJSON
#   include "rapidjson/document.h"
#   include "rapidjson/writer.h"
#   include "rapidjson/stringbuffer.h"
#   include "rapidjson/error/en.h"
#endif

#ifdef COMPARE_WITH_NLOHMANN
#   include "nlohmann/json.hpp"
#endif

#include <fstream>
#include <memory>
#include <chrono>
#include <cstring>
#include <cstdio>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////

using node = cxon::json::ordered_node;

struct test_time {
    size_t size = 0;
    double read = 0;
    double write = 0;
    double tidy_itr = 0;
    double tidy_str = 0;
#   ifdef COMPARE_WITH_BOOST_JSON
        double boostj_read = 0;
        double boostj_write = 0;
#   endif
#   ifdef COMPARE_WITH_RAPIDJSON
        double rapidj_read = 0;
        double rapidj_write = 0;
#   endif
#   ifdef COMPARE_WITH_NLOHMANN
        double nlohmann_read = 0;
        double nlohmann_write = 0;
#   endif
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

constexpr unsigned cxon_json_repeat = 3;

static void cxon_json_test_time(test_case& test) {
    std::ifstream is(test.source, std::ifstream::binary);
        if (!is) return test.error = "cannot be opened", void();
    std::string const json = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
    {   // size
        test.time.size = json.size() - 1;
    }
    {   // cxon
        std::vector<node> vj;
        test.time.read = measure(cxon_json_repeat, [&] {
            vj.emplace_back();
            auto const r = cxon::from_bytes(vj.back(), json);
            if (!r) test.error = format_error(r, json.begin());
        });
        auto n = vj.back(); vj.clear();
        {   std::string s;
            test.time.write = measure(cxon_json_repeat, [&] {
                s.clear(); cxon::to_bytes(s, n);
            });
        }
        {   std::string s;
            test.time.tidy_itr = measure(cxon_json_repeat, [&] {
                s.clear(); cxon::to_bytes(cxon::json::make_indenter(s), n);
            });
        }
        {   std::string s;
            test.time.tidy_str = measure(cxon_json_repeat, [&] {
                s = cxon::json::tidy(json);
            });
        }
    }
#   ifdef COMPARE_WITH_BOOST_JSON
    {   // Boost/JSON
        std::vector<boost::json::value> vj;
        test.time.boostj_read = measure(cxon_json_repeat, [&] {
            auto& j = vj.emplace_back();
            boost::json::error_code ec;
            j = boost::json::parse(json, ec);
            if (ec) test.error = std::string("Boost/JSON: ") + ec.category().name() + ": " + ec.message();
        });
        boost::json::value j = vj.back(); vj.clear();
        {   std::string s;
            test.time.boostj_write = measure(cxon_json_repeat, [&] {
                s = boost::json::serialize(j);
            });
        }
    }
#   endif
#   ifdef COMPARE_WITH_RAPIDJSON
    {   // RapidJSON
        std::vector<std::unique_ptr<rapidjson::Document>> vj;
        test.time.rapidj_read = measure(cxon_json_repeat, [&] {
            auto j = vj.emplace_back(std::make_unique<rapidjson::Document>()).get();
            rapidjson::ParseResult r = j->Parse(json.c_str());
            if (!r) test.error = std::string("RapidJSON error: ") + rapidjson::GetParseError_En(r.Code());
        });
        auto j = std::unique_ptr<rapidjson::Document>(vj.back().release()); vj.clear();
        {   const char *s;
            test.time.rapidj_write = measure(cxon_json_repeat, [&] {
                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                j->Accept(writer);
                s = buffer.GetString();
            });
        }
    }
#   endif
#   ifdef COMPARE_WITH_NLOHMANN
    {   // nlohmann/json
        std::vector<nlohmann::json> vj;
        test.time.nlohmann_read = measure(cxon_json_repeat, [&] {
            vj.emplace_back();
            vj.back() = nlohmann::json::parse(json);
        });
        auto j = vj.back(); vj.clear();
        {   std::string s;
            test.time.nlohmann_write = measure(cxon_json_repeat, [&] {
                s = j.dump();
            });
        }
    }
#   endif
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
            else    std::fprintf(stderr, "%s: cannot be opened\n", v);
        }
        else {
            c.push_back({v, {}, {}});
        }
    };
    for (int i = 1; i < argc; ++i) {
        restart:
#       define CXON_JSON_PROC(k)\
            if (std::strcmp(#k, argv[i]) == 0) {\
                if (++i == argc || key(argv[i])) return false;\
                do {\
                    if (key(argv[i])) goto restart;\
                    add(argv[i], k);\
                }   while (++i < argc);\
                continue;\
            }
            CXON_JSON_PROC(pass)
            CXON_JSON_PROC(fail)
            CXON_JSON_PROC(diff)
            CXON_JSON_PROC(time)
#       undef CXON_JSON_PROC
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
CXON_JSON_CLS(my_type,
    CXON_JSON_CLS_FIELD_ASIS(even),
    CXON_JSON_CLS_FIELD_ASIS(odd)
)

#ifndef TIME_ONLY
    static unsigned self() {
        unsigned a_ = 0;
        unsigned f_ = 0;
#       define CHECK(c) ++a_; if (!(c))\
            std::fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__), std::fflush(stderr), ++f_;\
            CXON_ASSERT((c), "check failed");
        {   // custom type binding + equal keys
            using node = cxon::json::basic_node<my_traits>;
            node n;
            cxon::from_bytes(n, "{\"k\": 42, \"k\": 43}");
            CHECK(n.is<node::object>() && n.get<node::object>().count(u"k") == 2);
        }
        {   //using node = cxon::json::node;

            char const s[] = "{\"key0\":[{\"1\":1},{\"2\":2}],\"key1\":[\"string\",3,{\"bool\":true,\"null\":null},null],\"key2\":4}";
            node jns;
            {   // from/to string
                    cxon::from_bytes(jns, s);
                std::string r;
                    cxon::to_bytes(std::back_inserter(r), jns);
                CHECK(r == s);
            }
            node jno;
            {   // from object
                jno = {
                    { "key0", {
                        { {"1", 1} },
                        { {"2", 2} }
                    } },
                    { "key1", {
                        "string",
                        3,
                        {
                            {"bool", true},
                            {"null", nullptr}
                        },
                        nullptr
                    } },
                    { "key2", 4 }
                };
                std::string r; cxon::to_bytes(r, jno);
                CHECK(r == s);
            }
            {   std::string s1;
                    cxon::to_bytes(cxon::json::make_indenter(s1), jns);
                std::string s2;
                    cxon::to_bytes(cxon::json::make_indenter(s2), jno);
                std::string const s0 =
                    cxon::json::tidy(s1);
                CHECK(s1 == s0);
                CHECK(s2 == s0);
            }
            {   node n;
                    cxon::from_bytes(n, "[3.1415926, 3.1415926, 3.1415926]");
                std::string s1;
#               if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                    cxon::to_bytes(cxon::json::make_indenter(s1, 4, ' '), n, cxon::json::fp_precision::set<4>());
#               else
                    // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                    // seems to be fixed around 10, but after the inclusion of cbor.hxx,
                    // this workaround does not work for to_bytes anymore
                    //cxon::to_bytes<cxon::JSON<>, cxon::json::ordered_node_traits>
                    //    (cxon::json::make_indenter(s1, 4, ' '), n, cxon::json::fp_precision::set<4>());
#               endif
                std::string const s0 =
                    cxon::json::tidy(s1, 4, ' ');
                CHECK(s1 == s0);
            }
            {   node n;
                    cxon::from_bytes(n, "[[3.1415926, 3.1415926, [3.1415926, 3.1415926]], [3.1415926]]");
                std::string s1;
                    cxon::to_bytes(cxon::json::make_indenter(s1, 2, ' '), n);
                std::string const s0 =
                    cxon::json::tidy(s1, 2, ' ');
                CHECK(s1 == s0);
            }
            {   std::vector<node> v;
                    cxon::from_bytes(v, "[[3.1415926, 3.1415926, [3.1415926, 3.1415926]], [3.1415926]]");
                std::string s1;
                    cxon::to_bytes(cxon::json::make_indenter(s1, 2, ' '), v);
                std::string const s0 =
                    cxon::json::tidy(s1, 2, ' ');
                CHECK(s1 == s0);
            }
            {   std::vector<node> v;
                    cxon::from_bytes(v, "[{\"even\": [2, 4, 6]}, {\"odd\": [1, 3, 5]}]");
                std::string s1;
                    cxon::to_bytes(cxon::json::make_indenter(s1, 2, ' '), v);
                std::string const s0 =
                    cxon::json::tidy(s1, 2, ' ');
                CHECK(s1 == s0);
            }
            {   std::map<std::string, node> m;
                    cxon::from_bytes(m, "{\"even\": [2, 4, 6], \"odd\": [1, 3, 5]}");
                std::string s1;
                    cxon::to_bytes(cxon::json::make_indenter(s1, 2, ' '), m);
                std::string const s0 =
                    cxon::json::tidy(s1, 2, ' ');
                CHECK(s1 == s0);
            }
            {   char b[2];
                auto const r = cxon::to_bytes(cxon::json::make_indenter(std::begin(b), std::end(b)), std::vector<int>{1});
                CHECK(!r && r.ec == cxon::json::write_error::output_failure);
            }
            {   char b[2];
                    cxon::json::tidy(std::begin(b), std::end(b), "[1]");
                CHECK(b[0] == '[' && b[1] == '\n');
            }
            {   node n;
                    cxon::from_bytes(n, "[[[[42]]]]");
                std::string s;
#               if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                    auto const r = cxon::to_bytes(cxon::json::make_indenter(s), n, cxon::node::recursion_depth::set<4>());
                    CHECK(!r && r.ec == cxon::node::error::recursion_depth_exceeded);
#               else
                    // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                    // seems to be fixed around 10, but after the inclusion of cbor.hxx,
                    // this workaround does not work for to_bytes anymore
                    //auto const r = cxon::to_bytes<cxon::JSON<>, cxon::json::ordered_node_traits>
                    //                    (cxon::json::make_indenter(s), n, cxon::node::recursion_depth::set<4>());
                    //CHECK(!r && r.ec == cxon::node::error::recursion_depth_exceeded);
#               endif
            }
            {   node jn;
                auto const r = cxon::from_bytes(jn, "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[");
                CHECK(!r && r.ec == cxon::node::error::recursion_depth_exceeded);
            }
            {   node jn;
#               if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                    auto const r = cxon::from_bytes(jn, "[[[[", cxon::node::recursion_depth::set<4>());
#               else
                    // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                    // seems to be fixed around 10, after the inclusion of cbor.hxx,
                    // funnily enough, this workaround continues to work for from_chars (unlike to_chars)
                    auto const r = cxon::from_bytes<cxon::JSON<>, cxon::json::ordered_node_traits>
                                        (jn, "[[[[", cxon::node::recursion_depth::set<4>());
#               endif
                CHECK(!r && r.ec == cxon::node::error::recursion_depth_exceeded);
            }
            {   node jn;
                auto const r = cxon::from_bytes(jn, "~");
                CHECK(!r && r.ec == cxon::node::error::invalid);
            }
            {   using namespace cxon::node;
                std::error_condition ec;
                ec = error::ok;
                    CXON_ASSERT(ec.category() == error_category::value(), "check failed");
                    CXON_ASSERT(std::strcmp(ec.category().name(), "cxon/node") == 0, "check failed");
                    CXON_ASSERT(ec.message() == "no error", "check failed");
                ec = error::invalid;
                    CXON_ASSERT(ec.message() == "invalid input", "check failed");
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

            std::string const tidy_json = cxon::json::tidy(json);
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
            node const n0 = {
                { "even", { 2U, 4U, 6U } },
                { "odd", { 1U, 3U, 5U } }
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
            node n1 = {
                { "object", { {"object", 0} } },
                { "array", {
                        { {"object", 0} }, // objects and
                        { 1, 2, 3 },        // arrays must be explicit
                        "4",        // string
                        3,          // signed
                        14U,        // unsigned
                        3.14,       // real
                        true,       // boolean
                        nullptr     // null
                    }
                },
                { "string", "string" }, // "key": value
                { "sint", 3 },
                { "uint", 14U },
                { "real", 3.14 },
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
                    o["sint"] = 3;                      CHECK(o["sint"].is<node::sint>());
                    o["uint"] = 14U;                    CHECK(o["uint"].is<node::uint>());
                    o["real"] = 3.14;                   CHECK(o["real"].is<node::real>());
                    o["boolean"] = false;               CHECK(o["boolean"].is<node::boolean>());
                    o["null"] = nullptr;                CHECK(o["null"].is<node::null>());
                auto& o1 = o["object"].get<node::object>(); // get value reference, the type is known
                    o1["object"] = 0;
                auto& a = o["array"].get<node::array>();    // get value reference, the type is known
                    a.push_back(node::object {});       CHECK(a.back().is<node::object>());
                    a.push_back(node::array {1, 2, 3}); CHECK(a.back().is<node::array>());
                    a.push_back("4");                   CHECK(a.back().is<node::string>());
                    a.push_back(3);                     CHECK(a.back().is<node::sint>());
                    a.push_back(14U);                   CHECK(a.back().is<node::uint>());
                    a.push_back(3.14);                  CHECK(a.back().is<node::real>());
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

            std::string const tidy_json = cxon::json::tidy(s1);
        }
        {
            node n1(1);     CHECK(n1.is<node::sint>() && n1.get<node::sint>() == 1);
            node n2(1L);    CHECK(n2.is<node::sint>() && n2.get<node::sint>() == 1);
            node n3(1LL);   CHECK(n3.is<node::sint>() && n3.get<node::sint>() == 1);
            node n4(1U);    CHECK(n4.is<node::uint>() && n4.get<node::uint>() == 1);
            node n5(1UL);   CHECK(n5.is<node::uint>() && n5.get<node::uint>() == 1);
            node n6(1ULL);  CHECK(n6.is<node::uint>() && n6.get<node::uint>() == 1);
            node n7(1.);    CHECK(n7.is<node::real>() && n7.get<node::real>() == 1.);
            node n8(1.F);   CHECK(n8.is<node::real>() && n8.get<node::real>() == 1.F);
        }
        {
            node n1; n1 = 1;    CHECK(n1.is<node::sint>() && n1.get<node::sint>() == 1);
            node n2; n2 = 1L;   CHECK(n2.is<node::sint>() && n2.get<node::sint>() == 1);
            node n3; n3 = 1LL;  CHECK(n3.is<node::sint>() && n3.get<node::sint>() == 1);
            node n4; n4 = 1U;   CHECK(n4.is<node::uint>() && n4.get<node::uint>() == 1);
            node n5; n5 = 1UL;  CHECK(n5.is<node::uint>() && n5.get<node::uint>() == 1);
            node n6; n6 = 1ULL; CHECK(n6.is<node::uint>() && n6.get<node::uint>() == 1);
            node n7; n7 = 1.;   CHECK(n7.is<node::real>() && n7.get<node::real>() == 1.);
            node n8; n8 = 1.F;  CHECK(n8.is<node::real>() && n8.get<node::real>() == 1.F);
        }
        {   // ex5
            using node = cxon::json::node;
            {   // (1)
                node n; CHECK(n.is<node::null>() && n.get<node::null>() == nullptr);
            }
            {   // (2)
                node o = true; CHECK(o.is<node::boolean>());
                node n(o); CHECK(n.is<node::boolean>() && n.get<node::boolean>());
            }
            {   // (3)
                node n(42.0); CHECK(n.is<node::real>() && n.get<node::real>() == 42.0);
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
                node n(3); CHECK(n.is<node::sint>() && n.get<node::sint>() == 3);
            }
            {   // (4)
                node n(14U); CHECK(n.is<node::uint>() && n.get<node::uint>() == 14U);
            }
            {   // (4)
                node n(3.14); CHECK(n.is<node::real>() && n.get<node::real>() == 3.14);
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
                cxon::json::node::real& v = n.imbue<cxon::json::node::real>(); CHECK(v == 0.0);
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
            {   node a = 42U, b;
                b = a; CHECK(a == b);
            }
            {   node a = 42.0, b;
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
            {   node a = 42U, b;
                b = a; CHECK(a == b);
            }
            {   node a = 42.0, b;
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
        {   // move assignment
            {   node a;
                a = node(42); CHECK(a.is<node::sint>() && a.get<node::sint>() == 42);
            }
            {   node a;
                a = node(42U); CHECK(a.is<node::uint>() && a.get<node::uint>() == 42U);
            }
            {   node a;
                a = node(42.0); CHECK(a.is<node::real>() && a.get<node::real>() == 42.0);
            }
            {   node a;
                a = node(nullptr); CHECK(a.is<node::null>() && a.get<node::null>() == nullptr);
            }
            {   node a;
                a = node(true); CHECK(a.is<node::boolean>() && a.get<node::boolean>() == true);
            }
            {   node a;
                a = node(""); CHECK(a.is<node::string>() && a.get<node::string>() == "");
            }
            {   node a;
                a = node(node::array {1}); CHECK(a.is<node::array>() && a.get<node::array>() == (node::array {1}));
            }
            {   node a;
                a = node(node::object {{"", 1}}); CHECK(a.is<node::object>() && a.get<node::object>() == (node::object {{"", 1}}));
            }
        }
        {   // less than
            CHECK(node(node::object{}) < node(node::array{}));
            CHECK(node(node::object{{1, 0}}) < node(node::object{{2, 0}}));
            CHECK(node(node::array{1}) < node(node::array{2}));
            CHECK(node(node::array{1}) < node(node::array{2, 3}));
            CHECK(node("A") < node("a"));
            CHECK(node("a") < node("aa"));
            CHECK(node(1) < node(2));
            CHECK(node(1U) < node(2U));
            CHECK(node(1.0) < node(2.0));
            CHECK(node(false) < node(true));
            CHECK(!(node(nullptr) < node(nullptr)));
        }
        {   // numbers
            node n; cxon::from_bytes_result<const char*> r;
                r = cxon::from_bytes(n, "2147483648"); // 2^31 = 2147483648
            CHECK(r && n.is<node::uint>() && n.get<node::uint>() == 2147483648);
                r = cxon::from_bytes(n, "2147483649"); // 2^31 = 2147483648
            CHECK(r && n.is<node::uint>() && n.get<node::uint>() == 2147483649);
                r = cxon::from_bytes(n, "-4294967295"); // 2^32 - 1 = 4294967295
            CHECK(r && n.is<node::sint>() && n.get<node::sint>() == -4294967295);
                r = cxon::from_bytes(n, "-4294967296"); // 2^32 - 1 = 4294967295
            CHECK(r && n.is<node::sint>() && n.get<node::sint>() == -4294967296);

                r = cxon::from_bytes(n, "18446744073709551615"); // 2^64 - 1 = 18446744073709551615
            CHECK(r && n.is<node::uint>() && n.get<node::uint>() == std::numeric_limits<node::uint>::max());
                r = cxon::from_bytes(n, "18446744073709551616"); // 2^64 - 1 = 18446744073709551615
            CHECK(r && n.is<node::real>() && n.get<node::real>() == 1.8446744073709552e19);
                r = cxon::from_bytes(n, "-9223372036854775808"); // 2^63 = 9223372036854775808
            CHECK(r && n.is<node::sint>() && n.get<node::sint>() == std::numeric_limits<node::sint>::min());
                r = cxon::from_bytes(n, "-9223372036854775809"); // 2^63 = 9223372036854775808
            CHECK(r && n.is<node::real>() && n.get<node::real>() == -9.2233720368547758e18);
        }
        {   node n = {
                {{{1, 2}}, 3},
                {{4}, 5},
                {"6", 7},
                {8, 9},
                {10U, 11},
                {12.0, 13},
                {true, 14},
                {nullptr, 15},
                {std::numeric_limits<node::real>::infinity(), 16},
                {-std::numeric_limits<node::real>::infinity(), 17},
                {std::numeric_limits<node::real>::quiet_NaN(), 18}
            };
            std::string s;
                cxon::to_bytes(s, n);
            CHECK(s == R"({"{\"1\":2}":3,"[4]":5,"6":7,"8":9,"10":11,"12":13,"true":14,"null":15,"inf":16,"-inf":17,"nan":18})");
        }
        {   // node::json::arbitrary_keys
            using node = cxon::json::node;
            {   char const in[] = "{{1: 2}: 3, [4]: 5, \"6\": 7, -8: 9, 10: 11, 12.0: 13, true: 14, null: 15, \"inf\": 16, \"-inf\": 17, \"nan\": 18}";
                node const out = {
                    {{{1U, 2U}}, 3U},
                    {{4U}, 5U},
                    {"6", 7U},
                    {-8, 9U},
                    {10U, 11U},
                    {12.0, 13U},
                    {true, 14U},
                    {nullptr, 15U},
                    {std::numeric_limits<node::real>::infinity(), 16U},
                    {-std::numeric_limits<node::real>::infinity(), 17U},
                    {std::numeric_limits<node::real>::quiet_NaN(), 18U}
                };
                node n;
                    cxon::from_bytes<cxon::JSON<>, cxon::json::node_traits>(n, in, cxon::node::json::arbitrary_keys::set<true>(), cxon::node::json::extract_nans::set<true>());
                CHECK(n == out);
            }
            {   node n;
                    auto const r = cxon::from_bytes<cxon::JSON<>, cxon::json::node_traits>(n, "{\"x: 0}", cxon::node::json::arbitrary_keys::set<true>(), cxon::node::json::extract_nans::set<true>());
                CHECK(!r && r.ec == cxon::json::read_error::unexpected);
            }
            {   node n;
                    auto const r = cxon::from_bytes<cxon::JSON<>, cxon::json::node_traits>(n, "{x: 0}", cxon::node::json::arbitrary_keys::set<true>());
                CHECK(!r && r.ec == cxon::node::error::invalid);
            }
            {
                node const n = node::object {{node::object {{1, 2}}, 3}, {node::array {4}, 5}, {"6", 7}, {8, 9}, {true, 10}, {nullptr, 11}};
                std::string s;
#               if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                    cxon::to_bytes(s, n, cxon::node::json::arbitrary_keys::set<true>());
                    CHECK(s == "{{1:2}:3,[4]:5,\"6\":7,8:9,true:10,null:11}");
#               else
                    // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                    // seems to be fixed around 10, but after the inclusion of cbor.hxx,
                    // this workaround does not work for to_bytes anymore
                    //cxon::to_bytes<cxon::JSON<>, cxon::json::ordered_node_traits>
                    //    (s, n, cxon::node::json::arbitrary_keys::set<true>());
                    //CHECK(s == "{{1:2}:3,[4]:5,\"6\":7,8:9,true:10,null:11}");
#               endif
            }
        }
        {   // cbor::node
            using node = cxon::cbor::node;
            {   char const in[] = "{\"a\":[{\"f\":2},[3,4],\"string\",5,false,null],\"b\":\"string\",\"c\":1,\"d\":true,\"e\":null}";
                node n;
                    cxon::from_bytes(n, in);
                std::string s;
                    cxon::to_bytes(s, n);
                CHECK(s == in);
            }
            {   char const in[] = "#[1]";
                node n;
                    auto r = cxon::from_bytes(n, in);
                CHECK(!r && r.ec == cxon::node::error::invalid);
            }
            {   node n = { 1, 2U, node::bytes {3}, "text", {4}, {{"5", 6}}, true, nullptr, node::undefined(), 7.0, node::simple(8) };
                std::string s;
                    cxon::to_bytes(s, n);
                CHECK(s == "[1,2,[3],\"text\",[4],{\"5\":6},true,null,null,7,8]");
            }
            {   node n = {
                    { 1, 0 },
                    { 2U, 0 },
                    { node::bytes {3}, 0 },
                    { "4", 0 },
                    { {5}, 0 },
                    { {{6, 0}}, 0 },
                    { node::tag {7, 8}, 0 },
                    { true, 0 },
                    { nullptr, 0 },
                    { node::undefined(), 0 },
                    { 9.0, 0 },
                    { node::simple(10), 0 }
                };
                std::string s;
                    cxon::to_bytes(s, n);
                CHECK(s == "{\"1\":0,\"2\":0,\"[3]\":0,\"4\":0,\"[5]\":0,\"{\\\"6\\\":0}\":0,\"8\":0,\"true\":0,\"null\":0,\"null\":0,\"9\":0,\"10\":0}");
            }
        }
        {   // round-trip
            using node = cxon::cbor::node;
            {   // cbor::node => json => cbor::node
                node const fr = { -1, 2U, node::bytes {3 , 4 }, "5, 6", {7 , 8 }, {{  9, 10 }, {"11", 12 }}, node::tag {13, 14}, true, nullptr, node::undefined(), 15.16, node::simple(17) };
                node const to = { -1, 2U,             {3U, 4U}, "5, 6", {7U, 8U}, {{"9", 10U}, {"11", 12U}},                14U, true, nullptr,           nullptr, 15.16,              17U };
                std::string s;
                    cxon::to_bytes(s, fr);
                node n;
                    cxon::from_bytes(n, s);
                CHECK(n == to);
            }
            {   // json => cbor::node => json
                std::string const fr = "[{\"1\": 2}, [3, 4], \"5, 6\", 7, true, null]";
                std::string const to = "[{\"1\":2},[3,4],\"5, 6\",7,true,null]";
                node n;
                    cxon::from_bytes(n, fr);
                std::string s;
                    cxon::to_bytes(s, n);
                CHECK(s == to);
            }
        }
    #   undef CHECK

        std::fprintf(stdout, "cxon/json/node/self:  %u of %3u failed\n", f_, a_); std::fflush(stdout);

        return f_;
    }
#endif

int main(int argc, char *argv[]) {
    if (argc == 1) {
#       ifndef TIME_ONLY
            return self();
#       else
            return -1;
#       endif
    }

    cases pass, fail, time, diff;

    if (!cl_parse(argc, argv, pass, fail, time, diff)) {
        return std::fprintf(stderr, "usage: cxon/json/node ((pass|fail|diff|time) (file|@file)+)+\n"), std::fflush(stderr), 1;
    }

    int err = 0;

#   ifndef TIME_ONLY
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
                        ++fc, std::fprintf(stderr, "%s: %s\n", c.source.c_str(), c.error.c_str()), std::fflush(stderr);
                    }
                }
            std::fprintf(stdout, "cxon/json/node/pass:  %zu of %3zu failed\n", fc, pass.size()); std::fflush(stdout);
        }

        if (!fail.empty()) {
            for (auto& c : fail) {
                std::ifstream is(c.source, std::ifstream::binary);
                    if (!is) {
                        ++err, c.error = "cannot be opened";
                        continue;
                    }
                node result;
                std::string const s = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                auto const e = s.end();
                if (auto r = cxon::from_bytes(result, s.begin(), e)) {
                    cxon::cio::consume<cxon::JSON<>>(r.end, e);
                        if (r.end != e) continue; // trailing chars
                    std::string pass; cxon::to_bytes(pass, result);
                    ++err, c.error += "must fail: '" + c.source + "' (passed as '" + pass + "')";
                }
            }
            size_t fc = 0;
                for (auto& c : fail) {
                    if (!c.error.empty()) {
                        ++fc, std::fprintf(stderr, "%s: %s\n", c.source.c_str(), c.error.c_str()), std::fflush(stderr);
                    }
                }
            std::fprintf(stdout, "cxon/json/node/fail:  %zu of %3zu failed\n", fc, fail.size()); std::fflush(stdout);
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
                {   // tidy
                    std::ofstream os(name(c.source) + ".jt(0).json", std::ofstream::binary);
                        if (!os) {
                            ++err, c.error = name(c.source) + ".jt(0).json" + ": cannot be opened";
                            continue;
                        }
                    cxon::json::tidy(std::ostreambuf_iterator<char>(os), json);
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
                    std::ofstream os(name(c.source) + ".jt(1).json", std::ofstream::binary);
                        if (!os) {
                            ++err, c.error = name(c.source) + ".jt(1).json" + "cannot be opened";
                            continue;
                        }
                    auto const w = cxon::to_bytes(cxon::json::make_indenter(std::ostreambuf_iterator<char>(os)), result);
                        if (!w) {
                            ++err, c.error += w.ec.category().name(),
                            c.error += ": " + w.ec.message();
                        }
                }
            }
            size_t fc = 0;
                for (auto& c : diff) {
                    if (!c.error.empty()) {
                        ++fc, std::fprintf(stderr, "%s:\n\tfailed: %s\n", c.source.c_str(), c.error.c_str()), std::fflush(stderr);
                    }
                }
            if (!fc) {
                for (auto& c : diff) {
                    std::fprintf(stdout, "%s %s ", (name(c.source) + ".jt(0).json").c_str(), (name(c.source) + ".jt(1).json").c_str()), std::fflush(stdout);
                }
            }
            else {
                std::fprintf(stdout, "cxon/json/node/diff:  %zu of %3zu failed\n", fc, diff.size()), std::fflush(stdout);
            }
        }
#   endif

    if (!time.empty()) {
        for (auto& c : time) {
            cxon_json_test_time(c);
        }

        std::vector<std::vector<std::string>> tab;
        {   // build the table
            static auto const fmt = [](double d) -> std::string {
                char b[64];
                int const r = std::snprintf(b, sizeof(b), "%.2f", d); CXON_ASSERT( r > 0 && r < (int)sizeof(b), "unexpected");
                return b;
            };
            {   // header
                tab.push_back({
                    "CXON/JSON",
                    "Size",
                    "Read",
#                   ifdef COMPARE_WITH_BOOST_JSON
                        "x", "Boost/JSON",
#                   endif
#                   ifdef COMPARE_WITH_RAPIDJSON
                        "x", "RapidJSON",
#                   endif
#                   ifdef COMPARE_WITH_NLOHMANN
                        "x", "nlohmann/json",
#                   endif
                    "Write",
#                   ifdef COMPARE_WITH_BOOST_JSON
                        "x", "Boost/JSON",
#                   endif
#                   ifdef COMPARE_WITH_RAPIDJSON
                        "x", "RapidJSON",
#                   endif
#                   ifdef COMPARE_WITH_NLOHMANN
                        "x", "nlohmann/JSON",
#                   endif
                    "Tidy/Itr",
                    "Tidy/Str"
                });
            }
            test_time total;
            {   // body
                for (auto& c : time) {
                    if (!c.error.empty()) {
                        std::fprintf(stderr, "%s: %s\n", c.source.c_str(), c.error.c_str());
                        continue;
                    }
                    double const size = double(c.time.size) / (1024. * 1024);
                    tab.push_back({
                        c.source.substr(c.source.rfind('/') + 1),
                        fmt(size),
                        fmt(size / (c.time.read / 1000)),
#                       ifdef COMPARE_WITH_BOOST_JSON
                            fmt(c.time.boostj_read / c.time.read), fmt(size / (c.time.boostj_read / 1000)),
#                       endif
#                       ifdef COMPARE_WITH_RAPIDJSON
                            fmt(c.time.rapidj_read / c.time.read), fmt(size / (c.time.rapidj_read / 1000)),
#                       endif
#                       ifdef COMPARE_WITH_NLOHMANN
                            fmt(c.time.nlohmann_read / c.time.read), fmt(size / (c.time.nlohmann_read / 1000)),
#                       endif
                        fmt(size / (c.time.write/ 1000)),
#                       ifdef COMPARE_WITH_BOOST_JSON
                            fmt(c.time.boostj_write / c.time.write), fmt(size / (c.time.boostj_write/ 1000)),
#                       endif
#                       ifdef COMPARE_WITH_RAPIDJSON
                            fmt(c.time.rapidj_write / c.time.write), fmt(size / (c.time.rapidj_write/ 1000)),
#                       endif
#                       ifdef COMPARE_WITH_NLOHMANN
                            fmt(c.time.nlohmann_write / c.time.write), fmt(size / (c.time.nlohmann_write/ 1000)),
#                       endif
                        fmt(size / (c.time.tidy_itr / 1000)),
                        fmt(size / (c.time.tidy_str / 1000))
                    });
                    total.read += size / (c.time.read / 1000);
                    total.write += size / (c.time.write / 1000);
                    total.tidy_itr += size / (c.time.tidy_itr / 1000);
                    total.tidy_str += size / (c.time.tidy_str / 1000);
#                   ifdef COMPARE_WITH_BOOST_JSON
                        total.boostj_read += size / (c.time.boostj_read / 1000);
                        total.boostj_write += size / (c.time.boostj_write / 1000);
#                   endif
#                   ifdef COMPARE_WITH_RAPIDJSON
                        total.rapidj_read += size / (c.time.rapidj_read / 1000);
                        total.rapidj_write += size / (c.time.rapidj_write / 1000);
#                   endif
#                   ifdef COMPARE_WITH_NLOHMANN
                        total.nlohmann_read += size / (c.time.nlohmann_read / 1000);
                        total.nlohmann_write += size / (c.time.nlohmann_write / 1000);
#                   endif
                }
            }
            {   // average
                tab.push_back({
                    "Average",
                    "",
                    fmt(total.read / time.size()),
#                   ifdef COMPARE_WITH_BOOST_JSON
                        fmt(total.read / total.boostj_read), fmt(total.boostj_read / time.size()),
#                   endif
#                   ifdef COMPARE_WITH_RAPIDJSON
                        fmt(total.read / total.rapidj_read), fmt(total.rapidj_read / time.size()),
#                   endif
#                   ifdef COMPARE_WITH_NLOHMANN
                        fmt(total.read / total.nlohmann_read), fmt(total.nlohmann_read / time.size()),
#                   endif
                    fmt(total.write/ time.size()),
#                   ifdef COMPARE_WITH_BOOST_JSON
                        fmt(total.write / total.boostj_write), fmt(total.boostj_write / time.size()),
#                   endif
#                   ifdef COMPARE_WITH_RAPIDJSON
                        fmt(total.write / total.rapidj_write), fmt(total.rapidj_write / time.size()),
#                   endif
#                   ifdef COMPARE_WITH_NLOHMANN
                        fmt(total.write / total.nlohmann_write), fmt(total.nlohmann_write / time.size()),
#                   endif
                    fmt(total.tidy_itr / time.size()),
                    fmt(total.tidy_str / time.size())
                });
            }
        }

        std::vector<size_t> wid(tab[0].size(), 0);
        {   // column width
            for (auto& r : tab)
                for (size_t i = 0, is = r.size(); i != is; ++i)
                    wid[i] = std::max(wid[i], r[i].length());
        }

        {   // print
            auto const line = [&wid]() {
                static auto const s = std::string(128, '-');
                std::fputc('+', stdout);
                for (auto s : wid)
                    std::fprintf(stdout, "-%s-+", std::string(s, '-').c_str());
                std::fputc('\n', stdout);
            };

            if (!diff.empty())
                std::fprintf(stdout, "\n");

            line();
            {   // header
                std::fprintf(stdout, "| %-*s |", (unsigned)wid[0], tab.front()[0].c_str());
                for (size_t i = 1, is = tab.front().size(); i != is; ++i)
                    std::fprintf(stdout, " %*s |", (unsigned)wid[i], tab.front()[i].c_str());
                std::fputc('\n', stdout);
            }
            line();
            {   // body
                for (size_t i = 1, is = tab.size() - 1; i != is; ++i) {
                    std::fprintf(stdout, "| %-*s |", (unsigned)wid[0], tab[i][0].c_str());
                    for (size_t j = 1, js = tab[i].size(); j != js; ++j)
                        std::fprintf(stdout, " %*s |", (unsigned)wid[j], tab[i][j].c_str());
                    std::fputc('\n', stdout);
                }
            }
            line();
            {   // average
                std::fprintf(stdout, "| %-*s |", (unsigned)wid[0], tab.back()[0].c_str());
                for (size_t i = 1, is = tab.back().size(); i != is; ++i)
                    std::fprintf(stdout, " %*s |", (unsigned)wid[i], tab.back()[i].c_str());
                std::fputc('\n', stdout);
            }
            line();
        }
    }

    return err;
}
