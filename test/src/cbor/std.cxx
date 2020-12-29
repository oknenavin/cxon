// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "../test.hxx"

#include "cxon/lib/std/array.hxx"
#include "cxon/lib/std/chrono.hxx"
#include "cxon/lib/std/complex.hxx"
#include "cxon/lib/std/deque.hxx"
#include "cxon/lib/std/forward_list.hxx"
#include "cxon/lib/std/list.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/set.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/string_view.hxx"
#include "cxon/lib/std/tuple.hxx"
#include "cxon/lib/std/unordered_map.hxx"
#include "cxon/lib/std/unordered_set.hxx"
#include "cxon/lib/std/utility.hxx"
#include "cxon/lib/std/variant.hxx"
#include "cxon/lib/std/vector.hxx"

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
            R_TEST((array<int, 0>{}), BS("\x9F\x00\xFF"), cbor::read_error::size_invalid, 0);
            R_TEST((array<char, 0>{}), BS("\x41\x00"), cbor::read_error::size_invalid, 0);
            R_TEST((array<char, 0>{}), BS("\x5F\x42\x02\x01\x41\x00\xFF"), cbor::read_error::size_invalid, 1);
            R_TEST((array<char, 0>{}), BS("\x7F\x61\x02\x62\x01\x00\xFF"), cbor::read_error::size_invalid, 1);
            R_TEST((array<char, 0>{}), BS("\x61\x00"), cbor::read_error::size_invalid, 0);
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


TEST_BEG(cxon::CBOR<>) // std::deque
    R_TEST(deque<int>{}, BS("\x80"));
    R_TEST(deque<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), deque<int>{});
    R_TEST(deque<int>{2, 1, 0}, BS("\x83\x02\x01\x00"));
    R_TEST(deque<int>{2, 1, 0}, BS("\x9F\x02\x01\x00\xFF"));
    R_TEST(deque<int>{2, 1, 0}, BS("\x43\x02\x01\x00"));
    R_TEST(deque<int>{2, 1, 0}, BS("\x5F\x43\x02\x01\x00\xFF"));
    R_TEST(deque<int>{2, 1, 0}, BS("\x5F\x42\x02\x01\x41\x00\xFF"));
    R_TEST(deque<int>{2, 1, 0}, BS("\x7F\x61\x02\x62\x01\x00\xFF"));
    R_TEST(deque<int>{2, 1, 0}, BS("\x7F\x63\x02\x01\x00\xFF"));
    R_TEST(deque<int>{2, 1, 0}, BS("\x63\x02\x01\x00"));
    W_TEST(BS("\x83\x02\x01\x00"), deque<int>{2, 1, 0});
    W_TEST(BS("\x43\x02\x01\x00"), deque<unsigned char>{2, 1, 0});
    W_TEST(BS("\x63\x02\x01\x00"), deque<char>{2, 1, 0});
TEST_END()


TEST_BEG(cxon::CBOR<>) // std::forward_list
    R_TEST(forward_list<int>{}, BS("\x80"));
    R_TEST(forward_list<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), forward_list<int>{});
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
    R_TEST(list<int>{}, BS("\x80"));
    R_TEST(list<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), list<int>{});
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


TEST_BEG(cxon::CBOR<>) // std::map
    R_TEST(map<int, int>{}, BS("\xA0"));
    R_TEST(map<int, int>{}, BS("\xBF\xFF"));
    W_TEST(BS("\xA0"), map<int, int>{});
    R_TEST(map<int, int>{{1, 2}}, BS("\xA1\x01\x02"));
    R_TEST(map<int, int>{{1, 2}}, BS("\xA2\x01\x02\x01\x02"));
    W_TEST(BS("\xA1\x01\x02"), map<int, int>{{1, 2}});
    R_TEST(map<int, string>{{1, "2"}}, BS("\xA1\x01\x61\x32"));
    W_TEST(BS("\xA1\x01\x61\x32"), map<int, string>{{1, "2"}});
    R_TEST(map<string, int>{{"1", 2}}, BS("\xA1\x61\x31\x02"));
    W_TEST(BS("\xA1\x61\x31\x02"), map<string, int>{{"1", 2}});
TEST_END()


TEST_BEG(cxon::CBOR<>) // std::multimap
    R_TEST(multimap<int, int>{}, BS("\xA0"));
    R_TEST(multimap<int, int>{}, BS("\xBF\xFF"));
    W_TEST(BS("\xA0"), multimap<int, int>{});
    R_TEST(multimap<int, int>{{1, 2}}, BS("\xA1\x01\x02"));
    R_TEST(multimap<int, int>{{1, 2}, {1, 3}}, BS("\xA2\x01\x02\x01\x03"));
    W_TEST(BS("\xA1\x01\x02"), multimap<int, int>{{1, 2}});
TEST_END()


