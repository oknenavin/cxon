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
                CXON_ASSERT(ec.category() == cbor::read_error_category::value(), "check failed");
                CXON_ASSERT(std::strcmp(ec.category().name(), "cxon/cbor/read") == 0, "check failed");
                CXON_ASSERT(ec.message() == "no error", "check failed");
            ec = cbor::read_error::unexpected;
                CXON_ASSERT(ec.message() == "unexpected input", "check failed");
            ec = cbor::read_error::boolean_invalid;
                CXON_ASSERT(ec.message() == "invalid boolean", "check failed");
            ec = cbor::read_error::null_invalid;
                CXON_ASSERT(ec.message() == "invalid null", "check failed");
            ec = cbor::read_error::integer_invalid;
                CXON_ASSERT(ec.message() == "invalid integer", "check failed");
            ec = cbor::read_error::floating_point_invalid;
                CXON_ASSERT(ec.message() == "invalid floating-point", "check failed");
            ec = cbor::read_error::size_invalid;
                CXON_ASSERT(ec.message() == "invalid size", "check failed");
            ec = cbor::read_error::array_invalid;
                CXON_ASSERT(ec.message() == "invalid array", "check failed");
            ec = cbor::read_error::tag_invalid;
                CXON_ASSERT(ec.message() == "invalid tag", "check failed");
            ec = cbor::read_error(255);
                CXON_ASSERT(ec.message() == "unknown error", "check failed");
    }
    {   std::error_condition ec;
            ec = cbor::write_error::ok;
                CXON_ASSERT(ec.category() == cbor::write_error_category::value(), "check failed");
                CXON_ASSERT(std::strcmp(ec.category().name(), "cxon/cbor/write") == 0, "check failed");
                CXON_ASSERT(ec.message() == "no error", "check failed");
            ec = cbor::write_error::output_failure;
                CXON_ASSERT(ec.message() == "output cannot be written", "check failed");
            ec = cbor::write_error::argument_invalid;
                CXON_ASSERT(ec.message() == "invalid argument", "check failed");
            ec = cbor::write_error(255);
                CXON_ASSERT(ec.message() == "unknown error", "check failed");
    }
TEST_END()


////////////////////////////////////////////////////////////////////////////////

int main() {
    using cxon::test::suite;
    for (auto t : suite::get())
        t->test();
    suite::err() ?
        // coverity[ copy_paste_error ]
        fprintf(stdout, "cxon/cbor: %u of %u failed\n", suite::err(), suite::all()) :
        fprintf(stdout, "cxon/cbor: %u of %u passed\n", suite::all(), suite::all())
    ;
    return suite::err();
}
