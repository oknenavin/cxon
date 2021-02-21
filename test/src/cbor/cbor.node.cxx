// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "cxon/lang/cbor/node.hxx"

#include "cxon/lib/std/list.hxx"

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

    {   // ex4
        // build using initializer lists
        node n1 = node::array {
            1,                          // sint
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
                { true, 1 },
                { nullptr, 1 },
                { 1.0, 1 }
            },
            true,                       // bool
            nullptr,                    // null
            1.0                         // real
        };

        // build using node's methods
        node n2;
            CHECK(n2.is<node::null>()); // default node type is node_kind::null
            auto& a = n2.imbue<node::array>(); // change the type and return its value
                CHECK(n2.is<node::array>());
                a.push_back(1);                         CHECK(a.back().is<node::sint>());
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
                    m[true] = 1;                        CHECK(m.find(node::boolean(true)) != m.end());
                    m[nullptr] = 1;                     CHECK(m.find(node::null(nullptr)) != m.end());
                    m[1.0] = 1;                         CHECK(m.find(node::real(1.0)) != m.end());
                a.push_back(true);                      CHECK(a.back().is<node::boolean>());
                a.push_back(nullptr);                   CHECK(a.back().is<node::null>());
                a.push_back(1.0);                       CHECK(a.back().is<node::real>());

        CHECK(n1 == n2);

        /*std::string s1;
            cxon::to_bytes(s1, n1);
        std::string s2;
            cxon::to_bytes(s2, n2);
        CHECK(s1 == s2);*/
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
#   undef CHECK
    f_ ?
        fprintf(stdout, "cxon/cbor/node/self: %u of %u failed\n", f_, a_) :
        fprintf(stdout, "cxon/cbor/node/self: %u of %u passed\n", a_, a_)
    ;
    return f_;
}

int main(int argc, char *[]) {
    if (argc == 1) {
        return self();
    }
    return 0;
}
