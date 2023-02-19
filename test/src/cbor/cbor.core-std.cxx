// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon/cbor.hxx"
#include "../test.hxx"

#include "cxon/lib/std/array.hxx"
#include "cxon/lib/std/bitset.hxx"
#include "cxon/lib/std/chrono.hxx"
#include "cxon/lib/std/complex.hxx"
#include "cxon/lib/std/deque.hxx"
#include "cxon/lib/std/forward_list.hxx"
#include "cxon/lib/std/list.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/queue.hxx"
#include "cxon/lib/std/set.hxx"
#include "cxon/lib/std/stack.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/string_view.hxx"
#include "cxon/lib/std/tuple.hxx"
#include "cxon/lib/std/unordered_map.hxx"
#include "cxon/lib/std/unordered_set.hxx"
#include "cxon/lib/std/utility.hxx"
#include "cxon/lib/std/valarray.hxx"
#include "cxon/lib/std/variant.hxx"
#include "cxon/lib/std/vector.hxx"

////////////////////////////////////////////////////////////////////////////////

using namespace std;

TEST_BEG(array, cxon::CBOR<>, "/std")
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
#       if !defined(__GNUC__) || defined(__clang__) // -Wstringop-overflow, not sure why for unsigned char only
            R_TEST((array<unsigned char, 0>{}), BS("\x40"));
            R_TEST((array<unsigned char, 0>{}), BS("\x5F\xFF"));
            R_TEST((array<unsigned char, 0>{}), BS("\x5F\x40\xFF"));
            R_TEST((array<unsigned char, 0>{}), BS("\x5F\x40\x40\xFF"));
            R_TEST((array<unsigned char, 0>{}), BS("\x7F\x60\x60\xFF"));
            R_TEST((array<unsigned char, 0>{}), BS("\x7F\x60\xFF"));
            R_TEST((array<unsigned char, 0>{}), BS("\x7F\xFF"));
            R_TEST((array<unsigned char, 0>{}), BS("\x60"));
            W_TEST("\x40", (array<unsigned char, 0>{}));
#       endif
        // more
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


TEST_BEG(basic_string, cxon::CBOR<>, "/std")
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
        W_TEST(BS("\x80"), wstring(L""));
    // char16_t
        R_TEST(u16string(u""), BS("\x40"));     // definite
        R_TEST(u16string(u""), BS("\x60"));     // definite
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
        R_TEST(u32string(U""), BS("\x40"));     // definite
        R_TEST(u32string(U""), BS("\x60"));     // definite
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


#ifdef CXON_HAS_LIB_STD_STRING_VIEW
    TEST_BEG(basic_string_view, cxon::CBOR<>, "/std")
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


