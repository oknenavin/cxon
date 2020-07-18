// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon.hxx"

#include "cxon/std/array.hxx"
#include "cxon/std/vector.hxx"
#include "cxon/std/string.hxx"
#include "cxon/std/list.hxx"
#include "cxon/std/map.hxx"

#include <cstdio>

///////////////////////////////////////////////////////////////////////////////

TEST_BEG(cxon::CXON<>) // interface/read
    // iterator
    {   int r; char const i[] = "1";
        TEST_CHECK(cxon::from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    {   int r; std::string const i = "1";
        TEST_CHECK(cxon::from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    {   int r; std::vector<char> const i = {'1', '\0'};
        TEST_CHECK(cxon::from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    // container
    {   int r; char const i[] = "1";
        TEST_CHECK(cxon::from_bytes(r, i) && r == 1);
    }
    {   int r; std::string const i = "1";
        TEST_CHECK(cxon::from_bytes(r, i) && r == 1);
    }
    {   int r; std::vector<char> const i = {'1', '\0'};
        TEST_CHECK(cxon::from_bytes(r, i) && r == 1);
    }
    {   int r; std::array<char, 2> const i = {'1', '\0'};
        TEST_CHECK(cxon::from_bytes(r, i) && r == 1);
    }
TEST_END()

TEST_BEG(cxon::CXON<>) // interface/write
    // output iterator
    {   std::string r; std::string const e = QS("1");
        TEST_CHECK(cxon::to_bytes<XXON>(std::back_inserter(r), "1") && r == e);
    }
    {   std::string r; std::string const e = "1";
        TEST_CHECK(cxon::to_bytes<XXON>(std::back_inserter(r), 1) && r == e);
    }
    {   std::string r; std::string const e = "true";
        TEST_CHECK(cxon::to_bytes<XXON>(std::back_inserter(r), true) && r == e);
    }
    // range
    {   char o[16]; char const e[] = QS("1");
        auto const r = cxon::to_bytes<XXON>(std::begin(o), std::end(o), "1");
        TEST_CHECK(r && std::strncmp(o, e, r.end - std::begin(o)) == 0);
    }
        {   char o[1];
            auto const r = cxon::to_bytes<XXON>(std::begin(o), std::end(o), "1");
            TEST_CHECK(r.ec == cxon::write_error::output_failure);
        }
    {   char o[16]; char const e[] = "1";
        auto const r = cxon::to_bytes<XXON>(std::begin(o), std::end(o), 1);
        TEST_CHECK(r && std::strncmp(o, e, r.end - std::begin(o)) == 0);
    }
        {   char o[1];
            auto const r = cxon::to_bytes<XXON>(std::begin(o), std::end(o), 42);
            TEST_CHECK(r.ec == cxon::write_error::output_failure);
        }
    {   char o[16]; char const e[] = "true";
        auto const r = cxon::to_bytes<XXON>(std::begin(o), std::end(o), true);
        TEST_CHECK(r && std::strncmp(o, e, r.end - std::begin(o)) == 0);
    }
        {   char o[1];
            auto const r = cxon::to_bytes<XXON>(std::begin(o), std::end(o), true);
            TEST_CHECK(r.ec == cxon::write_error::output_failure);
        }
    // container/std::string (push_back, append)
    {   std::string r; std::string const e = QS("1");
        TEST_CHECK(cxon::to_bytes<XXON>(r, "1") && r == e);
    }
    {   std::string r; std::string const e = "1";
        TEST_CHECK(cxon::to_bytes<XXON>(r, 1) && r == e);
    }
    {   std::string r; std::string const e = "true";
        TEST_CHECK(cxon::to_bytes<XXON>(r, true) && r == e);
    }
    // container/std::vector (push_back)
    {   std::vector<char> r; std::vector<char> const e = {'"', '1', '"'};
        TEST_CHECK(cxon::to_bytes<XXON>(r, "1") && r == e);
    }
    {   std::vector<char> r; std::vector<char> const e = {'1'};
        TEST_CHECK(cxon::to_bytes<XXON>(r, 1) && r == e);
    }
    {   std::vector<char> r; std::vector<char> const e = {'t', 'r', 'u', 'e'};
        TEST_CHECK(cxon::to_bytes<XXON>(r, true) && r == e);
    }
TEST_END()


enum Enum11 { one, two, three, four };

CXON_ENUM(Enum11,
    CXON_ENUM_VALUE_ASIS(one),
    CXON_ENUM_VALUE_NAME("Two (2)", two),
    CXON_ENUM_VALUE_ASIS(three)
)

struct Struct11 {
    int field;
    Struct11(int f = 0) : field(f) {}
    bool operator ==(const Struct11& t) const { return field == t.field; }
    CXON_STRUCT_READ_MEMBER(Struct11,
        CXON_STRUCT_FIELD_ASIS(field)
    )
};

TEST_BEG(cxon::CXON<>) // interface/parameters
    {   std::string r; std::string const e = "3.142";
        TEST_CHECK(cxon::to_bytes<XXON>(r, 3.1415926, cxon::fp_precision::set<4>()) && r == e);
    }
    {   int *r = nullptr;
        TEST_CHECK(cxon::from_bytes<XXON>(r, "42", cxon::allocator::set(std::allocator<char>())) && *r == 42);
    }
    {   size_t r = 0;
        TEST_CHECK(cxon::from_bytes<XXON>(r, std::string("123"), cxon::num_len_max::set<4>()) && r == 123);
    }
    {   unsigned r = 0; std::string const i = "123";
        auto const e = cxon::from_bytes<XXON>(r, i, cxon::num_len_max::set<2>());
        TEST_CHECK(!e && e.ec == cxon::read_error::overflow && *e.end == '1');
    }
    {   double r = 0; std::list<char> const i = {'1', '2', '3'};
        TEST_CHECK(cxon::from_bytes<XXON>(r, i, cxon::num_len_max::set<4>()) && r == 123);
    }
    {   float r = 0; std::list<char> const i = {'1', '2', '3'};
        auto const e = cxon::from_bytes<XXON>(r, i, cxon::num_len_max::set<2>());
        TEST_CHECK(!e && e.ec == cxon::read_error::overflow && *e.end == '1');
    }
    {   Enum11 r = Enum11::one;
        TEST_CHECK(cxon::from_bytes<XXON>(r, std::string("three"), cxon::ids_len_max::set<6>()) && r == Enum11::three);
    }
    {   Enum11 r = Enum11::one; std::string const i = "three";
        auto const e = cxon::from_bytes<XXON>(r, i, cxon::ids_len_max::set<2>());
        TEST_CHECK(!e && e.ec == cxon::read_error::overflow && *e.end == 't');
    }
    {   Struct11 r(42); std::string const i = "{ field: 42 }";
        TEST_CHECK(cxon::from_bytes<XXON>(r, "{ field: 42 }", cxon::ids_len_max::set<6>()) && r == Struct11(42));
    }
    {   Struct11 r(42); std::string const i = "{ field: 42 }";
        auto const e = cxon::from_bytes<XXON>(r, i, cxon::ids_len_max::set<2>());
        TEST_CHECK(!e && e.ec == cxon::read_error::overflow && *e.end == 'f');
    }
TEST_END()

TEST_BEG(cxon::CXON<>) // errors
    using namespace cxon;
    {   std::error_condition ec;
            ec = read_error::ok;
                CXON_ASSERT(ec.category() == read_error_category::value(), "check failed");
                CXON_ASSERT(std::strcmp(ec.category().name(), "cxon/read") == 0, "check failed");
                CXON_ASSERT(ec.message() == "no error", "check failed");
            ec = read_error::unexpected;
                CXON_ASSERT(ec.message() == "unexpected input", "check failed");
            ec = read_error::character_invalid;
                CXON_ASSERT(ec.message() == "invalid character", "check failed");
            ec = read_error::integral_invalid;
                CXON_ASSERT(ec.message() == "invalid integral or value out of range", "check failed");
            ec = read_error::floating_point_invalid;
                CXON_ASSERT(ec.message() == "invalid floating point", "check failed");
            ec = read_error::boolean_invalid;
                CXON_ASSERT(ec.message() == "invalid boolean", "check failed");
            ec = read_error::escape_invalid;
                CXON_ASSERT(ec.message() == "invalid escape sequence", "check failed");
            ec = read_error::surrogate_invalid;
                CXON_ASSERT(ec.message() == "invalid surrogate", "check failed");
            ec = read_error::overflow;
                CXON_ASSERT(ec.message() == "buffer overflow", "check failed");
            ec = read_error(255);
                CXON_ASSERT(ec.message() == "unknown error", "check failed");
    }
    {   std::error_condition ec;
            ec = write_error::ok;
                CXON_ASSERT(ec.category() == write_error_category::value(), "check failed");
                CXON_ASSERT(std::strcmp(ec.category().name(), "cxon/write") == 0, "check failed");
                CXON_ASSERT(ec.message() == "no error", "check failed");
            ec = write_error::output_failure;
                CXON_ASSERT(ec.message() == "output cannot be written", "check failed");
            ec = write_error::argument_invalid;
                CXON_ASSERT(ec.message() == "invalid argument", "check failed");
            ec = write_error(255);
                CXON_ASSERT(ec.message() == "unknown error", "check failed");
    }
TEST_END()

TEST_BEG(cxon::CXON<>) // pretty
    {   std::map<std::string, std::vector<int>> const m = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} };
        char const s0[] =
            "{\n"
            "  even: {\n"
            "    2,\n"
            "    4,\n"
            "    6\n"
            "  },\n"
            "  odd: {\n"
            "    1,\n"
            "    3,\n"
            "    5\n"
            "  }\n"
            "}"
        ;
        std::string s1;
            cxon::to_bytes<XXON>(cxon::make_indenter<XXON>(s1, 2, ' '), m);
        TEST_CHECK(s1 == s0);
    }
    {   std::map<std::string, std::vector<int>> const m = { {"even", {2, 4, 6}}, {"odd", {1, 3, 5}} };
        std::string s1;
            cxon::to_bytes<XXON>(cxon::make_indenter(s1), m);
        std::string const s0 =
            cxon::pretty<XXON>(s1);
        TEST_CHECK(s1 == s0);
    }
    {   std::map<std::string, std::string> const m = { {"ala", "ba\"la"}, {"bl ah", "blah"} };
        char const s0[] =
            "{\n"
            "  ala: \"ba\\\"la\",\n"
            "  bl\\ ah: \"blah\"\n"
            "}"
        ;
        std::string s1;
            cxon::to_bytes<XXON>(cxon::make_indenter<XXON>(s1, 2, ' '), m);
        TEST_CHECK(s1 == s0);
    }
    {   std::map<std::string, std::string> const m = { {"ala", "ba\"la"}, {"bl ah", "blah"} };
        std::string s1;
            cxon::to_bytes<XXON>(cxon::make_indenter(s1), m);
        std::string const s0 =
            cxon::pretty<XXON>(s1);
        TEST_CHECK(s1 == s0);
    }
TEST_END()

///////////////////////////////////////////////////////////////////////////////

int main() {
    for (auto t : suite::get())
        t->test();
    suite::err() ?
        fprintf(stdout, "cxon: %u of %u failed\n", suite::err(), suite::all()) :
        fprintf(stdout, "cxon: %u of %u passed\n", suite::all(), suite::all())
    ;
    return suite::err();
}
