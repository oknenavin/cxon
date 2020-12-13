// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "../test.hxx"

#include "cxon/lib/std/array.hxx"
#include "cxon/lib/std/chrono.hxx"
#include "cxon/lib/std/complex.hxx"

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
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x63\x02\x01\x00"));
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x7F\x63\x02\x01\x00\xFF"));
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x7F\x62\x02\x01\x61\x00\xFF"));
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x5F\x42\x02\x01\x41\x00\xFF"));
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x5F\x43\x02\x01\x00\xFF"));
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x43\x02\x01\x00"));
        W_TEST(BS("\x83\x02\x01\x00"), (array<int, 3>{2, 1, 0}));
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x82\x02\x01")); // less
        R_TEST((array<int, 3>{2, 1, 0}), BS("\x9F\x02\x01\xFF")); // less
        W_TEST(BS("\x63\x02\x01\x00"), (array<char, 3>{2, 1, 0}));
        W_TEST(BS("\x43\x02\x01\x00"), (array<unsigned char, 3>{2, 1, 0}));
TEST_END()


TEST_BEG(cxon::CBOR<>) // std::chrono
    using namespace std::chrono;
    R_TEST(duration<unsigned>(42), BS("\x18\x2A"));
    R_TEST(duration<unsigned>(42), BS("\x1C"), cbor::read_error::integer_invalid, 0);
    W_TEST(BS("\x18\x2A"), duration<unsigned>(42));
    R_TEST(time_point<system_clock>(system_clock::duration(42)), BS("\x18\x2A"));
    R_TEST(time_point<system_clock>(system_clock::duration(42)), BS("\x1C"), cbor::read_error::integer_invalid, 0);
    W_TEST(BS("\x18\x2A"), time_point<system_clock>(system_clock::duration(42)));
TEST_END()


TEST_BEG(cxon::CBOR<>) // std::complex
    using namespace std;
    R_TEST(complex<float>(), BS("\x82\x00\x00"));
    R_TEST(complex<float>(), BS("\x82\xFA\x00\x00\x00\x00\xFA\x00\x00\x00\x00"));
    R_TEST(complex<float>(1, 2), BS("\x82\xFA\x3F\x80\x00\x00\xFA\x40\x00\x00\x00"));
    R_TEST(complex<float>(), BS("\x80"), cbor::read_error::size_invalid, 0);
    R_TEST(complex<float>(), BS("\x81"), cbor::read_error::size_invalid, 0);
    R_TEST(complex<float>(), BS("\x83"), cbor::read_error::size_invalid, 0);
    R_TEST(complex<float>(), BS("\x82"), cbor::read_error::floating_point_invalid, 1);
    R_TEST(complex<float>(), BS("\x82\x00"), cbor::read_error::floating_point_invalid, 2);
    W_TEST(BS("\x82\xFA\x00\x00\x00\x00\xFA\x00\x00\x00\x00"), complex<float>());
    W_TEST(BS("\x82\xFA\x3F\x80\x00\x00\xFA\x40\x00\x00\x00"), complex<float>(1, 2));
    R_TEST(complex<double>(), BS("\x82\x00\x00"));
    R_TEST(complex<double>(), BS("\x82\xFB\x00\x00\x00\x00\x00\x00\x00\x00\xFB\x00\x00\x00\x00\x00\x00\x00\x00"));
    W_TEST(BS("\x82\xFB\x00\x00\x00\x00\x00\x00\x00\x00\xFB\x00\x00\x00\x00\x00\x00\x00\x00"), complex<double>());
    R_TEST(complex<double>(1, 2), BS("\x82\xFB\x3F\xF0\x00\x00\x00\x00\x00\x00\xFB\x40\x00\x00\x00\x00\x00\x00\x00"));
    W_TEST(BS("\x82\xFB\x3F\xF0\x00\x00\x00\x00\x00\x00\xFB\x40\x00\x00\x00\x00\x00\x00\x00"), complex<double>(1, 2));
TEST_END()