TEST_BEG(bitset, cxon::CBOR<>, "/std")
    R_TEST(bitset<32>{0x000000FF}, BS("\x45\x00\x00\x00\x00\xFF")); //                         11111111
    R_TEST(bitset<32>{0x0000FFFF}, BS("\x45\x00\x00\x00\xFF\xFF")); //                 1111111111111111
    R_TEST(bitset<32>{0xFFFFFFFF}, BS("\x45\x00\xFF\xFF\xFF\xFF")); // 11111111111111111111111111111111
    R_TEST(bitset<32>{0xFFFF0000}, BS("\x45\x00\xFF\xFF\x00\x00")); // 11111111111111110000000000000000
    R_TEST(bitset<32>{0xFF000000}, BS("\x45\x00\xFF\x00\x00\x00")); // 11111111000000000000000000000000
    W_TEST(BS("\x45\x00\xFF\x00\x00\x00"), bitset<32>{0xFF000000}); // 11111111000000000000000000000000
    W_TEST(BS("\x45\x00\xFF\xFF\x00\x00"), bitset<32>{0xFFFF0000}); // 11111111111111110000000000000000
    W_TEST(BS("\x45\x00\xFF\xFF\xFF\xFF"), bitset<32>{0xFFFFFFFF}); // 11111111111111111111111111111111
    W_TEST(BS("\x45\x00\x00\x00\xFF\xFF"), bitset<32>{0x0000FFFF}); //                 1111111111111111
    W_TEST(BS("\x45\x00\x00\x00\x00\xFF"), bitset<32>{0x000000FF}); //                         11111111
    R_TEST(bitset< 1>{0x00000001}, BS("\x42\x07\x01"));             //                               01
    R_TEST(bitset< 3>{0x00000005}, BS("\x42\x05\x05"));             //                             0101
    R_TEST(bitset< 5>{0x00000015}, BS("\x42\x03\x15"));             //                           010101
    R_TEST(bitset< 7>{0x00000055}, BS("\x42\x01\x55"));             //                         01010101
    R_TEST(bitset< 9>{0x00000155}, BS("\x43\x07\x01\x55"));         //                       0101010101
    R_TEST(bitset<11>{0x00000555}, BS("\x43\x05\x05\x55"));         //                     010101010101
    R_TEST(bitset<13>{0x00001555}, BS("\x43\x03\x15\x55"));         //                   01010101010101
    R_TEST(bitset<15>{0x00005555}, BS("\x43\x01\x55\x55"));         //                 0101010101010101
    R_TEST(bitset<18>{0x0002AAAA}, BS("\x44\x06\x02\xAA\xAA"));     //               101010101010101010
    R_TEST(bitset<20>{0x000AAAAA}, BS("\x44\x04\x0A\xAA\xAA"));     //             10101010101010101010
    R_TEST(bitset<22>{0x002AAAAA}, BS("\x44\x02\x2A\xAA\xAA"));     //           1010101010101010101010
    R_TEST(bitset<24>{0x00AAAAAA}, BS("\x44\x00\xAA\xAA\xAA"));     //         101010101010101010101010
    R_TEST(bitset<26>{0x02AAAAAA}, BS("\x45\x06\x02\xAA\xAA\xAA")); //       10101010101010101010101010
    R_TEST(bitset<28>{0x0AAAAAAA}, BS("\x45\x04\x0A\xAA\xAA\xAA")); //     1010101010101010101010101010
    R_TEST(bitset<30>{0x2AAAAAAA}, BS("\x45\x02\x2A\xAA\xAA\xAA")); //   101010101010101010101010101010
    R_TEST(bitset<32>{0xAAAAAAAA}, BS("\x45\x00\xAA\xAA\xAA\xAA")); // 10101010101010101010101010101010
    W_TEST(BS("\x45\x00\xAA\xAA\xAA\xAA"), bitset<32>{0xAAAAAAAA}); // 10101010101010101010101010101010
    W_TEST(BS("\x45\x02\x2A\xAA\xAA\xAA"), bitset<30>{0x2AAAAAAA}); //   101010101010101010101010101010
    W_TEST(BS("\x45\x04\x0A\xAA\xAA\xAA"), bitset<28>{0x0AAAAAAA}); //     1010101010101010101010101010
    W_TEST(BS("\x45\x06\x02\xAA\xAA\xAA"), bitset<26>{0x02AAAAAA}); //       10101010101010101010101010
    W_TEST(BS("\x44\x00\xAA\xAA\xAA"), bitset<24>{0x00AAAAAA});     //         101010101010101010101010
    W_TEST(BS("\x44\x02\x2A\xAA\xAA"), bitset<22>{0x002AAAAA});     //           1010101010101010101010
    W_TEST(BS("\x44\x04\x0A\xAA\xAA"), bitset<20>{0x000AAAAA});     //             10101010101010101010
    W_TEST(BS("\x44\x06\x02\xAA\xAA"), bitset<18>{0x0002AAAA});     //               101010101010101010
    W_TEST(BS("\x43\x01\x55\x55"), bitset<15>{0x00005555});         //                 0101010101010101
    W_TEST(BS("\x43\x03\x15\x55"), bitset<13>{0x00001555});         //                   01010101010101
    W_TEST(BS("\x43\x05\x05\x55"), bitset<11>{0x00000555});         //                     010101010101
    W_TEST(BS("\x43\x07\x01\x55"), bitset< 9>{0x00000155});         //                       0101010101
    W_TEST(BS("\x42\x01\x55"), bitset< 7>{0x00000055});             //                         01010101
    W_TEST(BS("\x42\x03\x15"), bitset< 5>{0x00000015});             //                           010101
    W_TEST(BS("\x42\x05\x05"), bitset< 3>{0x00000005});             //                             0101
    W_TEST(BS("\x42\x07\x01"), bitset< 1>{0x00000001});             //                               01
    // errors
    R_TEST(bitset<32>(), BS("\x5C"), cbor::read_error::size_invalid, 0);
    R_TEST(bitset<7>(), BS("\x41"), cbor::read_error::size_invalid, 0);
    R_TEST(bitset<7>(), BS("\x42"), cbor::read_error::unexpected, 1);
    R_TEST(bitset<7>(), BS("\x42\x02"), cbor::read_error::size_invalid, 1);
    R_TEST(bitset<7>(), BS("\x42\x01"), cbor::read_error::unexpected, 2);
    R_TEST(bitset<32>(), BS("\x44"), cbor::read_error::size_invalid, 0);
    R_TEST(bitset<32>(), BS("\x45"), cbor::read_error::unexpected, 1);
    R_TEST(bitset<32>(), BS("\x45\x01"), cbor::read_error::size_invalid, 1);
    R_TEST(bitset<32>(), BS("\x45\x00"), cbor::read_error::unexpected, 2);
    R_TEST(bitset<32>(), BS("\x64"), cbor::read_error::unexpected, 0);
    R_TEST(bitset<7>(), BS("\x5C"), cbor::read_error::size_invalid, 0);
    R_TEST(bitset<5>(), BS("\x41"), cbor::read_error::size_invalid, 0);
    R_TEST(bitset<3>(), BS("\x64"), cbor::read_error::unexpected, 0);
    R_TEST(bitset<3>(), BS("\xDC"), cbor::read_error::tag_invalid, 0);
    R_TEST(bitset<3>(), BS("\xCD\xFF"), cbor::read_error::unexpected, 1);
    {   bio::byte o[1];
        auto const r = to_bytes(std::begin(o), std::end(o), bitset<8 * 32>());
        TEST_CHECK(!r && r.ec == cbor::write_error::output_failure);
    }
    {   bio::byte o[1];
        auto const r = to_bytes(std::begin(o), std::end(o), bitset<32>());
        TEST_CHECK(!r && r.ec == cbor::write_error::output_failure);
    }
    {   bio::byte o[2];
        auto const r = to_bytes(std::begin(o), std::end(o), bitset<32>());
        TEST_CHECK(!r && r.ec == cbor::write_error::output_failure);
    }