#ifdef CXON_HAS_LIB_STD_OPTIONAL
    TEST_BEG(cxon::CBOR<>) // std::optional
        R_TEST(optional<int>(42), BS("\x18\x2A"));
        W_TEST(BS("\x18\x2A"), optional<int>(42));
        R_TEST(optional<int>(), BS("\xF6"));
        W_TEST(BS("\xF6"), optional<int>());
    TEST_END()
#endif


TEST_BEG(cxon::CBOR<>) // std::set
    R_TEST(set<int>{}, BS("\x80"));
    R_TEST(set<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), set<int>{});
    R_TEST(set<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(set<int>{1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x82\x01\x02"), set<int>{1, 2});
TEST_END()


TEST_BEG(cxon::CBOR<>) // std::multiset
    R_TEST(multiset<int>{}, BS("\x80"));
    R_TEST(multiset<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), multiset<int>{});
    R_TEST(multiset<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(multiset<int>{1, 1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x82\x01\x02"), multiset<int>{1, 2});
TEST_END()


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


TEST_BEG(cxon::CBOR<>) // std::basic_string
    // char
        R_TEST(string(""), BS("\x40"));     // definite
        R_TEST(string(""), BS("\x60"));     // definite
        R_TEST(string(""), BS("\x80"));     // definite
        R_TEST(string(""), BS("\x9F\xFF")); // indefinite
        W_TEST(BS("\x60"), string(""));
        R_TEST(string("\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80"), BS("\x6A\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80")); // definite
        R_TEST(string("\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80"), BS("\x9F\x18\x20\x18\xC2\x18\xAE\x18\xE2\x18\x9C\x18\x88\x18\xF0\x18\x9F\x18\x9A\x18\x80\xFF")); // indefinite
        W_TEST(BS("\x6A\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80"), string("\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80"));
        R_TEST(string("xXxXxXxXxXxXxXxXxXxXxXxX\xF0\x9F\x8D\xBA"), // definite
                   BS("\x58\x1C\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\xF0\x9F\x8D\xBA"));
        R_TEST(string("xXxXxXxXxXxXxXxXxXxXxXxX\xF0\x9F\x8D\xBA"), // definite
                   BS("\x78\x1C\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\xF0\x9F\x8D\xBA"));
        R_TEST(string("xXxXxXxXxXxXxXxXxXxXxXxX\xF0\x9F\x8D\xBA"), // definite
                   BS("\x98\x1C\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\xF0\x18\x9F\x18\x8D\x18\xBA"));
        R_TEST(string("xXxXxXxXxXxXxXxXxXxXxXxX\xF0\x9F\x8D\xBA"), // indefinite
                   BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\xF0\x18\x9F\x18\x8D\x18\xBA\xFF"));
        W_TEST(    BS("\x78\x1C\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\xF0\x9F\x8D\xBA"),
               string("xXxXxXxXxXxXxXxXxXxXxXxX\xF0\x9F\x8D\xBA"));
    // wchar_t
        R_TEST(wstring(L""), BS("\x80"));       // definite
        R_TEST(wstring(L""), BS("\x9F\xFF"));   // indefinite
        W_TEST(BS("\x80"), wstring(L""));
    // char16_t[]
        R_TEST(u16string(u""), BS("\x80"));     // definite
        R_TEST(u16string(u""), BS("\x9F\xFF")); // indefinite
        W_TEST(BS("\x80"), u16string(u""));
        R_TEST(u16string(u"\x00AE\x2708\xD83D\xDE80"), BS("\x84\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80"));     // definite
        R_TEST(u16string(u"\x00AE\x2708\xD83D\xDE80"), BS("\x9F\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80\xFF")); // indefinite
        W_TEST(BS("\x84\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80"), u16string(u"\x00AE\x2708\xD83D\xDE80"));
        R_TEST(u16string(u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A"), // definite
                       BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A"));
        R_TEST(u16string(u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A"), // indefinite
                       BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A\xFF"));
        W_TEST(        BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A"),
               u16string(u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A"));
    // char32_t[]
        R_TEST(u32string(U""), BS("\x80"));     // definite
        R_TEST(u32string(U""), BS("\x9F\xFF")); // indefinite
        W_TEST(BS("\x80"), u32string(U""));
        R_TEST(u32string(U"\x000000AE\x00002708\x0001F680"), BS("\x83\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80"));       // definite
        R_TEST(u32string(U"\x000000AE\x00002708\x0001F680"), BS("\x9F\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80\xFF"));   // indefinite
        W_TEST(BS("\x83\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80"), u32string(U"\x000000AE\x00002708\x0001F680"));
        R_TEST(u32string(U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A"), // definite
                       BS("\x98\x19\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A"));
        W_TEST(        BS("\x98\x19\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A"),
               u32string(U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A"));
        R_TEST(u32string(U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A"), // indefinite
                       BS("\x9F\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A\xFF"));
TEST_END()


#ifdef CXON_HAS_LIB_STD_STRING_VIEW
    TEST_BEG(cxon::CBOR<>) // std::basic_string_view
        // char
            W_TEST(BS("\x60"), string_view(""));
            W_TEST(BS("\x6A\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80"), string_view("\x20\xC2\xAE\xE2\x9C\x88\xF0\x9F\x9A\x80"));
            W_TEST(         BS("\x78\x1C\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\x78\x58\xF0\x9F\x8D\xBA"),
                   string_view("xXxXxXxXxXxXxXxXxXxXxXxX\xF0\x9F\x8D\xBA"));
        // wchar_t
            W_TEST(BS("\x80"), wstring_view(L""));
        // char16_t[]
            W_TEST(BS("\x80"), u16string_view(u""));
            W_TEST(BS("\x84\x18\xAE\x19\x27\x08\x19\xD8\x3D\x19\xDE\x80"), u16string_view(u"\x00AE\x2708\xD83D\xDE80"));
            W_TEST(             BS("\x98\x1A\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x19\xD8\x3C\x19\xDF\x7A"),
                   u16string_view(u"xXxXxXxXxXxXxXxXxXxXxXxX\xD83C\xDF7A"));
        // char32_t[]
            W_TEST(BS("\x80"), u32string_view(U""));
            W_TEST(BS("\x83\x18\xAE\x19\x27\x08\x1A\x00\x01\xF6\x80"), u32string_view(U"\x000000AE\x00002708\x0001F680"));
            W_TEST(             BS("\x98\x19\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x18\x78\x18\x58\x1A\x00\x01\xF3\x7A"),
                   u32string_view(U"xXxXxXxXxXxXxXxXxXxXxXxX\x1F37A"));
    TEST_END()
#endif


TEST_BEG(cxon::CBOR<>) // std::tuple
    R_TEST(tuple<>{}, BS("\x80"));
    W_TEST(BS("\x80"), tuple<>{});
    R_TEST(tuple<int, float>(1, 2.f), BS("\x82\x01\x02"));
    R_TEST(tuple<int, float>(1, 2.f), BS("\x82\x01\xFA\x40\x00\x00\x00"));
    W_TEST(BS("\x82\x01\xFA\x40\x00\x00\x00"), tuple<int, float>(1, 2.f));
    R_TEST(tuple<int, float>(), BS("\x80"), cbor::read_error::size_invalid, 0);
    R_TEST(tuple<int, float>(), BS("\x81"), cbor::read_error::size_invalid, 0);
    R_TEST(tuple<int, float>(), BS("\x83"), cbor::read_error::size_invalid, 0);
    R_TEST(tuple<int, float>(), BS("\x82"), cbor::read_error::integer_invalid, 1);
    R_TEST(tuple<int, float>(), BS("\x82\x00"), cbor::read_error::floating_point_invalid, 2);
TEST_END()


TEST_BEG(cxon::CBOR<>) // std::unordered_map
    R_TEST(unordered_map<int, int>{}, BS("\xA0"));
    R_TEST(unordered_map<int, int>{}, BS("\xBF\xFF"));
    W_TEST(BS("\xA0"), unordered_map<int, int>{});
    R_TEST(unordered_map<int, int>{{1, 2}}, BS("\xA1\x01\x02"));
    W_TEST(BS("\xA1\x01\x02"), unordered_map<int, int>{{1, 2}});
TEST_END()


TEST_BEG(cxon::CBOR<>) // std::unordered_multimap
    R_TEST(unordered_multimap<int, int>{}, BS("\xA0"));
    R_TEST(unordered_multimap<int, int>{}, BS("\xBF\xFF"));
    W_TEST(BS("\xA0"), unordered_multimap<int, int>{});
    R_TEST(unordered_multimap<int, int>{{1, 2}}, BS("\xA1\x01\x02"));
    W_TEST(BS("\xA1\x01\x02"), unordered_multimap<int, int>{{1, 2}});
TEST_END()


TEST_BEG(cxon::CBOR<>) // std::unordered_set
    R_TEST(unordered_set<int>{}, BS("\x80"));
    R_TEST(unordered_set<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), unordered_set<int>{});
    R_TEST(unordered_set<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(unordered_set<int>{1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x82\x01\x02"), unordered_set<int>{1, 2});
TEST_END()


TEST_BEG(cxon::CBOR<>) // std::unordered_multiset
    R_TEST(unordered_multiset<int>{}, BS("\x80"));
    R_TEST(unordered_multiset<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), unordered_multiset<int>{});
    R_TEST(unordered_multiset<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(unordered_multiset<int>{1, 1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x82\x01\x02"), unordered_multiset<int>{1, 2});
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
    R_TEST(vector<int>{}, BS("\x80"));
    R_TEST(vector<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), vector<int>{});
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
