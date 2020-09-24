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
    // unsigned char
        R_TEST((unsigned char)23, BS("\x17"));
        R_TEST((unsigned char)24, BS("\x18\x18"));
        W_TEST(BS("\x17"), (unsigned char)23);
        W_TEST(BS("\x18\x18"), (unsigned char)24);
    // unsigned short
        R_TEST((unsigned short)23, BS("\x17"));
        R_TEST((unsigned short)24, BS("\x18\x18"));
        R_TEST((unsigned short)384, BS("\x19\x1\x80"));
    // unsigned int
        R_TEST((unsigned int)23, BS("\x17"));
        R_TEST((unsigned int)24, BS("\x18\x18"));
        R_TEST((unsigned int)384, BS("\x19\x1\x80"));
    // unsigned long
        R_TEST((unsigned long)23, BS("\x17"));
        R_TEST((unsigned long)24, BS("\x18\x18"));
        R_TEST((unsigned long)384, BS("\x19\x1\x80"));
    // unsigned long long
        R_TEST((unsigned long long)23, BS("\x17"));
        R_TEST((unsigned long long)24, BS("\x18\x18"));
        R_TEST((unsigned long long)384, BS("\x19\x1\x80"));
TEST_END()