TEST_END()


TEST_BEG(chrono, cxon::CBOR<>, "/std")
    using namespace std::chrono;
    R_TEST(duration<unsigned>(42), BS("\x18\x2A"));
    R_TEST(duration<unsigned>(42), BS("\x1C"), cbor::read_error::integer_invalid, 0);
    W_TEST(BS("\x18\x2A"), duration<unsigned>(42));
    R_TEST(time_point<system_clock>(system_clock::duration(42)), BS("\x18\x2A"));
    R_TEST(time_point<system_clock>(system_clock::duration(42)), BS("\x1C"), cbor::read_error::integer_invalid, 0);
    W_TEST(BS("\x18\x2A"), time_point<system_clock>(system_clock::duration(42)));
TEST_END()


TEST_BEG(complex, cxon::CBOR<>, "/std")
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
        // errors
        R_TEST(complex<double>(), BS("\xFF"), cbor::read_error::size_invalid, 0);
        R_TEST(complex<double>(), BS("\x82\xFF"), cbor::read_error::floating_point_invalid, 1);
        R_TEST(complex<double>(), BS("\x82\xFB\x00\x00\x00\x00\x00\x00\x00\x00\xFF"), cbor::read_error::floating_point_invalid, 10);
        R_TEST(complex<double>(), BS("\xDC"), cbor::read_error::tag_invalid, 0);
        R_TEST(complex<double>(), BS("\xCD\xFF"), cbor::read_error::size_invalid, 1);
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<cxon::CBOR<>>(c, complex<double>(3, 1));
            TEST_CHECK(r.ec == cbor::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<cxon::CBOR<>>(c, complex<double>(3, 1));
            TEST_CHECK(r.ec == cbor::write_error::output_failure);
        }
        {   char b[10];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<cxon::CBOR<>>(c, complex<double>(3, 1));
            TEST_CHECK(r.ec == cbor::write_error::output_failure);
        }
