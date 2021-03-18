// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "cxon/json.hxx"
#include "cxon/lib/node.hxx"

#include "cxon/lib/std/list.hxx"

#include <cstring>
#include <fstream>


////////////////////////////////////////////////////////////////////////////////

//struct my_traits : cxon::cbor::node_traits {
//    using                               bytes_type = std::vector;
//    template <class T> using            array_type = std::list<T>;
//    template <class K, class V> using   map_type = std::multimap<K, V>;
//};

static unsigned self() {
    unsigned a_ = 0;
    unsigned f_ = 0;

#   define CHECK(c) ++a_; if (!(c))\
        fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__), fflush(stderr), ++f_;\
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
#           if !defined(__GNUG__) || defined(__clang__) || (__GNUG__ >= 10 && __GNUG_MINOR__ >=2)
                auto const r = cxon::from_bytes(n, "\x81\x81\x81\x81", cxon::node::recursion_depth::set<4>());
#           else
                // g++ (4.8.1->9.1) bug: overload resolution fail => workaround, add type parameters
                // seems to be fixed around 10, after the inclusion of cbor.hxx,
                // funnily enough, this workaround continues to work for from_chars (unlike to_chars)
                auto const r = cxon::from_bytes<cxon::CBOR<>, cxon::cbor::node_traits>
                                    (n, "\x81\x81\x81\x81", cxon::node::recursion_depth::set<4>());
#           endif
            CHECK(!r && r.ec == cxon::node::error::recursion_depth_exceeded);
        }
        {   node n;
            auto const r = cxon::from_bytes(n, "\xFF");
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
        CHECK(r1 && n1.is<node::array>() && n1.get<node::array>() == (node::array {1U, 2U, 3U}));

        node n2;
            auto const r2 = cxon::from_bytes(n2, "\xC1\xC2\x83\x01\x02\x03");
        CHECK(r2 && n2.is<node::array>() && n2.get<node::array>() == (node::array {1U, 2U, 3U}));

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
                { false, 1 },
                { nullptr, 1 },
                { 1.0, 1 }
            },
            true,                       // bool
            false,                      // bool
            nullptr,                    // null
            1.0                         // real
        };

        // build using node's methods
        node n2;
            CHECK(n2.is<node::null>()); // default node type is node_kind::null
            auto& a = n2.imbue<node::array>(); // change the type and return its value
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
                    m[false] = 1;                       CHECK(m.find(node::boolean(false)) != m.end());
                    m[nullptr] = 1;                     CHECK(m.find(node::null(nullptr)) != m.end());
                    m[1.0] = 1;                         CHECK(m.find(node::real(1.0)) != m.end());
                a.push_back(true);                      CHECK(a.back().is<node::boolean>());
                a.push_back(false);                     CHECK(a.back().is<node::boolean>());
                a.push_back(nullptr);                   CHECK(a.back().is<node::null>());
                a.push_back(1.0);                       CHECK(a.back().is<node::real>());

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
            node n(42.0); CHECK(n.is<node::real>() && n.get<node::real>() == 42.0);
        }
        {
            node o = true; CHECK(o.is<node::boolean>());
            node n(o); CHECK(n.is<node::boolean>() && n.get<node::boolean>());
        }
        {
            node n; CHECK(n.is<node::null>());
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
        node const n; CHECK(n.kind() == cxon::cbor::node_kind::null);
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
        {   node a = 42., b;
            b = a; CHECK(a == b);
        }
        {   node a = true, b;
            b = a; CHECK(a == b);
        }
        {   node a = nullptr, b;
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
            a = node(true); CHECK(a.is<node::boolean>() && a.get<node::boolean>() == true);
        }
        {   node a;
            a = node(nullptr); CHECK(a.is<node::null>() && a.get<node::null>() == nullptr);
        }
        {   node a;
            a = node(1.0); CHECK(a.is<node::real>() && a.get<node::real>() == 1.0);
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
                cxon::from_bytes(n1, "\x9F\x01\x21\x41\x03\x61\x34\x81\x05\xA1\x61\x36\x07\xF5\xF6\xFA\x00\x00\x00\x00\xC1\xC2\x08\xFF");
            node n2 = node::array {1, -2, node::array {3}, "4", node::array {5}, node::object{{"6", 7}}, true, nullptr, 0, 8 };
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
            // node::array {   -1,  2U, node::bytes {  3,   4}, "5, 6", node::array {  7,   8}, node::map {{  9,   10}, {"11",   12}}, true, nullptr, 13.0 };
            char const fr[] =   "\x89\x20\x02\x42\x03\x04\x64\x35\x2C\x20\x36\x82\x07\x08\xA2\x09\x0A\x62\x31\x31\x0C\xF5\xF6\xFB\x40\x2A\x00\x00\x00\x00\x00\x00";
            char const to[] =   "\x89\xFB\xBF\xF0\x00\x00\x00\x00\x00\x00\xFB\x40\x00\x00\x00\x00\x00\x00\x00"
                                "\x82\xFB\x40\x08\x00\x00\x00\x00\x00\x00\xFB\x40\x10\x00\x00\x00\x00\x00\x00"
                                "\x64\x35\x2C\x20\x36"
                                "\x82\xFB\x40\x1C\x00\x00\x00\x00\x00\x00\xFB\x40\x20\x00\x00\x00\x00\x00\x00"
                                "\xA2\x62\x31\x31\xFB\x40\x28\x00\x00\x00\x00\x00\x00\xFB\x40\x22\x00\x00\x00\x00\x00\x00\xFB\x40\x24\x00\x00\x00\x00\x00\x00"
                                "\xF5\xF6\xFB\x40\x2A\x00\x00\x00\x00\x00\x00";
            node n;
                cxon::from_bytes(n, fr);
            std::vector<unsigned char> s;
                cxon::to_bytes(s, n);
            CHECK(std::memcmp(s.data(), to, sizeof(to) - 1) == 0);
        }
    }
