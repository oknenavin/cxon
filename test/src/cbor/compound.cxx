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
TEST_END()