TEST_END()


TEST_BEG(deque, cxon::CBOR<>, "/std")
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


TEST_BEG(forward_list, cxon::CBOR<>, "/std")
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
    // errors
    R_TEST(forward_list<int>{}, BS("\xFF"), cbor::read_error::array_invalid, 0);
    R_TEST(forward_list<int>{}, BS("\x82\xFF"), cbor::read_error::integer_invalid, 1);
    R_TEST(forward_list<int>{}, BS("\x82\x03\xFF"), cbor::read_error::integer_invalid, 2);
TEST_END()


TEST_BEG(list, cxon::CBOR<>, "/std")
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


TEST_BEG(map, cxon::CBOR<>, "/std")
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
    // errors
    R_TEST(map<int, int>{}, BS("\xFF"), cbor::read_error::array_invalid, 0);
    R_TEST(map<int, int>{}, BS("\xA1\xFF"), cbor::read_error::integer_invalid, 1);
    R_TEST(map<int, int>{}, BS("\xA1\x01\xFF"), cbor::read_error::integer_invalid, 2);
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            c.push_back('x');
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, map<int, int>{{3, 1}});
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
#   if defined(__GNUC__) && __GNUC__ >= 7 && !defined(__clang__)
#       pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wstringop-overflow"
#   endif
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<cxon::CBOR<>>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == cbor::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<cxon::CBOR<>>(c, map<int, int>{{3, 1}});
            TEST_CHECK(r.ec == cbor::write_error::output_failure);
        }
#   if defined(__GNUC__) && __GNUC__ >= 7 && !defined(__clang__)
#       pragma GCC diagnostic pop
#   endif
TEST_END()


TEST_BEG(multimap, cxon::CBOR<>, "/std")
    R_TEST(multimap<int, int>{}, BS("\xA0"));
    R_TEST(multimap<int, int>{}, BS("\xBF\xFF"));
    W_TEST(BS("\xA0"), multimap<int, int>{});
    R_TEST(multimap<int, int>{{1, 2}}, BS("\xA1\x01\x02"));
    R_TEST(multimap<int, int>{{1, 2}, {1, 3}}, BS("\xA2\x01\x02\x01\x03"));
    W_TEST(BS("\xA1\x01\x02"), multimap<int, int>{{1, 2}});
TEST_END()


#ifdef CXON_HAS_LIB_STD_OPTIONAL
    TEST_BEG(optional, cxon::CBOR<>, "/std")
        R_TEST(optional<int>(42), BS("\x18\x2A"));
        W_TEST(BS("\x18\x2A"), optional<int>(42));
        R_TEST(optional<int>(), BS("\xF7"));
        W_TEST(BS("\xF7"), optional<int>());
        // errors
        R_TEST(optional<int>(42), BS("\xFF"), cbor::read_error::integer_invalid, 0);
        R_TEST(optional<int>(42), BS("\xDC"), cbor::read_error::tag_invalid, 0);
        R_TEST(optional<int>(42), BS("\xCD\xFF"), cbor::read_error::integer_invalid, 1);
    TEST_END()
#endif


TEST_BEG(queue, cxon::CBOR<>, "/std")
    R_TEST(queue<int>{}, BS("\x80"));
    R_TEST(queue<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), queue<int>{});
    R_TEST(queue<int>({1, 2, 3}), BS("\x83\x01\x02\x03"));
    W_TEST(BS("\x83\x01\x02\x03"), queue<int>({1, 2, 3}));
TEST_END()


