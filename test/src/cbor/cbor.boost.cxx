// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "../test.hxx"

#include "cxon/lib/boost/container/stable_vector.hxx"
#include "cxon/lib/boost/container/small_vector.hxx"
#include "cxon/lib/boost/container/static_vector.hxx"
#include "cxon/lib/boost/container/deque.hxx"
#include "cxon/lib/boost/container/list.hxx"
#include "cxon/lib/boost/container/slist.hxx"
#include "cxon/lib/boost/container/string.hxx"
#include "cxon/lib/boost/container/map.hxx"
#include "cxon/lib/boost/container/set.hxx"
#include "cxon/lib/boost/container/flat_map.hxx"
#include "cxon/lib/boost/container/flat_set.hxx"
#include "cxon/lib/boost/dynamic_bitset.hxx"
#include "cxon/lib/boost/variant2.hxx"
#include "cxon/lib/boost/variant.hxx"

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
    R_TEST(small_vector<int, 0>{}, BS("\x40"));
    R_TEST(small_vector<int, 4>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), small_vector<int, 0>{});
    R_TEST(small_vector<int, 1>{2, 1, 0}, BS("\x83\x02\x01\x00"));
    R_TEST(small_vector<int, 1>{2, 1, 0}, BS("\x43\x02\x01\x00"));
    R_TEST(small_vector<int, 2>{2, 1, 0}, BS("\x43\x02\x01\x00"));
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
        R_TEST((static_vector<int, 1>{1}), BS("\x41\x01"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x83\x02\x01\x00"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x9F\x02\x01\x00\xFF"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x63\x02\x01\x00"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x7F\x63\x02\x01\x00\xFF"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x7F\x62\x02\x01\x61\x00\xFF"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x5F\x42\x02\x01\x41\x00\xFF"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x5F\x43\x02\x01\x00\xFF"));
        R_TEST((static_vector<int, 3>{2, 1, 0}), BS("\x43\x02\x01\x00"));
            // more
            R_TEST((static_vector<int, 1>{}), BS("\x83\x02\x01\x00"), cbor::read_error::size_invalid, 0);
            R_TEST((static_vector<int, 2>{}), BS("\x9F\x02\x01\x00\xFF"), cbor::read_error::size_invalid, 0);
            R_TEST((static_vector<int, 2>{}), BS("\x63\x02\x01\x00"), cbor::read_error::size_invalid, 0);
            R_TEST((static_vector<int, 1>{}), BS("\x7F\x63\x02\x01\x00\xFF"), cbor::read_error::size_invalid, 1);
            R_TEST((static_vector<int, 1>{}), BS("\x7F\x62\x02\x01\x61\x00\xFF"), cbor::read_error::size_invalid, 1);
            R_TEST((static_vector<int, 2>{}), BS("\x5F\x42\x02\x01\x41\x00\xFF"), cbor::read_error::size_invalid, 4);
            R_TEST((static_vector<int, 2>{}), BS("\x5F\x43\x02\x01\x00\xFF"), cbor::read_error::size_invalid, 1);
            R_TEST((static_vector<int, 1>{}), BS("\x43\x02\x01\x00"), cbor::read_error::size_invalid, 0);
        W_TEST(BS("\x83\x02\x01\x00"), (static_vector<int, 3>{2, 1, 0}));
        R_TEST((static_vector<int, 3>{2, 1}), BS("\x82\x02\x01")); // less
        R_TEST((static_vector<int, 3>{2, 1}), BS("\x9F\x02\x01\xFF")); // less
        W_TEST(BS("\x63\x02\x01\x00"), (static_vector<char, 3>{2, 1, 0}));
        W_TEST(BS("\x43\x02\x01\x00"), (static_vector<unsigned char, 3>{2, 1, 0}));
TEST_END()


TEST_BEG(deque, cxon::CBOR<>, "/boost")
    using namespace boost::container;
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


TEST_BEG(list, cxon::CBOR<>, "/boost")
    using namespace boost::container;
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


TEST_BEG(slist, cxon::CBOR<>, "/boost")
    using namespace boost::container;
    R_TEST(slist<int>{}, BS("\x80"));
    R_TEST(slist<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), slist<int>{});
    R_TEST(slist<int>{2, 1, 0}, BS("\x83\x02\x01\x00"));
    R_TEST(slist<int>{2, 1, 0}, BS("\x9F\x02\x01\x00\xFF"));
    R_TEST(slist<int>{2, 1, 0}, BS("\x43\x02\x01\x00"));
    R_TEST(slist<int>{2, 1, 0}, BS("\x5F\x43\x02\x01\x00\xFF"));
    R_TEST(slist<int>{2, 1, 0}, BS("\x5F\x42\x02\x01\x41\x00\xFF"));
    R_TEST(slist<int>{2, 1, 0}, BS("\x7F\x61\x02\x62\x01\x00\xFF"));
    R_TEST(slist<int>{2, 1, 0}, BS("\x7F\x63\x02\x01\x00\xFF"));
    R_TEST(slist<int>{2, 1, 0}, BS("\x63\x02\x01\x00"));
    W_TEST(BS("\x83\x02\x01\x00"), slist<int>{2, 1, 0});
    W_TEST(BS("\x43\x02\x01\x00"), slist<unsigned char>{2, 1, 0});
    W_TEST(BS("\x63\x02\x01\x00"), slist<char>{2, 1, 0});
    // errors
    R_TEST(slist<int>{}, BS("\xFF"), cbor::read_error::array_invalid, 0);
    R_TEST(slist<int>{}, BS("\x81\xFF"), cbor::read_error::integer_invalid, 1);
    R_TEST(slist<int>{}, BS("\xDC"), cbor::read_error::tag_invalid, 0);
    R_TEST(slist<int>{}, BS("\xCD\xFF"), cbor::read_error::array_invalid, 1);
    R_TEST(slist<int>{}, BS("\xCD\x81\xFF"), cbor::read_error::integer_invalid, 2);
TEST_END()


TEST_BEG(basic_string, cxon::CBOR<>, "/boost")
    using namespace boost::container;
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
        R_TEST(wstring(L""), BS("\x40"));       // definite
        R_TEST(wstring(L""), BS("\x60"));       // definite
        R_TEST(wstring(L""), BS("\x80"));       // definite
        R_TEST(wstring(L""), BS("\x9F\xFF"));   // indefinite
        R_TEST(wstring(L""), BS("\x80"));       // definite
        R_TEST(wstring(L""), BS("\x9F\xFF"));   // indefinite
        W_TEST(BS("\x80"), wstring(L""));
    // char16_t
        using u16string = basic_string<char16_t, std::char_traits<char16_t>, new_allocator<char16_t>>;
        R_TEST(u16string(u""), BS("\x40"));     // definite
        R_TEST(u16string(u""), BS("\x60"));     // definite
        R_TEST(u16string(u""), BS("\x80"));     // definite
        R_TEST(u16string(u""), BS("\x9F\xFF")); // indefinite
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
    // char32_t
        using u32string = basic_string<char32_t, std::char_traits<char32_t>, new_allocator<char32_t>>;
        R_TEST(u32string(U""), BS("\x40"));     // definite
        R_TEST(u32string(U""), BS("\x60"));     // definite
        R_TEST(u32string(U""), BS("\x80"));     // definite
        R_TEST(u32string(U""), BS("\x9F\xFF")); // indefinite
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
        // errors
        R_TEST((string*)nullptr, BS("\xF7"), cbor::read_error::array_invalid, 0);
TEST_END()


TEST_BEG(map, cxon::CBOR<>, "/boost")
    using namespace boost::container;
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


TEST_BEG(multimap, cxon::CBOR<>, "/boost")
    using namespace boost::container;
    R_TEST(multimap<int, int>{}, BS("\xA0"));
    R_TEST(multimap<int, int>{}, BS("\xBF\xFF"));
    W_TEST(BS("\xA0"), multimap<int, int>{});
    R_TEST(multimap<int, int>{{1, 2}}, BS("\xA1\x01\x02"));
    R_TEST(multimap<int, int>{{1, 2}, {1, 3}}, BS("\xA2\x01\x02\x01\x03"));
    W_TEST(BS("\xA1\x01\x02"), multimap<int, int>{{1, 2}});
TEST_END()


TEST_BEG(set, cxon::CBOR<>, "/boost")
    using namespace boost::container;
    R_TEST(set<int>{}, BS("\x80"));
    R_TEST(set<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), set<int>{});
    R_TEST(set<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(set<int>{1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x82\x01\x02"), set<int>{1, 2});
TEST_END()


TEST_BEG(multiset, cxon::CBOR<>, "/boost")
    using namespace boost::container;
    R_TEST(multiset<int>{}, BS("\x80"));
    R_TEST(multiset<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), multiset<int>{});
    R_TEST(multiset<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(multiset<int>{1, 1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x82\x01\x02"), multiset<int>{1, 2});
TEST_END()



TEST_BEG(flat_map, cxon::CBOR<>, "/boost")
    using namespace boost::container;
    R_TEST(flat_map<int, int>{}, BS("\xA0"));
    R_TEST(flat_map<int, int>{}, BS("\xBF\xFF"));
    W_TEST(BS("\xA0"), flat_map<int, int>{});
    R_TEST(flat_map<int, int>{{1, 2}}, BS("\xA1\x01\x02"));
    R_TEST(flat_map<int, int>{{1, 2}}, BS("\xA2\x01\x02\x01\x02"));
    W_TEST(BS("\xA1\x01\x02"), flat_map<int, int>{{1, 2}});
    R_TEST(flat_map<int, string>{{1, "2"}}, BS("\xA1\x01\x61\x32"));
    W_TEST(BS("\xA1\x01\x61\x32"), flat_map<int, string>{{1, "2"}});
    R_TEST(flat_map<string, int>{{"1", 2}}, BS("\xA1\x61\x31\x02"));
    W_TEST(BS("\xA1\x61\x31\x02"), flat_map<string, int>{{"1", 2}});
TEST_END()


TEST_BEG(flat_multimap, cxon::CBOR<>, "/boost")
    using namespace boost::container;
    R_TEST(flat_multimap<int, int>{}, BS("\xA0"));
    R_TEST(flat_multimap<int, int>{}, BS("\xBF\xFF"));
    W_TEST(BS("\xA0"), flat_multimap<int, int>{});
    R_TEST(flat_multimap<int, int>{{1, 2}}, BS("\xA1\x01\x02"));
    R_TEST(flat_multimap<int, int>{{1, 2}, {1, 3}}, BS("\xA2\x01\x02\x01\x03"));
    W_TEST(BS("\xA1\x01\x02"), flat_multimap<int, int>{{1, 2}});
TEST_END()


TEST_BEG(flat_set, cxon::CBOR<>, "/boost")
    using namespace boost::container;
    R_TEST(flat_set<int>{}, BS("\x80"));
    R_TEST(flat_set<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), flat_set<int>{});
    R_TEST(flat_set<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(flat_set<int>{1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x82\x01\x02"), flat_set<int>{1, 2});
TEST_END()


TEST_BEG(flat_multiset, cxon::CBOR<>, "/boost")
    using namespace boost::container;
    R_TEST(flat_multiset<int>{}, BS("\x80"));
    R_TEST(flat_multiset<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), flat_multiset<int>{});
    R_TEST(flat_multiset<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(flat_multiset<int>{1, 1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x82\x01\x02"), flat_multiset<int>{1, 2});
TEST_END()


TEST_BEG(dynamic_bitset, cxon::CBOR<>, "/boost")
    using namespace boost;
    using bitset = dynamic_bitset<unsigned char>;
    R_TEST(bitset {32, 0x000000FF}, BS("\x45\x00\x00\x00\x00\xFF")); //                         11111111
    R_TEST(bitset {32, 0x0000FFFF}, BS("\x45\x00\x00\x00\xFF\xFF")); //                 1111111111111111
    R_TEST(bitset {32, 0xFFFFFFFF}, BS("\x45\x00\xFF\xFF\xFF\xFF")); // 11111111111111111111111111111111
    R_TEST(bitset {32, 0xFFFF0000}, BS("\x45\x00\xFF\xFF\x00\x00")); // 11111111111111110000000000000000
    R_TEST(bitset {32, 0xFF000000}, BS("\x45\x00\xFF\x00\x00\x00")); // 11111111000000000000000000000000
    W_TEST(BS("\x45\x00\xFF\x00\x00\x00"), bitset {32, 0xFF000000}); // 11111111000000000000000000000000
    W_TEST(BS("\x45\x00\xFF\xFF\x00\x00"), bitset {32, 0xFFFF0000}); // 11111111111111110000000000000000
    W_TEST(BS("\x45\x00\xFF\xFF\xFF\xFF"), bitset {32, 0xFFFFFFFF}); // 11111111111111111111111111111111
    W_TEST(BS("\x45\x00\x00\x00\xFF\xFF"), bitset {32, 0x0000FFFF}); //                 1111111111111111
    W_TEST(BS("\x45\x00\x00\x00\x00\xFF"), bitset {32, 0x000000FF}); //                         11111111
    R_TEST(bitset { 1, 0x00000001}, BS("\x42\x07\x01"));             //                               01
    R_TEST(bitset { 3, 0x00000005}, BS("\x42\x05\x05"));             //                             0101
    R_TEST(bitset { 5, 0x00000015}, BS("\x42\x03\x15"));             //                           010101
    R_TEST(bitset { 7, 0x00000055}, BS("\x42\x01\x55"));             //                         01010101
    R_TEST(bitset { 9, 0x00000155}, BS("\x43\x07\x01\x55"));         //                       0101010101
    R_TEST(bitset {11, 0x00000555}, BS("\x43\x05\x05\x55"));         //                     010101010101
    R_TEST(bitset {13, 0x00001555}, BS("\x43\x03\x15\x55"));         //                   01010101010101
    R_TEST(bitset {15, 0x00005555}, BS("\x43\x01\x55\x55"));         //                 0101010101010101
    R_TEST(bitset {18, 0x0002AAAA}, BS("\x44\x06\x02\xAA\xAA"));     //               101010101010101010
    R_TEST(bitset {20, 0x000AAAAA}, BS("\x44\x04\x0A\xAA\xAA"));     //             10101010101010101010
    R_TEST(bitset {22, 0x002AAAAA}, BS("\x44\x02\x2A\xAA\xAA"));     //           1010101010101010101010
    R_TEST(bitset {24, 0x00AAAAAA}, BS("\x44\x00\xAA\xAA\xAA"));     //         101010101010101010101010
    R_TEST(bitset {26, 0x02AAAAAA}, BS("\x45\x06\x02\xAA\xAA\xAA")); //       10101010101010101010101010
    R_TEST(bitset {28, 0x0AAAAAAA}, BS("\x45\x04\x0A\xAA\xAA\xAA")); //     1010101010101010101010101010
    R_TEST(bitset {30, 0x2AAAAAAA}, BS("\x45\x02\x2A\xAA\xAA\xAA")); //   101010101010101010101010101010
    R_TEST(bitset {32, 0xAAAAAAAA}, BS("\x45\x00\xAA\xAA\xAA\xAA")); // 10101010101010101010101010101010
    W_TEST(BS("\x45\x00\xAA\xAA\xAA\xAA"), bitset {32, 0xAAAAAAAA}); // 10101010101010101010101010101010
    W_TEST(BS("\x45\x02\x2A\xAA\xAA\xAA"), bitset {30, 0x2AAAAAAA}); //   101010101010101010101010101010
    W_TEST(BS("\x45\x04\x0A\xAA\xAA\xAA"), bitset {28, 0x0AAAAAAA}); //     1010101010101010101010101010
    W_TEST(BS("\x45\x06\x02\xAA\xAA\xAA"), bitset {26, 0x02AAAAAA}); //       10101010101010101010101010
    W_TEST(BS("\x44\x00\xAA\xAA\xAA"), bitset {24, 0x00AAAAAA});     //         101010101010101010101010
    W_TEST(BS("\x44\x02\x2A\xAA\xAA"), bitset {22, 0x002AAAAA});     //           1010101010101010101010
    W_TEST(BS("\x44\x04\x0A\xAA\xAA"), bitset {20, 0x000AAAAA});     //             10101010101010101010
    W_TEST(BS("\x44\x06\x02\xAA\xAA"), bitset {18, 0x0002AAAA});     //               101010101010101010
    W_TEST(BS("\x43\x01\x55\x55"), bitset {15, 0x00005555});         //                 0101010101010101
    W_TEST(BS("\x43\x03\x15\x55"), bitset {13, 0x00001555});         //                   01010101010101
    W_TEST(BS("\x43\x05\x05\x55"), bitset {11, 0x00000555});         //                     010101010101
    W_TEST(BS("\x43\x07\x01\x55"), bitset { 9, 0x00000155});         //                       0101010101
    W_TEST(BS("\x42\x01\x55"), bitset { 7, 0x00000055});             //                         01010101
    W_TEST(BS("\x42\x03\x15"), bitset { 5, 0x00000015});             //                           010101
    W_TEST(BS("\x42\x05\x05"), bitset { 3, 0x00000005});             //                             0101
    W_TEST(BS("\x42\x07\x01"), bitset { 1, 0x00000001});             //                               01
    // errors
    R_TEST(bitset(), BS("\x5C"), cbor::read_error::size_invalid, 0);
    R_TEST(bitset(), BS("\x41"), cbor::read_error::unexpected, 1);
    R_TEST(bitset(), BS("\x42"), cbor::read_error::unexpected, 1);
    R_TEST(bitset(), BS("\x42\x01"), cbor::read_error::unexpected, 2);
    R_TEST(bitset(), BS("\x44"), cbor::read_error::unexpected, 1);
    R_TEST(bitset(), BS("\x45"), cbor::read_error::unexpected, 1);
    R_TEST(bitset(), BS("\x45\x01"), cbor::read_error::unexpected, 2);
    R_TEST(bitset(), BS("\x45\x00"), cbor::read_error::unexpected, 2);
    R_TEST(bitset(), BS("\x64"), cbor::read_error::unexpected, 0);
    R_TEST(bitset(), BS("\x5C"), cbor::read_error::size_invalid, 0);
    R_TEST(bitset(), BS("\x41"), cbor::read_error::unexpected, 1);
    R_TEST(bitset(), BS("\x64"), cbor::read_error::unexpected, 0);
    R_TEST(bitset(), BS("\xDC"), cbor::read_error::tag_invalid, 0);
    R_TEST(bitset(), BS("\xCD\xFF"), cbor::read_error::unexpected, 1);
    {   bio::byte o[1];
        auto const r = to_bytes(std::begin(o), std::end(o), bitset(8 * 32));
        TEST_CHECK(!r && r.ec == cbor::write_error::output_failure);
    }
    {   bio::byte o[1];
        auto const r = to_bytes(std::begin(o), std::end(o), bitset(9));
        TEST_CHECK(!r && r.ec == cbor::write_error::output_failure);
    }
    {   bio::byte o[2];
        auto const r = to_bytes(std::begin(o), std::end(o), bitset(9));
        TEST_CHECK(!r && r.ec == cbor::write_error::output_failure);
    }
TEST_END()


#ifdef CXON_HAS_BOOST_VARIANT2
    TEST_BEG(variant2, cxon::CBOR<>, "/boost")
        using namespace boost::variant2;
        R_TEST(variant<int, double>(in_place_index_t<0>(), 1), BS("\x82\x00\x01"));
        R_TEST(variant<int, double>(in_place_index_t<1>(), 0), BS("\x82\x01\x00"));
        R_TEST(variant<int, double>(in_place_index_t<1>(), 0), BS("\x82\x02\x00"), cbor::read_error::unexpected, 1);
        W_TEST(BS("\x82\x00\x01"), variant<int, double>(1));
        W_TEST(BS("\x82\x01\xFB\x00\x00\x00\x00\x00\x00\x00\x00"), variant<int, double>(in_place_index_t<1>(), 0));
        R_TEST(variant<monostate, int>(), BS("\x82\x00\xF7"));
        R_TEST(variant<monostate, int>(), BS("\x82\x00\x00"), cbor::read_error::unexpected, 2);
        W_TEST(BS("\x82\x00\xF7"), variant<monostate, int>());
        // errors
        R_TEST(variant<monostate, int>(), BS("\xFF"), cbor::read_error::size_invalid, 0);
        R_TEST(variant<monostate, int>(), BS("\x82\xFF"), cbor::read_error::unexpected, 1);
        R_TEST(variant<monostate, int>(), BS("\x82\x00\xFF"), cbor::read_error::unexpected, 2);
        R_TEST(variant<int, double>(), BS("\xFF"), cbor::read_error::size_invalid, 0);
        R_TEST(variant<int, double>(), BS("\x82\xFF"), cbor::read_error::unexpected, 1);
        R_TEST(variant<int, double>(), BS("\x82\x00\xFF"), cbor::read_error::integer_invalid, 2);
        R_TEST(variant<int, double>(), BS("\xDC"), cbor::read_error::tag_invalid, 0);
        R_TEST(variant<int, double>(), BS("\xCD\xFF"), cbor::read_error::size_invalid, 1);
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<cxon::CBOR<>>(c, variant<int, double>(3));
            TEST_CHECK(r.ec == cbor::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<cxon::CBOR<>>(c, variant<int, double>(3));
            TEST_CHECK(r.ec == cbor::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<cxon::CBOR<>>(c, variant<int, double>(3));
            TEST_CHECK(r.ec == cbor::write_error::output_failure);
        }
    TEST_END()
#endif


namespace cxon { namespace test {
    template <typename T>
        struct match<boost::recursive_wrapper<T>> {
            static bool values(const boost::recursive_wrapper<T>& t0, const boost::recursive_wrapper<T>& t1) {
                return t0.get() == t1.get();
            }
        };
}}
TEST_BEG(variant, cxon::CBOR<>, "/boost")
    using namespace boost;
    R_TEST(variant<int, double>(  1), BS("\x82\x00\x01"));
    R_TEST(variant<int, double>(0.0), BS("\x82\x01\x00"));
    R_TEST(variant<int, double>(  0), BS("\x82\x02\x00"), cbor::read_error::unexpected, 1);
    W_TEST(BS("\x82\x00\x01"), variant<int, double>(1));
    W_TEST(BS("\x82\x01\xFB\x00\x00\x00\x00\x00\x00\x00\x00"), variant<int, double>(0.0));
    R_TEST(recursive_wrapper<int>(3), BS("\x03"));
    W_TEST(BS("\x03"), recursive_wrapper<int>(3));
    // errors
    R_TEST(variant<int, double>(), BS("\xFF"), cbor::read_error::size_invalid, 0);
    R_TEST(variant<int, double>(), BS("\x82\xFF"), cbor::read_error::unexpected, 1);
    R_TEST(variant<int, double>(), BS("\x82\x00\xFF"), cbor::read_error::integer_invalid, 2);
    R_TEST(variant<int, double>(), BS("\xDC"), cbor::read_error::tag_invalid, 0);
    R_TEST(variant<int, double>(), BS("\xCD\xFF"), cbor::read_error::size_invalid, 1);
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            c.push_back('x');
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, variant<int, double>(3));
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, variant<int, double>(3));
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
    {   char b[2];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, variant<int, double>(3));
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
TEST_END()
