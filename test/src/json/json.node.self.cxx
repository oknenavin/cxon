// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_TIME_ONLY

#include "json.node.hxx"
#include "test.hxx"

#include "cxon/json.hxx"
#include "cxon/cbor.hxx"
#include "cxon/lang/json/tidy.hxx"
#include "cxon/lib/node.ordered.hxx"
#include "cxon/lib/std/list.hxx"

#include <unordered_set>
#include <unordered_map>

#if __cplusplus >= 201703L
#   if defined(__has_include) && __has_include(<memory_resource>)
#       include <memory_resource>
#       define CXON_HAS_LIB_STD_MEMORY_RESOURCE
#   endif
#endif


////////////////////////////////////////////////////////////////////////////////

namespace test { namespace kind {

    using node = cxon::json::ordered_node;

    struct my_traits : cxon::json::node_traits<> {
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

    template <typename Al = std::allocator<void>>
        struct unordered_node_traits : cxon::json::node_traits<Al> {
            template <typename K, typename V> using object_type = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, cxon::alc::rebind_t<Al, std::pair<const K, V>>>;
        };
    
    template <typename T>
        struct my_allocator : std::allocator<T> {
            using std::allocator<T>::allocator;
            my_allocator(int id = 0) : id(id) {}
            template <typename U>
                my_allocator(const my_allocator<U>& al) : id(al.id) {}
            template <class U>
                struct rebind { using other = my_allocator<U>; };
            int id;
        };

    using UK_JSON = cxon::JSON<cxon::test::unquoted_keys_traits<>>;
    using UQK_JSON = cxon::JSON<cxon::test::unquoted_quoted_keys_traits<>>;

    int self() {
        int a_ = 0;
        int f_ = 0;

#       define CHECK(...)\
            ++a_;\
            if (!(__VA_ARGS__))\
                std::fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__), std::fflush(stderr), ++f_, CXON_ASSERT(false, "check failed")