TEST_BEG(priority_queue, cxon::CBOR<>, "/std")
    R_TEST(priority_queue<int>{}, BS("\x80"));
    R_TEST(priority_queue<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), priority_queue<int>{});
    R_TEST(priority_queue<int>(less<int>(), {1, 2, 3}), BS("\x83\x03\x02\x01"));
    W_TEST(BS("\x83\x03\x02\x01"), priority_queue<int>(less<int>(), {1, 2, 3}));
TEST_END()


TEST_BEG(set, cxon::CBOR<>, "/std")
    R_TEST(set<int>{}, BS("\x80"));
    R_TEST(set<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), set<int>{});
    R_TEST(set<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(set<int>{1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x82\x01\x02"), set<int>{1, 2});
    // errors
    R_TEST(set<int>{1, 2}, BS("\xFF"), cbor::read_error::array_invalid, 0);
    R_TEST(set<int>{1, 2}, BS("\x82\xFF"), cbor::read_error::integer_invalid, 1);
    R_TEST(set<int>{1, 2}, BS("\x82\x01\xFF"), cbor::read_error::integer_invalid, 2);
TEST_END()


TEST_BEG(multiset, cxon::CBOR<>, "/std")
    R_TEST(multiset<int>{}, BS("\x80"));
    R_TEST(multiset<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), multiset<int>{});
    R_TEST(multiset<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(multiset<int>{1, 1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x82\x01\x02"), multiset<int>{1, 2});
TEST_END()


TEST_BEG(pair, cxon::CBOR<>, "/std")
    R_TEST(pair<int, float>(1, 2.f), BS("\x82\x01\x02"));
    R_TEST(pair<int, float>(1, 2.f), BS("\x82\x01\xFA\x40\x00\x00\x00"));
    W_TEST(BS("\x82\x01\xFA\x40\x00\x00\x00"), pair<int, float>(1, 2.f));
    R_TEST(pair<int, float>(), BS("\x80"), cbor::read_error::size_invalid, 0);
    R_TEST(pair<int, float>(), BS("\x81"), cbor::read_error::size_invalid, 0);
    R_TEST(pair<int, float>(), BS("\x83"), cbor::read_error::size_invalid, 0);
    R_TEST(pair<int, float>(), BS("\x82"), cbor::read_error::integer_invalid, 1);
    R_TEST(pair<int, float>(), BS("\x82\x00"), cbor::read_error::floating_point_invalid, 2);
    // errors
    R_TEST(pair<int, float>(), BS("\xFF"), cbor::read_error::size_invalid, 0);
    R_TEST(pair<int, float>(), BS("\x82\xFF"), cbor::read_error::integer_invalid, 1);
    R_TEST(pair<int, float>(), BS("\x82\x01\xFF"), cbor::read_error::floating_point_invalid, 2);
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            c.push_back('x');
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, pair<int, float>(3, 1.f));
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, pair<int, float>(3, 1.f));
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
    {   char b[2];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, pair<int, float>(3, 1.f));
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
TEST_END()


TEST_BEG(stack, cxon::CBOR<>, "/std")
    R_TEST(stack<int>{}, BS("\x80"));
    R_TEST(stack<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), stack<int>{});
    R_TEST(stack<int>({1, 2}), BS("\x82\x01\x02"));
    W_TEST(BS("\x82\x01\x02"), stack<int>({1, 2}));
TEST_END()


TEST_BEG(tuple, cxon::CBOR<>, "/std")
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
    // errors
    R_TEST(tuple<>(), BS("\xFF"), cbor::read_error::size_invalid, 0);
    R_TEST(tuple<>(), BS("\xDC"), cbor::read_error::tag_invalid, 0);
    R_TEST(tuple<>(), BS("\xCD\xFF"), cbor::read_error::size_invalid, 1);
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            c.push_back('x');
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, tuple<>());
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
    R_TEST(tuple<int, float>(), BS("\xFF"), cbor::read_error::size_invalid, 0);
    R_TEST(tuple<int, float>(), BS("\x82\xFF"), cbor::read_error::integer_invalid, 1);
    R_TEST(tuple<int, float>(), BS("\x82\x01\xFF"), cbor::read_error::floating_point_invalid, 2);
    R_TEST(tuple<int, float>(), BS("\xDC"), cbor::read_error::tag_invalid, 0);
    R_TEST(tuple<int, float>(), BS("\xCD\xFF"), cbor::read_error::size_invalid, 1);
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            c.push_back('x');
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, tuple<int, float>(3, 1.f));
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, tuple<int, float>(3, 1.f));
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
    {   char b[2];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, tuple<int, float>(3, 1.f));
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
TEST_END()


