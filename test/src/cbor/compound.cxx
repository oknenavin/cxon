// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "../test.hxx"

////////////////////////////////////////////////////////////////////////////////

TEST_BEG(cxon::CBOR<>)
    // T*
        {   int n = 0x01010101;
            R_TEST(&n, BS("\x1B\x01\x01\x01\x01"));
            W_TEST(BS("\x1B\x01\x01\x01\x01"), &n);
        }
        R_TEST((int*)nullptr, BS("\xF6"));
        W_TEST(BS("\xF6"), (int*)nullptr);
    // T[]
        {   unsigned a[] = {1, 2, 3};
            R_TEST(a, BS("\x83\x01\x02\x03"));
                R_TEST(a, BS("\xA3\x01\x02\x03"), cbor::read_error::size_invalid, 0);       // bad major type
                R_TEST(a, BS("\x82\x01\x02"), cbor::read_error::size_invalid, 0);           // bad size
                R_TEST(a, BS("\x84\x01\x02\x03\x04"), cbor::read_error::size_invalid, 0);   // bad size
                R_TEST(a, BS("\x83\x01\x02\xF5"), cbor::read_error::integer_invalid, 3);    // bad element type
            W_TEST(BS("\x83\x01\x02\x03"), a);
        }
        {   int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
            R_TEST(a, BS("\x98\x19\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\x18\x19"));
            W_TEST(BS("\x98\x19\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\x18\x19"), a);
        }
TEST_END()
