// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "cxon/json.hxx"

#include "cxon/lib/node.ordered.hxx"
#include "cxon/lib/std/list.hxx"
#include "cxon/lib/std/string.hxx"

#include "cxon/lang/json/tidy.hxx"

#include "../utility.hxx"

#include <fstream>
#include <cstring>
#include <unordered_set>
#include <unordered_map>

#if __cplusplus >= 201703L
#   if defined(__has_include) && __has_include(<memory_resource>)
#       include <memory_resource>
#       define CXON_HAS_LIB_STD_MEMORY_RESOURCE
#   endif
#endif


////////////////////////////////////////////////////////////////////////////////

//struct my_traits : cxon::cbor::node_traits<> {
//    using                               bytes_type = std::vector;
//    template <class T> using            array_type = std::list<T>;
//    template <class K, class V> using   map_type = std::multimap<K, V>;
//};

struct fixture {
    struct fix {
        std::string act;
        std::string data;
        std::string fail;

        CXON_JSON_CLS_READ_MEMBER(fix,
            CXON_JSON_CLS_FIELD_ASIS(act),
            CXON_JSON_CLS_FIELD_ASIS(data),
            CXON_JSON_CLS_FIELD_ASIS(fail)
        )
    };
    std::string type;
    std::vector<std::string> in;
    std::map<std::string, fix> fix;

    CXON_JSON_CLS_READ_MEMBER(fixture,
        CXON_JSON_CLS_FIELD_ASIS(type),
        CXON_JSON_CLS_FIELD_ASIS(in),
        CXON_JSON_CLS_FIELD_ASIS(fix)
    )
};

struct result {
    int err = 0;
    int all = 0;
};

template <typename Al = std::allocator<void>>
    struct unordered_node_traits : cxon::cbor::node_traits<Al> {
        template <typename K, typename V> using map_type = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, cxon::alc::rebind_t<Al, std::pair<const K, V>>>;
    };

template <typename T>
    struct my_allocator : std::allocator<T> {
        using std::allocator<T>::allocator;
    };