#   undef CHECK
    f_ ?
        // coverity[ copy_paste_error ]
        fprintf(stdout, "cxon/cbor/node/self: %u of %u failed\n", f_, a_) :
        fprintf(stdout, "cxon/cbor/node/self: %u of %u passed\n", a_, a_)
    ;
    return f_;
}

struct test {
    std::string hex;
    bool roundtrip;
    cxon::json::node decoded;
    std::string diagnostic;

    using suite = std::vector<test>;

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

struct fixture {
    struct fix {
        std::string justification;
        bool skip;

        CXON_JSON_CLS_READ_MEMBER(fix,
            CXON_JSON_CLS_FIELD_ASIS(justification),
            CXON_JSON_CLS_FIELD_ASIS(skip)
        )
    };
    std::string in;
    std::map<std::string, fix> fix;

    CXON_JSON_CLS_READ_MEMBER(fixture,
        CXON_JSON_CLS_FIELD_ASIS(in),
        CXON_JSON_CLS_FIELD_ASIS(fix)
    )
};

int main(int argc, char *argv[]) {
    if (argc == 1) {
        return self();
    }
    {
        std::ifstream is(argv[1]);
            if (!is) return -1;

        fixture fixture;
            auto const r = cxon::from_bytes<cxon::JSON<>>(fixture, std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            CXON_ASSERT(r, "TODO");
        {
            std::ifstream is(fixture.in);
                if (!is) return -1;

            test::suite suite;
                auto const r = cxon::from_bytes<cxon::JSON<>>(suite, std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
                CXON_ASSERT(r, "TODO");

            for (auto& test : suite) {
                auto const fix = fixture.fix.find(test.hex);
                if (fix == fixture.fix.end()) {
                    auto const cbor = test.bin();
                    cxon::json::node json;
                        auto const r = cxon::from_bytes(json, cbor);
                    if (r) {
                        if (json != test.decoded) {
                            fprintf(stdout, "fail: cbor: %s\n", test.hex.c_str());
                        }
                    }
                    else {
                        fprintf(stdout, "error: cbor: %s\n", test.hex.c_str());
                    }
                }
                else {
                    fprintf(stdout, "skip: cbor: %s (%s)\n", test.hex.c_str(), fix->second.justification.c_str());
                }
            }
        }
    }

    return 0;
}