TEST_BEG(unordered_map, cxon::CBOR<>, "/std")
    R_TEST(unordered_map<int, int>{}, BS("\xA0"));
    R_TEST(unordered_map<int, int>{}, BS("\xBF\xFF"));
    W_TEST(BS("\xA0"), unordered_map<int, int>{});
    R_TEST(unordered_map<int, int>{{1, 2}}, BS("\xA1\x01\x02"));
    W_TEST(BS("\xA1\x01\x02"), unordered_map<int, int>{{1, 2}});
TEST_END()


TEST_BEG(unordered_multimap, cxon::CBOR<>, "/std")
    R_TEST(unordered_multimap<int, int>{}, BS("\xA0"));
    R_TEST(unordered_multimap<int, int>{}, BS("\xBF\xFF"));
    W_TEST(BS("\xA0"), unordered_multimap<int, int>{});
    R_TEST(unordered_multimap<int, int>{{1, 2}}, BS("\xA1\x01\x02"));
    W_TEST(BS("\xA1\x01\x02"), unordered_multimap<int, int>{{1, 2}});
TEST_END()


TEST_BEG(unordered_set, cxon::CBOR<>, "/std")
    R_TEST(unordered_set<int>{}, BS("\x80"));
    R_TEST(unordered_set<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), unordered_set<int>{});
    R_TEST(unordered_set<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(unordered_set<int>{1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x81\x01"), unordered_set<int>{1});
TEST_END()


TEST_BEG(unordered_multiset, cxon::CBOR<>, "/std")
    R_TEST(unordered_multiset<int>{}, BS("\x80"));
    R_TEST(unordered_multiset<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), unordered_multiset<int>{});
    R_TEST(unordered_multiset<int>{1, 2}, BS("\x82\x01\x02"));
    R_TEST(unordered_multiset<int>{1, 1, 2}, BS("\x83\x01\x02\x01"));
    W_TEST(BS("\x81\x01"), unordered_multiset<int>{1});
TEST_END()


TEST_BEG(valarray,cxon::CBOR<>, "/std")
    R_TEST(valarray<unsigned char>{}, BS("\x40"));
    R_TEST(valarray<unsigned char>{}, BS("\x5F\xFF"));
    R_TEST(valarray<unsigned char>{}, BS("\x7F\xFF"));
    R_TEST(valarray<unsigned char>{}, BS("\x60"));
    R_TEST(valarray<int>{}, BS("\x80"));
    R_TEST(valarray<int>{}, BS("\x9F\xFF"));
    W_TEST(BS("\x80"), valarray<int>{});
    R_TEST(valarray<signed char>{1, 2, 3}, BS("\x43\x01\x02\x03"));
    R_TEST(valarray<signed char>{1, 2, 3}, BS("\x63\x01\x02\x03"));
    R_TEST(valarray<int>{1, 2, 3}, BS("\x83\x01\x02\x03"));
    W_TEST(BS("\x83\x01\x02\x03"), valarray<int>{1, 2, 3});
    // errors
    R_TEST(valarray<int>{}, BS("\xFF"), cbor::read_error::array_invalid, 0);
    R_TEST(valarray<int>{}, BS("\x81\xFF"), cbor::read_error::integer_invalid, 1);
    R_TEST(valarray<int>{}, BS("\xDC"), cbor::read_error::tag_invalid, 0);
    R_TEST(valarray<int>{}, BS("\xCD"), cbor::read_error::array_invalid, 1);
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            c.push_back('x');
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, valarray<int>{3, 1});
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<cxon::CBOR<>>(c, valarray<int>{3, 1});
        TEST_CHECK(r.ec == cbor::write_error::output_failure);
    }
TEST_END()


