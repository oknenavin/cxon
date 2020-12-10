// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "../test.hxx"

#include "cxon/lib/std/array.hxx"

////////////////////////////////////////////////////////////////////////////////

TEST_BEG(cxon::CBOR<>)
    using namespace std;
    // std::array<T, 0>
        R_TEST((array<int, 0>{}), BS("\x80"));
        R_TEST((array<int, 0>{}), BS("\x9F\xFF"));
        R_TEST((array<int, 0>{}), BS("\x40"));
        R_TEST((array<int, 0>{}), BS("\x5F\xFF"));
        R_TEST((array<int, 0>{}), BS("\x60"));
        R_TEST((array<int, 0>{}), BS("\x7F\xFF"));
        W_TEST("\x80", (array<int, 0>{}));
        R_TEST((array<char, 0>{}), BS("\x60"));
        R_TEST((array<char, 0>{}), BS("\x7F\xFF"));
        R_TEST((array<char, 0>{}), BS("\x7F\x60\xFF"));
        R_TEST((array<char, 0>{}), BS("\x7F\x60\x60\xFF"));
        R_TEST((array<char, 0>{}), BS("\x5F\x40\x40\xFF"));
        R_TEST((array<char, 0>{}), BS("\x5F\x40\xFF"));
        R_TEST((array<char, 0>{}), BS("\x5F\xFF"));
        R_TEST((array<char, 0>{}), BS("\x40"));
        W_TEST("\x60", (array<char, 0>{}));
        R_TEST((array<unsigned char, 0>{}), BS("\x40"));
        R_TEST((array<unsigned char, 0>{}), BS("\x5F\xFF"));
        R_TEST((array<unsigned char, 0>{}), BS("\x5F\x40\xFF"));
        R_TEST((array<unsigned char, 0>{}), BS("\x5F\x40\x40\xFF"));
        R_TEST((array<unsigned char, 0>{}), BS("\x7F\x60\x60\xFF"));
        R_TEST((array<unsigned char, 0>{}), BS("\x7F\x60\xFF"));
        R_TEST((array<unsigned char, 0>{}), BS("\x7F\xFF"));
        R_TEST((array<unsigned char, 0>{}), BS("\x60"));
        W_TEST("\x40", (array<unsigned char, 0>{}));
            R_TEST((array<int, 0>{}), BS("\x81\x00"), cbor::read_error::size_invalid, 0);
    // std::array<T, n>
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x83\x02\x01\x00"));
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x9F\x02\x01\x00\xFF"));
        W_TEST(BS("\x83\x02\x01\x00"), (array<int, 3>{2, 1, 0}));
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x82\x02\x01")); // less
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x9F\x02\x01\xFF")); // less
TEST_END()
