// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "../test.hxx"

#include "cxon/lib/std/array.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/string_view.hxx"
#include "cxon/lib/std/list.hxx"
#include "cxon/lib/std/map.hxx"

#include "../pretty.hxx"

#include <cstdio>

///////////////////////////////////////////////////////////////////////////////

TEST_BEG(cxon::JSON<>) // interface/read
    // iterator
    {   int r; char const i[] = "1";
        TEST_CHECK(from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    {   int r; std::string const i = "1";
        TEST_CHECK(from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    {   int r; std::vector<char> const i = {'1', '\0'};
        TEST_CHECK(from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    // container
    {   int r; char const i[] = "1";
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
    {   int r; std::string const i = "1";
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
#   ifdef CXON_HAS_LIB_STD_STRING_VIEW
    {   int r; std::string_view const i("1", 1);
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
#   endif
    {   int r; std::vector<char> const i = {'1', '\0'};
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
    {   int r; std::array<char, 2> const i = {'1', '\0'};
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
TEST_END()

TEST_BEG(cxon::JSON<>) // interface/write
    // output iterator
    {   std::string r; std::string const e = QS("1");
        TEST_CHECK(to_bytes<XXON>(std::back_inserter(r), "1") && r == e);
    }
    {   std::string r; std::string const e = "1";
        TEST_CHECK(to_bytes<XXON>(std::back_inserter(r), 1) && r == e);
    }
    {   std::string r; std::string const e = "true";
        TEST_CHECK(to_bytes<XXON>(std::back_inserter(r), true) && r == e);
    }
    // range
    {   char o[16]; char const e[] = QS("1");
        auto const r = to_bytes<XXON>(std::begin(o), std::end(o), "1");
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
    {   char o[3]; char const e[] = QS("1");
        auto const r = to_bytes<XXON>(std::begin(o), std::end(o), "1");
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
        {   char o[1];
            auto const r = to_bytes<XXON>(std::begin(o), std::end(o), "42");
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
    {   char o[16]; char const e[] = "1";
        auto const r = to_bytes<XXON>(std::begin(o), std::end(o), 1);
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
    {   char o[2]; char const e[] = "42";
        auto const r = to_bytes<XXON>(std::begin(o), std::end(o), 42);
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
        {   char o[1];
            auto const r = to_bytes<XXON>(std::begin(o), std::end(o), 42);
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
    {   char o[16]; char const e[] = "true";
        auto const r = to_bytes<XXON>(std::begin(o), std::end(o), true);
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
    {   char o[4]; char const e[] = "true";
        auto const r = to_bytes<XXON>(std::begin(o), std::end(o), true);
        TEST_CHECK(r && std::memcmp(o, e, std::strlen(e)) == 0);
    }
        {   char o[1];
            auto const r = to_bytes<XXON>(std::begin(o), std::end(o), true);
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
    // container/std::string (push_back, append)
    {   std::string r; std::string const e = QS("1");
        TEST_CHECK(to_bytes<XXON>(r, "1") && r == e);
    }
    {   std::string r; std::string const e = "1";
        TEST_CHECK(to_bytes<XXON>(r, 1) && r == e);
    }
    {   std::string r; std::string const e = "true";
        TEST_CHECK(to_bytes<XXON>(r, true) && r == e);
    }
    // container/std::vector (push_back)
    {   std::vector<char> r; std::vector<char> const e = {'"', '1', '"'};
        TEST_CHECK(to_bytes<XXON>(r, "1") && r == e);
    }
    {   std::vector<char> r; std::vector<char> const e = {'1'};
        TEST_CHECK(to_bytes<XXON>(r, 1) && r == e);
    }
    {   std::vector<char> r; std::vector<char> const e = {'t', 'r', 'u', 'e'};
        TEST_CHECK(to_bytes<XXON>(r, true) && r == e);
    }
TEST_END()


enum Enum11 { one, two, three, four };

CXON_ENUM(Enum11,
    CXON_JSON_ENUM_VALUE_ASIS(one),
    CXON_JSON_ENUM_VALUE_NAME("Two (2)", two),
    CXON_JSON_ENUM_VALUE_ASIS(three)
)

struct Struct11 {
    int field;
    Struct11(int f = 0) : field(f) {}
    bool operator ==(const Struct11& t) const { return field == t.field; }
    CXON_JSON_CLS_READ_MEMBER(Struct11,
        CXON_JSON_CLS_FIELD_ASIS(field)
    )
};

TEST_BEG(cxon::JSON<>) // interface/parameters
    {   std::string r; std::string const e = "3.142";
        TEST_CHECK(to_bytes<XXON>(r, 3.1415926, json::fp_precision::set<4>()) && r == e);
    }
    {   int *r = nullptr;
        TEST_CHECK(from_bytes<XXON>(r, "42", json::allocator::set(std::allocator<char>())) && *r == 42);
    }
    {   size_t r = 0;
        TEST_CHECK(from_bytes<XXON>(r, std::string("123"), json::num_len_max::set<4>()) && r == 123);
    }
    {   unsigned r = 0; std::string const i = "123";
        auto ib = test::make_force_input_iterator(i.begin()), ie = test::make_force_input_iterator(i.end());
        auto const e = from_bytes<XXON>(r, ib, ie, json::num_len_max::set<2>());
        TEST_CHECK(!e && e.ec == json::read_error::overflow && *e.end == '3');
    }
    {   double r = 0; std::list<char> const i = {'1', '2', '3'};
        TEST_CHECK(from_bytes<XXON>(r, i, json::num_len_max::set<4>()) && r == 123);
    }
    {   float r = 0; std::list<char> const i = {'1', '2', '3'};
        auto const e = from_bytes<XXON>(r, i, json::num_len_max::set<2>());
        TEST_CHECK(!e && e.ec == json::read_error::overflow && *e.end == '1');
    }
    {   Enum11 r = Enum11::one;
        TEST_CHECK(from_bytes<XXON>(r, QS("three"), json::ids_len_max::set<8>()) && r == Enum11::three);
    }
    {   Enum11 r = Enum11::one; std::string const i = QS("three");
        auto const e = from_bytes<XXON>(r, i, json::ids_len_max::set<2>());
        TEST_CHECK(!e && e.ec == json::read_error::overflow && *e.end == '"');
    }
    {   Struct11 r(42);
        TEST_CHECK(from_bytes<XXON>(r, "{ \"field\": 42 }", json::ids_len_max::set<8>()) && r == Struct11(42));
    }
    {   Struct11 r(42);
        auto const e = from_bytes<XXON>(r, "{ \"field\": 42 }", json::ids_len_max::set<2>());
        TEST_CHECK(!e && e.ec == json::read_error::overflow && *e.end == '"');
    }
TEST_END()

TEST_BEG(cxon::JSON<>) // errors
    using namespace cxon;
    {   std::error_condition ec;
            ec = json::read_error::ok;
                CXON_ASSERT(ec.category() == json::read_error_category::value(), "check failed");
                CXON_ASSERT(std::strcmp(ec.category().name(), "cxon/chio/read") == 0, "check failed");
                CXON_ASSERT(ec.message() == "no error", "check failed");
            ec = json::read_error::unexpected;
                CXON_ASSERT(ec.message() == "unexpected input", "check failed");
            ec = json::read_error::character_invalid;
                CXON_ASSERT(ec.message() == "invalid character", "check failed");
            ec = json::read_error::integral_invalid;
                CXON_ASSERT(ec.message() == "invalid integral or value out of range", "check failed");
            ec = json::read_error::floating_point_invalid;
                CXON_ASSERT(ec.message() == "invalid floating point", "check failed");
            ec = json::read_error::boolean_invalid;
                CXON_ASSERT(ec.message() == "invalid boolean", "check failed");
            ec = json::read_error::escape_invalid;
                CXON_ASSERT(ec.message() == "invalid escape sequence", "check failed");
            ec = json::read_error::surrogate_invalid;
                CXON_ASSERT(ec.message() == "invalid surrogate", "check failed");
            ec = json::read_error::overflow;
                CXON_ASSERT(ec.message() == "buffer overflow", "check failed");
            ec = json::read_error(255);
                CXON_ASSERT(ec.message() == "unknown error", "check failed");
    }
    {   std::error_condition ec;
            ec = json::write_error::ok;
                CXON_ASSERT(ec.category() == json::write_error_category::value(), "check failed");
                CXON_ASSERT(std::strcmp(ec.category().name(), "cxon/chio/write") == 0, "check failed");
                CXON_ASSERT(ec.message() == "no error", "check failed");
            ec = json::write_error::output_failure;
                CXON_ASSERT(ec.message() == "output cannot be written", "check failed");
            ec = json::write_error::argument_invalid;
                CXON_ASSERT(ec.message() == "invalid argument", "check failed");
            ec = json::write_error(255);
                CXON_ASSERT(ec.message() == "unknown error", "check failed");
    }
TEST_END()

TEST_BEG(cxon::JSON<>) // pretty
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
            to_bytes<XXON>(test::make_indenter<XXON>(s1, 2, ' '), m);
        TEST_CHECK(s1 == s0);
    }
    {   std::map<std::string, std::vector<int>> const m = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} };
        std::string s1;
            to_bytes<XXON>(test::make_indenter(s1), m);
        std::string const s0 =
            test::pretty<XXON>(s1);
        TEST_CHECK(s1 == s0);
    }
    {   std::map<std::string, std::string> const m = { {"ala", "ba\"la"}, {"bl ah", "blah"} };
        char const s0[] =
            "{\n"
            "  \"ala\": \"ba\\\"la\",\n"
            "  \"bl ah\": \"blah\"\n"
            "}"
        ;
        std::string s1;
            to_bytes<XXON>(test::make_indenter<XXON>(s1, 2, ' '), m);
        TEST_CHECK(s1 == s0);
    }
    {   std::map<std::string, std::string> const m = { {"ala", "ba\"la"}, {"bl ah", "blah"} };
        std::string s1;
            to_bytes<XXON>(test::make_indenter(s1), m);
        std::string const s0 =
            test::pretty<XXON>(s1);
        TEST_CHECK(s1 == s0);
    }
TEST_END()

///////////////////////////////////////////////////////////////////////////////

int main() {
    using cxon::test::suite;
    for (auto t : suite::get())
        t->test();
    suite::err() ?
        fprintf(stdout, "cxon/json: %u of %u failed\n", suite::err(), suite::all()) :
        fprintf(stdout, "cxon/json: %u of %u passed\n", suite::all(), suite::all())
    ;
    return suite::err();
}
