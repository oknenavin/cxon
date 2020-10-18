// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "../test.hxx"

////////////////////////////////////////////////////////////////////////////////

TEST_BEG(cxon::CBOR<>)
    using namespace test;
    // bool
        R_TEST(true, BS("\xF5"));
        W_TEST(BS("\xF5"), true);
        R_TEST(false, "\xF4");
        W_TEST(BS("\xF4"), false);
            R_TEST(false, BS("\xF6"), cbor::read_error::boolean_invalid, 0);
            R_TEST(false, BS("\xFF"), cbor::read_error::boolean_invalid, 0);
            R_TEST(false, BS(""), cbor::read_error::boolean_invalid, 0);
    // nullptr_t
        R_TEST(nullptr, BS("\xF6"));
        W_TEST(BS("\xF6"), nullptr);
            R_TEST(nullptr, BS(""), cbor::read_error::null_invalid, 0);
            R_TEST(nullptr, BS("\xF7"), cbor::read_error::null_invalid, 0);
            R_TEST(nullptr, BS("\xFF"), cbor::read_error::null_invalid, 0);
    // char
        R_TEST((char)0x00, BS("\x00"));
        R_TEST((char)0x17, BS("\x17"));
        R_TEST((char)0x18, BS("\x18\x18"));
        W_TEST(BS("\x18\x18"), (char)0x18);
        W_TEST(BS("\x17"), (char)0x17);
        W_TEST(BS("\x00"), (char)0x00);
    // char16_t
        R_TEST((char16_t)0x17, BS("\x17"));
        R_TEST((char16_t)0x18, BS("\x18\x18"));
        R_TEST((char16_t)0x0101, BS("\x19\x01\x01"));
        W_TEST(BS("\x19\x01\x01"), (char16_t)0x0101);
        W_TEST(BS("\x18\x18"), (char16_t)0x18);
        W_TEST(BS("\x17"), (char16_t)0x17);
    // char32_t
        R_TEST((char32_t)0x17, BS("\x17"));
        R_TEST((char32_t)0x18, BS("\x18\x18"));
        R_TEST((char32_t)0x0101, BS("\x19\x01\x01"));
        R_TEST((char32_t)0x010101, BS("\x1A\x01\x01\x01"));
        R_TEST((char32_t)0x01010101, BS("\x1B\x01\x01\x01\x01"));
        W_TEST(BS("\x1B\x01\x01\x01\x01"), (char32_t)0x01010101);
        W_TEST(BS("\x1A\x01\x01\x01"), (char32_t)0x010101);
        W_TEST(BS("\x19\x01\x01"), (char32_t)0x0101);
        W_TEST(BS("\x18\x18"), (char32_t)0x18);
        W_TEST(BS("\x17"), (char32_t)0x17);
    // wchar_t
        R_TEST((wchar_t)0x17, BS("\x17"));
        R_TEST((wchar_t)0x18, BS("\x18\x18"));
        R_TEST((wchar_t)0x0101, BS("\x19\x01\x01"));
        W_TEST(BS("\x19\x01\x01"), (wchar_t)0x0101);
        W_TEST(BS("\x18\x18"), (wchar_t)0x18);
        W_TEST(BS("\x17"), (wchar_t)0x17);
    // unsigned char
        R_TEST((unsigned char)0x00, BS("\x00"));
        R_TEST((unsigned char)0x17, BS("\x17"));
        R_TEST((unsigned char)0x18, BS("\x18\x18"));
        W_TEST(BS("\x18\x18"), (unsigned char)0x18);
        W_TEST(BS("\x17"), (unsigned char)0x17);
        W_TEST(BS("\x00"), (unsigned char)0x00);
    // signed char
        R_TEST((signed char)0x17, BS("\x17"));
        R_TEST((signed char)0x18, BS("\x18\x18"));
        W_TEST(BS("\x18\x18"), (signed char)0x18);
        W_TEST(BS("\x17"), (signed char)0x17);
        R_TEST((signed char)-0x18, BS("\x37"));
        R_TEST((signed char)-0x19, BS("\x38\x18"));
        W_TEST(BS("\x38\x18"), (signed char)-0x19);
        W_TEST(BS("\x37"), (signed char)-0x18);
    // unsigned short
        R_TEST((unsigned short)0x17, BS("\x17"));
        R_TEST((unsigned short)0x18, BS("\x18\x18"));
        R_TEST((unsigned short)0x0101, BS("\x19\x01\x01"));
        W_TEST(BS("\x19\x01\x01"), (unsigned short)0x0101);
        W_TEST(BS("\x18\x18"), (unsigned short)0x18);
        W_TEST(BS("\x17"), (unsigned short)0x17);
            R_TEST((unsigned short)0x00, BS(""), cbor::read_error::integer_invalid, 0);
            R_TEST((unsigned short)0x0101, BS("\x18"), cbor::read_error::integer_invalid, 0);
            R_TEST((unsigned short)0x0101, BS("\x19"), cbor::read_error::integer_invalid, 0);
            R_TEST((unsigned short)0x0101, BS("\x19\x01"), cbor::read_error::integer_invalid, 0);
            {   byte bs[2];
                auto const r = to_bytes<XXON>(std::begin(bs), std::end(bs), (unsigned short)0x0101);
                TEST_CHECK(r.ec == cbor::write_error::output_failure);
            }
    // short
        R_TEST((short)0x17, BS("\x17"));
        R_TEST((short)0x18, BS("\x18\x18"));
        R_TEST((short)0x0101, BS("\x19\x01\x01"));
        W_TEST(BS("\x19\x01\x01"), (short)0x0101);
        W_TEST(BS("\x18\x18"), (short)0x18);
        W_TEST(BS("\x17"), (short)0x17);
        R_TEST((short)-0x18, BS("\x37"));
        R_TEST((short)-0x19, BS("\x38\x18"));
        R_TEST((short)-0x0102, BS("\x39\x01\x01"));
        W_TEST(BS("\x39\x01\x01"), (short)-0x0102);
        W_TEST(BS("\x38\x18"), (short)-0x19);
        W_TEST(BS("\x37"), (short)-0x18);
    // unsigned int
        R_TEST((unsigned int)0x17, BS("\x17"));
        R_TEST((unsigned int)0x18, BS("\x18\x18"));
        R_TEST((unsigned int)0x0101, BS("\x19\x01\x01"));
        R_TEST((unsigned int)0x010101, BS("\x1A\x01\x01\x01"));
        R_TEST((unsigned int)0x01010101, BS("\x1B\x01\x01\x01\x01"));
        W_TEST(BS("\x1B\x01\x01\x01\x01"), (unsigned int)0x01010101);
        W_TEST(BS("\x1A\x01\x01\x01"), (unsigned int)0x010101);
        W_TEST(BS("\x19\x01\x01"), (unsigned int)0x0101);
        W_TEST(BS("\x18\x18"), (unsigned int)0x18);
        W_TEST(BS("\x17"), (unsigned int)0x17);
    // int
        R_TEST((int)0x17, BS("\x17"));
        R_TEST((int)0x18, BS("\x18\x18"));
        R_TEST((int)0x0101, BS("\x19\x01\x01"));
        R_TEST((int)0x010101, BS("\x1A\x01\x01\x01"));
        R_TEST((int)0x01010101, BS("\x1B\x01\x01\x01\x01"));
        W_TEST(BS("\x1B\x01\x01\x01\x01"), (int)0x01010101);
        W_TEST(BS("\x1A\x01\x01\x01"), (int)0x010101);
        W_TEST(BS("\x19\x01\x01"), (int)0x0101);
        W_TEST(BS("\x18\x18"), (int)0x18);
        W_TEST(BS("\x17"), (int)0x17);
        R_TEST((int)-0x18, BS("\x37"));
        R_TEST((int)-0x19, BS("\x38\x18"));
        R_TEST((int)-0x0102, BS("\x39\x01\x01"));
        R_TEST((int)-0x010102, BS("\x3A\x01\x01\x01"));
        R_TEST((int)-0x01010102, BS("\x3B\x01\x01\x01\x01"));
        W_TEST(BS("\x3B\x01\x01\x01\x01"), (int)-0x01010102);
        W_TEST(BS("\x3A\x01\x01\x01"), (int)-0x010102);
        W_TEST(BS("\x39\x01\x01"), (int)-0x0102);
        W_TEST(BS("\x38\x18"), (int)-0x19);
        W_TEST(BS("\x37"), (int)-0x18);
    // unsigned long
        R_TEST((unsigned long)0x17, BS("\x17"));
        R_TEST((unsigned long)0x18, BS("\x18\x18"));
        R_TEST((unsigned long)0x0101, BS("\x19\x01\x01"));
        R_TEST((unsigned long)0x010101, BS("\x1A\x01\x01\x01"));
        R_TEST((unsigned long)0x01010101, BS("\x1B\x01\x01\x01\x01"));
        W_TEST(BS("\x1B\x01\x01\x01\x01"), (unsigned long)0x01010101);
        W_TEST(BS("\x1A\x01\x01\x01"), (unsigned long)0x010101);
        W_TEST(BS("\x19\x01\x01"), (unsigned long)0x0101);
        W_TEST(BS("\x18\x18"), (unsigned long)0x18);
        W_TEST(BS("\x17"), (unsigned long)0x17);
    // long
        R_TEST((long)0x17, BS("\x17"));
        R_TEST((long)0x18, BS("\x18\x18"));
        R_TEST((long)0x0101, BS("\x19\x01\x01"));
        R_TEST((long)0x010101, BS("\x1A\x01\x01\x01"));
        R_TEST((long)0x01010101, BS("\x1B\x01\x01\x01\x01"));
        W_TEST(BS("\x1B\x01\x01\x01\x01"), (long)0x01010101);
        W_TEST(BS("\x1A\x01\x01\x01"), (long)0x010101);
        W_TEST(BS("\x19\x01\x01"), (long)0x0101);
        W_TEST(BS("\x18\x18"), (long)0x18);
        W_TEST(BS("\x17"), (long)0x17);
        R_TEST((long)-0x18, BS("\x37"));
        R_TEST((long)-0x19, BS("\x38\x18"));
        R_TEST((long)-0x0102, BS("\x39\x01\x01"));
        R_TEST((long)-0x010102, BS("\x3A\x01\x01\x01"));
        R_TEST((long)-0x01010102, BS("\x3B\x01\x01\x01\x01"));
        W_TEST(BS("\x3B\x01\x01\x01\x01"), (long)-0x01010102);
        W_TEST(BS("\x3A\x01\x01\x01"), (long)-0x010102);
        W_TEST(BS("\x39\x01\x01"), (long)-0x0102);
        W_TEST(BS("\x38\x18"), (long)-0x19);
        W_TEST(BS("\x37"), (long)-0x18);
    // unsigned long long
        R_TEST((unsigned long long)0x17, BS("\x17"));
        R_TEST((unsigned long long)0x18, BS("\x18\x18"));
        R_TEST((unsigned long long)0x0101, BS("\x19\x01\x01"));
        R_TEST((unsigned long long)0x010101, BS("\x1A\x01\x01\x01"));
        R_TEST((unsigned long long)0x01010101, BS("\x1B\x01\x01\x01\x01"));
        R_TEST((unsigned long long)0x0101010101, BS("\x1C\x01\x01\x01\x01\x01"));
        R_TEST((unsigned long long)0x010101010101, BS("\x1D\x01\x01\x01\x01\x01\x01"));
        R_TEST((unsigned long long)0x01010101010101, BS("\x1E\x01\x01\x01\x01\x01\x01\x01"));
        R_TEST((unsigned long long)0x0101010101010101, BS("\x1F\x01\x01\x01\x01\x01\x01\x01\x01"));
        W_TEST(BS("\x1F\x01\x01\x01\x01\x01\x01\x01\x01"), (unsigned long long)0x0101010101010101);
        W_TEST(BS("\x1E\x01\x01\x01\x01\x01\x01\x01"), (unsigned long long)0x01010101010101);
        W_TEST(BS("\x1D\x01\x01\x01\x01\x01\x01"), (unsigned long long)0x010101010101);
        W_TEST(BS("\x1C\x01\x01\x01\x01\x01"), (unsigned long long)0x0101010101);
        W_TEST(BS("\x1B\x01\x01\x01\x01"), (unsigned long long)0x01010101);
        W_TEST(BS("\x1A\x01\x01\x01"), (unsigned long long)0x010101);
        W_TEST(BS("\x19\x01\x01"), (unsigned long long)0x0101);
        W_TEST(BS("\x18\x18"), (unsigned long long)0x18);
        W_TEST(BS("\x17"), (unsigned long long)0x17);
    // long long
        R_TEST((long long)0x17, BS("\x17"));
        R_TEST((long long)0x18, BS("\x18\x18"));
        R_TEST((long long)0x0101, BS("\x19\x01\x01"));
        R_TEST((long long)0x010101, BS("\x1A\x01\x01\x01"));
        R_TEST((long long)0x01010101, BS("\x1B\x01\x01\x01\x01"));
        R_TEST((long long)0x0101010101, BS("\x1C\x01\x01\x01\x01\x01"));
        R_TEST((long long)0x010101010101, BS("\x1D\x01\x01\x01\x01\x01\x01"));
        R_TEST((long long)0x01010101010101, BS("\x1E\x01\x01\x01\x01\x01\x01\x01"));
        R_TEST((long long)0x0101010101010101, BS("\x1F\x01\x01\x01\x01\x01\x01\x01\x01"));
        W_TEST(BS("\x1F\x01\x01\x01\x01\x01\x01\x01\x01"), (long long)0x0101010101010101);
        W_TEST(BS("\x1E\x01\x01\x01\x01\x01\x01\x01"), (long long)0x01010101010101);
        W_TEST(BS("\x1D\x01\x01\x01\x01\x01\x01"), (long long)0x010101010101);
        W_TEST(BS("\x1C\x01\x01\x01\x01\x01"), (long long)0x0101010101);
        W_TEST(BS("\x1B\x01\x01\x01\x01"), (long long)0x01010101);
        W_TEST(BS("\x1A\x01\x01\x01"), (long long)0x010101);
        W_TEST(BS("\x19\x01\x01"), (long long)0x0101);
        W_TEST(BS("\x18\x18"), (long long)0x18);
        W_TEST(BS("\x17"), (long long)0x17);
        R_TEST((long long)-0x18, BS("\x37"));
        R_TEST((long long)-0x19, BS("\x38\x18"));
        R_TEST((long long)-0x0102, BS("\x39\x01\x01"));
        R_TEST((long long)-0x010102, BS("\x3A\x01\x01\x01"));
        R_TEST((long long)-0x01010102, BS("\x3B\x01\x01\x01\x01"));
        R_TEST((long long)-0x0101010102, BS("\x3C\x01\x01\x01\x01\x01"));
        R_TEST((long long)-0x010101010102, BS("\x3D\x01\x01\x01\x01\x01\x01"));
        R_TEST((long long)-0x01010101010102, BS("\x3E\x01\x01\x01\x01\x01\x01\x01"));
        R_TEST((long long)-0x0101010101010102, BS("\x3F\x01\x01\x01\x01\x01\x01\x01\x01"));
        W_TEST(BS("\x3F\x01\x01\x01\x01\x01\x01\x01\x01"), (long long)-0x0101010101010102);
        W_TEST(BS("\x3E\x01\x01\x01\x01\x01\x01\x01"), (long long)-0x01010101010102);
        W_TEST(BS("\x3D\x01\x01\x01\x01\x01\x01"), (long long)-0x010101010102);
        W_TEST(BS("\x3C\x01\x01\x01\x01\x01"), (long long)-0x0101010102);
        W_TEST(BS("\x3B\x01\x01\x01\x01"), (long long)-0x01010102);
        W_TEST(BS("\x3A\x01\x01\x01"), (long long)-0x010102);
        W_TEST(BS("\x39\x01\x01"), (long long)-0x0102);
        W_TEST(BS("\x38\x18"), (long long)-0x19);
        W_TEST(BS("\x37"), (long long)-0x18);
    // float
        R_TEST((float)0, BS("\xFA\x00\x00\x00\x00"));
        W_TEST(BS("\xFA\x00\x00\x00\x00"), (float)0);
        R_TEST((float)3.1415926, BS("\xFA\x40\x49\x0F\xDA"));
        W_TEST(BS("\xFA\x40\x49\x0F\xDA"), (float)3.1415926);
        //R_TEST((float)0, BS("\xFB\x00\x00\x00\x00\x00\x00\x00\x00")); // narrowing
        // from integral
        R_TEST((float)0x17, BS("\x17"));
        R_TEST((float)0x18, BS("\x18\x18"));
        R_TEST((float)0x0101, BS("\x19\x01\x01"));
        R_TEST((float)0x010101, BS("\x1A\x01\x01\x01"));
        R_TEST((float)0x01010101, BS("\x1B\x01\x01\x01\x01"));
        R_TEST((float)-0x01010102, BS("\x3B\x01\x01\x01\x01"));
        R_TEST((float)-0x010102, BS("\x3A\x01\x01\x01"));
        R_TEST((float)-0x0102, BS("\x39\x01\x01"));
        R_TEST((float)-0x19, BS("\x38\x18"));
        R_TEST((float)-0x18, BS("\x37"));
    // double
        R_TEST((double)0, BS("\xFB\x00\x00\x00\x00\x00\x00\x00\x00"));
        W_TEST(BS("\xFB\x00\x00\x00\x00\x00\x00\x00\x00"), (double)0);
        R_TEST((double)3.1415926, BS("\xFB\x40\x09\x21\xFB\x4D\x12\xD8\x4A"));
        W_TEST(BS("\xFB\x40\x09\x21\xFB\x4D\x12\xD8\x4A"), (double)3.1415926);
        R_TEST((double)0, BS("\xFA\x00\x00\x00\x00"));
        // from integral
        R_TEST((double)0x17, BS("\x17"));
        R_TEST((double)0x18, BS("\x18\x18"));
        R_TEST((double)0x0101, BS("\x19\x01\x01"));
        R_TEST((double)0x010101, BS("\x1A\x01\x01\x01"));
        R_TEST((double)0x01010101, BS("\x1B\x01\x01\x01\x01"));
        R_TEST((double)0x0101010101, BS("\x1C\x01\x01\x01\x01\x01"));
        R_TEST((double)0x010101010101, BS("\x1D\x01\x01\x01\x01\x01\x01"));
        R_TEST((double)0x01010101010101, BS("\x1E\x01\x01\x01\x01\x01\x01\x01"));
        R_TEST((double)0x0101010101010101, BS("\x1F\x01\x01\x01\x01\x01\x01\x01\x01"));
        R_TEST((double)-0x0101010101010102, BS("\x3F\x01\x01\x01\x01\x01\x01\x01\x01"));
        R_TEST((double)-0x01010101010102, BS("\x3E\x01\x01\x01\x01\x01\x01\x01"));
        R_TEST((double)-0x010101010102, BS("\x3D\x01\x01\x01\x01\x01\x01"));
        R_TEST((double)-0x0101010102, BS("\x3C\x01\x01\x01\x01\x01"));
        R_TEST((double)-0x01010102, BS("\x3B\x01\x01\x01\x01"));
        R_TEST((double)-0x010102, BS("\x3A\x01\x01\x01"));
        R_TEST((double)-0x0102, BS("\x39\x01\x01"));
        R_TEST((double)-0x19, BS("\x38\x18"));
        R_TEST((double)-0x18, BS("\x37"));
TEST_END()
