// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "test.hxx"

#include "cxon/lib/std/array.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/string_view.hxx"
#include "cxon/lib/std/list.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/tuple.hxx"

#include "cxon/lang/json/tidy.hxx"

#include <cstdio>

////////////////////////////////////////////////////////////////////////////////

TEST_BEG(interface_read, cxon::JSON<>, "/core") // interface/read
    // iterator
    {   int r = 0; char const i[] = "1";
        TEST_CHECK(from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    {   int r = 0; std::string const i = "1";
        TEST_CHECK(from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    {   int r = 0; std::vector<char> const i = {'1', '\0'};
        TEST_CHECK(from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    // container
    {   int r = 0; char const i[] = "1";
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
    {   int r = 0; std::string const i = "1";
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
#   ifdef CXON_HAS_LIB_STD_STRING_VIEW
    {   int r = 0; std::string_view const i("1", 1);
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
#   endif
    {   int r = 0; std::vector<char> const i = {'1', '\0'};
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
    {   int r = 0; std::array<char, 2> const i = {'1', '\0'};
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
TEST_END()

TEST_BEG(interface_write, cxon::JSON<>, "/core") // interface/write
    // output iterator
    {   std::string r; std::string const e = QS("1");
        TEST_CHECK(to_bytes(std::back_inserter(r), "1") && r == e);
    }
    {   std::string r; std::string const e = "1";
        TEST_CHECK(to_bytes(std::back_inserter(r), 1) && r == e);
    }
    {   std::string r; std::string const e = "true";
        TEST_CHECK(to_bytes(std::back_inserter(r), true) && r == e);
    }
    // range
    {   char o[16]; char const e[] = QS("1");
        auto const r = to_bytes(std::begin(o), std::end(o), "1");
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
    {   char o[3]; char const e[] = QS("1");
        auto const r = to_bytes(std::begin(o), std::end(o), "1");
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
        {   char o[1];
            auto const r = to_bytes(std::begin(o), std::end(o), "42");
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
    {   char o[16]; char const e[] = "1";
        auto const r = to_bytes(std::begin(o), std::end(o), 1);
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
    {   char o[2]; char const e[] = "42";
        auto const r = to_bytes(std::begin(o), std::end(o), 42);
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
        {   char o[1];
            auto const r = to_bytes(std::begin(o), std::end(o), 42);
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
    {   char o[16]; char const e[] = "true";
        auto const r = to_bytes(std::begin(o), std::end(o), true);
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
    {   char o[4]; char const e[] = "true";
        auto const r = to_bytes(std::begin(o), std::end(o), true);
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
        {   char o[1];
            auto const r = to_bytes(std::begin(o), std::end(o), true);
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
    // container/std::string (push_back, append)
    {   std::string r; std::string const e = QS("1");
        TEST_CHECK(to_bytes(r, "1") && r == e);
    }
    {   std::string r; std::string const e = "1";
        TEST_CHECK(to_bytes(r, 1) && r == e);
    }
    {   std::string r; std::string const e = "true";
        TEST_CHECK(to_bytes(r, true) && r == e);
    }
    // container/std::vector (push_back)
    {   std::vector<char> r; std::vector<char> const e = {'"', '1', '"'};
        TEST_CHECK(to_bytes(r, "1") && r == e);
    }
    {   std::vector<char> r; std::vector<char> const e = {'1'};
        TEST_CHECK(to_bytes(r, 1) && r == e);
    }
    {   std::vector<char> r; std::vector<char> const e = {'t', 'r', 'u', 'e'};
        TEST_CHECK(to_bytes(r, true) && r == e);
    }
TEST_END()


namespace {
    enum Enum11 { one, two, three, four };
}

CXON_JSON_ENM(Enum11,
    CXON_JSON_ENM_VALUE_ASIS(one),
    CXON_JSON_ENM_VALUE_NAME("Two (2)", two),
    CXON_JSON_ENM_VALUE_ASIS(three)
)

namespace {

    struct Struct11 {
        int field;
        Struct11(int f = 0) : field(f) {}
        bool operator ==(const Struct11& t) const { return field == t.field; }
        CXON_JSON_CLS_READ_MEMBER(Struct11,
            CXON_JSON_CLS_FIELD_ASIS(field)
        )
    };

}

TEST_BEG(interface_parameters, cxon::JSON<>, "/core") // interface/parameters
    {   std::string r; std::string const e = "3.142";
        TEST_CHECK(to_bytes(r, 3.1415926, json::fp_precision::set<4>()) && r == e);
    }
    {   int *r = nullptr;
        std::allocator<int> a;
        TEST_CHECK(from_bytes(r, "42", json::allocator::set(a)) && *r == 42);
        std::allocator_traits<std::allocator<int>>::deallocate(a, r, 4);
    }
    {   std::size_t r = 0;
        TEST_CHECK(from_bytes(r, std::string("123"), json::num_len_max::set<4>()) && r == 123);
    }
    {   unsigned r = 0; std::string const i = "123";
        auto ib = test::make_force_input_iterator(i.begin()), ie = test::make_force_input_iterator(i.end());
        auto const e = from_bytes(r, ib, ie, json::num_len_max::set<2>());
        TEST_CHECK(!e && e.ec == json::read_error::overflow && *e.end == '3');
    }
    {   double r = 0; std::list<char> const i = {'1', '2', '3'};
        TEST_CHECK(from_bytes(r, i, json::num_len_max::set<6>()) && r == 123);
    }
    {   float r = 0; std::list<char> const i = {'1', '2', '3'};
        auto const e = from_bytes(r, i, json::num_len_max::set<2>());
        TEST_CHECK(!e && e.ec == json::read_error::overflow && *e.end == '1');
    }
    {   Enum11 r = Enum11::one;
        TEST_CHECK(from_bytes(r, QS("three"), json::ids_len_max::set<8>()) && r == Enum11::three);
    }
    {   Enum11 r = Enum11::one; std::string const i = QS("three");
        auto const e = from_bytes(r, i, json::ids_len_max::set<2>());
        TEST_CHECK(!e && e.ec == json::read_error::overflow && *e.end == 't');
    }
    {   Struct11 r(42);
        TEST_CHECK(from_bytes(r, "{ \"field\": 42 }", json::ids_len_max::set<8>()) && r == Struct11(42));
    }
    {   Struct11 r(42);
        auto const e = from_bytes(r, "{ \"field\": 42 }", json::ids_len_max::set<2>());
        TEST_CHECK(!e && e.ec == json::read_error::overflow && *e.end == '\"');
    }
TEST_END()

TEST_BEG(interface_errors, cxon::JSON<>, "/core") // errors
    using namespace cxon;
    {   std::error_condition ec;
            ec = json::read_error::ok;
                TEST_CHECK(ec.category() == json::read_error_category::value());
                TEST_CHECK(std::strcmp(ec.category().name(), "cxon/json/read") == 0);
                TEST_CHECK(ec.message() == "no error");
            ec = json::read_error::unexpected;
                TEST_CHECK(ec.message() == "unexpected input");
            ec = json::read_error::character_invalid;
                TEST_CHECK(ec.message() == "invalid character");
            ec = json::read_error::integral_invalid;
                TEST_CHECK(ec.message() == "invalid integral or value out of range");
            ec = json::read_error::floating_point_invalid;
                TEST_CHECK(ec.message() == "invalid floating point");
            ec = json::read_error::boolean_invalid;
                TEST_CHECK(ec.message() == "invalid boolean");
            ec = json::read_error::escape_invalid;
                TEST_CHECK(ec.message() == "invalid escape sequence");
            ec = json::read_error::surrogate_invalid;
                TEST_CHECK(ec.message() == "invalid surrogate");
            ec = json::read_error::overflow;
                TEST_CHECK(ec.message() == "buffer overflow");
            ec = json::read_error(255);
                TEST_CHECK(ec.message() == "unknown error");
    }
    {   std::error_condition ec;
            ec = json::write_error::ok;
                TEST_CHECK(ec.category() == json::write_error_category::value());
                TEST_CHECK(std::strcmp(ec.category().name(), "cxon/json/write") == 0);
                TEST_CHECK(ec.message() == "no error");
            ec = json::write_error::output_failure;
                TEST_CHECK(ec.message() == "output cannot be written");
            ec = json::write_error::argument_invalid;
                TEST_CHECK(ec.message() == "invalid argument");
            ec = json::write_error(255);
                TEST_CHECK(ec.message() == "unknown error");
    }
TEST_END()

TEST_BEG(interface_pretty, cxon::JSON<>, "/core") // pretty
    {   std::map<std::string, std::vector<int>> const m = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} };
        char const s0[] =
            "{\n"
            "  \"even\": [\n"
            "    2,\n"
            "    4,\n"
            "    6\n"
            "  ],\n"
            "  \"odd\": [\n"
            "    1,\n"
            "    3,\n"
            "    5\n"
            "  ]\n"
            "}"
        ;
        std::string s1;
            to_bytes(json::make_indenter(s1, 2, ' '), m);
        TEST_CHECK(s1 == s0);
    }
    {   std::map<std::string, std::vector<int>> const m = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} };
        std::string s1;
            to_bytes(json::make_indenter(s1), m);
        std::string const s0 =
            json::tidy(s1);
        TEST_CHECK(s1 == s0);
    }
    {   std::map<std::string, std::string> const m = { {"ala", "ba\"la"}, {"bl\nah", "blah"}, {"bl ah", "blah"} };
        char const s0[] =
            "{\n"
            "  \"ala\": \"ba\\\"la\",\n"
            "  \"bl\\nah\": \"blah\",\n"
            "  \"bl ah\": \"blah\"\n"
            "}"
        ;
        std::string s1;
            to_bytes(json::make_indenter(s1, 2, ' '), m);
        TEST_CHECK(s1 == s0);
    }
    {   std::map<std::string, std::string> const m = { {"ala", "ba\"la"}, {"bl\nah", "blah"}, {"bl ah", "blah"} };
        std::string s1;
            to_bytes(json::make_indenter(s1), m);
        std::string const s0 =
            json::tidy(s1);
        TEST_CHECK(s1 == s0);
    }
TEST_END()


namespace {
    struct struct_json {
        std::vector<int> even, odd;
        bool operator ==(const struct_json& t) const { return even == t.even && odd == t.odd; }
    };
}
CXON_JSON_CLS(struct_json,
    CXON_JSON_CLS_FIELD_ASIS(even),
    CXON_JSON_CLS_FIELD_ASIS(odd)
)

TEST_BEG(struct_json, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<>>, "/core")
    struct_json sb = {{2, 4}, {5, 7}};
    char const s0[] =
        "{\n"
        "  even: [\n"
        "    2,\n"
        "    4\n"
        "  ],\n"
        "  odd: [\n"
        "    5,\n"
        "    7\n"
        "  ]\n"
        "}"
    ;
    std::string s1;
        cxon::to_bytes<XXON>(json::make_indenter(s1, 2, ' '), sb);
    TEST_CHECK(s1 == s0);
TEST_END()

TEST_BEG(struct_json_comments, cxon::JSON<cxon::test::allow_comments_traits<>>, "/core")
    R_TEST(struct_json {}, "{/}", json::read_error::unexpected, 2);
TEST_END()


namespace {
    struct struct_bare_1 {
        std::vector<int> even, odd;
        bool operator ==(const struct_bare_1& t) const { return even == t.even && odd == t.odd; }
    };
}
CXON_JSON_CLS(struct_bare_1,
    CXON_JSON_CLS_FIELD_ASIS(even),
    CXON_JSON_CLS_FIELD_ASIS(odd)
)
CXON_JSON_CLS_BARE(struct_bare_1);

TEST_BEG(struct_bare_1, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<>>, "/core")
    R_TEST(struct_bare_1 {}, "");
    W_TEST("even:[]\nodd:[]", struct_bare_1 {});
    R_TEST(struct_bare_1 {{2, 4}, {5, 7}}, "even:[2,4]\nodd:[5,7]");
    W_TEST("even:[2,4]\nodd:[5,7]", struct_bare_1 {{2, 4}, {5, 7}});
    R_TEST(struct_bare_1 {{2, 4}, {5, 7}}, "even:[2,4] odd:[5,7]");
    R_TEST(struct_bare_1 {{2, 4}, {5, 7}}, "even:x2,4] odd:[5,7]", json::read_error::unexpected, 5);
    // tidy
    {   char const s0[] =
            "even: [\n"
            "  2,\n"
            "  4\n"
            "]\n"
            "odd: [\n"
            "  5,\n"
            "  7\n"
            "]"
        ;
        struct_bare_1 b0;
            TEST_CHECK(cxon::from_bytes<XXON>(b0, s0));
        std::string s1;
            TEST_CHECK(cxon::to_bytes<XXON>(json::make_indenter(s1, 2, ' '), b0));
        TEST_CHECK(s1 == s0);
    }
    {   char const s0[] =
            "[\n"
            "  \"\",\n"
            "  \"undefined\"\n"
            "]"
        ;
        std::vector<std::string> b0;
            TEST_CHECK(cxon::from_bytes<XXON>(b0, s0));
        std::string s1;
            TEST_CHECK(cxon::to_bytes<XXON>(json::make_indenter(s1, 2, ' '), b0));
        TEST_CHECK(s1 == s0);
    }
    {
        TEST_CHECK((
            "\"a\": [\n\t\"b\",\n\t1,\n\t\"c\"\n],\n\"d\": \"e\",\n\"f\": {\n\t\"g\": [\n\t\t2,\n\t\t\"h\",\n\t\t3\n\t],\n\t\"j\": \"k\"\n}" ==
            cxon::json::tidy<XXON, std::string>(R"("a": ["b", 1, "c"], "d": "e", "f": {"g": [2, "h", 3], "j": "k"})")
        ));
        TEST_CHECK((
            "\"a\": [\n\t\"b\"\n\t1\n\t\"c\"\n]\n\"d\": \"e\"\n\"f\": {\n\t\"g\": [\n\t\t2\n\t\t\"h\"\n\t\t3\n\t]\n\t\"j\": \"k\"\n}" ==
            cxon::json::tidy<XXON, std::string>(R"("a": ["b"  1  "c"]  "d": "e"  "f": {"g": [2  "h"  3]  "j": "k"})")
        ));
        TEST_CHECK((
            "a: [\n\tb\n\t1\n\tc\n]\nd: e\nf: {\n\tg: [\n\t\t2\n\t\th\n\t\t3\n\t]\n\tj: k\n}" ==
            cxon::json::tidy<XXON, std::string>(R"( a : [ b   1   c ]   d :  e    f : { g : [2   h   3]   j :  k })")
        ));
        TEST_CHECK((R"(a\:b: c)" == cxon::json::tidy<XXON, std::string>(R"(a\:b: c)")));
    }
TEST_END()

TEST_BEG(struct_bare_1_comments, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<cxon::test::allow_comments_traits<>>>, "/core")
    R_TEST(struct_bare_1 {{2, 4}, {5, 7}}, "even:[2,4]/ odd:[5,7]", json::read_error::unexpected, 11);
TEST_END()


namespace {
    struct struct_bare_2 {
        using bare_class_tag = cxon::json::cls::bare_class_tag;
        std::vector<int> even, odd;
        bool operator ==(const struct_bare_2& t) const { return even == t.even && odd == t.odd; }
    };
}
CXON_JSON_CLS(struct_bare_2,
    CXON_JSON_CLS_FIELD_ASIS(even),
    CXON_JSON_CLS_FIELD_ASIS(odd)
)

TEST_BEG(struct_bare_2, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<>>, "/core")
    R_TEST(struct_bare_2 {}, "");
    W_TEST("even:[]\nodd:[]", struct_bare_2 {});
    R_TEST(struct_bare_2 {{2, 4}, {5, 7}}, "even:[2,4]\nodd:[5,7]");
    W_TEST("even:[2,4]\nodd:[5,7]", struct_bare_2 {{2, 4}, {5, 7}});
    R_TEST(struct_bare_2 {{2, 4}, {5, 7}}, "even:[2,4] odd:[5,7]");
TEST_END()

namespace {
    struct struct_bare_3 : cxon::cio::cls::bare_class {
        struct_bare_3() : even(), odd() {}
        struct_bare_3(const std::vector<int>& e, const std::vector<int>& o) : even(e), odd(o) {}
        std::vector<int> even, odd;
        bool operator ==(const struct_bare_3& t) const { return even == t.even && odd == t.odd; }
    };
}
CXON_JSON_CLS(struct_bare_3,
    CXON_JSON_CLS_FIELD_ASIS(even),
    CXON_JSON_CLS_FIELD_ASIS(odd)
)

TEST_BEG(struct_bare_3, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<>>, "/core")
    R_TEST(struct_bare_3 {}, "");
    W_TEST("even:[]\nodd:[]", struct_bare_3 {});
    R_TEST(struct_bare_3 {{2, 4}, {5, 7}}, "even:[2,4]\nodd:[5,7]");
    W_TEST("even:[2,4]\nodd:[5,7]", struct_bare_3 {{2, 4}, {5, 7}});
    R_TEST(struct_bare_3 {{2, 4}, {5, 7}}, "even:[2,4] odd:[5,7]");
TEST_END()


TEST_BEG(trailing_separator_1, cxon::JSON<cxon::test::allow_trailing_separators_traits<>>, "/core")
    R_TEST(std::vector<int> {1, 3}, "[1, 3,]");
    R_TEST(std::vector<int> {1, 3}, "[1, 3 ]");
    R_TEST(std::map<char, int> {{'x', 1}, {'y', 3}}, R"({"x": 1, "y": 3,})");
    R_TEST(std::map<char, int> {{'x', 1}, {'y', 3}}, R"({"x": 1, "y": 3 })");
TEST_END()

namespace {
    struct struct_trailing_separator_1 {
        int x, y;
        bool operator ==(const struct_trailing_separator_1& t) const { return x == t.x && y == t.y; }
    };
}
CXON_JSON_CLS(struct_trailing_separator_1,
    CXON_JSON_CLS_FIELD_ASIS(x),
    CXON_JSON_CLS_FIELD_ASIS(y)
)

TEST_BEG(trailing_separator_2, cxon::JSON<cxon::test::allow_trailing_separators_traits<>>, "/core")
    R_TEST(struct_trailing_separator_1 {1, 3}, R"({"x": 1, "y": 3,})");
    R_TEST(struct_trailing_separator_1 {1, 3}, R"({"x": 1, "y": 3 })");
TEST_END()

TEST_BEG(trailing_separator_3, cxon::JSON<cxon::test::allow_trailing_separators_traits<>>, "/core")
    TEST_CHECK(("[\n\ta,\n\tb,\n]" == cxon::json::tidy<XXON, std::string>(R"([a, b,])")));
    TEST_CHECK(("[\n\ta,\n\tb\n]" == cxon::json::tidy<XXON, std::string>(R"([a, b ])")));
    TEST_CHECK(("{\n\ta: 1,\n\tb: 2,\n}" == cxon::json::tidy<XXON, std::string>(R"({a: 1, b: 2,})")));
    TEST_CHECK(("{\n\ta: 1,\n\tb: 2\n}" == cxon::json::tidy<XXON, std::string>(R"({a: 1, b: 2 })")));
TEST_END()


namespace {
    enum class enum_comments { x, y };
    struct struct_comments {
        int x, y;
        bool operator ==(const struct_comments& t) const { return x == t.x && y == t.y; }
    };
}
CXON_JSON_ENM(enum_comments,
    CXON_JSON_ENM_VALUE_ASIS(x),
    CXON_JSON_ENM_VALUE_ASIS(y)
)
CXON_JSON_CLS(struct_comments,
    CXON_JSON_CLS_FIELD_ASIS(x),
    CXON_JSON_CLS_FIELD_ASIS(y)
)

TEST_BEG(allow_comments_core, cxon::JSON<cxon::test::allow_comments_traits<>>, "/core")
    R_TEST((unsigned)42, "// comment\n42");
    R_TEST((unsigned)42, "//\n42");
    R_TEST((unsigned)42, "/* comment */ 42");
    R_TEST((unsigned)42, "/*\n comment\n*/ 42");
    R_TEST((unsigned)42, "/***/ 42");
    R_TEST((unsigned)42, "/*/*/ 42");
    R_TEST((unsigned)42, "/**/ 42");
    R_TEST((unsigned)42, "// comment1\n// comment2\n42");
    R_TEST((unsigned)42, "// comment1\n/* comment2 */ 42");
    R_TEST((unsigned)42, "/* comment1 */// comment2\n 42");
    R_TEST((unsigned)42, "/* comment1 */\n// comment2\n 42");
    R_TEST((unsigned)42, "/* comment1 *//* comment2 */ 42");
    R_TEST((unsigned)42, "/* comment1 */\n/* comment2 */ 42");
    R_TEST((unsigned)42, "/", json::read_error::unexpected, 1);
    R_TEST((unsigned)42, "//", json::read_error::integral_invalid, 2);
    R_TEST((unsigned)42, "/*", json::read_error::integral_invalid, 2);
    R_TEST((unsigned)42, "/**", json::read_error::integral_invalid, 3);
    R_TEST((unsigned)42, "/**/", json::read_error::integral_invalid, 4);
    R_TEST(nullptr, "/null", json::read_error::unexpected, 1);
    R_TEST(true, "/true", json::read_error::unexpected, 1);
    R_TEST((unsigned)42, "/42", json::read_error::unexpected, 1);
    R_TEST((const char*)nullptr, "/ \"\"", json::read_error::unexpected, 1);
    R_TEST(enum_comments::x, R"(/x)", json::read_error::unexpected, 1);
    R_TEST(struct_comments {1, 2}, R"({"x":1/,"y":2})", json::read_error::unexpected, 7);
    {   int x = 42;
        R_TEST(&x, "/42", json::read_error::unexpected, 1);
    }
TEST_END()

TEST_BEG(allow_comments_core_input_iterator, cxon::JSON<cxon::test::input_iterator_traits<cxon::test::allow_comments_traits<>>>, "/core")
    R_TEST((unsigned)42, "// comment\n42");
    R_TEST((unsigned)42, "//\n42");
    R_TEST((unsigned)42, "/* comment */ 42");
    R_TEST((unsigned)42, "/*\n comment\n*/ 42");
    R_TEST((unsigned)42, "/***/ 42");
    R_TEST((unsigned)42, "/*/*/ 42");
    R_TEST((unsigned)42, "/**/ 42");
    R_TEST((unsigned)42, "// comment1\n// comment2\n42");
    R_TEST((unsigned)42, "// comment1\n/* comment2 */ 42");
    R_TEST((unsigned)42, "/* comment1 */// comment2\n 42");
    R_TEST((unsigned)42, "/* comment1 */\n// comment2\n 42");
    R_TEST((unsigned)42, "/* comment1 *//* comment2 */ 42");
    R_TEST((unsigned)42, "/* comment1 */\n/* comment2 */ 42");
    R_TEST((unsigned)42, "/", json::read_error::unexpected, 1);
    R_TEST((unsigned)42, "//", json::read_error::integral_invalid, 2);
    R_TEST((unsigned)42, "/*", json::read_error::integral_invalid, 2);
    R_TEST((unsigned)42, "/**", json::read_error::integral_invalid, 3);
    R_TEST((unsigned)42, "/**/", json::read_error::integral_invalid, 4);
    R_TEST(nullptr, "/null", json::read_error::unexpected, 1);
    R_TEST(true, "/true", json::read_error::unexpected, 1);
    R_TEST((unsigned)42, "/42", json::read_error::unexpected, 1);
    R_TEST((const char*)nullptr, "/ \"\"", json::read_error::unexpected, 1);
    R_TEST(enum_comments::x, R"(/x)", json::read_error::unexpected, 1);
    R_TEST(struct_comments {1, 2}, R"({"x":1/,"y":2})", json::read_error::unexpected, 7);
    {   int x = 42;
        R_TEST(&x, "/42", json::read_error::unexpected, 1);
    }
TEST_END()


TEST_BEG(single_quotes_tidy, cxon::JSON<>, "/core")
    TEST_CHECK(("[\n\t'xxx',\n\t'yyy'\n]" == cxon::json::tidy<XXON, std::string>(R"(['xxx', 'yyy'])")));
    TEST_CHECK(("[\n\t'x\\'x',\n\t'y\\'y'\n]" == cxon::json::tidy<XXON, std::string>(R"(['x\'x', 'y\'y'])")));
    TEST_CHECK(("[\n\t'x\"x',\n\t'y\"y'\n]" == cxon::json::tidy<XXON, std::string>(R"(['x"x', 'y"y'])")));
    TEST_CHECK(("[\n\t'x]x',\n\t'y]y'\n]" == cxon::json::tidy<XXON, std::string>(R"(['x]x', 'y]y'])")));
TEST_END()


TEST_BEG(map_div_sep_tidy, cxon::JSON<>, "/core")
    TEST_CHECK(("{\n\t'aaa' = 'bbb';\n\t'ccc' = 'ddd'\n}" == cxon::json::tidy<XXON, std::string>(R"({'aaa' = 'bbb'; 'ccc' = 'ddd'})")));
    TEST_CHECK(("{\n\t'aaa': 'bbb',\n\t'ccc': 'ddd'\n}" == cxon::json::tidy<XXON, std::string>(R"({'aaa': 'bbb', 'ccc': 'ddd'})")));
    TEST_CHECK(("{\n\t'aaa': 'bbb'\n\t'ccc' = 'ddd'\n}" == cxon::json::tidy<XXON, std::string>(R"({'aaa': 'bbb' 'ccc' = 'ddd'})")));
    TEST_CHECK(("[\n\ta,\n\tb;\n\tc\n\td\n]" == cxon::json::tidy<XXON, std::string>(R"([a, b; c d])")));
TEST_END()


namespace jsonrpc {

    // request

    template <typename T>
        struct napa { // named parameter
            char const*const key;
            T const value;

            template <typename X, typename O, typename Cx, typename Y = X>
                auto write_value(O& o, Cx& cx) const -> cxon::enable_for_t<Y, cxon::JSON> {
                    return cxon::cio::write_map_key<Y>(o, key, cx) && cxon::cio::write_map_val<Y>(o, value, cx);
                }
        };

    template <typename T>
        constexpr napa<T> make_napa(const char* k, T&& v) {
            return {k, v};
        }

    template <typename ...P>
        struct request {
            static char const*const jsonrpc;
            std::size_t const       id;
            char const*const        method;
            std::tuple<P...> const  params;

            constexpr request(std::size_t id, const char* method, P&&... params) noexcept
            :   id(id), method(method), params(std::forward<P>(params)...)
            {
            }

            CXON_JSON_CLS_WRITE_MEMBER(request,
                CXON_JSON_CLS_FIELD_ASIS(jsonrpc),
                CXON_JSON_CLS_FIELD_ASIS(id),
                CXON_JSON_CLS_FIELD_ASIS(method),
                CXON_JSON_CLS_FIELD_ASIS(params)
            )
        };
    template <typename ...P>
        char const*const request<P...>::jsonrpc = "2.0";

    template <typename ...P>
        constexpr request<P...> make_request(std::size_t id, const char* method, P&&... params) {
            return request<P...>(id, method, std::forward<P>(params)...);
        }
    template <typename ...P>
        constexpr request<napa<P>...> make_request(std::size_t id, const char* method, napa<P>&&... params) {
            return request<napa<P>...>(id, method, std::forward<napa<P>>(params)...);
        }

    // response

    template <typename D>
        struct error {
            int         code;
            std::string message;
            D           data;

            CXON_JSON_CLS_READ_MEMBER(error,
                CXON_JSON_CLS_FIELD_ASIS(code),
                CXON_JSON_CLS_FIELD_ASIS(message),
                CXON_JSON_CLS_FIELD_ASIS(data)
            )
        };

    template <typename R, typename D = cxon::cio::val::sink<>>
        struct response {
            char            jsonrpc[8];
            std::size_t     id;
            R               result;
            struct error<D> error;

            constexpr response() noexcept
            :   jsonrpc{0}, id(), result(), error()
            {
            }

            CXON_JSON_CLS_READ_MEMBER(response,
                CXON_JSON_CLS_FIELD_ASIS(jsonrpc),
                CXON_JSON_CLS_FIELD_ASIS(id),
                CXON_JSON_CLS_FIELD_ASIS(result),
                CXON_JSON_CLS_FIELD_ASIS(error)
            )
        };

}

namespace cxon { // json-rpc - serialize tuple of named parameters as a JSON object instead of an array

    template <typename X, typename ...T>
        struct write<JSON<X>, std::tuple<jsonrpc::napa<T>...>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::tuple<jsonrpc::napa<T>...>& t, Cx& cx) {
                    return  cio::poke<J>(o, J::map::beg, cx) &&
                                cio::cnt::write_tuple<J>(o, t, cx) &&
                            cio::poke<J>(o, J::map::end, cx)
                    ;
                }
        };

}

TEST_BEG(json_rpc, cxon::JSON<>, "/core") // json-rpc
    {   // params array
        auto const call = jsonrpc::make_request(1, "sub", 42, 23);
        std::string req; // serialize call to req
            auto const w = to_bytes(req, call);
        TEST_CHECK(w && req == R"({"jsonrpc":"2.0","id":1,"method":"sub","params":[42,23]})");
    }
    {   // params object
        auto const call = jsonrpc::make_request(1, "sub", jsonrpc::make_napa("x", 42), jsonrpc::make_napa("y", 23));
        std::string req; // serialize call to req
            auto const w = to_bytes(req, call);
        TEST_CHECK(w && req == R"({"jsonrpc":"2.0","id":1,"method":"sub","params":{"x":42,"y":23}})");
    }
    {   // round-trip: req -> res ok
        char const res[] = R"({"jsonrpc": "2.0", "result": 19, "id": 1})";
        jsonrpc::response<int> ret; // serialize res to ret
            auto const r = from_bytes(ret, res);
        TEST_CHECK(r && ret.id == 1 && ret.result == 19);
    }
    {   // round-trip: req -> res ko
        char const res[] = R"({
            "jsonrpc": "2.0",
            "error": {"code": 42, "message": "divide by zero", "data": "a black hole has been created somewhere"},
            "id": 1
        })";
        {   // serialize res to ret, error's data will be skipped
            jsonrpc::response<int> ret;
                auto const r = from_bytes(ret, res);
            TEST_CHECK( r &&
                        ret.id == 1 &&
                        ret.error.code == 42 &&
                        ret.error.message == "divide by zero"
            );
        }
        {   // serialize res to ret, error's data is bound to std::string
            jsonrpc::response<int, std::string> ret;
                auto const r = from_bytes(ret, res);
            TEST_CHECK( r &&
                        ret.id == 1 &&
                        ret.error.code == 42 &&
                        ret.error.message == "divide by zero" &&
                        ret.error.data == "a black hole has been created somewhere"
            );
        }
    }
TEST_END()

////////////////////////////////////////////////////////////////////////////////

int main() {
    using cxon::test::suite;
    for (auto& c : suite::info::get()) {
        for (auto s : c.second.suites)
            s->test();
        std::fprintf(stdout, "cxon/json%-12s: %u of %4u failed\n", c.first.c_str(), suite::info::errors(c.first.c_str()), suite::info::count(c.first.c_str())); std::fflush(stdout);
    }
    return suite::info::errors();
}
