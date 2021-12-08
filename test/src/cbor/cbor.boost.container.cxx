// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "../test.hxx"

#include "cxon/lib/boost/container/stable_vector.hxx"
#include "cxon/lib/boost/container/small_vector.hxx"
#include "cxon/lib/boost/container/static_vector.hxx"

////////////////////////////////////////////////////////////////////////////////

TEST_BEG(stable_vector, cxon::CBOR<>, "/boost")
    using namespace boost::container;
    R_TEST(stable_vector<int>{}, BS("\x80"));
    R_TEST(stable_vector<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), stable_vector<int>{});
    R_TEST(stable_vector<int>{2, 1, 0}, BS("\x83\x02\x01\x00"));
    R_TEST(stable_vector<int>{2, 1, 0}, BS("\x9F\x02\x01\x00\xFF"));
    R_TEST(stable_vector<int>{2, 1, 0}, BS("\x43\x02\x01\x00"));
    R_TEST(stable_vector<int>{2, 1, 0}, BS("\x5F\x43\x02\x01\x00\xFF"));
    R_TEST(stable_vector<int>{2, 1, 0}, BS("\x5F\x42\x02\x01\x41\x00\xFF"));
    R_TEST(stable_vector<int>{2, 1, 0}, BS("\x7F\x61\x02\x62\x01\x00\xFF"));
    R_TEST(stable_vector<int>{2, 1, 0}, BS("\x7F\x63\x02\x01\x00\xFF"));
    R_TEST(stable_vector<int>{2, 1, 0}, BS("\x63\x02\x01\x00"));
    W_TEST(BS("\x83\x02\x01\x00"), stable_vector<int>{2, 1, 0});
    W_TEST(BS("\x43\x02\x01\x00"), stable_vector<unsigned char>{2, 1, 0});
    W_TEST(BS("\x63\x02\x01\x00"), stable_vector<char>{2, 1, 0});
TEST_END()


TEST_BEG(small_vector, cxon::CBOR<>, "/boost")
    using namespace boost::container;
    R_TEST(small_vector<int, 0>{}, BS("\x80"));
    R_TEST(small_vector<int, 4>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), small_vector<int, 0>{});
    R_TEST(small_vector<int, 1>{2, 1, 0}, BS("\x83\x02\x01\x00"));
    R_TEST(small_vector<int, 2>{2, 1, 0}, BS("\x9F\x02\x01\x00\xFF"));
    R_TEST(small_vector<int, 3>{2, 1, 0}, BS("\x43\x02\x01\x00"));
    R_TEST(small_vector<int, 4>{2, 1, 0}, BS("\x5F\x43\x02\x01\x00\xFF"));
    R_TEST(small_vector<int, 5>{2, 1, 0}, BS("\x5F\x42\x02\x01\x41\x00\xFF"));
    R_TEST(small_vector<int, 6>{2, 1, 0}, BS("\x7F\x61\x02\x62\x01\x00\xFF"));
    R_TEST(small_vector<int, 7>{2, 1, 0}, BS("\x7F\x63\x02\x01\x00\xFF"));
    R_TEST(small_vector<int, 8>{2, 1, 0}, BS("\x63\x02\x01\x00"));
    W_TEST(BS("\x83\x02\x01\x00"), small_vector<int, 1>{2, 1, 0});
    W_TEST(BS("\x43\x02\x01\x00"), small_vector<unsigned char, 2>{2, 1, 0});
    W_TEST(BS("\x63\x02\x01\x00"), small_vector<char, 3>{2, 1, 0});
TEST_END()


TEST_BEG(static_vector, cxon::CBOR<>, "/boost")
    using namespace boost::container;
    // std::static_vector<T, 0>
        R_TEST((static_vector<int, 0>{}), BS("\x80"));
        R_TEST((static_vector<int, 0>{}), BS("\x9F\xFF"));
        R_TEST((static_vector<int, 0>{}), BS("\x40"));
        R_TEST((static_vector<int, 0>{}), BS("\x5F\xFF"));
        R_TEST((static_vector<int, 0>{}), BS("\x60"));
        R_TEST((static_vector<int, 0>{}), BS("\x7F\xFF"));
        W_TEST("\x80", (static_vector<int, 0>{}));
        R_TEST((static_vector<char, 0>{}), BS("\x60"));
        R_TEST((static_vector<char, 0>{}), BS("\x7F\xFF"));
        R_TEST((static_vector<char, 0>{}), BS("\x7F\x60\xFF"));
        R_TEST((static_vector<char, 0>{}), BS("\x7F\x60\x60\xFF"));
        R_TEST((static_vector<char, 0>{}), BS("\x5F\x40\x40\xFF"));
        R_TEST((static_vector<char, 0>{}), BS("\x5F\x40\xFF"));
        R_TEST((static_vector<char, 0>{}), BS("\x5F\xFF"));
        R_TEST((static_vector<char, 0>{}), BS("\x40"));
        W_TEST("\x60", (static_vector<char, 0>{}));
        R_TEST((static_vector<unsigned char, 0>{}), BS("\x40"));
        R_TEST((static_vector<unsigned char, 0>{}), BS("\x5F\xFF"));
        R_TEST((static_vector<unsigned char, 0>{}), BS("\x5F\x40\xFF"));
        R_TEST((static_vector<unsigned char, 0>{}), BS("\x5F\x40\x40\xFF"));
        R_TEST((static_vector<unsigned char, 0>{}), BS("\x7F\x60\x60\xFF"));
        R_TEST((static_vector<unsigned char, 0>{}), BS("\x7F\x60\xFF"));
        R_TEST((static_vector<unsigned char, 0>{}), BS("\x7F\xFF"));
        R_TEST((static_vector<unsigned char, 0>{}), BS("\x60"));
        W_TEST("\x40", (static_vector<unsigned char, 0>{}));
            R_TEST((static_vector<int, 0>{}), BS("\x81\x00"), cbor::read_error::size_invalid, 0);
            R_TEST((static_vector<int, 0>{}), BS("\x9F\x00\xFF"), cbor::read_error::size_invalid, 0);
            R_TEST((static_vector<char, 0>{}), BS("\x41\x00"), cbor::read_error::size_invalid, 0);
            R_TEST((static_vector<char, 0>{}), BS("\x5F\x42\x02\x01\x41\x00\xFF"), cbor::read_error::size_invalid, 1);
            R_TEST((static_vector<char, 0>{}), BS("\x7F\x61\x02\x62\x01\x00\xFF"), cbor::read_error::size_invalid, 1);
            R_TEST((static_vector<char, 0>{}), BS("\x61\x00"), cbor::read_error::size_invalid, 0);
    // std::static_vector<T, n>
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x83\x02\x01\x00"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x9F\x02\x01\x00\xFF"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x63\x02\x01\x00"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x7F\x63\x02\x01\x00\xFF"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x7F\x62\x02\x01\x61\x00\xFF"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x5F\x42\x02\x01\x41\x00\xFF"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x5F\x43\x02\x01\x00\xFF"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x43\x02\x01\x00"));
        W_TEST(BS("\x83\x02\x01\x00"), (static_vector<int, 3>{2, 1, 0}));
        R_TEST((static_vector<int, 3>{2, 1}), BS("\x82\x02\x01")); // less
        R_TEST((static_vector<int, 3>{2, 1}), BS("\x9F\x02\x01\xFF")); // less
        W_TEST(BS("\x63\x02\x01\x00"), (static_vector<char, 3>{2, 1, 0}));
        W_TEST(BS("\x43\x02\x01\x00"), (static_vector<unsigned char, 3>{2, 1, 0}));
TEST_END()
