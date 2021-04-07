

// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "cxon/json.hxx"

#include "cxon/lib/node.ordered.hxx"
#include "cxon/lib/std/list.hxx"

#include "cxon/lang/json/tidy.hxx"

#include <fstream>
#include <chrono>
#include <cstring>


////////////////////////////////////////////////////////////////////////////////

//struct my_traits : cxon::cbor::node_traits {
//    using                               bytes_type = std::vector;
//    template <class T> using            array_type = std::list<T>;
//    template <class K, class V> using   map_type = std::multimap<K, V>;
//};

static unsigned execute_self() {
    unsigned a_ = 0;
    unsigned f_ = 0;

#   define CHECK(c) ++a_; if (!(c))\
        fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__), ++f_;\
        CXON_ASSERT((c), "check failed");

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
#           if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                auto const r = cxon::from_bytes(n, "\x81\x81\x81\x81", cxon::node::recursion_depth::set<4>());
#           else
                // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                // seems to be fixed around 10
                auto const r = cxon::from_bytes<cxon::CBOR<>, cxon::cbor::node_traits>(n, "\x81\x81\x81\x81", cxon::node::recursion_depth::set<4>());
#           endif
            CHECK(!r && r.ec == cxon::node::error::recursion_depth_exceeded);
        }
        {   node n;
            auto const r = cxon::from_bytes(n, "\xFF");
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
        node const n0 = node::map {
            { "even", node::array { 2U, 4U } },
            { "odd", node::array { 1U, 3U } }
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
        CHECK(r1 && n1.is<node::tag>() && n1.get<node::tag>().tag == 1 && n1.get<node::tag>().value == (node::array {1U, 2U, 3U}));

        node n2;
            auto const r2 = cxon::from_bytes(n2, "\xC1\xC2\x83\x01\x02\x03");
        CHECK(
            r2 &&
            n2.is<node::tag>() &&
            n2.get<node::tag>().tag == 1 &&
            n2.get<node::tag>().value.is<node::tag>() &&
            n2.get<node::tag>().value.get<node::tag>().tag == 2 &&
            n2.get<node::tag>().value.get<node::tag>().value == (node::array {1U, 2U, 3U})
        );

        node n3;
            auto const r3 = cxon::from_bytes(n3, "\xDC\x83\x01\x02\x03");
        CHECK(!r3 && r3.ec == cxon::cbor::read_error::tag_invalid);
    }
    {   // ex4
        // build using initializer lists
        node n1 = node::array {
            -1,                         // sint
            node::uint(1),              // uint
            node::bytes {0x01, 0x02},   // bytes
            "text",                     // text
            node::array {1, 2, 3},      // array
            node::map {                 // map
                { 1, 1 }, // { key, value }
                { node::uint(1), 1 },
                { node::bytes {0x01, 0x02}, 1 },
                { "text", 1 },
                { node::array {1, 2, 3}, 1 },
                { node::map {{1, 2}, {3, 4}}, 1 },
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
                a.push_back(node:: map {});             CHECK(a.back().is<node::map>());
                auto& m = a.back().get<node::map>();
                    m[1] = 1;                           CHECK(m.find(node::sint(1)) != m.end());
                    m[node::uint(1)] = 1;               CHECK(m.find(node::uint(1)) != m.end());
                    m[node::bytes {0x01, 0x02}] = 1;    CHECK(m.find(node::bytes {0x01, 0x02}) != m.end());
                    m["text"] = 1;                      CHECK(m.find(node::text("text")) != m.end());
                    m[node::array {1, 2, 3}] = 1;       CHECK(m.find(node::array({1, 2, 3})) != m.end());
                    m[node::map {{1, 2}, {3, 4}}] = 1;  CHECK(m.find(node::map({{1, 2}, {3, 4}})) != m.end());
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
        {
            node n(42); CHECK(n.is<node::sint>() && n.get<node::sint>() == 42);
        }
        {
            node n(42U); CHECK(n.is<node::uint>() && n.get<node::uint>() == 42U);
        }
        {
            node n(node::bytes {0x01, 0x02}); CHECK(n.is<node::bytes>() && n.get<node::bytes>() == (node::bytes {0x01, 0x02}));
        }
        {
            node n("string"); CHECK(n.is<node::text>() && n.get<node::text>() == "string");
        }
        {
            node::array const a = { 1, "string" };
            node n(a); CHECK(n.is<node::array>() && n.get<node::array>() == a);
        }
        {
            node::map const o = { {42, "value"} };
            node n(o); CHECK(n.is<node::map>() && n.get<node::map>() == o);
        }
        {
            node::map const o = { {42U, "value"} };
            node n(o); CHECK(n.is<node::map>() && n.get<node::map>() == o);
        }
        {
            node::map const o = { {node::bytes {'k','e','y'}, "value"} };
            node n(o); CHECK(n.is<node::map>() && n.get<node::map>() == o);
        }
        {
            node::map const o = { {"key", "value"} };
            node n(o); CHECK(n.is<node::map>() && n.get<node::map>() == o);
        }
        {
            node::tag const o = { 1, "value" };
            node n(o); CHECK(n.is<node::tag>() && n.get<node::tag>() == o);
        }
        {
            node::map const o = { {0.0, "value"} };
            node n(o); CHECK(n.is<node::map>() && n.get<node::map>() == o);
        }
        {
            node::map const o = { {true, "value"} };
            node n(o); CHECK(n.is<node::map>() && n.get<node::map>() == o);
        }
        {
            node::map const o = { {nullptr, "value"} };
            node n(o); CHECK(n.is<node::map>() && n.get<node::map>() == o);
        }
        {
            node::map const o = { {node::undefined {}, "value"} };
            node n(o); CHECK(n.is<node::map>() && n.get<node::map>() == o);
        }
        {
            node n(42.0); CHECK(n.is<node::real>() && n.get<node::real>() == 42.0);
        }
        {
            node::map const o = { {node::simple {255}, "value"} };
            node n(o); CHECK(n.is<node::map>() && n.get<node::map>() == o);
        }
        {
            node o = true; CHECK(o.is<node::boolean>());
            node n(o); CHECK(n.is<node::boolean>() && n.get<node::boolean>());
        }
        {
            node n; CHECK(n.is<node::undefined>());
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
        CHECK(n.get<node::text>() == "another");
    }
    {   // ex8
        node n = "one";
            auto *v = n.get_if<node::text>(); CHECK(v != nullptr);
        CHECK(n.get_if<node::array>() == nullptr);
    }
    {   // ex9
        node const n; CHECK(n.kind() == cxon::cbor::node_kind::undefined);
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
    {   // move assignment
        {   node a;
            a = node(1); CHECK(a.is<node::sint>() && a.get<node::sint>() == 1);
        }
        {   node a;
            a = node(1U); CHECK(a.is<node::uint>() && a.get<node::uint>() == 1);
        }
        {   node a;
            a = node(node::bytes {0x01}); CHECK(a.is<node::bytes>() && a.get<node::bytes>() == (node::bytes {0x01}));
        }
        {   node a;
            a = node("x"); CHECK(a.is<node::text>() && a.get<node::text>() == "x");
        }
        {   node a;
            a = node(node::array {1}); CHECK(a.is<node::array>() && a.get<node::array>() == (node::array {1}));
        }
        {   node a;
            a = node(node::map {{1, 2}}); CHECK(a.is<node::map>() && a.get<node::map>() == (node::map {{1, 2}}));
        }
        {   node a;
            a = node(node::tag {1, 2}); CHECK(a.is<node::tag>() && a.get<node::tag>() == (node::tag {1, 2}));
        }
        {   node a;
            a = node(true); CHECK(a.is<node::boolean>() && a.get<node::boolean>() == true);
        }
        {   node a;
            a = node(nullptr); CHECK(a.is<node::null>() && a.get<node::null>() == nullptr);
        }
        {   node a;
            a = node(node::undefined {}); CHECK(a.is<node::undefined>() && a.get<node::undefined>() == node::undefined {});
        }
        {   node a;
            a = node(1.0); CHECK(a.is<node::real>() && a.get<node::real>() == 1.0);
        }
        {   node a;
            a = node(node::simple {}); CHECK(a.is<node::simple>() && a.get<node::simple>() == 0);
        }
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
            node n2 = node::array {1, -2, node::array {3}, "4", node::array {5}, node::object{{"6", 7}}, 8, true, nullptr, nullptr, 0, 9 };
            CHECK(n2 == n1);
        }
        {   node n;
                auto r = cxon::from_bytes(n, "\xDC");
            CHECK(!r && r.ec == cxon::cbor::read_error::tag_invalid);
        }
        {   node n;
                auto r = cxon::from_bytes(n, "\xFF");
            CHECK(!r && r.ec == cxon::cbor::read_error::unexpected);
        }
        {   node n = node::array { node::object {{"1", 2}}, node::array {3}, "4", 5, true, nullptr };
            std::string s1;
                cxon::to_bytes(s1, n);
            char const s2[] = "\x86\xA1\x61\x31\xFB\x40\x00\x00\x00\x00\x00\x00\x00\x81\xFB\x40\x08\x00\x00\x00\x00\x00\x00\x61\x34\xFB\x40\x14\x00\x00\x00\x00\x00\x00\xF5\xF6";
            CHECK(std::memcmp(s1.c_str(), s2, sizeof(s2)) == 0);
        }
        {   node n1;
                cxon::from_bytes(n1, "\xA1\x01\x02");
            node n2 = node::object{{1, 2}};
            CHECK(n2 == n1);
        }
    }
    {   // round-trip
        using node = cxon::json::node;
        {   // json::node => cbor => json::node
            node const fr = node::array { node::object {{"1", 2}, {3, 4}}, node::array {5, 6}, "7, 8", 9, true, nullptr };
            node const to = node::array { node::object {{"1", 2}, {3, 4}}, node::array {5, 6}, "7, 8", 9, true, nullptr };
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
                                "\xFB\xBF\xF0\x00\x00\x00\x00\x00\x00"
                                "\xFB\x40\x00\x00\x00\x00\x00\x00\x00"
                                "\x82\xFB\x40\x08\x00\x00\x00\x00\x00\x00\xFB\x40\x10\x00\x00\x00\x00\x00\x00"
                                "\x64\x35\x2C\x20\x36"
                                "\x82\xFB\x40\x1C\x00\x00\x00\x00\x00\x00\xFB\x40\x20\x00\x00\x00\x00\x00\x00"
                                "\xA2\x62\x31\x31\xFB\x40\x28\x00\x00\x00\x00\x00\x00\xFB\x40\x22\x00\x00\x00\x00\x00\x00\xFB\x40\x24\x00\x00\x00\x00\x00\x00"
                                "\xFB\x40\x2C\x00\x00\x00\x00\x00\x00"
                                "\xF5"
                                "\xF6"
                                "\xF6"
                                "\xFB\x40\x2E\x00\x00\x00\x00\x00\x00"
                                "\xFB\x00\x00\x00\x00\x00\x00\x00\x00";
            node n;
                cxon::from_bytes(n, fr);
            std::vector<unsigned char> s;
                cxon::to_bytes(s, n);
            CHECK(std::memcmp(s.data(), to, sizeof(to) - 1) == 0);
        }
    }
#   undef CHECK
    
    fprintf(stdout, "cxon/cbor/node/self:  %u of %3u failed\n", f_, a_); fflush(stdout);

    return f_;
}

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
                for (size_t i = 0, is = hex.length(); i != is; i += 2) {
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
                    ++res.err, fprintf(stderr, "error: test-vector invalid: '%s'\n", file.c_str());
                    continue;
                }

            for (auto& test : vector) {
                ++res.all;
                cxon::json::node decoded;
                std::string fail;
                {   auto const fix = fixture.fix.find(test.hex);
                    if (fix != fixture.fix.end()) {
                        if (fix->second.act == "json") {
#                           if !defined(__GNUC__) || (__GNUC__ > 10 || (__GNUC__ == 10 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
                                auto const r = cxon::from_bytes<cxon::JSON<>>(
                                    decoded, fix->second.data,
                                    cxon::node::json::arbitrary_keys::set<true>(),
                                    cxon::node::json::extract_nans::set<true>()
                                );
#                           else
                                // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                                // seems to be fixed around 10
                                auto const r = cxon::from_bytes<cxon::JSON<>, cxon::json::node_traits>(
                                    decoded, fix->second.data,
                                    cxon::node::json::arbitrary_keys::set<true>(),
                                    cxon::node::json::extract_nans::set<true>()
                                );
#                           endif
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

namespace timing
{

    struct test_time {
        std::string source;
        std::string error;
        size_t size = 0;
        double read = 0;
        double write = 0;
    };

    constexpr unsigned cxon_cbor_repeat = 3;

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

    static result execute(const fixture& fixture) {
        CXON_ASSERT(fixture.type == "timing", "unexpected");
        
        using JSON = cxon::JSON<>;
        using CBOR = cxon::CBOR<>;

        std::vector<test_time> time;

        for (auto& file: fixture.in) {
            test_time t;

            std::vector<unsigned char> cbor;
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

                std::vector<cxon::cbor::ordered_node> vn;
                t.read = measure(cxon_cbor_repeat, [&] {
                    vn.emplace_back();
                    auto const r = cxon::from_bytes(vn.back(), cbor);
                    if (!r) t.error = format_error(r, cbor.cbegin());
                });

                auto const& n = vn.back();
                std::vector<std::string> vs;
                t.write = measure(cxon_cbor_repeat, [&] {
                    vs.emplace_back();
                    cxon::to_bytes(vs.back(), n);
                });
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
                    tab.push_back({"CBOR/File", "Size", "Read", "MB/s", "Write", "MB/s"});
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
                            t.source,
                            fmt(size),
                            fmt(t.read), fmt(size / (t.read / 1000)),
                            fmt(t.write), fmt(size / (t.write/ 1000))
                        });
                        total.size += t.size,
                        total.read += t.read,
                        total.write += t.write;
                    }
                }
                {   // average
                    double const size = double(total.size) / (1024. * 1024);
                    tab.push_back({
                        "Average",
                        "",
                        "", fmt(size / (total.read / 1000)),
                        "", fmt(size / (total.write/ 1000))
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

        result res;
            for (auto& t : time)
                ++res.all, res.err += !t.error.empty();
        return res;
    }

}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        return execute_self();
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
                ++res.err, fprintf(stderr, "error: fixture invalid: '%s'\n", argv[i]);
                continue;
            }

        if (fixture.type == "test-vector") {
            result const r = test_vector::execute(fixture);
            res.all += r.all, res.err += r.err;
        }
        else if (fixture.type == "round-trip") {
            has_diff = !fixture.in.empty();
            result const r = round_trip::execute(fixture);
            res.all += r.all, res.err += r.err;
        }
        else if (fixture.type == "timing") {
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
