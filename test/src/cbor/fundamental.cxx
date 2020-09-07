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
        //R_TEST(false, "\xF6", cbor::read_error::boolean_invalid, 0);
        //R_TEST(false, "\xFF", cbor::read_error::boolean_invalid, 0);
        //R_TEST(false, "", cbor::read_error::boolean_invalid, 0);
    // nullptr_t
        R_TEST(nullptr, "\xF6");
        W_TEST(BS("\xF6"), nullptr);
        //R_TEST(nullptr, "", cbor::read_error::unexpected, 0);
        //R_TEST(nullptr, "\xF7", cbor::read_error::unexpected, 0);
        //R_TEST(nullptr, "\xFF", cbor::read_error::unexpected, 0);
TEST_END()
