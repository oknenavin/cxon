// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"

#include "../test.hxx"

#include "cxon/lib/std/array.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/string_view.hxx"

////////////////////////////////////////////////////////////////////////////////

TEST_BEG(cxon::CBOR<>) // interface/read
    // iterator
    {   int r; bio::byte const i[] = { 0x01 };
        TEST_CHECK(from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    {   int r; test::bytes const i = BS("\x01");
        TEST_CHECK(from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    {   int r; std::vector<bio::byte> const i = { 0x01 };
        TEST_CHECK(from_bytes(r, std::begin(i), std::end(i)) && r == 1);
    }
    // container
    {   int r; bio::byte const i[] = { 0x01 };
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
    {   int r; test::bytes const i = BS("\x01");
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
#   ifdef CXON_HAS_LIB_STD_STRING_VIEW
    {   int r; std::basic_string_view<bio::byte> const i((bio::byte*)"\x01", 1);
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
#   endif
    {   int r; std::vector<bio::byte> const i = { 0x01 };
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
    {   int r; std::array<bio::byte, 1> const i = { 0x01 };
        TEST_CHECK(from_bytes(r, i) && r == 1);
    }
TEST_END()

TEST_BEG(cxon::CBOR<>) // interface/write
    // output iterator
    {   test::bytes r; test::bytes const e = BS("\x62\x01\x00");
        TEST_CHECK(to_bytes(std::back_inserter(r), "\x01") && r == e);
    }
    {   test::bytes r; test::bytes const e = BS("\x01");
        TEST_CHECK(to_bytes(std::back_inserter(r), 1) && r == e);
    }
    {   test::bytes r; test::bytes const e = BS("\xF5");
        TEST_CHECK(to_bytes(std::back_inserter(r), true) && r == e);
    }
    // range
    {   bio::byte o[16]; bio::byte const e[] = { 0x01 };
        auto const r = to_bytes(std::begin(o), std::end(o), 1);
        TEST_CHECK(r && std::memcmp(o, e, sizeof(e)) == 0);
    }
    {   bio::byte o[2]; bio::byte const e[] = { 0x18, 0x18 };
        auto const r = to_bytes(std::begin(o), std::end(o), 24);
        TEST_CHECK(r && std::memcmp(o, e, sizeof(e)) == 0);
    }
        {   bio::byte o[1];
            auto const r = to_bytes(std::begin(o), std::end(o), 24);
            TEST_CHECK(r.ec == cbor::write_error::output_failure);
        }
    {   bio::byte o[1]; bio::byte const e[] = { 0xF5 };
        auto const r = to_bytes(std::begin(o), std::end(o), true);
        TEST_CHECK(r && std::memcmp(o, e, sizeof(e)) == 0);
    }
    // container/std::string (push_back, append)
    {   test::bytes r; test::bytes const e = BS("\x64\x01\x02\x03\x00");
        TEST_CHECK(to_bytes(r, "\x01\x02\x03") && r == e);
    }
    {   test::bytes r; test::bytes const e = { 0xF5 };
        TEST_CHECK(to_bytes(r, true) && r == e);
    }
    // container/std::vector (push_back)
    {   std::vector<bio::byte> r; std::vector<bio::byte> const e = { 0x64, 0x01, 0x02, 0x03, 0x00 };
        TEST_CHECK(to_bytes(r, "\x01\x02\x03") && r == e);
    }
    {   std::vector<bio::byte> r; std::vector<bio::byte> const e = { 0xF5 };
        TEST_CHECK(to_bytes(r, true) && r == e);
    }
TEST_END()

TEST_BEG(cxon::CBOR<>) // interface/parameters
TEST_END()

TEST_BEG(cxon::CBOR<>) // errors
    using namespace cxon;
    {   std::error_condition ec;
            ec = cbor::read_error::ok;
                TEST_CHECK(ec.category() == cbor::read_error_category::value());
                TEST_CHECK(std::strcmp(ec.category().name(), "cxon/cbor/read") == 0);
                TEST_CHECK(ec.message() == "no error");
            ec = cbor::read_error::unexpected;
                TEST_CHECK(ec.message() == "unexpected input");
            ec = cbor::read_error::boolean_invalid;
                TEST_CHECK(ec.message() == "invalid boolean");
            ec = cbor::read_error::null_invalid;
                TEST_CHECK(ec.message() == "invalid null");
            ec = cbor::read_error::integer_invalid;
                TEST_CHECK(ec.message() == "invalid integer");
            ec = cbor::read_error::floating_point_invalid;
                TEST_CHECK(ec.message() == "invalid floating-point");
            ec = cbor::read_error::size_invalid;
                TEST_CHECK(ec.message() == "invalid size");
            ec = cbor::read_error::array_invalid;
                TEST_CHECK(ec.message() == "invalid array");
            ec = cbor::read_error::tag_invalid;
                TEST_CHECK(ec.message() == "invalid tag");
            ec = cbor::read_error(255);
                TEST_CHECK(ec.message() == "unknown error");
    }
    {   std::error_condition ec;
            ec = cbor::write_error::ok;
                TEST_CHECK(ec.category() == cbor::write_error_category::value());
                TEST_CHECK(std::strcmp(ec.category().name(), "cxon/cbor/write") == 0);
                TEST_CHECK(ec.message() == "no error");
            ec = cbor::write_error::output_failure;
                TEST_CHECK(ec.message() == "output cannot be written");
            ec = cbor::write_error::argument_invalid;
                TEST_CHECK(ec.message() == "invalid argument");
            ec = cbor::write_error(255);
                TEST_CHECK(ec.message() == "unknown error");
    }
TEST_END()


////////////////////////////////////////////////////////////////////////////////

int main() {
    using cxon::test::suite;
    for (auto t : suite::get())
        t->test();
    fprintf(stdout, "cxon/cbor: %u of %4u failed\n", suite::err(), suite::all()); fflush(stdout);
    return suite::err();
}