        {   // custom type binding + equal keys
            using node = cxon::json::basic_node<my_traits>;
            node n;
            cxon::from_bytes(n, R"({"k": 42, "k": 43})");
            CHECK(n.is<node::object>() && n.get<node::object>().count(u"k") == 2);
        }
        {   //using node = cxon::json::node;

            char const s[] = R"({"key0":[{"1":1},{"2":2}],"key1":["string",3,{"bool":true,"null":null},null],"key2":4})";
            node jns;
            {   // from/to string
                cxon::from_bytes(jns, s);
                {   std::string r;
                        cxon::to_bytes(r, jns);
                    CHECK(r == s);
                }
                {   std::string r;
                        cxon::to_bytes(std::back_inserter(r), jns);
                    CHECK(r == s);
                }
                {   char b[2];
                    auto o = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                        auto r = cxon::to_bytes(o, jns);
                    CHECK(r.ec == cxon::json::write_error::output_failure);
                }
                {   char b[8];
                    auto o = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                        auto r = cxon::to_bytes(o, jns);
                    CHECK(r.ec == cxon::json::write_error::output_failure);
                }
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
                    //cxon::to_bytes<cxon::JSON<>, cxon::json::ordered_node_traits<>>
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
                    cxon::from_bytes(v, R"([{"even": [2, 4, 6]}, {"odd": [1, 3, 5]}])");
                std::string s1;
                    cxon::to_bytes(cxon::json::make_indenter(s1, 2, ' '), v);
                std::string const s0 =
                    cxon::json::tidy(s1, 2, ' ');
                CHECK(s1 == s0);
            }
            {   std::map<std::string, node> m;
                    cxon::from_bytes(m, R"({"even": [2, 4, 6], "odd": [1, 3, 5]})");
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
                    //auto const r = cxon::to_bytes<cxon::JSON<>, cxon::json::ordered_node_traits<>>
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
                    auto const r = cxon::from_bytes<cxon::JSON<>, cxon::json::ordered_node_traits<>>
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
    
            char const s0[] = R"({"even":[2,4,6],"odd":[1,3,5]})";
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
                        3.14,       // real
                        3,          // signed
                        14U,        // unsigned
                        true,       // boolean
                        nullptr     // null
                    }
                },
                { "string", "string" }, // "key": value
                { "real", 3.14 },
                { "sint", 3 },
                { "uint", 14U },
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
                    o["real"] = 3.14;                   CHECK(o["real"].is<node::real>());
                    o["sint"] = 3;                      CHECK(o["sint"].is<node::sint>());
                    o["uint"] = 14U;                    CHECK(o["uint"].is<node::uint>());
                    o["boolean"] = false;               CHECK(o["boolean"].is<node::boolean>());
                    o["null"] = nullptr;                CHECK(o["null"].is<node::null>());
                auto& o1 = o["object"].get<node::object>(); // get value reference, the type is known
                    o1["object"] = 0;
                auto& a = o["array"].get<node::array>();    // get value reference, the type is known
                    a.push_back(node::object {});       CHECK(a.back().is<node::object>());
                    a.push_back(node::array {1, 2, 3}); CHECK(a.back().is<node::array>());
                    a.push_back("4");                   CHECK(a.back().is<node::string>());
                    a.push_back(3.14);                  CHECK(a.back().is<node::real>());
                    a.push_back(3);                     CHECK(a.back().is<node::sint>());
                    a.push_back(14U);                   CHECK(a.back().is<node::uint>());
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
            node n1(1);     CHECK(n1 == 1);
            node n2(1L);    CHECK(n2 == 1);
            node n3(1LL);   CHECK(n3 == 1);
            node n4(1U);    CHECK(n4 == 1U);
            node n5(1UL);   CHECK(n5 == 1U);
            node n6(1ULL);  CHECK(n6 == 1U);
            node n7(1.);    CHECK(n7 == 1.);
            node n8(1.F);   CHECK(n8 == 1.F);
        }
        {
            node n1; n1 = 1;    CHECK(n1 == 1);
            node n2; n2 = 1L;   CHECK(n2 == 1);
            node n3; n3 = 1LL;  CHECK(n3 == 1);
            node n4; n4 = 1U;   CHECK(n4 == 1U);
            node n5; n5 = 1UL;  CHECK(n5 == 1U);
            node n6; n6 = 1ULL; CHECK(n6 == 1U);
            node n7; n7 = 1.;   CHECK(n7 == 1.);
            node n8; n8 = 1.F;  CHECK(n8 == 1.F);
        }
        {   // ex5
            using node = cxon::json::node;
            {   // (1)
                node n; CHECK(n == nullptr);
            }
            {   // (2)
                node o = true; CHECK(o == true);
                node n(o); CHECK(n == true);
            }
            {   // (3)
                node n(42.0); CHECK(n == 42.0);
            }
            {   // (3)
                node::object const o = { {"key", "value"} };
                node n(o); CHECK(n == o);
            }
            {   // (3)
                node::array const a = { 1, "string" };
                node n(a); CHECK(n == a);
            }
            {   // (4)
                node n(3); CHECK(n == 3);
            }
            {   // (4)
                node n(14U); CHECK(n == 14U);
            }
            {   // (4)
                node n("string"); CHECK(n == "string");
            }
            {   // (5)
                node n({{1, 2}, {3, 4}}); CHECK(n == node::object {{1, 2}, {3, 4}});
            }
            {   // (5)
                node n({1, 2, 3, 4}); CHECK(n == node::array {1, 2, 3, 4});
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
            CHECK(n == "another");
        }
        {   // ex8
            cxon::json::node const n = "one";
                auto *v = n.get_if<cxon::json::node::string>(); CHECK(v != nullptr);
            CHECK(n.get_if<cxon::json::node::array>() == nullptr);
        }
        {   // ex9
            cxon::json::node n; CHECK(n.kind() == cxon::json::node_kind::null);
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
        {   // move construction with allocator
            using node = cxon::json::node;
            std::allocator<node> al;
            node n0 = node(node({{"42", 42}}), al); CHECK(n0 == node::object {{"42", 42}});
            node n1 = node(node({42, 42}), al); CHECK(n1 == node::array {42, 42});
            node n2 = node(node("42"), al); CHECK(n2 == node::string {"42"});
            node n3 = node(node(42.0), al); CHECK(n3 == 42.0);
            node n4 = node(node(42), al); CHECK(n4 == 42);
            node n5 = node(node(42U), al); CHECK(n5 == 42U);
            node n6 = node(node(true), al); CHECK(n6 == true);
            node n7 = node(node(nullptr), al); CHECK(n7 == nullptr);

            node n8  = node({{"42", 42}}, al); CHECK(n8 == node::object {{"42", 42}});
            node n9  = node({42, 42}, al); CHECK(n9 == node::array {42, 42});
            node n10 = node(node::string {"42"}, al); CHECK(node::string {"42"} == n10);
            node n11 = node(42.0, al); CHECK(n11 == 42.0);
            node n12 = node(42, al); CHECK(n12 == 42);
            node n13 = node(42U, al); CHECK(n13 == 42U);
            node n14 = node(true, al); CHECK(n14 == true);
            node n15 = node(nullptr, al); CHECK(n15 == nullptr);

            node n20 = node({{"42", 42}}, al); CHECK(n20 == node::object {{"42", 42}});
            node n21 = node("42", al); CHECK(n21 == "42");

            CHECK(n21.get_allocator() == al);
        }
        {   // move assignment
            using node = cxon::json::node;
            // different kind
            {   node a;
                a = node(42); CHECK(a == 42);
            }
            {   node a;
                a = node(42U); CHECK(a == 42U);
            }
            {   node a;
                a = node(42.0); CHECK(a == 42.0);
            }
            {   node a;
                a = node(nullptr); CHECK(a == nullptr);
            }
            {   node a;
                a = node(true); CHECK(a == true);
            }
            {   node a;
                a = node("42"); CHECK(a == "42");
            }
            {   node a;
                a = node(node::array {42}); CHECK(a == node::array {42});
            }
            {   node a;
                a = node(node::object {{"42", 42}}); CHECK(a == node::object {{"42", 42}});
            }
            // same kind
            {   node a = 24;
                a = node(42); CHECK(42 == a);
            }
            {   node a = 24U;
                a = node(42U); CHECK(42U == a);
            }
            {   node a = 24.0;
                a = node(42.0); CHECK(42.0 == a);
            }
            {   node a = nullptr;
                a = node(nullptr); CHECK(nullptr == a);
            }
            {   node a = false;
                a = node(true); CHECK(true == a);
            }
            {   node a = "24";
                a = node("42"); CHECK("42" == a);
            }
            {   node a = node::array {24};
                a = node(node::array {42}); CHECK(node::array {42} == a);
            }
            {   node a = {{"24", 24}};
                a = node(node::object {{"42", 42}}); CHECK(node::object {{"42", 42}} == a);
            }
        }
        {   // copy construction with allocator
            using node = cxon::json::node;
            std::allocator<node> al;
            node v0 = {{"42", 42}}, n0(v0, al); CHECK(n0 == node::object {{"42", 42}});
            node v1 = {42, 42}, n1(v1, al); CHECK(n1 == node::array {42, 42});
            node v2 = "42", n2(v2, al); CHECK(n2 == "42");
            node v3 = 42.0, n3(v3, al); CHECK(n3 == 42.0);
            node v4 = 42, n4(v4, al); CHECK(n4 == 42);
            node v5 = 42U, n5(v5, al); CHECK(n5 == 42U);
            node v6 = true, n6(v6, al); CHECK(n6 == true);
            node v7 = nullptr, n7(v7, al); CHECK(n7 == nullptr);

            auto v8  = node::object {{"42", 42}}; node n8(v8, al); CHECK(n8 == node::object {{"42", 42}});
            auto v9  = node::array {42, 42}; node n9(v9, al); CHECK(n9 == node::array {42, 42});
            auto v10 = node::string {"42"}; node n10(v10, al); CHECK(n10 == "42");
            auto v11 = 42.0; node n11(v11, al); CHECK(n11 == 42.0);
            auto v12 = 42; node n12(v12, al); CHECK(n12 == 42);
            auto v13 = 42U; node n13(v13, al); CHECK(n13 == 42U);
            auto v14 = true; node n14(v14, al); CHECK(n14 == true);
            auto v15 = nullptr; node n15(v15, al); CHECK(n15 == nullptr);
        }
        {   // copy assignment
            // different kind
            {   node a, b = 42;
                a = b; CHECK(a == 42);
            }
            {   node a, b = 42U;
                a = b; CHECK(a == 42U);
            }
            {   node a, b = 42.0;
                a = b;CHECK(a == 42.0);
            }
            {   node a = 42, b = nullptr;
                a = b; CHECK(a == nullptr);
            }
            {   node a, b = true;
                a = b; CHECK(a == true);
            }
            {   node a, b = "42";
                a = b; CHECK(a == "42");
            }
            {   node a = {24}, b = {42};
                a = b; CHECK(a == node::array {42});
            }
            {   node a = {{"42", 24}}, b = {{"42", 42}};
                a = b; CHECK(a == node::object {{"42", 42}});
            }
            // same kind
            {   node a = 24, b = 42;
                a = b; CHECK(42 == a);
            }
            {   node a = 24U, b = 42U;
                a = b; CHECK(42U == a);
            }
            {   node a = 24.0, b = 42.0;
                a = b;CHECK(42.0 == a);
            }
            {   node a = nullptr, b = nullptr;
                a = b; CHECK(nullptr == a);
            }
            {   node a = false, b = true;
                a = b; CHECK(true == a);
            }
            {   node a = "24", b = "42";
                a = b; CHECK("42" == a);
            }
            {   node a = {24}, b = {42};
                a = b; CHECK(node::array {42} == a);
            }
            {   node a = {{"24", 24}}, b = {{"42", 42}};
                a = b; CHECK(node::object {{"42", 42}} == a);
            }
        }
        {   // value comparison
            node n0 = {{1, 2}}; CHECK(n0 == node::object {{1, 2}} && node::object {{1, 2}} == n0 && n0 != node::array {1} && node::array {1} != n0 && n0 != node::object {{3, 4}} && node::object {{3, 4}} != n0);
            node n1 = {1, 2}; CHECK(n1 == node::array {1, 2} && node::array {1, 2} == n1 && n1 != 1 && 1 != n1 && n1 != node::array {3, 4} && node::array {3, 4} != n1);
            node n2 = "42"; CHECK(n2 == node::string {"42"} && node::string {"42"} == n2 && n2 != 1 && 1 != n2 && n2 != node::string {"24"} && node::string {"24"} != n2);
            node n21 = "42"; CHECK(n21 == "42" && "42" == n21 && n21 != 1 && 1 != n21 && n21 != "24" && "24" != n21);
            node n3 = 42.0; CHECK(n3 == 42.0 && 42.0 == n3 && n3 != 1 && 1 != n3 && n3 != 24.0 && 24.0 != n3);
            node n4 = 42; CHECK(n4 == 42 && 42 == n4 && n4 != 1.0 && 1.0 != n4 && n4 != 24 && 24 != n4);
            node n5 = 42U; CHECK(n5 == 42U && 42U == n5 && n5 != 1.0 && 1.0 != n5 && n5 != 24U && 24U != n5);
            node n6 = true; CHECK(n6 == true && true == n6 && n6 != 1 && 1 != n6 && n6 != false && false != n6);
            node n7 = nullptr; CHECK(n7 == nullptr && nullptr == n7 && n7 != 1 && 1 != n7);
        }
        {   // equal
            CHECK(node(node::object{{0, 0}}) == node(node::object{{0, 0}}));
            CHECK(!(node(node::object{{1, 0}}) == node(node::object{{0, 0}})));
            CHECK(!(node(node::object{{0, 0}}) == node(node::object{{0, 1}})));
            CHECK(!(node(node::object{{0, 0}}) != node(node::object{{0, 0}})));
            CHECK(node(node::object{{1, 0}}) != node(node::object{{0, 0}}));
            CHECK(node(node::object{{0, 0}}) != node(node::object{{0, 1}}));
        }
        {   // less than
            CHECK(node(node::object{}) < node(node::array{}));
            CHECK(node(node::object{{0, 0}}) < node(node::object{{1, 0}}));
            CHECK(!(node(node::object{{1, 0}}) < node(node::object{{0, 0}})));
            CHECK(node(node::object{{0, 0}}) < node(node::object{{0, 1}}));
            CHECK(!(node(node::object{{0, 0}}) < node(node::object{{0, 0}})));
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
        {   // swap
            using node = cxon::json::node;
            {   node f = {{"24", 24}}, s = {{"42", 42}};
                f.swap(s); CHECK(f == node {{"42", 42}} && s == node {{"24", 24}});
                f.swap(s); CHECK(f == node {{"24", 24}} && s == node {{"42", 42}});
            }
            {   node f = {{"42", 42}}, s = {42};
                f.swap(s); CHECK(f == node {42} && s == node {{"42", 42}});
                f.swap(s); CHECK(f == node {{"42", 42}} && s == node {42});
            }
            {   node f = {{"42", 42}}, s = "42";
                f.swap(s); CHECK(f == "42" && s == node {{"42", 42}});
                f.swap(s); CHECK(f == node {{"42", 42}} && s == "42");
            }
            {   node f = {{"42", 42}}, s = 42;
                f.swap(s); CHECK(f == 42 && s == node {{"42", 42}});
                f.swap(s); CHECK(f == node {{"42", 42}} && s == 42);
            }
            {   node f = {{"42", 42}}, s = 42U;
                f.swap(s); CHECK(f == 42U && s == node {{"42", 42}});
                f.swap(s); CHECK(f == node {{"42", 42}} && s == 42U);
            }
            {   node f = {{"42", 42}}, s = 42.0;
                f.swap(s); CHECK(f == 42.0 && s == node {{"42", 42}});
                f.swap(s); CHECK(f == node {{"42", 42}} && s == 42.0);
            }
            {   node f = {{"42", 42}}, s = true;
                f.swap(s); CHECK(f == true && s == node {{"42", 42}});
                f.swap(s); CHECK(f == node {{"42", 42}} && s == true);
            }
            {   node f = {{"42", 42}}, s = nullptr;
                f.swap(s); CHECK(f == nullptr && s == node {{"42", 42}});
                f.swap(s); CHECK(f == node {{"42", 42}} && s == nullptr);
            }
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
            CHECK(s == R"({"{1:2}":3,"[4]":5,"6":7,"8":9,"10":11,"12":13,"true":14,"null":15,"inf":16,"-inf":17,"nan":18})");
        }
        {   // unquoted keys
            using node = cxon::json::node;
            {
                {   char const in[] = R"({{1: 2}: 3, [4]: 5, "6": 7, -8: 9, 10: 11, 12.0: 13, true: 14, null: 15, "inf": 16, "-inf": 17})";
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
                        {-std::numeric_limits<node::real>::infinity(), 17U}
                    };
                    node n;
                        cxon::from_bytes<UK_JSON, cxon::json::node_traits<>>(n, in, cxon::node::json::extract_nans::set<true>());
                    CHECK(n == out);
                    std::string s;
                        cxon::to_bytes<UK_JSON>(s, n);
                    CHECK(s == R"({{1:2}:3,[4]:5,"6":7,"-inf":17,12:13,"inf":16,-8:9,10:11,true:14,null:15})");
                }
                // probably not a good idea to use NaNs as keys
                {   char const in[] = R"({"nan": 18})";
                    node const out  = { {std::numeric_limits<node::real>::quiet_NaN(), 18U} };
                    node n;
                        cxon::from_bytes<UK_JSON, cxon::json::node_traits<>>(n, in, cxon::node::json::extract_nans::set<true>());
                    CHECK(
                        n.is<node::object>() && n.get<node::object>().size() == 1 &&
                        (n.get<node::object>().begin()->first.is<node::real>() && std::isnan(n.get<node::object>().begin()->first.get<node::real>())) &&
                        n.get<node::object>().begin()->second == 18U
                    );
                }
            }
            {   node n; node const o = std::numeric_limits<node::real>::infinity();
                    cxon::from_bytes<UK_JSON>(n, R"("inf")", cxon::node::json::extract_nans::set<true>());
                CHECK(n == o);
            }
            {   node n;
                    auto const r = cxon::from_bytes<UK_JSON, cxon::json::node_traits<>>(n, "{\"x: 0}", cxon::node::json::extract_nans::set<true>());
                CHECK(!r && r.ec == cxon::json::read_error::unexpected);
            }
            {   node n;
                    auto const r = cxon::from_bytes<UK_JSON>(n, "{x: 0}");
                CHECK(!r && r.ec == cxon::node::error::invalid);
            }
            {
                node const n = node::object {{node::object {{1, 2}}, 3}, {node::array {4}, 5}, {"6", 7}, {8, 9}, {true, 10}, {nullptr, 11}};
                std::string s;
                    cxon::to_bytes<UK_JSON>(s, n);
                CHECK(s == R"({{1:2}:3,[4]:5,"6":7,8:9,true:10,null:11})");
            }
        }
        {   // cbor::node
            using node = cxon::cbor::node;
            {   char const in[] = R"({"a":[{"f":2},[3,4],"string",5,false,null],"b":"string","c":1,"d":true,"e":null})";
                node n;
                    cxon::from_bytes(n, in);
                std::string s;
                    cxon::to_bytes(s, n);
                CHECK(s == in);
            }
            {   char const in[] = R"({{1: 2}: 3, [4]: 5, "6": 7, -8: 9, 10: 11, 12.0: 13, true: 14, null: 15, "inf": 16, "-inf": 17})";
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
                    {-std::numeric_limits<node::real>::infinity(), 17U}
                };
                node n;
                    cxon::from_bytes<UK_JSON, cxon::cbor::node_traits<>>(n, in, cxon::node::json::extract_nans::set<true>());
                CHECK(n == out);
                std::string s;
                    cxon::to_bytes<UK_JSON>(s, n);
                CHECK(s == R"({{1:2}:3,[4]:5,"6":7,"-inf":17,12:13,"inf":16,-8:9,10:11,true:14,null:15})");
            }
            {   node n; node const o = std::numeric_limits<node::real>::infinity();
                    cxon::from_bytes<UK_JSON>(n, R"("inf")", cxon::node::json::extract_nans::set<true>());
                CHECK(n == o);
            }
            {   node n;
                    auto const r = cxon::from_bytes<UK_JSON>(n, "{x: 0}");
                CHECK(!r && r.ec == cxon::node::error::invalid);
            }
            {   char const in[] = "#[1]";
                node n;
                    auto r = cxon::from_bytes(n, in);
                CHECK(!r && r.ec == cxon::node::error::invalid);
            }
            {   node n = { 1, 2U, node::bytes {3}, "text", {4}, {{"5", 6}}, true, nullptr, node::undefined(), 7.0, node::simple(8) };
                std::string s;
                    cxon::to_bytes(s, n);
                CHECK(s == R"([1,2,[3],"text",[4],{"5":6},true,null,null,7,8])");
            }
            {   node n = {
                    { {{1, 0}}, 0 },
                    { {2}, 0 },
                    { node::tag {3, 4}, 0 },
                    { node::bytes {5}, 0 },
                    { "6", 0 },
                    { 7.0, 0 },
                    { 8, 0 },
                    { 9U, 0 },
                    { node::simple(10), 0 },
                    { true, 0 },
                    { nullptr, 0 },
                    { node::undefined(), 0 }
                };
                std::string s;
                    cxon::to_bytes(s, n);
                CHECK(s == R"({"{1:0}":0,"[2]":0,"4":0,"[5]":0,"6":0,"7":0,"8":0,"9":0,"10":0,"true":0,"null":0,"null":0})");
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
                std::string const fr = R"([{"1": 2}, [3, 4], "5, 6", 7, true, null])";
                std::string const to = R"([{"1":2},[3,4],"5, 6",7,true,null])";
                node n;
                    cxon::from_bytes(n, fr);
                std::string s;
                    cxon::to_bytes(s, n);
                CHECK(s == to);
            }
        }
        {   // errors with comments
            {   using COM = cxon::JSON<cxon::test::allow_comments_traits<>>;
                cxon::json::node n;
                    auto const r = cxon::from_bytes<COM>(n, "/{}");
                CHECK(r.ec == cxon::json::read_error::unexpected);
            }
            {   using COM = cxon::JSON<cxon::test::unquoted_quoted_keys_traits<cxon::test::allow_comments_traits<>>>;
                cxon::json::node n;
                    auto const r = cxon::from_bytes<COM>(n, "{1:2,/3:4}");
                CHECK(r.ec == cxon::json::read_error::unexpected);
            }
            {   using COM = cxon::JSON<cxon::test::unquoted_keys_traits<cxon::test::allow_comments_traits<>>>;
                cxon::json::node n;
                    auto const r = cxon::from_bytes<COM>(n, "{1:2,/3:4}");
                CHECK(r.ec == cxon::json::read_error::unexpected);
            }
            {   using COM = cxon::JSON<cxon::test::allow_comments_traits<>>;
                cxon::cbor::node n;
                    auto const r = cxon::from_bytes<COM>(n, "/{}");
                CHECK(r.ec == cxon::json::read_error::unexpected);
            }
            {   using COM = cxon::JSON<cxon::test::unquoted_quoted_keys_traits<cxon::test::allow_comments_traits<>>>;
                cxon::cbor::node n;
                    auto const r = cxon::from_bytes<COM>(n, "{1:2,/3:4}");
                CHECK(r.ec == cxon::json::read_error::unexpected);
            }
            {   using COM = cxon::JSON<cxon::test::unquoted_keys_traits<cxon::test::allow_comments_traits<>>>;
                cxon::cbor::node n;
                    auto const r = cxon::from_bytes<COM>(n, "{1:2,/3:4}");
                CHECK(r.ec == cxon::json::read_error::unexpected);
            }
        }
        {   // test the test (coverage)
            int t = 0; std::string const f = "\n";
            auto const r = cxon::from_bytes(t, f);
            auto const s = format_error(r, f.begin());
            CHECK(s == "cxon/json/read:(2,1): invalid integral or value out of range");
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
        {
            using node = cxon::json::basic_node<unordered_node_traits<>>;
            node n; n = { {1, 2} };
            CHECK(n.is<node::object>() && n.get<node::object>().size() == 1);
        }
        {
            using node = cxon::json::basic_node<cxon::json::node_traits<my_allocator<void>>>;
            my_allocator<node> al;
            node n(al);
                n = { 1, 2, 3};
            CHECK(n.is<node::array>() && n.get<node::array>().size() == 3);
        }
#       ifdef CXON_HAS_LIB_STD_MEMORY_RESOURCE
        {
            using node = cxon::json::basic_node<cxon::json::node_traits<std::pmr::polymorphic_allocator<void>>>;
            char bf[4096];
            std::pmr::monotonic_buffer_resource ar(bf, sizeof(bf));
            std::pmr::polymorphic_allocator<node> al(&ar);
            {   node n(al);
                    n = { 1, 2, 3};
                CHECK(n == node::array { 1, 2, 3});
            }
            {   node n0("42", al), n1(al);
                    n1 = std::move(n0);
                CHECK(n1 == "42");
            }
            {   node n0("42", al), n1;
                    n1 = std::move(n0);
                CHECK(n1 == "42");
            }
            {   node n0("42", al);
                node n1(std::move(n0), al);
                CHECK(n1 == "42");
            }
            {   node n0("42", al);
                node n1(std::move(n0));
                CHECK(n1 == "42");
            }
            {   node n(al);
                    auto const r = cxon::from_bytes(n, R"({"1": "2"})");
                CHECK(r && n == node::object {{"1", "2"}});
            }
            {   node n(al);
                    auto const r = cxon::from_bytes(n, R"({"1": [2, 3]})");
                CHECK(r && n == node::object {{"1", {2U, 3U}}});
            }
            {   node n(al);
                    auto const r = cxon::from_bytes(n, R"({"1": ["2", "3"]})");
                CHECK(r && n == node::object {{"1", {"2", "3"}}});
            }
            {   node n(al);
                    auto const r = cxon::from_bytes(n, R"({"1": {"2": 3.0}})");
                CHECK(r && n == node::object {{"1", {{"2", 3.0}}}});
            }
            // move assign
            //  same type
            {   node n0("42", al), n1("24", al);
                    n1 = std::move(n0);
                CHECK(n1 == "42");
            }
            {   node n0("42", al), n1("24");
                    n1 = std::move(n0);
                CHECK(n1 == "42");
            }
            {   node n0({1, 2, 3}, al), n1({3, 2, 1}, al);
                    n1 = std::move(n0);
                CHECK(n1 == node::array { 1, 2, 3});
            }
            {   node n0({1, 2, 3}, al), n1({3, 2, 1});
                    n1 = std::move(n0);
                CHECK(n1 == node::array { 1, 2, 3});
            }
            //  different type
            {   node n0("42", al), n1(24, al);
                    n1 = std::move(n0);
                CHECK(n1 == "42");
            }
            {   node n0("42", al), n1(24);
                    n1 = std::move(n0);
                CHECK(n1 == "42");
            }
            {   node n0({1, 2, 3}, al), n1(321, al);
                    n1 = std::move(n0);
                CHECK(n1 == node::array { 1, 2, 3});
            }
            {   node n0({1, 2, 3}, al), n1(321);
                    n1 = std::move(n0);
                CHECK(n1 == node::array { 1, 2, 3});
            }
        }
#       endif
        {   // alc::create_using_allocator_of
            using string = std::basic_string<char, std::char_traits<char>, my_allocator<char>>;
            my_allocator<string> al(42);
            std::vector<string, my_allocator<string>> of(al);
            auto x1 = cxon::alc::create_using_allocator_of<string>(of, "42");
            CHECK(of.get_allocator().id == x1.get_allocator().id);
            auto x2 = cxon::alc::create_using_allocator_of<std::tuple<string>>(of);
            CHECK(of.get_allocator().id == std::get<0>(x2).get_allocator().id);
            auto x3 = cxon::alc::create_using_allocator_of<my_type>(of);
            //CHECK(...);
            auto x4 = cxon::alc::create_using_allocator_of<std::pair<string, int>>(of, "42", 24);
            CHECK(of.get_allocator().id == x4.first.get_allocator().id && x4.first == "42" && x4.second == 24);
            auto x5 = cxon::alc::create_using_allocator_of<std::pair<string, string>>(
                of, std::piecewise_construct, std::forward_as_tuple("42"), std::forward_as_tuple(3U, 'x')
            );
            CHECK(of.get_allocator().id == x5.first.get_allocator().id && of.get_allocator().id == x5.second.get_allocator().id && x5.first == "42" && x5.second == "xxx");
        }
        {   // alc::uninitialized_construct_using_allocator
            using string = std::basic_string<char, std::char_traits<char>, my_allocator<char>>;
            using pair = std::pair<int, string>;
            typename cxon::aligned_union<pair>::type s;
            my_allocator<string> al(42);
            pair *p = cxon::alc::uninitialized_construct_using_allocator<pair>((pair*)&s, al, 42, "24");
            CHECK(al.id == p->second.get_allocator().id && p->first == 42 && p->second == "24");
        }
        {   // alc::uninitialized_construct_using_allocator
            using string = std::basic_string<char, std::char_traits<char>, my_allocator<char>>;
            using pair = std::pair<string, string>;
            typename cxon::aligned_union<pair>::type s;
            my_allocator<string> al(42);
            pair *p = cxon::alc::uninitialized_construct_using_allocator<pair>(
                (pair*)&s, al, std::piecewise_construct, std::forward_as_tuple("42"), std::forward_as_tuple(3U, 'x')
            );
            CHECK(al.id == p->first.get_allocator().id && al.id == p->second.get_allocator().id && p->first == "42" && p->second == "xxx");
        }
        {   // unquote quoted keys
            using node = cxon::json::node;
            {   std::string const s0 = R"({a:"b",c:"d"})";
                node n;         cxon::from_bytes<UQK_JSON>(n, s0);
                std::string s1; cxon::to_bytes<UQK_JSON>(s1, n);
                CHECK(s1 == s0);
            }
            {   std::string const s0 = R"({a:{b"c:{d\ e:"f"}},g\:h:"i"})";
                node n;         cxon::from_bytes<UQK_JSON>(n, s0);
                std::string s1; cxon::to_bytes<UQK_JSON>(s1, n);
                CHECK(s1 == s0);
            }
            {   std::string const s0 = R"({{a:{b:"c"}}:"d"})";
                node n;         cxon::from_bytes<UQK_JSON>(n, s0);
                std::string s1; cxon::to_bytes<UQK_JSON>(s1, n);
                CHECK(s1 == s0);
            }
            {   std::string const s0 = R"({["a",{b:"c"}]:"d"})";
                node n;         cxon::from_bytes<UQK_JSON>(n, s0);
                std::string s1; cxon::to_bytes<UQK_JSON>(s1, n);
                CHECK(s1 == s0);
            }
            {   std::string const s0 = R"({1:2})";
                node n;         cxon::from_bytes<UQK_JSON>(n, s0); CHECK(n.is<node::object>() && n.get<node::object>()["1"].is<node::uint>());
                std::string s1; cxon::to_bytes<UQK_JSON>(s1, n);
                CHECK(s1 == s0);
            }
            // cbor::node
            {   using node = cxon::cbor::node;
                {   std::string const s0 = R"({a:"b",c:"d"})";
                    node n;         cxon::from_bytes<UQK_JSON>(n, s0);
                    std::string s1; cxon::to_bytes<UQK_JSON>(s1, n);
                    CHECK(s1 == s0);
                }
                {   std::string const s0 = R"({a:{b"c:{d\ e:"f"}},g\:h:"i"})";
                    node n;         cxon::from_bytes<UQK_JSON>(n, s0);
                    std::string s1; cxon::to_bytes<UQK_JSON>(s1, n);
                    CHECK(s1 == s0);
                }
                {   std::string const s0 = R"({{a:{b:"c"}}:"d"})";
                    node n;         cxon::from_bytes<UQK_JSON>(n, s0);
                    std::string s1; cxon::to_bytes<UQK_JSON>(s1, n);
                    CHECK(s1 == s0);
                }
                {   std::string const s0 = R"({["a",{b:"c"}]:"d"})";
                    node n;         cxon::from_bytes<UQK_JSON>(n, s0);
                    std::string s1; cxon::to_bytes<UQK_JSON>(s1, n);
                    CHECK(s1 == s0);
                }
                {   std::string const s0 = R"({1:2})";
                    node n;         cxon::from_bytes<UQK_JSON>(n, s0); CHECK(n.is<node::map>() && n.get<node::map>()["1"].is<node::uint>());
                    std::string s1; cxon::to_bytes<UQK_JSON>(s1, n);
                    CHECK(s1 == s0);
                }
            }
        }
#       undef CHECK

        std::fprintf(stdout, "%-21s: %i of %4i failed\n", "cxon/json/node/self", f_, a_); std::fflush(stdout);

        return f_;
    }

}}

#endif // CXON_TIME_ONLY
