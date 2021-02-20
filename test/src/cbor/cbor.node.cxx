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
        node n1 = node::map {
            { "object", node::map { {"object", 0} } },
            { "array", node::array {
                    node::map { {"object", 0} },    // objects and
                    node::array { 1, 2, 3 },        // arrays must be explicit
                    (unsigned char*)"4",            // byte_string
                    "4",                            // text_string
                    3,                              // signed_integer
                    14U,                            // unsigned_integer
                    3.14,                           // floating_point
                    true,                           // boolean
                    nullptr                         // null
                }
            },
            { "byte_string", (unsigned char*)"string" }, // "key": value
            { "text_string", "string" }, // "key": value
            { "signed_integer", 3 },
            { "unsigned_integer", 14U },
            { "floating_point", 3.14 },
            { "boolean", false },
            { "null", nullptr }
        };

        // build using node's methods
        node n2;
            CHECK(n2.is<node::null>()); // default node type is node_kind::null
            auto& o = n2.imbue<node::map>(); // change the type and return its value
                CHECK(n2.is<node::map>());
                o["object"] = node::map {};                     CHECK(o["object"].is<node::map>());
                o["array"] = node::array {};                    CHECK(o["array"].is<node::array>());
                o["byte_string"] = (unsigned char*)"string";    CHECK(o["byte_string"].is<node::byte_string>());
                o["text_string"] = "string";                    CHECK(o["text_string"].is<node::text_string>());
                o["signed_integer"] = 3;                        CHECK(o["signed_integer"].is<node::signed_integer>());
                o["unsigned_integer"] = 14U;                    CHECK(o["unsigned_integer"].is<node::unsigned_integer>());
                o["floating_point"] = 3.14;                     CHECK(o["floating_point"].is<node::floating_point>());
                o["boolean"] = false;                           CHECK(o["boolean"].is<node::boolean>());
                o["null"] = nullptr;                            CHECK(o["null"].is<node::null>());
            auto& o1 = o["object"].get<node::map>();    // get value reference, the type is known
                o1["object"] = 0;
            auto& a = o["array"].get<node::array>();    // get value reference, the type is known
                a.push_back(node::map {});                      CHECK(a.back().is<node::map>());
                a.push_back(node::array {1, 2, 3});             CHECK(a.back().is<node::array>());
                a.push_back((unsigned char*)"4");               CHECK(a.back().is<node::byte_string>());
                a.push_back("4");                               CHECK(a.back().is<node::text_string>());
                a.push_back(3);                                 CHECK(a.back().is<node::signed_integer>());
                a.push_back(14U);                               CHECK(a.back().is<node::unsigned_integer>());
                a.push_back(3.14);                              CHECK(a.back().is<node::floating_point>());
                a.push_back(true);                              CHECK(a.back().is<node::boolean>());
                a.push_back(nullptr);                           CHECK(a.back().is<node::null>());
            auto* o2 = a[0].get_if<node::map>(); // get value pointer if the type match
                (*o2)["object"] = 0;
        CHECK(n1 == n2);

        /*std::string s1;
            cxon::to_bytes(s1, n1);
        std::string s2;
            cxon::to_bytes(s2, n2);
        CHECK(s1 == s2);*/
    }
    {   // ex5
        {
            node n(42); CHECK(n.is<node::signed_integer>() && n.get<node::signed_integer>() == 42);
        }
        {
            node n(42U); CHECK(n.is<node::unsigned_integer>() && n.get<node::unsigned_integer>() == 42U);
        }
        {
            node n((unsigned char*)"string"); CHECK(n.is<node::byte_string>() && n.get<node::byte_string>() == (unsigned char*)"string");
        }
        {
            node n("string"); CHECK(n.is<node::text_string>() && n.get<node::text_string>() == "string");
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
            node::map const o = { {(unsigned char*)"key", "value"} };
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
            node n(42.0); CHECK(n.is<node::floating_point>() && n.get<node::floating_point>() == 42.0);
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
            node::text_string& v = n.imbue<node::text_string>(); CHECK(v == "string");
        }
        {   // T is not the same
            node n = "string";
            node::floating_point& v = n.imbue<node::floating_point>(); CHECK(v == 0.0);
        }
    }
    {   // ex7
        node n = "one";
            n.get<node::text_string>() = "another";
        CHECK(n.get<node::text_string>() == "another");
    }
    {   // ex8
        node n = "one";
            auto *v = n.get_if<node::text_string>(); CHECK(v != nullptr);
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
        {   node a = (unsigned char*)"blah", b;
            b = a; CHECK(a == b);
        }
        {   node a = "blah", b;
            b = a; CHECK(a == b);
        }
        {   node a = node::array {4, 2}, b;
            b = a; CHECK(a == b);
        }
        {   node a = node::map {{"q", "?"}, {"a", 42}}, b;
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