#ifndef CXON_TIME_ONLY
    static unsigned execute_self() {
        unsigned a_ = 0;
        unsigned f_ = 0;

#       define CHECK(...)\
            ++a_;\
            if (!(__VA_ARGS__))\
                fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__), ++f_, CXON_ASSERT(false, "check failed")

        using node = cxon::cbor::node;

        {
            {   node n; // default 64
                auto const r = cxon::from_bytes(n,  "\x81\x81\x81\x81\x81\x81\x81\x81"
                                                    "\x81\x81\x81\x81\x81\x81\x81\x81"
                                                    "\x81\x81\x81\x81\x81\x81\x81\x81"
                                                    "\x81\x81\x81\x81\x81\x81\x81\x81"
                                                    "\x81\x81\x81\x81\x81\x81\x81\x81"
                                                    "\x81\x81\x81\x81\x81\x81\x81\x81"
                                                    "\x81\x81\x81\x81\x81\x81\x81\x81"
                                                    "\x81\x81\x81\x81\x81\x81\x81\x81"
                );
                CHECK(!r && r.ec == cxon::node::error::recursion_depth_exceeded);
            }
            {   node n;
#               if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                    auto const r = cxon::from_bytes(n, "\x81\x81\x81\x81", cxon::node::recursion_depth::set<4>());
#               else
                    // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                    // seems to be fixed around 10
                    auto const r = cxon::from_bytes<cxon::CBOR<>, cxon::cbor::node_traits<>>(n, "\x81\x81\x81\x81", cxon::node::recursion_depth::set<4>());
#               endif
                CHECK(!r && r.ec == cxon::node::error::recursion_depth_exceeded);
            }
            {   node n;
                    auto const r = cxon::from_bytes(n, "\xFF");
                CHECK(!r && r.ec == cxon::node::error::invalid);
            }
            {   node n;
                    auto const r = cxon::from_bytes(n, "\xFE");
                CHECK(!r && r.ec == cxon::cbor::read_error::unexpected);
            }
            {   node n = node::simple {0x14}; std::string s;
                    auto r = cxon::to_bytes(s, n);
                CHECK(!r && r.ec == cxon::cbor::write_error::argument_invalid);
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
        {   // ex3
            unsigned char const b0[] = "\xA2\144even\x82\x02\x04\x63odd\x82\x01\x03";
            node const n0 = {
                { "even", { 2U, 4U } },
                { "odd", { 1U, 3U } }
            };

            node n1; // read
                cxon::from_bytes(n1, b0);
            CHECK(n1 == n0);

            std::basic_string<unsigned char> b1; // write
                cxon::to_bytes(b1, n0);
            CHECK(b1 == b0);
        }
        {   // tags
            node n1;
                auto const r1 = cxon::from_bytes(n1, "\xC1\x83\x01\x02\x03");
            CHECK(r1 && n1 == node::tag {1, node::array {1U, 2U, 3U}});

            node n2;
                auto const r2 = cxon::from_bytes(n2, "\xC1\xC2\x83\x01\x02\x03");
            CHECK(r2 && n2 == node::tag {1, node::tag {2, node::array {1U, 2U, 3U}}});

            node n3;
                auto const r3 = cxon::from_bytes(n3, "\xDC\x83\x01\x02\x03");
            CHECK(!r3 && r3.ec == cxon::cbor::read_error::tag_invalid);
        }
        {   // ex4
            // build using initializer lists
            node n1 = {
                -1,                         // sint
                1U,                         // uint
                node::bytes {0x01, 0x02},   // bytes
                "text",                     // text
                {1, 2, 3},                  // array
                {                           // map
                    { 1, 1 }, // { key, value }
                    { 1U, 1 },
                    { node::bytes {0x01, 0x02}, 1 },
                    { "text", 1 },
                    { {1, 2, 3}, 1 },
                    { {{1, 2}, {3, 4}}, 1 },
                    { node::tag {1, 2}, 1 },
                    { false, 1 },
                    { nullptr, 1 },
                    { node::undefined {}, 1 },
                    { 1.0, 1 },
                    { node::simple {16}, 1 }
                },
                node::tag {1, 2},           // tag
                true,                       // bool
                false,                      // bool
                nullptr,                    // null
                node::undefined {},         // undefined
                1.0,                        // real
                node::simple {24}           // simple
            };

            // build using node's methods
            node n2;
                CHECK(n2.is<node::undefined>());    // default node type is node_kind::undefined
                auto& a = n2.imbue<node::array>();  // change the type and return its value
                    CHECK(n2.is<node::array>());
                    a.push_back(-1);                        CHECK(a.back().is<node::sint>());
                    a.push_back(node::uint(1));             CHECK(a.back().is<node::uint>());
                    a.push_back(node::bytes {0x01, 0x02});  CHECK(a.back().is<node::bytes>());
                    a.push_back("text");                    CHECK(a.back().is<node::text>());
                    a.push_back(node:: array {});           CHECK(a.back().is<node::array>());
                    auto* a1 = a.back().get_if<node::array>();
                        a1->push_back(1);                   CHECK(a1->back().is<node::sint>());
                        a1->push_back(2);                   CHECK(a1->back().is<node::sint>());
                        a1->push_back(3);                   CHECK(a1->back().is<node::sint>());
                    //a.push_back(node({}));
                    a.push_back(node::map {});              CHECK(a.back().is<node::map>());
                    auto& m = a.back().get<node::map>();
                        m[1] = 1;                           CHECK(m.find(node::sint(1)) != m.end());
                        m[node::uint(1)] = 1;               CHECK(m.find(node::uint(1)) != m.end());
                        m[node::bytes {0x01, 0x02}] = 1;    CHECK(m.find(node::bytes {0x01, 0x02}) != m.end());
                        m["text"] = 1;                      CHECK(m.find(node::text("text")) != m.end());
                        m[{1, 2, 3}] = 1;                   CHECK(m.find(node::array({1, 2, 3})) != m.end());
                        m[{{1, 2}, {3, 4}}] = 1;            CHECK(m.find(node::map({{1, 2}, {3, 4}})) != m.end());
                        m[node::tag {1, 2}] = 1;            CHECK(m.find(node::tag(1, 2)) != m.end());
                        m[false] = 1;                       CHECK(m.find(node::boolean(false)) != m.end());
                        m[nullptr] = 1;                     CHECK(m.find(node::null(nullptr)) != m.end());
                        m[node::undefined {}] = 1;          CHECK(m.find(node::undefined {}) != m.end());
                        m[1.0] = 1;                         CHECK(m.find(node::real(1.0)) != m.end());
                        m[node::simple {16}] = 1;           CHECK(m.find(node::simple {16}) != m.end());
                    a.push_back(node::tag(1, 2));           CHECK(a.back().is<node::tag>());
                    a.push_back(true);                      CHECK(a.back().is<node::boolean>());
                    a.push_back(false);                     CHECK(a.back().is<node::boolean>());
                    a.push_back(nullptr);                   CHECK(a.back().is<node::null>());
                    a.push_back(node::undefined {});        CHECK(a.back().is<node::undefined>());
                    a.push_back(1.0);                       CHECK(a.back().is<node::real>());
                    a.push_back(node::simple {24});         CHECK(a.back().is<node::simple>());

            CHECK(n1 == n2);

            std::vector<unsigned char> b1;
                cxon::to_bytes(b1, n1);
            std::vector<unsigned char> b2;
                cxon::to_bytes(b2, n2);
            CHECK(b1 == b2);

            node n3;
                cxon::from_bytes(n3, b2);
            node n4;
                cxon::from_bytes(n4, b1);
            CHECK(n4 == n3);
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
            {
                node n(42); CHECK(n == 42);
            }
            {
                node n(42U); CHECK(n == 42U);
            }
            {
                node n(node::bytes {0x01, 0x02}); CHECK(n == node::bytes {0x01, 0x02});
            }
            {
                node n("string"); CHECK(n == "string");
            }
            {
                node::array const o = { 1, "string" };
                node n(o); CHECK(n == o);
            }
            {
                node::map const o = { {42, "value"} };
                node n(o); CHECK(n == o);
            }
            {
                node::map const o = { {42U, "value"} };
                node n(o); CHECK(n == o);
            }
            {
                node::map const o = { {node::bytes {'k','e','y'}, "value"} };
                node n(o); CHECK(n == o);
            }
            {
                node::map const o = { {"key", "value"} };
                node n(o); CHECK(n == o);
            }
            {
                node::tag const o = { 1, "value" };
                node n(o); CHECK(n == o);
            }
            {
                node::map const o = { {0.0, "value"} };
                node n(o); CHECK(n == o);
            }
            {
                node::map const o = { {true, "value"} };
                node n(o); CHECK(n == o);
            }
            {
                node::map const o = { {nullptr, "value"} };
                node n(o); CHECK(n == o);
            }
            {
                node::map const o = { {node::undefined {}, "value"} };
                node n(o); CHECK(n == o);
            }
            {
                node n(42.0); CHECK(n == 42.0);
            }
            {
                node::map const o = { {node::simple {255}, "value"} };
                node n(o); CHECK(n == o);
            }
            {
                node o = true; CHECK(o.is<node::boolean>());
                node n(o); CHECK(n == true);
            }
            {
                node n; CHECK(n == node::undefined {});
            }
            {
                node n({{1, 2}, {3, 4}}); CHECK(n == node::map {{1, 2}, {3, 4}});
            }
            {
                node n({1, 2, 3, 4}); CHECK(n == node::array {1, 2, 3, 4});
            }
        }
        {   // ex6
            {   // T is the same
                node n = "string";
                node::text& v = n.imbue<node::text>(); CHECK(v == "string");
            }
            {   // T is not the same
                node n = "string";
                node::real& v = n.imbue<node::real>(); CHECK(v == 0.0);
            }
        }
        {   // ex7
            node n = "one";
                n.get<node::text>() = "another";
            CHECK(n == "another");
        }
        {   // ex8
            node const n = "one";
                auto *v = n.get_if<node::text>(); CHECK(v != nullptr);
            CHECK(n.get_if<node::array>() == nullptr);
        }
        {   // ex9
            node const n; CHECK(n.is<node::undefined>());
        }
        {   // ex10
            {   node a = 42, b;
                b = a; CHECK(a == b);
            }
            {   node a = 42U, b;
                b = a; CHECK(a == b);
            }
            {   node a = node::bytes {'b','y','t','e','s'}, b;
                b = a; CHECK(a == b);
            }
            {   node a = "text", b;
                b = a; CHECK(a == b);
            }
            {   node a = node::array {4, 2}, b;
                b = a; CHECK(a == b);
            }
            {   node a = node::map {{"q", "?"}, {"a", 42}}, b;
                b = a; CHECK(a == b);
            }
            {   node a = node::tag {42, "?"}, b;
                b = a; CHECK(a == b);
            }
            {   node a = true, b;
                b = a; CHECK(a == b);
            }
            {   node a = nullptr, b;
                b = a; CHECK(a == b);
            }
            {   node a = node::undefined {}, b;
                b = a; CHECK(a == b);
            }
            {   node a = 42., b;
                b = a; CHECK(a == b);
            }
            {   node a = node::simple {}, b;
                b = a; CHECK(a == b);
            }
        }
        {   // move construction with allocator
            std::allocator<node> al;
            {
                node n0  = node(node({{"42", 42}}), al); CHECK(n0 == node::map {{"42", 42}});
                node n1  = node(node({42, 42}), al); CHECK(n1 == node::array {42, 42});
                node n2  = node(node(node::bytes {42, 42}), al); CHECK(n2 == node::bytes {42, 42});
                node n3  = node(node("42"), al); CHECK(n3 == node::text {"42"});
                node n4  = node(node(node::tag {1, 2}), al); CHECK(n4 == node::tag {1, 2});
                node n5  = node(node(42.0), al); CHECK(n5 == 42.0);
                node n6  = node(node(42), al); CHECK(n6 == 42);
                node n7  = node(node(42U), al); CHECK(n7 == 42U);
                node n8  = node(node(node::simple {42}), al); CHECK(n8 == node::simple {42});
                node n9  = node(node(true), al); CHECK(n9 == true);
                node n10 = node(node(nullptr), al); CHECK(n10 == nullptr);
                node n11 = node(node(node::undefined {}), al); CHECK(n11 == node::undefined {});
            }
            {
                node n0  = node({{"42", 42}}, al); CHECK(n0 == node::map {{"42", 42}});
                node n1  = node({42, 42}, al); CHECK(n1 == node::array {42, 42});
                node n2  = node(node::bytes {42, 42}, al); CHECK(n2 == node::bytes {42, 42});
                node n3  = node(node::text {"42"}, al); CHECK(node::text {"42"} == n3);
                node n4  = node(node::tag {1, 2}, al); CHECK(n4 == node::tag {1, 2});
                node n5  = node(42.0, al); CHECK(n5 == 42.0);
                node n6  = node(42, al); CHECK(n6 == 42);
                node n7  = node(42U, al); CHECK(n7 == 42U);
                node n8  = node(node::simple {42}, al); CHECK(n8 == node::simple {42});
                node n9  = node(true, al); CHECK(n9 == true);
                node n10 = node(nullptr, al); CHECK(n10 == nullptr);
                node n11 = node(node::undefined {}, al); CHECK(n11 == node::undefined {});
            }
            node n0 = node("42", al); CHECK(n0 == "42");
            CHECK(n0.get_allocator() == al);
        }
        {   // move assignment
            // different kind
            {   node a;
                a = node(1); CHECK(a == 1);
            }
            {   node a;
                a = node(1U); CHECK(a == 1U);
            }
            {   node a;
                a = node(node::bytes {0x01}); CHECK(a == node::bytes {0x01});
            }
            {   node a;
                a = node("x"); CHECK(a == "x");
            }
            {   node a;
                a = node(node::array {1}); CHECK(a == node::array {1});
            }
            {   node a;
                a = node(node::map {{1, 2}}); CHECK(a == node::map {{1, 2}});
            }
            {   node a;
                a = node(node::tag {1, 2}); CHECK(a == node::tag {1, 2});
            }
            {   node a;
                a = node(true); CHECK(a == true);
            }
            {   node a;
                a = node(nullptr); CHECK(a == nullptr);
            }
            {   node a;
                a = node(node::undefined {}); CHECK(a == node::undefined {});
            }
            {   node a;
                a = node(1.0); CHECK(a == 1.0);
            }
            {   node a;
                a = node(node::simple {}); CHECK(a == node::simple {0});
            }
            // same kind
            {   node a = 24;
                a = node(42); CHECK(42 == a);
            }
            {   node a = 24U;
                a = node(42U); CHECK(42U == a);
            }
            {   node a = 0x18;
                a = node(node::bytes {0x2A}); CHECK(node::bytes {0x2A} == a);
            }
            {   node a = "24";
                a = node("42"); CHECK("42" == a);
            }
            {   node a = {24};
                a = node(node::array {42}); CHECK(node::array {42} == a);
            }
            {   node a = {{"24", 24}};
                a = node(node::map {{"42", 42}}); CHECK(node::map {{"42", 42}} == a);
            }
            {   node a = node::tag {24, 24};
                a = node(node::tag {42, 42}); CHECK(node::tag {42, 42} == a);
            }
            {   node a = false;
                a = node(true); CHECK(true == a);
            }
            {   node a = nullptr;
                a = node(nullptr); CHECK(nullptr == a);
            }
            {   node a = node::undefined {};
                a = node(node::undefined {}); CHECK(node::undefined {} == a);
            }
            {   node a = 24.0;
                a = node(42.0); CHECK(42.0 == a);
            }
            {   node a = node::simple {24};
                a = node(node::simple {42}); CHECK(node::simple {42} == a);
            }
        }
        {   // copy construction with allocator
            std::allocator<node> al;
            {
                node v0  = node({{"42", 42}}), n0(v0, al); CHECK(n0 == node::map {{"42", 42}});
                node v1  = node({42, 42}), n1(v1, al); CHECK(n1 == node::array {42, 42});
                node v2  = node(node::bytes {42, 42}), n2(v2, al); CHECK(n2 == node::bytes {42, 42});
                node v3  = node("42"), n3(v3, al); CHECK(n3 == node::text {"42"});
                node v4  = node(node::tag {1, 2}), n4(v4, al); CHECK(n4 == node::tag {1, 2});
                node v5  = node(42.0), n5(v5, al); CHECK(n5 == 42.0);
                node v6  = node(42), n6(v6, al); CHECK(n6 == 42);
                node v7  = node(42U), n7(v7, al); CHECK(n7 == 42U);
                node v8  = node(node::simple {42}), n8(v8, al); CHECK(n8 == node::simple {42});
                node v9  = node(true), n9(v9, al); CHECK(n9 == true);
                node v10 = node(nullptr), n10(v10, al); CHECK(n10 == nullptr);
                node v11 = node(node::undefined {}), n11(v11, al); CHECK(n11 == node::undefined {});
            }
            {
                auto v0  = node::map {{"42", 42}}; node n0(v0, al); CHECK(n0 == node::map {{"42", 42}});
                auto v1  = node::array {42, 42}; node n1(v1, al); CHECK(n1 == node::array {42, 42});
                auto v2  = node::bytes {42, 42}; node n2(v2, al); CHECK(n2 == node::bytes {42, 42});
                auto v3  = node::text {"42"}; node n3(v3, al); CHECK(node::text {"42"} == n3);
                auto v4  = node::tag {1, 2}; node n4(v4, al); CHECK(n4 == node::tag {1, 2});
                auto v5  = 42.0; node n5(v5, al); CHECK(n5 == 42.0);
                auto v6  = 42; node n6(v6, al); CHECK(n6 == 42);
                auto v7  = 42U; node n7(v7, al); CHECK(n7 == 42U);
                auto v8  = node::simple {42}; node n8(v8, al); CHECK(n8 == node::simple {42});
                auto v9  = true; node n9(v9, al); CHECK(n9 == true);
                auto v10 = nullptr; node n10(v10, al); CHECK(n10 == nullptr);
                auto v11 = node::undefined {}; node n11(v11, al); CHECK(n11 == node::undefined {});
            }
        }
        {   // copy assignment
            // different kind
            {   node a, b = 42;
                a = b; CHECK(a == 42);
            }
            {   node a, b = 42U;
                a = b; CHECK(a == 42U);
            }
            {   node a, b = node::bytes {0x2A};
                a = b; CHECK(a == node::bytes {0x2A});
            }
            {   node a, b = "42";
                a = b; CHECK(a == "42");
            }
            {   node a, b = {42};
                a = b; CHECK(a == node::array {42});
            }
            {   node a, b = {{"42", 42}};
                a = b; CHECK(a == node::map {{"42", 42}});
            }
            {   node a, b = node::tag {42, "42"};
                a = b; CHECK(a == node::tag {42, "42"});
            }
            {   node a, b = true;
                a = b; CHECK(a == true);
            }
            {   node a, b = nullptr;
                a = b; CHECK(a == nullptr);
            }
            {   node a = 42, b = node::undefined {};
                a = b; CHECK(a == node::undefined {});
            }
            {   node a, b = 42.0;
                a = b; CHECK(a == 42.0);
            }
            {   node a, b = node::simple {42};
            a = b; CHECK(a == node::simple {42});
            }
            // same kind
            {   node a = 24, b = 42;
                a = b; CHECK(a == 42);
            }
            {   node a = 24U, b = 42U;
                a = b; CHECK(a == 42U);
            }
            {   node a = node::bytes {0x18}, b = node::bytes {0x2A};
                a = b; CHECK(a == node::bytes {0x2A});
            }
            {   node a = "24", b = "42";
                a = b; CHECK(a == "42");
            }
            {   node a = {24}, b = {42};
                a = b; CHECK(a == node::array {42});
            }
            {   node a = {{"24", 24}}, b = {{"42", 42}};
                a = b; CHECK(a == node::map {{"42", 42}});
            }
            {   node a = node::tag {24, "24"}, b = node::tag {42, "42"};
                a = b; CHECK(a == node::tag {42, "42"});
            }
            {   node a = false, b = true;
                a = b; CHECK(a == true);
            }
            {   node a = nullptr, b = nullptr;
                a = b; CHECK(a == nullptr);
            }
            {   node a = node::undefined {}, b = node::undefined {};
                a = b; CHECK(a == node::undefined {});
            }
            {   node a = 24.0, b = 42.0;
                a = b; CHECK(a == 42.0);
            }
            {   node a = node::simple {24}, b = node::simple {42};
                a = b; CHECK(a == node::simple {42});
            }
            // value
            node n; n = "42"; CHECK(n == "42");
        }
        {   // swap
            using node = cxon::cbor::node;
            {   node f = 24, s = 42;
                f.swap(s); CHECK(f == 42 && s == 24);
                f.swap(s); CHECK(f == 24 && s == 42);
            }
            {   node f = 24, s = 42U;
                f.swap(s); CHECK(f == 42U && s == 24);
                f.swap(s); CHECK(f == 24 && s == 42U);
            }
            {   node f = 24, s = node::bytes {0x2A};
                f.swap(s); CHECK(f == node::bytes {0x2A} && s == 24);
                f.swap(s); CHECK(f == 24 && s == node::bytes {0x2A});
            }
            {   node f = 24, s = "42";
                f.swap(s); CHECK(f == "42" && s == 24);
                f.swap(s); CHECK(f == 24 && s == "42");
            }
            {   node f = 24, s = {42};
                f.swap(s); CHECK(f == node::array {42} && s == 24);
                f.swap(s); CHECK(f == 24 && s == node::array {42});
            }
            {   node f = 24, s = {{"42", 42}};
                f.swap(s); CHECK(f == node::map {{"42", 42}} && s == 24);
                f.swap(s); CHECK(f == 24 && s == node::map {{"42", 42}});
            }
            {   node f = 24, s = node::tag {42, "42"};
                f.swap(s); CHECK(f == node::tag {42, "42"} && s == 24);
                f.swap(s); CHECK(f == 24 && s == node::tag {42, "42"});
            }
            {   node f = 24, s = true;
                f.swap(s); CHECK(f == true && s == 24);
                f.swap(s); CHECK(f == 24 && s == true);
            }
            {   node f = 24, s = nullptr;
                f.swap(s); CHECK(f == nullptr && s == 24);
                f.swap(s); CHECK(f == 24 && s == nullptr);
            }
            {   node f = 24, s = node::undefined {};
                f.swap(s); CHECK(f == node::undefined {} && s == 24);
                f.swap(s); CHECK(f == 24 && s == node::undefined {});
            }
            {   node f = 24, s = 42.0;
                f.swap(s); CHECK(f == 42.0 && s == 24);
                f.swap(s); CHECK(f == 24 && s == 42.0);
            }
            {   node f = 24, s = node::simple {42};
                f.swap(s); CHECK(f == node::simple {42} && s == 24);
                f.swap(s); CHECK(f == 24 && s == node::simple {42});
            }
        }
        {   // value comparison
            node n0 = {{1, 2}}; CHECK(n0 == node::map {{1, 2}} && node::map {{1, 2}} == n0 && n0 != node::array {1} && node::array {1} != n0 && n0 != node::map {{3, 4}} && node::map {{3, 4}} != n0);
            node n1 = {1, 2}; CHECK(n1 == node::array {1, 2} && node::array {1, 2} == n1 && n1 != 1 && 1 != n1 && n1 != node::array {3, 4} && node::array {3, 4} != n1);
            node n2 = node::bytes {1, 2}; CHECK(n2 == node::bytes {1, 2} && node::bytes {1, 2} == n2 && n2 != 1 && 1 != n2 && n2 != node::bytes {3, 4} && node::bytes {3, 4} != n2);
            node n3 = node::text {"42"}; CHECK(n3 == node::text {"42"} && node::text {"42"} == n3 && n3 != 1 && 1 != n3 && n3 != node::text {"24"} && node::text {"24"} != n3);
            node n4 = "42"; CHECK(n4 == "42" && "42" == n4 && n4 != 1 && 1 != n4 && n4 != "24" && "24" != n4);
            node n5 = node::tag {1, 2}; CHECK(n5 == node::tag {1, 2} && node::tag {1, 2} == n5 && n5 != 5 && 1 != n5 && n5 != node::tag {3, 4} && node::tag {3, 4} != n5);
            node n6 = 42.0; CHECK(n6 == 42.0 && 42.0 == n6 && n6 != 1 && 1 != n6 && n6 != 24.0 && 24.0 != n6);
            node n7 = 42; CHECK(n7 == 42 && 42 == n7 && n7 != 1.0 && 1.0 != n7 && n7 != 24 && 24 != n7);
            node n8 = 42U; CHECK(n8 == 42U && 42U == n8 && n8 != 1.0 && 1.0 != n8 && n8 != 24U && 24U != n8);
            node n9 = node::simple {42}; CHECK(n9 == node::simple {42} && node::simple {42} == n9 && n9 != 1 && 1 != n9 && n9 != node::simple {24}&& node::simple {24} != n9);
            node n10 = true; CHECK(n10 == true && true == n10 && n10 != 1 && 1 != n10 && n10 != false && false != n10);
            node n11 = nullptr; CHECK(n11 == nullptr && nullptr == n11 && n11 != 1 && 1 != n11);
            node n12 = node::undefined {}; CHECK(n12 == node::undefined {} && node::undefined {} == n12 && n12 != 1 && 1 != n12);
        }
        {   // taggle
            using node = cxon::cbor::node;
            using taggle = cxon::cbor::taggle<int, node>;
            taggle t0;                      CHECK(t0.tag = -1 && t0.value.is<node::undefined>());
            taggle t1 = {42, "42"};         CHECK(t1.tag = 42 && t1.value == "42");
            taggle t2 = {42, t1.value};     CHECK(t2.tag = 42 && t2.value == "42");
            taggle t3 = std::move(t2);      CHECK(t3.tag = 42 && t3.value == "42");
            taggle t4; t4 = std::move(t3);  CHECK(t4.tag = 42 && t4.value == "42");
            taggle t5 = t4;                 CHECK(t5.tag = 42 && t5.value == "42");
            taggle t6; t6 = t5;             CHECK(t6.tag = 42 && t6.value == "42");
            CHECK(taggle {42, "42"} == taggle {42, "42"});
            CHECK(taggle {24, "42"} != taggle {42, "42"});
            CHECK(taggle {42, "24"} != taggle {42, "42"});
            CHECK(taggle {24, "42"}  < taggle {42, "42"});
            CHECK(taggle {42, "24"}  < taggle {42, "42"});
        }
        {   // json::node
            using node = cxon::json::node;
            {   node n;
                    cxon::from_bytes(n, "\x82\xF5\x66string");
                std::string s;
                    cxon::to_bytes(s, n);
                CHECK(s == "\x82\xF5\x66string");
            }
            {   node n1;
                    cxon::from_bytes(n1, "\x9F\x01\x21\x41\x03\x61\x34\x81\x05\xA1\x61\x36\x07\xC1\x08\xF5\xF6\xF7\xFA\x00\x00\x00\x00\x09\xFF");
                node n2 = {1U, -2, {3U}, "4", {5U}, {{"6", 7U}}, 8U, true, nullptr, nullptr, 0.0, 9U };
                CHECK(n2 == n1);
            }
            {   node n;
                    auto r = cxon::from_bytes(n, "\xDC");
                CHECK(!r && r.ec == cxon::cbor::read_error::tag_invalid);
            }
            {   node n;
                    auto r = cxon::from_bytes(n, "\xFF");
                CHECK(!r && r.ec == cxon::node::error::invalid);
            }
            {   node n = { {{"1", 2.0}}, {3.0}, "4", 5.0, true, nullptr };
                std::string s1;
                    cxon::to_bytes(s1, n);
                char const s2[] = "\x86\xA1\x61\x31\xFB\x40\x00\x00\x00\x00\x00\x00\x00\x81\xFB\x40\x08\x00\x00\x00\x00\x00\x00\x61\x34\xFB\x40\x14\x00\x00\x00\x00\x00\x00\xF5\xF6";
                CHECK(std::memcmp(s1.c_str(), s2, sizeof(s2)) == 0);
            }
            {   node n1;
                    cxon::from_bytes(n1, "\xA1\x01\x02");
                node n2 = {{1U, 2U}};
                CHECK(n2 == n1);
            }
        }
        {   // round-trip
            using node = cxon::json::node;
            {   // json::node => cbor => json::node
                node const fr = { {{"1", 2 }, {3 , 4 }}, {5 , 6 }, "7, 8", 9 , true, nullptr };
                node const to = { {{"1", 2U}, {3U, 4U}}, {5U, 6U}, "7, 8", 9U, true, nullptr };
                std::vector<unsigned char> s;
                    cxon::to_bytes(s, fr);
                node n;
                    cxon::from_bytes(n, s);
                CHECK(n == to);
            }
            {   // cbor => json::node => cbor
                // node::array {
                //      -1,
                //      2U,
                //      node::bytes {  3,   4},
                //      "5, 6",
                //      node::array {  7,   8},
                //      node::map {{  9,   10}, {"11",   12}},
                //      node::tag {13, 14},
                //      true,
                //      nullptr,
                //      node::undefined {},
                //      15.0,
                //      node::simple {}
                //};
                char const fr[] =   "\x8C"
                                    "\x20"
                                    "\x02"
                                    "\x42\x03\x04"
                                    "\x64\x35\x2C\x20\x36"
                                    "\x82\x07\x08"
                                    "\xA2\x09\x0A\x62\x31\x31\x0C"
                                    "\xCD\x0E"
                                    "\xF5"
                                    "\xF6"
                                    "\xF7"
                                    "\xFB\x40\x2E\x00\x00\x00\x00\x00\x00"
                                    "\xE0";
                char const to[] =   "\x8C"
                                    "\x20"
                                    "\x02"
                                    "\x82\x03\x04"
                                    "\x64\x35\x2C\x20\x36"
                                    "\x82\x07\x08"
                                    "\xA2\x62\x31\x31\x0C\x09\x0A"
                                    "\x0E"
                                    "\xF5"
                                    "\xF6"
                                    "\xF6"
                                    "\xFB\x40\x2E\x00\x00\x00\x00\x00\x00"
                                    "\x00";
                node n;
                    cxon::from_bytes(n, fr);
                std::vector<unsigned char> s;
                    cxon::to_bytes(s, n);
                CHECK(std::memcmp(s.data(), to, sizeof(to) - 1) == 0);
            }
        }
        {   // hash
            using node = cxon::cbor::node;
            std::unordered_set<node> s = {
                {{1, 2}, {3, 4}},
                {5, 6},
                node::bytes {0x07, 0x08},
                node::tag {9, 10},
                node::simple {11},
                "12",
                13.,
                14,
                15U,
                true,
                nullptr,
                node::undefined {},
                node::undefined {} // duplicate
            };
            CHECK(s.size() == 12);
            std::unordered_multiset<node> m = {
                {{1, 2}, {3, 4}},
                {5, 6},
                node::bytes {0x07, 0x08},
                node::tag {9, 10},
                node::simple {11},
                "12",
                13.,
                14,
                15U,
                true,
                nullptr,
                node::undefined {},
                node::undefined {} // duplicate
            };
            CHECK(m.size() == 13);
        }
        {
            using node = cxon::cbor::basic_node<unordered_node_traits<>>;
            node n; n = { {1, 2} };
            CHECK(n.is<node::map>() && n.get<node::map>().size() == 1);
        }
        {
            using node = cxon::cbor::basic_node<cxon::cbor::node_traits<my_allocator<void>>>;
            my_allocator<node> al;
            node n(al);
                n = { 1, 2, 3};
            CHECK(n.is<node::array>() && n.get<node::array>().size() == 3);
        }
#       ifdef CXON_HAS_LIB_STD_MEMORY_RESOURCE
        {
            using node = cxon::cbor::basic_node<cxon::cbor::node_traits<std::pmr::polymorphic_allocator<void>>>;
            char bf[4096];
            std::pmr::monotonic_buffer_resource ar(bf, sizeof(bf));
            std::pmr::polymorphic_allocator<node> al(&ar);
            node n(al);
                n = { 1, 2, 3};
            CHECK(n.is<node::array>() && n.get<node::array>().size() == 3);
        }
#       endif
#       undef CHECK
    
        fprintf(stdout, "cxon/cbor/node/self:  %u of %3u failed\n", f_, a_); fflush(stdout);

        return f_;
    }

    namespace test_vector
    {

        struct test {
            std::string hex;
            bool roundtrip;
            cxon::json::node decoded;
            std::string diagnostic;

            using vector = std::vector<test>;

            CXON_JSON_CLS_READ_MEMBER(test,
                CXON_JSON_CLS_FIELD_SKIP("cbor"),
                CXON_JSON_CLS_FIELD_ASIS(hex),
                CXON_JSON_CLS_FIELD_ASIS(roundtrip),
                CXON_JSON_CLS_FIELD_ASIS(decoded),
                CXON_JSON_CLS_FIELD_ASIS(diagnostic)
            )

            std::string bin() const {
                return hex.length() % 2 == 0 ?
                    hex2bin(hex) :
                    std::string {}
                ;
            }

            static std::string hex2bin(const std::string& hex) {
                static constexpr unsigned char hex_[256] = {
                    /*  0*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    /* 16*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    /* 32*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    /* 48*/ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
                    /* 64*/ 0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    /* 80*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    /* 96*/ 0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0
                };
                CXON_ASSERT(hex.length() % 2 == 0, "invalid input");
                std::string bin;
                    for (std::size_t i = 0, is = hex.length(); i != is; i += 2) {
                        char const b = char((hex_[(unsigned)hex[i]] << 4) + hex_[(unsigned)hex[i + 1]]);
                        bin += b;
                    }
                return bin;
            }
        };

        static result execute(const fixture& fixture) {
            CXON_ASSERT(fixture.type == "test-vector", "unexpected");

            result res; int skip = 0;

            for (auto& file: fixture.in) {
                std::ifstream is(file);
                    if (!is) {
                        CXON_ASSERT(0, "unexpected");
                        ++res.err, fprintf(stderr, "error: cannot be opened: '%s'\n", file.c_str());
                        continue;
                    }
                test::vector vector;
                    auto const r = cxon::from_bytes<cxon::JSON<>>(vector, std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                    if (!r) {
                        CXON_ASSERT(0, "unexpected");
                        ++res.err, fprintf(stderr, "error: test-vector invalid: '%s': %s\n", file.c_str(), r.ec.message().c_str());
                        continue;
                    }

                for (auto& test : vector) {
                    ++res.all;
                    cxon::json::node decoded;
                    std::string fail;
                    {   auto const fix = fixture.fix.find(test.hex);
                        if (fix != fixture.fix.end()) {
                            if (fix->second.act == "json") {
#                               if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                                    auto const r = cxon::from_bytes<cxon::JSON<>>(
                                        decoded, fix->second.data,
                                        cxon::node::json::arbitrary_keys::set<true>(),
                                        cxon::node::json::extract_nans::set<true>()
                                    );
#                               else
                                    // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                                    // seems to be fixed around 10
                                    auto const r = cxon::from_bytes<cxon::JSON<>, cxon::json::node_traits<>>(
                                        decoded, fix->second.data,
                                        cxon::node::json::arbitrary_keys::set<true>(),
                                        cxon::node::json::extract_nans::set<true>()
                                    );
#                               endif
                                CXON_ASSERT(r, "invalid fixture");
                            }
                            else if (fix->second.act == "skip") {
                                ++skip/*, fprintf(stdout, "skip: '%s' (%s)\n", test.hex.c_str(), fix->second.data.c_str()), fflush(stdout)*/;
                                continue;
                            }
                            fail = fix->second.fail;
                        }
                        else {
                            decoded = test.decoded;
                        }
                    }
                    {   cxon::json::node json;
                            auto const r = cxon::from_bytes(json, test.bin());
                        if (!r) {
                            fail.empty() ?
                                (++res.err, fprintf(stderr, "fail: '%s'\n", test.hex.c_str())) :
                                (/*fprintf(stderr, "must fail: '%s' (%s)\n", test.hex.c_str(), fail.c_str()), */0)
                            ;
                        }
                        else if (json != decoded) {
                            fail.empty() ?
                                (++res.err, fprintf(stderr, "fail: '%s'\n", test.hex.c_str())) :
                                (/*fprintf(stderr, "must fail: '%s' (%s)\n", test.hex.c_str(), fail.c_str()), */0)
                            ;
                        }
                        else if (!fail.empty()) {
                            ++res.err, fprintf(stderr, "must fail but passed: '%s' (%s)\n", test.hex.c_str(), fail.c_str());
                        }
                    }
                }
            }

            fprintf(stdout, "cxon/cbor/node/suite: %d of %3d failed (%d skipped)\n", res.err, res.all, skip); fflush(stdout);

            return res;
        }

    }

    namespace round_trip
    {

        static result execute(const fixture& fixture) {
            CXON_ASSERT(fixture.type == "round-trip", "unexpected");
        
            using JSON = cxon::JSON<>;
            using CBOR = cxon::CBOR<>;

            static auto const name = [](const std::string& p) {
                auto f = p.find_last_of("/\\"); f = f != p.npos ? f + 1 : 0;
                auto l = p.rfind('.'); if (l == p.npos) l = p.size();
                return std::string(p, f, l - f);
            };

            result res;

            for (auto& file: fixture.in) {
                ++res.all;
                std::ifstream is(file);
                    if (!is) {
                        CXON_ASSERT(0, "unexpected");
                        ++res.err, fprintf(stderr, "error: cannot be opened: '%s'\n", file.c_str());
                        continue;
                    }
                std::string const f0 = name(file) + ".ct(0).json";
                std::string const f1 = name(file) + ".ct(1).json";
                {   // 0
                    std::ofstream os(f0, std::ofstream::binary);
                        if (!os) {
                            ++res.err, fprintf(stderr, "error: cannot be opened: %s\n", f0.c_str());
                            continue;
                        }
                    cxon::json::tidy<JSON>(
                        std::ostreambuf_iterator<char>(os),
                        std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()
                    );
                }
                {   // json => cbor => json
                    is.seekg(0);
                    cxon::cbor::ordered_node n0;
                        {   auto const r = cxon::from_bytes<JSON>(n0, std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                            if (!r) {
                                ++res.err, fprintf(stderr, "error: %s: %s\n", r.ec.category().name(), r.ec.message().c_str());
                                continue;
                            }
                        }
                    std::string cbor;
                        {   auto const r = cxon::to_bytes<CBOR>(cbor, n0);
                            if (!r) {
                                ++res.err, fprintf(stderr, "error: %s: %s\n", r.ec.category().name(), r.ec.message().c_str());
                                continue;
                            }
                        }
                    cxon::json::ordered_node n1;
                        {   auto const r = cxon::from_bytes<CBOR>(n1, cbor);
                            if (!r) {
                                ++res.err, fprintf(stderr, "error: %s: %s\n", r.ec.category().name(), r.ec.message().c_str());
                                continue;
                            }
                        }
                    {   // 1
                        std::ofstream os(f1, std::ofstream::binary);
                        if (!os) {
                            ++res.err, fprintf(stderr, "error: cannot be opened: %s\n", f1.c_str());
                            continue;
                        }
                        auto const r = cxon::to_bytes<JSON>(cxon::json::make_indenter<JSON>(std::ostreambuf_iterator<char>(os)), n1);
                        if (!r) {
                            ++res.err, fprintf(stderr, "error: %s: %s\n", r.ec.category().name(), r.ec.message().c_str());
                            continue;
                        }
                    }
                    std::fprintf(stdout, "%s %s ", f0.c_str(), f1.c_str());
                }
            }

            return res;
        }

    }
#endif

namespace timing
{

    struct test_time {
        std::string source;
        std::string error;
        std::size_t size = 0;
        double read = 0;
        double write = 0;
    };

    static result execute(const fixture& fixture) {
        CXON_ASSERT(fixture.type == "timing", "unexpected");
        
        using JSON = cxon::JSON<>;
        using CBOR = cxon::CBOR<>;

        std::vector<test_time> time;

        for (auto& file: fixture.in) {
            test_time t;

            std::string cbor;
            {   // JSON => CBOR
                std::ifstream is(file);
                    if (!is) {
                        CXON_ASSERT(0, "unexpected");
                        t.error = "error: cannot be opened: ''" + file + "'";
                        continue;
                    }
                cxon::cbor::ordered_node n;
                {   auto const r = cxon::from_bytes<JSON>(n, std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                    if (!r) {
                        t.error = r.ec.category().name() + std::string(": ") + r.ec.message();
                        continue;
                    }
                }
                {   auto const r = cxon::to_bytes<CBOR>(cbor, n);
                    if (!r) {
                        t.error = r.ec.category().name() + std::string(": ") + r.ec.message();
                        continue;
                    }
                }
            }
            {   // time
                t.source = file;
                t.size = cbor.size();
                // read
                    std::vector<cxon::cbor::ordered_node> vo;
                    t.read = CXON_MEASURE(
                        vo.emplace_back();
                        auto const r = cxon::from_bytes(vo.back(), cbor);
                        if (!r) t.error = test::format_error(r, cbor.cbegin());
                    );
                // write
                    auto o = vo.back(); vo.clear();
                    std::vector<std::string> vs;
                    t.write = CXON_MEASURE(
                        vs.emplace_back();
                        cxon::to_bytes(vs.back(), o);
                    );
            }
            time.push_back(t);
        }

        {   // print
            std::vector<std::vector<std::string>> tab;
            {   // build the table
                static auto const fmt = [](double d) -> std::string {
                    char b[64];
                    int const r = std::snprintf(b, sizeof(b), "%.2f", d); CXON_ASSERT( r > 0 && r < (int)sizeof(b), "unexpected");
                    return b;
                };
                {   // header
                    tab.push_back({"CBOR/File", "Size", "Read", "Write"});
                }
                test_time total;
                {   // body
                    for (auto& t : time) {
                        if (!t.error.empty()) {
                            std::fprintf(stderr, "%s: %s\n", t.source.c_str(), t.error.c_str());
                            continue;
                        }
                        double const size = double(t.size) / (1024. * 1024);
                        tab.push_back({
                            t.source.substr(t.source.rfind('/') + 1),
                            fmt(size),
                            fmt(size / (t.read / 1000)),
                            fmt(size / (t.write/ 1000))
                        });
                        total.read += size / (t.read / 1000),
                        total.write += size / (t.write / 1000);
                    }
                }
                {   // average
                    tab.push_back({
                        "Average",
                        "",
                        fmt(total.read / time.size()),
                        fmt(total.write/ time.size())
                    });
                }
            }

            std::vector<std::size_t> wid(tab[0].size(), 0);
            {   // column width
                for (auto& r : tab)
                    for (std::size_t i = 0, is = r.size(); i != is; ++i)
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

                line();
                {   // header
                    std::fprintf(stdout, "| %-*s |", (unsigned)wid[0], tab.front()[0].c_str());
                    for (std::size_t i = 1, is = tab.front().size(); i != is; ++i)
                        std::fprintf(stdout, " %*s |", (unsigned)wid[i], tab.front()[i].c_str());
                    std::fputc('\n', stdout);
                }
                line();
                {   // body
                    for (std::size_t i = 1, is = tab.size() - 1; i != is; ++i) {
                        std::fprintf(stdout, "| %-*s |", (unsigned)wid[0], tab[i][0].c_str());
                        for (std::size_t j = 1, js = tab[i].size(); j != js; ++j)
                            std::fprintf(stdout, " %*s |", (unsigned)wid[j], tab[i][j].c_str());
                        std::fputc('\n', stdout);
                    }
                }
                line();
                {   // average
                    std::fprintf(stdout, "| %-*s |", (unsigned)wid[0], tab.back()[0].c_str());
                    for (std::size_t i = 1, is = tab.back().size(); i != is; ++i)
                        std::fprintf(stdout, " %*s |", (unsigned)wid[i], tab.back()[i].c_str());
                    std::fputc('\n', stdout);
                }
                line();
            }
        }

        result res;
            for (auto& t : time)
                ++res.all, res.err += !t.error.empty();
        return res;
    }

}

int main(int argc, char *argv[]) {
    if (argc == 1) {
#       ifndef CXON_TIME_ONLY
            return execute_self();
#       else
            return -1;
#       endif
    }

    result res;
    bool has_diff = false;

    for (int i = 1; i != argc; ++i) {
        std::ifstream is(argv[i]);
            if (!is) {
                CXON_ASSERT(0, "unexpected");
                ++res.err, fprintf(stderr, "cannot be opened: '%s'\n", argv[i]);
                continue;
            }
        fixture fixture;
            auto const r = cxon::from_bytes<cxon::JSON<>>(fixture, std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            if (!r) {
                CXON_ASSERT(0, "unexpected");
                ++res.err, fprintf(stderr, "error: fixture invalid: '%s': %s\n", argv[i], r.ec.message().c_str());
                continue;
            }

#       ifndef CXON_TIME_ONLY
            if (fixture.type == "test-vector") {
                result const r = test_vector::execute(fixture);
                res.all += r.all, res.err += r.err;
            }
            else if (fixture.type == "round-trip") {
                has_diff = !fixture.in.empty();
                result const r = round_trip::execute(fixture);
                res.all += r.all, res.err += r.err;
            }
            else
#       endif
        if (fixture.type == "timing") {
            if (!fixture.in.empty() && has_diff)
                std::fprintf(stdout, "\n");
            result const r = timing::execute(fixture);
            res.all += r.all, res.err += r.err;
        }
        else {
            CXON_ASSERT(0, "unexpected");
            ++res.err, fprintf(stderr, "error: fixture type invalid: '%s'\n", argv[i]);
            continue;
        }
    }

    return res.err;
}
