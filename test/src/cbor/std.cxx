// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "../test.hxx"

#include "cxon/lib/std/array.hxx"
#include "cxon/lib/std/chrono.hxx"
#include "cxon/lib/std/complex.hxx"
#include "cxon/lib/std/forward_list.hxx"
#include "cxon/lib/std/list.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/utility.hxx"
#include "cxon/lib/std/variant.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/tuple.hxx"

////////////////////////////////////////////////////////////////////////////////

using namespace std;

TEST_BEG(cxon::CBOR<>) // std::array
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


TEST_BEG(cxon::CBOR<>) // std::forward_list
    R_TEST(forward_list<int>{2, 1, 0}, BS("\x83\x02\x01\x00"));
    R_TEST(forward_list<int>{2, 1, 0}, BS("\x9F\x02\x01\x00\xFF"));
    R_TEST(forward_list<int>{2, 1, 0}, BS("\x43\x02\x01\x00"));
    R_TEST(forward_list<int>{2, 1, 0}, BS("\x5F\x43\x02\x01\x00\xFF"));
    R_TEST(forward_list<int>{2, 1, 0}, BS("\x5F\x42\x02\x01\x41\x00\xFF"));
    R_TEST(forward_list<int>{2, 1, 0}, BS("\x7F\x61\x02\x62\x01\x00\xFF"));
    R_TEST(forward_list<int>{2, 1, 0}, BS("\x7F\x63\x02\x01\x00\xFF"));
    R_TEST(forward_list<int>{2, 1, 0}, BS("\x63\x02\x01\x00"));
    W_TEST(BS("\x83\x02\x01\x00"), forward_list<int>{2, 1, 0});
    W_TEST(BS("\x43\x02\x01\x00"), forward_list<unsigned char>{2, 1, 0});
    W_TEST(BS("\x63\x02\x01\x00"), forward_list<char>{2, 1, 0});
TEST_END()


TEST_BEG(cxon::CBOR<>) // std::list
    R_TEST(list<int>{2, 1, 0}, BS("\x83\x02\x01\x00"));
    R_TEST(list<int>{2, 1, 0}, BS("\x9F\x02\x01\x00\xFF"));
    R_TEST(list<int>{2, 1, 0}, BS("\x43\x02\x01\x00"));
    R_TEST(list<int>{2, 1, 0}, BS("\x5F\x43\x02\x01\x00\xFF"));
    R_TEST(list<int>{2, 1, 0}, BS("\x5F\x42\x02\x01\x41\x00\xFF"));
    R_TEST(list<int>{2, 1, 0}, BS("\x7F\x61\x02\x62\x01\x00\xFF"));
    R_TEST(list<int>{2, 1, 0}, BS("\x7F\x63\x02\x01\x00\xFF"));
    R_TEST(list<int>{2, 1, 0}, BS("\x63\x02\x01\x00"));
    W_TEST(BS("\x83\x02\x01\x00"), list<int>{2, 1, 0});
    W_TEST(BS("\x43\x02\x01\x00"), list<unsigned char>{2, 1, 0});
    W_TEST(BS("\x63\x02\x01\x00"), list<char>{2, 1, 0});
TEST_END()


#ifdef CXON_HAS_LIB_STD_OPTIONAL
    TEST_BEG(cxon::CBOR<>) // std::optional
        R_TEST(optional<int>(42), BS("\x18\x2A"));
        W_TEST(BS("\x18\x2A"), optional<int>(42));
        R_TEST(optional<int>(), BS("\xF6"));
        W_TEST(BS("\xF6"), optional<int>());
    TEST_END()
#endif


