// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "../test.hxx"

////////////////////////////////////////////////////////////////////////////////

TEST_BEG(cxon::CBOR<>) // interface/read
TEST_END()

TEST_BEG(cxon::CBOR<>) // interface/write
TEST_END()

TEST_BEG(cxon::CBOR<>) // interface/parameters
TEST_END()

TEST_BEG(cxon::CBOR<>) // errors
TEST_END()


////////////////////////////////////////////////////////////////////////////////

int main() {
    using cxon::test::suite;
    for (auto t : suite::get())
        t->test();
    suite::err() ?
        fprintf(stdout, "cxon/cbor: %u of %u failed\n", suite::err(), suite::all()) :
        fprintf(stdout, "cxon/cbor: %u of %u passed\n", suite::all(), suite::all())
    ;
    return suite::err();
}