#ifdef CXON_HAS_LIB_STD_VARIANT
    TEST_BEG(variant, cxon::CBOR<>, "/std")
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


TEST_BEG(vector, cxon::CBOR<>, "/std")
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


TEST_BEG(vector_bool, cxon::CBOR<>, "/std")
    R_TEST(vector<bool>{}, BS("\x80"));
    W_TEST(BS("\x80"), vector<bool>{});
    R_TEST(vector<bool>{true, false}, BS("\x82\xF5\xF4"));
    W_TEST(BS("\x82\xF5\xF4"), vector<bool>{true, false});
TEST_END()


TEST_BEG(tags, cxon::CBOR<>, "/std")
    using namespace std::chrono;
    R_TEST(bitset<32>{0xAAAAAAAA}, BS("\xC1\x45\x00\xAA\xAA\xAA\xAA"));
    R_TEST(bitset<32>{0xAAAAAAAA}, BS("\xD8\x18\x45\x00\xAA\xAA\xAA\xAA"));
    R_TEST(bitset<32>{0xAAAAAAAA}, BS("\xD9\x01\x01\x45\x00\xAA\xAA\xAA\xAA"));
    R_TEST(bitset<32>{0xAAAAAAAA}, BS("\xDA\x01\x01\x01\x01\x45\x00\xAA\xAA\xAA\xAA"));
    R_TEST(bitset<32>{0xAAAAAAAA}, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x45\x00\xAA\xAA\xAA\xAA"));
    R_TEST(complex<float>(1, 2), BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x82\xFA\x3F\x80\x00\x00\xFA\x40\x00\x00\x00"));
    R_TEST(complex<float>(1, 2), BS("\xDA\x01\x01\x01\x01\x82\xFA\x3F\x80\x00\x00\xFA\x40\x00\x00\x00"));
    R_TEST(complex<float>(1, 2), BS("\xD9\x01\x01\x82\xFA\x3F\x80\x00\x00\xFA\x40\x00\x00\x00"));
    R_TEST(complex<float>(1, 2), BS("\xD8\x18\x82\xFA\x3F\x80\x00\x00\xFA\x40\x00\x00\x00"));
    R_TEST(complex<float>(1, 2), BS("\xC1\x82\xFA\x3F\x80\x00\x00\xFA\x40\x00\x00\x00"));
    R_TEST(duration<unsigned>(42), BS("\xC1\x18\x2A"));
    R_TEST(duration<unsigned>(42), BS("\xD8\x18\x18\x2A"));
    R_TEST(duration<unsigned>(42), BS("\xD9\x01\x01\x18\x2A"));
    R_TEST(duration<unsigned>(42), BS("\xDA\x01\x01\x01\x01\x18\x2A"));
    R_TEST(duration<unsigned>(42), BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x18\x2A"));
    R_TEST(time_point<system_clock>(system_clock::duration(42)), BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x18\x2A"));
    R_TEST(time_point<system_clock>(system_clock::duration(42)), BS("\xDA\x01\x01\x01\x01\x18\x2A"));
    R_TEST(time_point<system_clock>(system_clock::duration(42)), BS("\xD9\x01\x01\x18\x2A"));
    R_TEST(time_point<system_clock>(system_clock::duration(42)), BS("\xD8\x18\x18\x2A"));
    R_TEST(time_point<system_clock>(system_clock::duration(42)), BS("\xC1\x18\x2A"));
    R_TEST(map<int, int>{}, BS("\xC1\xA0"));
    R_TEST(map<int, int>{}, BS("\xD8\x18\xA0"));
    R_TEST(map<int, int>{}, BS("\xD9\x01\x01\xA0"));
    R_TEST(map<int, int>{}, BS("\xDA\x01\x01\x01\x01\xA0"));
    R_TEST(map<int, int>{}, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\xA0"));
#   ifdef CXON_HAS_LIB_STD_OPTIONAL
        R_TEST(optional<int>(42), BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x18\x2A"));
        R_TEST(optional<int>(42), BS("\xDA\x01\x01\x01\x01\x18\x2A"));
        R_TEST(optional<int>(42), BS("\xD9\x01\x01\x18\x2A"));
        R_TEST(optional<int>(42), BS("\xD8\x18\x18\x2A"));
        R_TEST(optional<int>(42), BS("\xC1\x18\x2A"));
        R_TEST(optional<int>(), BS("\xC1\xF7"));
        R_TEST(optional<int>(), BS("\xD8\x18\xF7"));
        R_TEST(optional<int>(), BS("\xD9\x01\x01\xF7"));
        R_TEST(optional<int>(), BS("\xDA\x01\x01\x01\x01\xF7"));
        R_TEST(optional<int>(), BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\xF7"));
#   endif
    R_TEST(pair<int, float>(1, 2.f), BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x82\x01\x02"));
    R_TEST(pair<int, float>(1, 2.f), BS("\xDA\x01\x01\x01\x01\x82\x01\x02"));
    R_TEST(pair<int, float>(1, 2.f), BS("\xD9\x01\x01\x82\x01\x02"));
    R_TEST(pair<int, float>(1, 2.f), BS("\xD8\x18\x82\x01\x02"));
    R_TEST(pair<int, float>(1, 2.f), BS("\xC1\x82\x01\x02"));
    R_TEST(tuple<int, float>(1, 2.f), BS("\xC1\x82\x01\x02"));
    R_TEST(tuple<int, float>(1, 2.f), BS("\xD8\x18\x82\x01\x02"));
    R_TEST(tuple<int, float>(1, 2.f), BS("\xD9\x01\x01\x82\x01\x02"));
    R_TEST(tuple<int, float>(1, 2.f), BS("\xDA\x01\x01\x01\x01\x82\x01\x02"));
    R_TEST(tuple<int, float>(1, 2.f), BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x82\x01\x02"));
    R_TEST(tuple<>{}, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x80"));
    R_TEST(tuple<>{}, BS("\xDA\x01\x01\x01\x01\x80"));
    R_TEST(tuple<>{}, BS("\xD9\x01\x01\x80"));
    R_TEST(tuple<>{}, BS("\xD8\x18\x80"));
    R_TEST(tuple<>{}, BS("\xC1\x80"));
#   ifdef CXON_HAS_LIB_STD_VARIANT
        R_TEST(variant<int, double>(in_place_index_t<0>(), 1), BS("\xC1\x82\x00\x01"));
        R_TEST(variant<int, double>(in_place_index_t<0>(), 1), BS("\xD8\x18\x82\x00\x01"));
        R_TEST(variant<int, double>(in_place_index_t<0>(), 1), BS("\xD9\x01\x01\x82\x00\x01"));
        R_TEST(variant<int, double>(in_place_index_t<0>(), 1), BS("\xDA\x01\x01\x01\x01\x82\x00\x01"));
        R_TEST(variant<int, double>(in_place_index_t<0>(), 1), BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x82\x00\x01"));
        R_TEST(variant<monostate, int>(), BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x82\x00\xF7"));
        R_TEST(variant<monostate, int>(), BS("\xDA\x01\x01\x01\x01\x82\x00\xF7"));
        R_TEST(variant<monostate, int>(), BS("\xD9\x01\x01\x82\x00\xF7"));
        R_TEST(variant<monostate, int>(), BS("\xD8\x18\x82\x00\xF7"));
        R_TEST(variant<monostate, int>(), BS("\xC1\x82\x00\xF7"));
#   endif
    R_TEST(vector<int>{}, BS("\xDB\x01\x01\x01\x01\x01\x01\x01\x01\x80"));
    R_TEST(vector<int>{}, BS("\xDA\x01\x01\x01\x01\x60"));
    R_TEST(vector<int>{}, BS("\xD9\x01\x01\x40"));
    R_TEST(vector<int>{}, BS("\xD8\x18\x60"));
    R_TEST(vector<int>{}, BS("\xC1\x80"));
    // errors
    R_TEST((array<int, 0>{}), BS("\xDC\x80"), cbor::read_error::tag_invalid, 0);
    R_TEST(string(""), BS("\xDC\x40"), cbor::read_error::tag_invalid, 0);
TEST_END()