TEST_BEG(cxon::CBOR<>) // std::pair
    R_TEST(pair<int, float>(1, 2.f), BS("\x82\x01\x02"));
    R_TEST(pair<int, float>(1, 2.f), BS("\x82\x01\xFA\x40\x00\x00\x00"));
    W_TEST(BS("\x82\x01\xFA\x40\x00\x00\x00"), pair<int, float>(1, 2.f));
    R_TEST(pair<int, float>(), BS("\x80"), cbor::read_error::size_invalid, 0);
    R_TEST(pair<int, float>(), BS("\x81"), cbor::read_error::size_invalid, 0);
    R_TEST(pair<int, float>(), BS("\x83"), cbor::read_error::size_invalid, 0);
    R_TEST(pair<int, float>(), BS("\x82"), cbor::read_error::integer_invalid, 1);
    R_TEST(pair<int, float>(), BS("\x82\x00"), cbor::read_error::floating_point_invalid, 2);
TEST_END()


#ifdef CXON_HAS_LIB_STD_VARIANT
    TEST_BEG(cxon::CBOR<>) // std::variant
        R_TEST(variant<int, double>(in_place_index_t<0>(), 1), BS("\x82\x00\x01"));
        R_TEST(variant<int, double>(in_place_index_t<1>(), 0), BS("\x82\x01\x00"));
        R_TEST(variant<int, double>(in_place_index_t<1>(), 0), BS("\x82\x02\x00"), cbor::read_error::unexpected, 1);
        W_TEST(BS("\x82\x00\x01"), variant<int, double>(1));
        W_TEST(BS("\x82\x01\xFB\x00\x00\x00\x00\x00\x00\x00\x00"), variant<int, double>(in_place_index_t<1>(), 0));
        R_TEST(variant<monostate, int>(), BS("\x82\x00\xF6"));
        R_TEST(variant<monostate, int>(), BS("\x82\x00\x00"), cbor::read_error::unexpected, 2);
        W_TEST(BS("\x82\x00\xF6"), variant<monostate, int>());
    TEST_END()
#endif


TEST_BEG(cxon::CBOR<>) // std::vector
    R_TEST(vector<int>{2, 1, 0}, BS("\x83\x02\x01\x00"));
    R_TEST(vector<int>{2, 1, 0}, BS("\x9F\x02\x01\x00\xFF"));
    R_TEST(vector<int>{2, 1, 0}, BS("\x43\x02\x01\x00"));
    R_TEST(vector<int>{2, 1, 0}, BS("\x5F\x43\x02\x01\x00\xFF"));
    R_TEST(vector<int>{2, 1, 0}, BS("\x5F\x42\x02\x01\x41\x00\xFF"));
    R_TEST(vector<int>{2, 1, 0}, BS("\x7F\x61\x02\x62\x01\x00\xFF"));
    R_TEST(vector<int>{2, 1, 0}, BS("\x7F\x63\x02\x01\x00\xFF"));
    R_TEST(vector<int>{2, 1, 0}, BS("\x63\x02\x01\x00"));
    W_TEST(BS("\x83\x02\x01\x00"), vector<int>{2, 1, 0});
    W_TEST(BS("\x43\x02\x01\x00"), vector<unsigned char>{2, 1, 0});
    W_TEST(BS("\x63\x02\x01\x00"), vector<char>{2, 1, 0});
TEST_END()


TEST_BEG(cxon::CBOR<>) // std::tuple
    R_TEST(tuple<int, float>(1, 2.f), BS("\x82\x01\x02"));
    R_TEST(tuple<int, float>(1, 2.f), BS("\x82\x01\xFA\x40\x00\x00\x00"));
    W_TEST(BS("\x82\x01\xFA\x40\x00\x00\x00"), tuple<int, float>(1, 2.f));
    R_TEST(tuple<int, float>(), BS("\x80"), cbor::read_error::size_invalid, 0);
    R_TEST(tuple<int, float>(), BS("\x81"), cbor::read_error::size_invalid, 0);
    R_TEST(tuple<int, float>(), BS("\x83"), cbor::read_error::size_invalid, 0);
    R_TEST(tuple<int, float>(), BS("\x82"), cbor::read_error::integer_invalid, 1);
    R_TEST(tuple<int, float>(), BS("\x82\x00"), cbor::read_error::floating_point_invalid, 2);
TEST_END()
