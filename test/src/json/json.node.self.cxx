// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_TIME_ONLY

#include "json.node.hxx"

#include "cxon/json.hxx"
#include "cxon/cbor.hxx"

#include "../utility.hxx"

#include "cxon/lib/node.ordered.hxx"
#include "cxon/lib/std/list.hxx"

#include "cxon/lang/json/tidy.hxx"

#include <unordered_set>


////////////////////////////////////////////////////////////////////////////////

using node = cxon::json::ordered_node;

namespace test { namespace kind {

    struct my_traits : cxon::json::node_traits {
        using                               string_type = std::u16string;
        template <class T> using            array_type = std::list<T>;
        template <class K, class V> using   object_type = std::multimap<K, V>;
    };

    struct my_type {
        std::vector<int> even;
        std::list<int> odd;
        bool operator ==(const my_type& v) const { return even == v.even && odd == v.odd; }
        CXON_JSON_CLS_MEMBER(my_type,
            CXON_JSON_CLS_FIELD_ASIS(even),
            CXON_JSON_CLS_FIELD_ASIS(odd)
        )
    };

    int self() {
        int a_ = 0;
        int f_ = 0;

#       define CHECK(c)\
            ++a_;\
            if (!(c))\
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
#                   if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                    cxon::to_bytes(cxon::json::make_indenter(s1, 4, ' '), n, cxon::json::fp_precision::set<4>());
#                   else
                    // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                    // seems to be fixed around 10, but after the inclusion of cbor.hxx,
                    // this workaround does not work for to_bytes anymore
                    //cxon::to_bytes<cxon::JSON<>, cxon::json::ordered_node_traits>
                    //    (cxon::json::make_indenter(s1, 4, ' '), n, cxon::json::fp_precision::set<4>());
#                   endif
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
#                   if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                    auto const r = cxon::to_bytes(cxon::json::make_indenter(s), n, cxon::node::recursion_depth::set<4>());
                    CHECK(!r && r.ec == cxon::node::error::recursion_depth_exceeded);
#                   else
                    // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                    // seems to be fixed around 10, but after the inclusion of cbor.hxx,
                    // this workaround does not work for to_bytes anymore
                    //auto const r = cxon::to_bytes<cxon::JSON<>, cxon::json::ordered_node_traits>
                    //                    (cxon::json::make_indenter(s), n, cxon::node::recursion_depth::set<4>());
                    //CHECK(!r && r.ec == cxon::node::error::recursion_depth_exceeded);
#                   endif
            }
            {   node jn;
                auto const r = cxon::from_bytes(jn, "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[");
                CHECK(!r && r.ec == cxon::node::error::recursion_depth_exceeded);
            }
            {   node jn;
#                   if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                    auto const r = cxon::from_bytes(jn, "[[[[", cxon::node::recursion_depth::set<4>());
#                   else
                    // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                    // seems to be fixed around 10, after the inclusion of cbor.hxx,
                    // funnily enough, this workaround continues to work for from_chars (unlike to_chars)
                    auto const r = cxon::from_bytes<cxon::JSON<>, cxon::json::ordered_node_traits>
                                        (jn, "[[[[", cxon::node::recursion_depth::set<4>());
#                   endif
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
            {   // (5)
                node n({{1, 2}, {3, 4}}); CHECK(n.is<node::object>() && n.get<node::object>() == (node::object {{1, 2}, {3, 4}}));
            }
            {   // (5)
                node n({1, 2, 3, 4}); CHECK(n.is<node::array>() && n.get<node::array>() == (node::array {1, 2, 3, 4}));
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
#                   if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                    cxon::to_bytes(s, n, cxon::node::json::arbitrary_keys::set<true>());
                    CHECK(s == "{{1:2}:3,[4]:5,\"6\":7,8:9,true:10,null:11}");
#                   else
                    // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                    // seems to be fixed around 10, but after the inclusion of cbor.hxx,
                    // this workaround does not work for to_bytes anymore
                    //cxon::to_bytes<cxon::JSON<>, cxon::json::ordered_node_traits>
                    //    (s, n, cxon::node::json::arbitrary_keys::set<true>());
                    //CHECK(s == "{{1:2}:3,[4]:5,\"6\":7,8:9,true:10,null:11}");
#                   endif
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
        {   // test the test (coverage)
            int t = 0; std::string const f = "";
            auto const r = cxon::from_bytes(t, f);
            auto const s = format_error(r, f.begin());
            CHECK(s == "cxon/json/read:0: invalid integral or value out of range");
        }
        {   // hash
            using node = cxon::json::node;
            std::unordered_set<node> s = {
                node::object {{1, 2}, {3, 4}},
                node::array {5, 6},
                "7",
                8.,
                9,
                10U,
                true,
                node::null {},
                node::null {} // duplicate
            };
            CHECK(s.size() == 8);
            std::unordered_multiset<node> m = {
                node::object {{1, 2}, {3, 4}},
                node::array {5, 6},
                "7",
                8.,
                9,
                10U,
                true,
                node::null {},
                node::null {} // duplicate
            };
            CHECK(m.size() == 9);
        }
#       undef CHECK

        std::fprintf(stdout, "cxon/json/node/self:  %i of %3i failed\n", f_, a_); std::fflush(stdout);

        return f_;
    }

}}

#endif // CXON_TIME_ONLY
