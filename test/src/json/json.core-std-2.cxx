// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#include "test.hxx"

#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/tuple.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/variant.hxx"
#include "cxon/lib/std/bitset.hxx"
#include "cxon/lib/std/memory.hxx"


////////////////////////////////////////////////////////////////////////////////

namespace std {
    template <size_t N>
        inline bool operator <(const std::bitset<N>& t1, const std::bitset<N>& t2) {
            return t1.to_string() < t2.to_string();
        };
}

enum class keys_enum { E1, E2 };
CXON_JSON_ENM(keys_enum,
    CXON_JSON_ENM_VALUE_ASIS(E1),
    CXON_JSON_ENM_VALUE_ASIS(E2)
)

struct key_struct {
    int x, y;
    friend bool operator ==(const key_struct& t1, const key_struct& t2) {
        return t1.x == t2.x && t1.y == t2.y;
    }
    friend bool operator <(const key_struct& t1, const key_struct& t2) {
        return t1.x < t2.x || (t1.x == t2.x && t1.y < t2.y);
    }
};
CXON_JSON_CLS(key_struct,
    CXON_JSON_CLS_FIELD_ASIS(x),
    CXON_JSON_CLS_FIELD_ASIS(y)
)

TEST_BEG(quoted_keys, cxon::JSON<>, "/std")
    using namespace std;

    R_TEST(map<string, int>{{"1", 2}, {"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<string, int>{{"1", 2}, {"3", 4}});
    R_TEST(map<wstring, int>{{L"1", 2}, {L"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<wstring, int>{{L"1", 2}, {L"3", 4}});
    R_TEST(map<u16string, int>{{u"1", 2}, {u"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u16string, int>{{u"1", 2}, {u"3", 4}});
    R_TEST(map<u32string, int>{{U"1", 2}, {U"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u32string, int>{{U"1", 2}, {U"3", 4}});
    W_TEST(R"({"1":2,"3":4})", map<char, int>{{'1', 2}, {'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<wchar_t, int>{{L'1', 2}, {L'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char16_t, int>{{u'1', 2}, {u'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char32_t, int>{{U'1', 2}, {U'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<signed char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<unsigned char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<unsigned short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<int, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<unsigned, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<unsigned long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<unsigned long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({"1":2,"3":4})", map<float, int>{{1.f, 2}, {3.f, 4}});
    W_TEST(R"({"1":2,"3":4})", map<double, int>{{1., 2}, {3., 4}});
    W_TEST(R"({"1":2,"3":4})", map<long double, int>{{1., 2}, {3., 4}});
    {   // spaces around the key
        R_TEST(map<bool, int>{{true, 1}, {false, 0}}, R"({" true": 1, "false ": 0})");
        R_TEST(map<char, int>{{'1', 1}}, R"({" 1": 1})", json::read_error::unexpected, 3);
        R_TEST(map<char, int>{{'1', 1}}, R"({"1 ": 1})", json::read_error::unexpected, 3);
#       if defined(__cpp_char8_t)
            R_TEST(map<char8_t, int>{{u8'1', 1}}, R"({" 1": 1})", json::read_error::unexpected, 3);
            R_TEST(map<char8_t, int>{{u8'1', 1}}, R"({"1 ": 1})", json::read_error::unexpected, 3);
#       endif
        R_TEST(map<char16_t, int>{{u'1', 1}}, R"({" 1": 1})", json::read_error::unexpected, 3);
        R_TEST(map<char16_t, int>{{u'1', 1}}, R"({"1 ": 1})", json::read_error::unexpected, 3);
        R_TEST(map<char32_t, int>{{U'1', 1}}, R"({" 1": 1})", json::read_error::unexpected, 3);
        R_TEST(map<char32_t, int>{{U'1', 1}}, R"({"1 ": 1})", json::read_error::unexpected, 3);
        R_TEST(map<wchar_t, int>{{L'1', 1}}, R"({" 1": 1})", json::read_error::unexpected, 3);
        R_TEST(map<wchar_t, int>{{L'1', 1}}, R"({"1 ": 1})", json::read_error::unexpected, 3);
        R_TEST(map<int, int>{{1, 2}, {3, 4}}, R"({" 1": 2, "3 ": 4})");
        R_TEST(map<double, int>{{1, 2}, {3, 4}}, R"({" 1": 2, "3 ": 4})");
    }
    {   struct less { constexpr bool operator ()(nullptr_t, nullptr_t) const noexcept { return true; } };
        W_TEST(R"({"null":0})", map<nullptr_t, int, less>{{nullptr, 0}});
        R_TEST(map<std::nullptr_t, int, less>{{nullptr, 0}}, R"({" null": 0})");
        R_TEST(map<std::nullptr_t, int, less>{{nullptr, 0}}, R"({"null ": 0})");
    }
    // escape quotes
    {   using xmap = map<map<char, int>, int>;
        R_TEST(xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}}, R"({"{\"1\":2}":3,"{\"4\":5}":6})");
        W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}});
    }
    {   using xmap = map<map<wchar_t, int>, int>;
        R_TEST(xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}}, R"({"{\"1\":2}":3,"{\"4\":5}":6})");
        W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}});
    }
    {   using xmap = map<map<char16_t, int>, int>;
        R_TEST(xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}}, R"({"{\"1\":2}":3,"{\"4\":5}":6})");
        W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}});
    }
    {   using xmap = map<map<char32_t, int>, int>;
        R_TEST(xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}}, R"({"{\"1\":2}":3,"{\"4\":5}":6})");
        W_TEST(R"({"{\"1\":2}":3,"{\"4\":5}":6})", xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}});
    }
    {   using xmap = map<map<string, int>, int>;
        R_TEST(xmap{{{{"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\u0022z\":1}":3})");
        W_TEST(R"({"{\"x\ty\u0022z\":1}":3})", xmap{{{{"x\ty\"z", 1}}, 3}});
        R_TEST(xmap{{{{"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\"z\":1}":3})", json::read_error::unexpected, 11);
    }
    {   using xmap = map<map<wstring, int>, int>;
        R_TEST(xmap{{{{L"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\u0022z\":1}":3})");
        W_TEST(R"({"{\"x\ty\u0022z\":1}":3})", xmap{{{{L"x\ty\"z", 1}}, 3}});
        R_TEST(xmap{{{{L"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\"z\":1}":3})", json::read_error::unexpected, 11);
    }
    {   using xmap = map<map<u16string, int>, int>;
        R_TEST(xmap{{{{u"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\u0022z\":1}":3})");
        W_TEST(R"({"{\"x\ty\u0022z\":1}":3})", xmap{{{{u"x\ty\"z", 1}}, 3}});
        R_TEST(xmap{{{{u"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\"z\":1}":3})", json::read_error::unexpected, 11);
    }
    {   using xmap = map<map<u32string, int>, int>;
        R_TEST(xmap{{{{U"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\u0022z\":1}":3})");
        W_TEST(R"({"{\"x\ty\u0022z\":1}":3})", xmap{{{{U"x\ty\"z", 1}}, 3}});
        R_TEST(xmap{{{{U"x\ty\"z", 1}}, 3}}, R"({"{\"x\ty\"z\":1}":3})", json::read_error::unexpected, 11);
    }
    {   using xmap = map<map<map<string, int>, int>, int>;
        R_TEST(xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}}, R"({"{{\"x\ty\u0022z\":1}:3}":5})");
        W_TEST(R"({"{{\"x\ty\u0022z\":1}:3}":5})", xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<wstring, int>, int>, int>;
        R_TEST(xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}}, R"({"{{\"x\ty\u0022z\":1}:3}":5})");
        W_TEST(R"({"{{\"x\ty\u0022z\":1}:3}":5})", xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u16string, int>, int>, int>;
        R_TEST(xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}}, R"({"{{\"x\ty\u0022z\":1}:3}":5})");
        W_TEST(R"({"{{\"x\ty\u0022z\":1}:3}":5})", xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u32string, int>, int>, int>;
        R_TEST(xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}}, R"({"{{\"x\ty\u0022z\":1}:3}":5})");
        W_TEST(R"({"{{\"x\ty\u0022z\":1}:3}":5})", xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<string, int>, int>;
        R_TEST(xmap{}, R"({"{\"1\)", json::read_error::escape_invalid, 6);
        R_TEST(xmap{}, R"({"{\"1\1)", json::read_error::escape_invalid, 6);
    }
    {   using xmap = map<map<wstring, int>, int>;
        R_TEST(xmap{}, R"({"{\"1\)", json::read_error::escape_invalid, 6);
        R_TEST(xmap{}, R"({"{\"1\1)", json::read_error::escape_invalid, 6);
    }
    {   using xmap = map<map<u16string, int>, int>;
        R_TEST(xmap{}, R"({"{\"1\)", json::read_error::escape_invalid, 6);
        R_TEST(xmap{}, R"({"{\"1\1)", json::read_error::escape_invalid, 6);
    }
    {   using xmap = map<map<u32string, int>, int>;
        R_TEST(xmap{}, R"({"{\"1\)", json::read_error::escape_invalid, 6);
        R_TEST(xmap{}, R"({"{\"1\1)", json::read_error::escape_invalid, 6);
    }
    {   char b[1]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[2]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[3]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[4]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[5]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[6]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[7]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[8]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[9]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[10]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[11]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[12]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[13]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[14]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[15]; using xmap = map<map<string, int>, int>;
        auto c = cxon::cnt::make_range_container(begin(b), end(b));
            auto r = cxon::to_bytes<X>(c, xmap{{{{"1", 2}}, 3}});
        TEST_CHECK(r);
    }
    // inf/nan
    {
        constexpr auto ninf = -numeric_limits<double>::infinity();
        constexpr auto pinf =  numeric_limits<double>::infinity();
        //constexpr auto qnan =  numeric_limits<double>::quiet_NaN();
        {   using xmap = map<double, int>;
            R_TEST(xmap{{ninf, 1}, {pinf, 2}}, R"({"-inf":1,"inf":2})");
            W_TEST(R"({"-inf":1,"inf":2})", xmap{{ninf, 1}, {pinf, 2}});
        }
        {   using xmap = map<map<double, int>, int>;
            R_TEST(xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}}, R"({"{-inf:1}":2,"{inf:3}":4})");
            W_TEST(R"({"{-inf:1}":2,"{inf:3}":4})", xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}});
        }
    }
    // std::bitset
    {   using xmap = map<bitset<8>, int>;
        R_TEST(xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}}, R"({"00000000":1,"11111111":2})");
        W_TEST(R"({"00000000":1,"11111111":2})", xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}});
    }
    {   using xmap = map<map<bitset<8>, int>, int>;
        R_TEST(xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}}, R"({"{\"00000000\":1}":2,"{\"11111111\":3}":4})");
        W_TEST(R"({"{\"00000000\":1}":2,"{\"11111111\":3}":4})", xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}});
    }
    // enum
    {   using xmap = map<keys_enum, int>;
        R_TEST(xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}}, R"({"E1":1,"E2":2})");
        W_TEST(R"({"E1":1,"E2":2})", xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}});
    }
    {   using xmap = map<map<keys_enum, int>, int>;
        R_TEST(xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}}, R"({"{\"E1\":1}":2,"{\"E2\":3}":4})");
        W_TEST(R"({"{\"E1\":1}":2,"{\"E2\":3}":4})", xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}});
    }
    // struct
    {   using xmap = map<key_struct, int>;
        R_TEST(xmap{{{1, 2}, 3}, {{4, 5}, 6}}, R"({"{\"x\":1,\"y\":2}":3,"{\"x\":4,\"y\":5}":6})");
        W_TEST(R"({"{\"x\":1,\"y\":2}":3,"{\"x\":4,\"y\":5}":6})", xmap{{{1, 2}, 3}, {{4, 5}, 6}});
    }
TEST_END()

namespace {
    struct single_quotes_traits : cxon::json::format_traits {
        struct string {
            static constexpr char del = '\'';
        };
    };
}
TEST_BEG(single_quoted_keys, cxon::JSON<single_quotes_traits>, "/std")
    using namespace std;
    {   using xmap = map<map<string, int>, int>;
        R_TEST(xmap{{{{"x\ty'z", 1}}, 3}}, R"({'{\'x\ty\u0027z\':1}':3})");
        R_TEST(xmap{{{{"x\ty'z", 1}}, 3}}, R"({'{\'x\ty'z\':1}':3})");
        W_TEST(R"({'{\'x\ty'z\':1}':3})", xmap{{{{"x\ty'z", 1}}, 3}});
    }
    {   using xmap = map<map<wstring, int>, int>;
        R_TEST(xmap{{{{L"x\ty'z", 1}}, 3}}, R"({'{\'x\ty\u0027z\':1}':3})");
        R_TEST(xmap{{{{L"x\ty'z", 1}}, 3}}, R"({'{\'x\ty'z\':1}':3})");
        W_TEST(R"({'{\'x\ty'z\':1}':3})", xmap{{{{L"x\ty'z", 1}}, 3}});
    }
    {   using xmap = map<map<u16string, int>, int>;
        R_TEST(xmap{{{{u"x\ty'z", 1}}, 3}}, R"({'{\'x\ty\u0027z\':1}':3})");
        R_TEST(xmap{{{{u"x\ty'z", 1}}, 3}}, R"({'{\'x\ty'z\':1}':3})");
        W_TEST(R"({'{\'x\ty'z\':1}':3})", xmap{{{{u"x\ty'z", 1}}, 3}});
    }
    {   using xmap = map<map<u32string, int>, int>;
        R_TEST(xmap{{{{U"x\ty'z", 1}}, 3}}, R"({'{\'x\ty\u0027z\':1}':3})");
        R_TEST(xmap{{{{U"x\ty'z", 1}}, 3}}, R"({'{\'x\ty'z\':1}':3})");
        W_TEST(R"({'{\'x\ty'z\':1}':3})", xmap{{{{U"x\ty'z", 1}}, 3}});
    }
    {   using xmap = map<map<map<string, int>, int>, int>;
        R_TEST(xmap{{{{{{"x\ty'z", 1}}, 3}}, 5}}, R"({'{{\'x\ty'z\':1}:3}':5})");
        W_TEST(R"({'{{\'x\ty'z\':1}:3}':5})", xmap{{{{{{"x\ty'z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<wstring, int>, int>, int>;
        R_TEST(xmap{{{{{{L"x\ty'z", 1}}, 3}}, 5}}, R"({'{{\'x\ty'z\':1}:3}':5})");
        W_TEST(R"({'{{\'x\ty'z\':1}:3}':5})", xmap{{{{{{L"x\ty'z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u16string, int>, int>, int>;
        R_TEST(xmap{{{{{{u"x\ty'z", 1}}, 3}}, 5}}, R"({'{{\'x\ty'z\':1}:3}':5})");
        W_TEST(R"({'{{\'x\ty'z\':1}:3}':5})", xmap{{{{{{u"x\ty'z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u32string, int>, int>, int>;
        R_TEST(xmap{{{{{{U"x\ty'z", 1}}, 3}}, 5}}, R"({'{{\'x\ty'z\':1}:3}':5})");
        W_TEST(R"({'{{\'x\ty'z\':1}:3}':5})", xmap{{{{{{U"x\ty'z", 1}}, 3}}, 5}});
    }
TEST_END()

TEST_BEG(unquoted_keys, cxon::JSON<cxon::test::unquoted_keys_traits<>>, "/std")
    using namespace std;

    R_TEST(map<string, int>{{"1", 2}, {"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<string, int>{{"1", 2}, {"3", 4}});
    R_TEST(map<wstring, int>{{L"1", 2}, {L"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<wstring, int>{{L"1", 2}, {L"3", 4}});
    R_TEST(map<u16string, int>{{u"1", 2}, {u"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u16string, int>{{u"1", 2}, {u"3", 4}});
    R_TEST(map<u32string, int>{{U"1", 2}, {U"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u32string, int>{{U"1", 2}, {U"3", 4}});
    W_TEST(R"({"1":2,"3":4})", map<char, int>{{'1', 2}, {'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<wchar_t, int>{{L'1', 2}, {L'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char16_t, int>{{u'1', 2}, {u'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char32_t, int>{{U'1', 2}, {U'3', 4}});
    W_TEST(R"({1:2,3:4})", map<signed char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<int, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<float, int>{{1.f, 2}, {3.f, 4}});
    W_TEST(R"({1:2,3:4})", map<double, int>{{1., 2}, {3., 4}});
    W_TEST(R"({1:2,3:4})", map<long double, int>{{1., 2}, {3., 4}});
    {   struct less { constexpr bool operator ()(nullptr_t, nullptr_t) const noexcept { return true; } };
        W_TEST(R"({null:0})", map<nullptr_t, int, less>{{nullptr, 0}});
    }
    // escape quotes
    {   using xmap = map<map<char, int>, int>;
        R_TEST(xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}});
    }
    {   using xmap = map<map<wchar_t, int>, int>;
        R_TEST(xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}});
    }
    {   using xmap = map<map<char16_t, int>, int>;
        R_TEST(xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}});
    }
    {   using xmap = map<map<char32_t, int>, int>;
        R_TEST(xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}});
    }
    {   using xmap = map<map<string, int>, int>;
        R_TEST(xmap{ {{{"x\ty\"z", 1}}, 3} }, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<wstring, int>, int>;
        R_TEST(xmap{{{{L"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{L"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<u16string, int>, int>;
        R_TEST(xmap{{{{u"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{u"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<u32string, int>, int>;
        R_TEST(xmap{{{{U"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{U"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<map<string, int>, int>, int>;
        R_TEST(xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<wstring, int>, int>, int>;
        R_TEST(xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u16string, int>, int>, int>;
        R_TEST(xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u32string, int>, int>, int>;
        R_TEST(xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}});
    }
    // inf/nan
    {
        constexpr auto ninf = -numeric_limits<double>::infinity();
        constexpr auto pinf =  numeric_limits<double>::infinity();
        //constexpr auto qnan =  numeric_limits<double>::quiet_NaN();
        {   using xmap = map<double, int>;
            R_TEST(xmap{{ninf, 1}, {pinf, 2}}, R"({"-inf":1,"inf":2})");
            W_TEST(R"({"-inf":1,"inf":2})", xmap{{ninf, 1}, {pinf, 2}});
        }
        {   using xmap = map<map<double, int>, int>;
            R_TEST(xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}}, R"({{"-inf":1}:2,{"inf":3}:4})");
            W_TEST(R"({{"-inf":1}:2,{"inf":3}:4})", xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}});
        }
    }
    // std::bitset
    {   using xmap = map<bitset<8>, int>;
        R_TEST(xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}}, R"({"00000000":1,"11111111":2})");
        W_TEST(R"({"00000000":1,"11111111":2})", xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}});
    }
    {   using xmap = map<map<bitset<8>, int>, int>;
        R_TEST(xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}}, R"({{"00000000":1}:2,{"11111111":3}:4})");
        W_TEST(R"({{"00000000":1}:2,{"11111111":3}:4})", xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}});
    }
    // enum
    {   using xmap = map<keys_enum, int>;
        R_TEST(xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}}, R"({"E1":1,"E2":2})");
        W_TEST(R"({"E1":1,"E2":2})", xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}});
    }
    {   using xmap = map<map<keys_enum, int>, int>;
        R_TEST(xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}}, R"({{"E1":1}:2,{"E2":3}:4})");
        W_TEST(R"({{"E1":1}:2,{"E2":3}:4})", xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}});
    }
    // struct
    {   using xmap = map<key_struct, int>;
        R_TEST(xmap{{{1, 2}, 3}, {{4, 5}, 6}}, R"({{"x":1,"y":2}:3,{"x":4,"y":5}:6})");
        W_TEST(R"({{"x":1,"y":2}:3,{"x":4,"y":5}:6})", xmap{{{1, 2}, 3}, {{4, 5}, 6}});
    }
TEST_END()

TEST_BEG(unquoted_keys_input_iterator_keys, cxon::JSON<cxon::test::unquoted_keys_traits<cxon::test::input_iterator_traits<>>>, "/std")
    using namespace std;

    R_TEST(map<string, int>{{"1", 2}, {"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<string, int>{{"1", 2}, {"3", 4}});
    R_TEST(map<wstring, int>{{L"1", 2}, {L"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<wstring, int>{{L"1", 2}, {L"3", 4}});
    R_TEST(map<u16string, int>{{u"1", 2}, {u"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u16string, int>{{u"1", 2}, {u"3", 4}});
    R_TEST(map<u32string, int>{{U"1", 2}, {U"3", 4}}, R"({"1": 2, "3": 4})");
    W_TEST(R"({"1":2,"3":4})", map<u32string, int>{{U"1", 2}, {U"3", 4}});
    W_TEST(R"({"1":2,"3":4})", map<char, int>{{'1', 2}, {'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<wchar_t, int>{{L'1', 2}, {L'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char16_t, int>{{u'1', 2}, {u'3', 4}});
    W_TEST(R"({"1":2,"3":4})", map<char32_t, int>{{U'1', 2}, {U'3', 4}});
    W_TEST(R"({1:2,3:4})", map<signed char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned char, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned short, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<int, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<unsigned long long, int>{{1, 2}, {3, 4}});
    W_TEST(R"({1:2,3:4})", map<float, int>{{1.f, 2}, {3.f, 4}});
    W_TEST(R"({1:2,3:4})", map<double, int>{{1., 2}, {3., 4}});
    W_TEST(R"({1:2,3:4})", map<long double, int>{{1., 2}, {3., 4}});
    {   struct less { constexpr bool operator ()(nullptr_t, nullptr_t) const noexcept { return true; } };
        W_TEST(R"({null:0})", map<nullptr_t, int, less>{{nullptr, 0}});
    }
    // escape quotes
    {   using xmap = map<map<char, int>, int>;
        R_TEST(xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{'1', 2}}, 3}, {{{'4', 5}}, 6}});
    }
    {   using xmap = map<map<wchar_t, int>, int>;
        R_TEST(xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{L'1', 2}}, 3}, {{{L'4', 5}}, 6}});
    }
    {   using xmap = map<map<char16_t, int>, int>;
        R_TEST(xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{u'1', 2}}, 3}, {{{u'4', 5}}, 6}});
    }
    {   using xmap = map<map<char32_t, int>, int>;
        R_TEST(xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}}, R"({{"1":2}:3,{"4":5}:6})");
        W_TEST(R"({{"1":2}:3,{"4":5}:6})", xmap{{{{U'1', 2}}, 3}, {{{U'4', 5}}, 6}});
    }
    {   using xmap = map<map<string, int>, int>;
        R_TEST(xmap{ {{{"x\ty\"z", 1}}, 3} }, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<wstring, int>, int>;
        R_TEST(xmap{{{{L"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{L"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<u16string, int>, int>;
        R_TEST(xmap{{{{u"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{u"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<u32string, int>, int>;
        R_TEST(xmap{{{{U"x\ty\"z", 1}}, 3}}, R"({{"x\ty\"z":1}:3})");
        W_TEST(R"({{"x\ty\"z":1}:3})", xmap{{{{U"x\ty\"z", 1}}, 3}});
    }
    {   using xmap = map<map<map<string, int>, int>, int>;
        R_TEST(xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<wstring, int>, int>, int>;
        R_TEST(xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{L"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u16string, int>, int>, int>;
        R_TEST(xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{u"x\ty\"z", 1}}, 3}}, 5}});
    }
    {   using xmap = map<map<map<u32string, int>, int>, int>;
        R_TEST(xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}}, R"({{{"x\ty\"z":1}:3}:5})");
        W_TEST(R"({{{"x\ty\"z":1}:3}:5})", xmap{{{{{{U"x\ty\"z", 1}}, 3}}, 5}});
    }
    // inf/nan
    {
        constexpr auto ninf = -numeric_limits<double>::infinity();
        constexpr auto pinf =  numeric_limits<double>::infinity();
        //constexpr auto qnan =  numeric_limits<double>::quiet_NaN();
        {   using xmap = map<double, int>;
            R_TEST(xmap{{ninf, 1}, {pinf, 2}}, R"({"-inf":1,"inf":2})");
            W_TEST(R"({"-inf":1,"inf":2})", xmap{{ninf, 1}, {pinf, 2}});
        }
        {   using xmap = map<map<double, int>, int>;
            R_TEST(xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}}, R"({{"-inf":1}:2,{"inf":3}:4})");
            W_TEST(R"({{"-inf":1}:2,{"inf":3}:4})", xmap{{{{ninf, 1}}, 2}, {{{pinf, 3}}, 4}});
        }
    }
    // std::bitset
    {   using xmap = map<bitset<8>, int>;
        R_TEST(xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}}, R"({"00000000":1,"11111111":2})");
        W_TEST(R"({"00000000":1,"11111111":2})", xmap{{bitset<8>(0), 1}, {bitset<8>(255), 2}});
    }
    {   using xmap = map<map<bitset<8>, int>, int>;
        R_TEST(xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}}, R"({{"00000000":1}:2,{"11111111":3}:4})");
        W_TEST(R"({{"00000000":1}:2,{"11111111":3}:4})", xmap{{{{bitset<8>(0), 1}}, 2}, {{{bitset<8>(255), 3}}, 4}});
    }
    // enum
    {   using xmap = map<keys_enum, int>;
        R_TEST(xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}}, R"({"E1":1,"E2":2})");
        W_TEST(R"({"E1":1,"E2":2})", xmap{{keys_enum::E1, 1}, {keys_enum::E2, 2}});
    }
    {   using xmap = map<map<keys_enum, int>, int>;
        R_TEST(xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}}, R"({{"E1":1}:2,{"E2":3}:4})");
        W_TEST(R"({{"E1":1}:2,{"E2":3}:4})", xmap{{{{keys_enum::E1, 1}}, 2}, {{{keys_enum::E2, 3}}, 4}});
    }
    // struct
    {   using xmap = map<key_struct, int>;
        R_TEST(xmap{{{1, 2}, 3}, {{4, 5}, 6}}, R"({{"x":1,"y":2}:3,{"x":4,"y":5}:6})");
        W_TEST(R"({{"x":1,"y":2}:3,{"x":4,"y":5}:6})", xmap{{{1, 2}, 3}, {{4, 5}, 6}});
    }
TEST_END()


namespace {
    struct struct_1 {
        std::string a;
        bool operator ==(const struct_1& t) const { return a == t.a; }
        bool operator  <(const struct_1& t) const { return a  < t.a; }
    };
    struct struct_2 {
        std::string a, c, e, g;
        bool operator ==(const struct_2& t) const { return a == t.a && c == t.c && e == t.e && g == t.g; }
        bool operator  <(const struct_2& t) const { return std::make_tuple(a, c, e, g) < std::make_tuple(t.a, t.c, t.e, t.g); }
    };
    enum class enum_1 { aaa, bbb, ccc, ddd };
}
CXON_JSON_CLS(struct_1,
    CXON_JSON_CLS_FIELD_ASIS(a)
)
CXON_JSON_CLS(struct_2,
    CXON_JSON_CLS_FIELD_ASIS(a), CXON_JSON_CLS_FIELD_ASIS(c), CXON_JSON_CLS_FIELD_ASIS(e), CXON_JSON_CLS_FIELD_ASIS(g)
)
CXON_JSON_ENM(enum_1,
    CXON_JSON_ENM_VALUE_ASIS(aaa),
    CXON_JSON_ENM_VALUE_NAME("b\"b", bbb),
    CXON_JSON_ENM_VALUE_NAME("c c", ccc),
    CXON_JSON_ENM_VALUE_NAME("d:d", ddd)
)

TEST_BEG(unquote_quoted_keys, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<>>, "/std")
    using namespace std;
    {   using xmap = map<string, string>;
        R_TEST(xmap {{"x", "y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{"x", "y"}});
        R_TEST(xmap {{"", "y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{"", "y"}});
        R_TEST(xmap {{"x\"y", "z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{"x\"y", "z"}});
        R_TEST(xmap {{"x:y", "z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{"x:y", "z"}});
        R_TEST(xmap {{"x:y", "z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{"x y", "z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{"x y", "z"}});
        R_TEST(xmap {{"x y", "z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {
        R_TEST(struct_2 {"b", "d", "f", "h"}, "{a\t:\"b\", c:\"d\", e:\"f\", g:\"h\"}");
        R_TEST(struct_2 {"b", "d", "f", "h"}, "{a\n:\"b\", c:\"d\", e:\"f\", g:\"h\"}");
        R_TEST(struct_2 {"b", "d", "f", "h"}, "{a\r:\"b\", c:\"d\", e:\"f\", g:\"h\"}");
        R_TEST(struct_2 {"b", "d", "f", "h"}, "{a :\"b\", c:\"d\", e:\"f\", g:\"h\"}");
        R_TEST(struct_2 {"b",  "",  "",  ""}, "{a\t:\"b\"}");
        R_TEST(struct_2 {"b",  "",  "",  ""}, "{a\n:\"b\"}");
        R_TEST(struct_2 {"b",  "",  "",  ""}, "{a\r:\"b\"}");
        R_TEST(struct_2 {"b",  "",  "",  ""}, "{a :\"b\"}");
    }
    {   using xmap = map<wstring, wstring>;
        R_TEST(xmap {{L"x", L"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{L"x", L"y"}});
        R_TEST(xmap {{L"", L"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{L"", L"y"}});
        R_TEST(xmap {{L"x\"y", L"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{L"x\"y", L"z"}});
        R_TEST(xmap {{L"x:y", L"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{L"x:y", L"z"}});
        R_TEST(xmap {{L"x:y", L"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{L"x y", L"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{L"x y", L"z"}});
        R_TEST(xmap {{L"x y", L"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<u16string, u16string>;
        R_TEST(xmap {{u"x", u"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{u"x", u"y"}});
        R_TEST(xmap {{u"", u"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{u"", u"y"}});
        R_TEST(xmap {{u"x\"y", u"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{u"x\"y", u"z"}});
        R_TEST(xmap {{u"x:y", u"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{u"x:y", u"z"}});
        R_TEST(xmap {{u"x:y", u"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{u"x y", u"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{u"x y", u"z"}});
        R_TEST(xmap {{u"x y", u"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<u32string, u32string>;
        R_TEST(xmap {{U"x", U"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{U"x", U"y"}});
        R_TEST(xmap {{U"", U"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{U"", U"y"}});
        R_TEST(xmap {{U"x\"y", U"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{U"x\"y", U"z"}});
        R_TEST(xmap {{U"x:y", U"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{U"x:y", U"z"}});
        R_TEST(xmap {{U"x:y", U"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{U"x y", U"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{U"x y", U"z"}});
        R_TEST(xmap {{U"x y", U"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<map<string, string>, string>;
        R_TEST(xmap {{{{"x", "y"}}, "z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{"x", "y"}}, "z"}});
        R_TEST(xmap {{{{"", "y"}}, "z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{"", "y"}}, "z"}});
        R_TEST(xmap {{{{"a\"b", "c"}}, "d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{"a\"b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a:b", "c"}}, "d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{"a:b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a:b", "c"}}, "d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{"a b", "c"}}, "d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{"a b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a b", "c"}}, "d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<wstring, wstring>, wstring>;
        R_TEST(xmap {{{{L"x", L"y"}}, L"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{L"x", L"y"}}, L"z"}});
        R_TEST(xmap {{{{L"", L"y"}}, L"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{L"", L"y"}}, L"z"}});
        R_TEST(xmap {{{{L"a\"b", L"c"}}, L"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{L"a\"b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a:b", L"c"}}, L"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{L"a:b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a:b", L"c"}}, L"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{L"a b", L"c"}}, L"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{L"a b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a b", L"c"}}, L"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<u16string, u16string>, u16string>;
        R_TEST(xmap {{{{u"x", u"y"}}, u"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{u"x", u"y"}}, u"z"}});
        R_TEST(xmap {{{{u"", u"y"}}, u"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{u"", u"y"}}, u"z"}});
        R_TEST(xmap {{{{u"a\"b", u"c"}}, u"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{u"a\"b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a:b", u"c"}}, u"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{u"a:b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a:b", u"c"}}, u"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{u"a b", u"c"}}, u"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{u"a b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a b", u"c"}}, u"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<u32string, u32string>, u32string>;
        R_TEST(xmap {{{{U"x", U"y"}}, U"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{U"x", U"y"}}, U"z"}});
        R_TEST(xmap {{{{U"", U"y"}}, U"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{U"", U"y"}}, U"z"}});
        R_TEST(xmap {{{{U"a\"b", U"c"}}, U"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{U"a\"b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a:b", U"c"}}, U"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{U"a:b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a:b", U"c"}}, U"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{U"a b", U"c"}}, U"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{U"a b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a b", U"c"}}, U"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<vector<string>, string>;
        R_TEST(xmap {{{"x", "y"}, "z"}}, R"({["x","y"]:"z"})");
        W_TEST(R"({["x","y"]:"z"})", xmap {{{"x", "y"}, "z"}});
    }
    // char
    {   using xmap = map<char, char>;
        R_TEST(xmap {{'x', 'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{'x', 'y'}});
        R_TEST(xmap {{'"', 'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{'"', 'x'}});
        R_TEST(xmap {{':', 'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{':', 'x'}});
        //R_TEST(xmap {{':', 'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{' ', 'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{' ', 'x'}});
        R_TEST(xmap {{' ', 'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<wchar_t, wchar_t>;
        R_TEST(xmap {{L'x', L'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{L'x', L'y'}});
        R_TEST(xmap {{L'"', L'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{L'"', L'x'}});
        R_TEST(xmap {{L':', L'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{L':', L'x'}});
        //R_TEST(xmap {{L':', L'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{L' ', L'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{L' ', L'x'}});
        R_TEST(xmap {{L' ', L'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<char16_t, char16_t>;
        R_TEST(xmap {{u'x', u'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{u'x', u'y'}});
        R_TEST(xmap {{u'"', u'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{u'"', u'x'}});
        R_TEST(xmap {{u':', u'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{u':', u'x'}});
        //R_TEST(xmap {{u':', u'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{u' ', u'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{u' ', u'x'}});
        R_TEST(xmap {{u' ', u'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<char32_t, char32_t>;
        R_TEST(xmap {{U'x', U'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{U'x', U'y'}});
        R_TEST(xmap {{U'"', U'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{U'"', U'x'}});
        R_TEST(xmap {{U':', U'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{U':', U'x'}});
        //R_TEST(xmap {{U':', U'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{U' ', U'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{U' ', U'x'}});
        R_TEST(xmap {{U' ', U'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    // enum
    {   using xmap = map<enum_1, int>;
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c\ c:3,d\:d:4})");
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b\"b:2,c\ c:3,d\:d:4})");
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c c:3,d\:d:4})", json::read_error::unexpected, 13);
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c\ c:3,d:d:4})", json::read_error::unexpected, 20);
        W_TEST(R"({aaa:1,b"b:2,c\ c:3,d\:d:4})", xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}});
    }
    {   using xmap = map<map<enum_1, int>, int>;
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})");
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b\"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})");
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c c:5}:6,{d\:d:7}:8})", json::read_error::unexpected, 22);
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d:d:7}:8})", json::read_error::unexpected, 33);
        W_TEST(R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})", xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}});
    }
    // struct
    {   using xmap = map<struct_1, string>;
        R_TEST(xmap {{{"b"}, "c"}}, R"({{a:"b"}:"c"})");
        W_TEST(R"({{a:"b"}:"c"})", xmap {{{"b"}, "c"}});
    }
    {   using xmap = map<map<struct_1, string>, string>;
        R_TEST(xmap {{{{{"b"}, "c"}}, "d"}}, R"({{{a:"b"}:"c"}:"d"})");
        W_TEST(R"({{{a:"b"}:"c"}:"d"})", xmap {{{{{"b"}, "c"}}, "d"}});
    }
    // std::variant
#   ifdef CXON_HAS_LIB_STD_VARIANT
    {   using xmap = map<variant<string, int>, string>;
        R_TEST(xmap {{{"1"}, "2"}}, R"({{0:"1"}:"2"})");
        W_TEST(R"({{0:"1"}:"2"})", xmap {{{"1"}, "2"}});
    }
#   endif
    // std::bitset
    {   using xmap = map<bitset<8>, bitset<8>>;
        R_TEST(xmap {{bitset<8>(85), bitset<8>(170)}}, R"({01010101:"10101010"})");
        W_TEST(R"({01010101:"10101010"})", xmap {{bitset<8>(85), bitset<8>(170)}});
    }
TEST_END()

TEST_BEG(unquote_quoted_keys_input_iterator, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<cxon::test::input_iterator_traits<>>>, "/std")
    using namespace std;
    {   using xmap = map<string, string>;
        R_TEST(xmap {{"x", "y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{"x", "y"}});
        R_TEST(xmap {{"", "y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{"", "y"}});
        R_TEST(xmap {{"x\"y", "z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{"x\"y", "z"}});
        R_TEST(xmap {{"x:y", "z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{"x:y", "z"}});
        R_TEST(xmap {{"x:y", "z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{"x y", "z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{"x y", "z"}});
        R_TEST(xmap {{"x y", "z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {
        R_TEST(struct_2 {"b", "d", "f", "h"}, "{a\t:\"b\", c:\"d\", e:\"f\", g:\"h\"}");
        R_TEST(struct_2 {"b", "d", "f", "h"}, "{a\n:\"b\", c:\"d\", e:\"f\", g:\"h\"}");
        R_TEST(struct_2 {"b", "d", "f", "h"}, "{a\r:\"b\", c:\"d\", e:\"f\", g:\"h\"}");
        R_TEST(struct_2 {"b", "d", "f", "h"}, "{a :\"b\", c:\"d\", e:\"f\", g:\"h\"}");
        R_TEST(struct_2 {"b",  "",  "",  ""}, "{a\t:\"b\"}");
        R_TEST(struct_2 {"b",  "",  "",  ""}, "{a\n:\"b\"}");
        R_TEST(struct_2 {"b",  "",  "",  ""}, "{a\r:\"b\"}");
        R_TEST(struct_2 {"b",  "",  "",  ""}, "{a :\"b\"}");
    }
    {   using xmap = map<wstring, wstring>;
        R_TEST(xmap {{L"x", L"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{L"x", L"y"}});
        R_TEST(xmap {{L"", L"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{L"", L"y"}});
        R_TEST(xmap {{L"x\"y", L"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{L"x\"y", L"z"}});
        R_TEST(xmap {{L"x:y", L"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{L"x:y", L"z"}});
        R_TEST(xmap {{L"x:y", L"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{L"x y", L"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{L"x y", L"z"}});
        R_TEST(xmap {{L"x y", L"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<u16string, u16string>;
        R_TEST(xmap {{u"x", u"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{u"x", u"y"}});
        R_TEST(xmap {{u"", u"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{u"", u"y"}});
        R_TEST(xmap {{u"x\"y", u"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{u"x\"y", u"z"}});
        R_TEST(xmap {{u"x:y", u"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{u"x:y", u"z"}});
        R_TEST(xmap {{u"x:y", u"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{u"x y", u"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{u"x y", u"z"}});
        R_TEST(xmap {{u"x y", u"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<u32string, u32string>;
        R_TEST(xmap {{U"x", U"y"}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{U"x", U"y"}});
        R_TEST(xmap {{U"", U"y"}}, R"({:"y"})");
        W_TEST(R"({:"y"})", xmap {{U"", U"y"}});
        R_TEST(xmap {{U"x\"y", U"z"}}, R"({x"y:"z"})");
        W_TEST(R"({x"y:"z"})", xmap {{U"x\"y", U"z"}});
        R_TEST(xmap {{U"x:y", U"z"}}, R"({x\:y:"z"})");
        W_TEST(R"({x\:y:"z"})", xmap {{U"x:y", U"z"}});
        R_TEST(xmap {{U"x:y", U"z"}}, R"({x:y:"z"})", json::read_error::unexpected, 3);
        R_TEST(xmap {{U"x y", U"z"}}, R"({x\ y:"z"})");
        W_TEST(R"({x\ y:"z"})", xmap {{U"x y", U"z"}});
        R_TEST(xmap {{U"x y", U"z"}}, R"({x y:"z"})", json::read_error::unexpected, 3);
    }
    {   using xmap = map<map<string, string>, string>;
        R_TEST(xmap {{{{"x", "y"}}, "z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{"x", "y"}}, "z"}});
        R_TEST(xmap {{{{"", "y"}}, "z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{"", "y"}}, "z"}});
        R_TEST(xmap {{{{"a\"b", "c"}}, "d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{"a\"b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a:b", "c"}}, "d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{"a:b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a:b", "c"}}, "d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{"a b", "c"}}, "d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{"a b", "c"}}, "d"}});
        R_TEST(xmap {{{{"a b", "c"}}, "d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<wstring, wstring>, wstring>;
        R_TEST(xmap {{{{L"x", L"y"}}, L"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{L"x", L"y"}}, L"z"}});
        R_TEST(xmap {{{{L"", L"y"}}, L"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{L"", L"y"}}, L"z"}});
        R_TEST(xmap {{{{L"a\"b", L"c"}}, L"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{L"a\"b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a:b", L"c"}}, L"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{L"a:b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a:b", L"c"}}, L"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{L"a b", L"c"}}, L"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{L"a b", L"c"}}, L"d"}});
        R_TEST(xmap {{{{L"a b", L"c"}}, L"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<u16string, u16string>, u16string>;
        R_TEST(xmap {{{{u"x", u"y"}}, u"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{u"x", u"y"}}, u"z"}});
        R_TEST(xmap {{{{u"", u"y"}}, u"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{u"", u"y"}}, u"z"}});
        R_TEST(xmap {{{{u"a\"b", u"c"}}, u"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{u"a\"b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a:b", u"c"}}, u"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{u"a:b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a:b", u"c"}}, u"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{u"a b", u"c"}}, u"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{u"a b", u"c"}}, u"d"}});
        R_TEST(xmap {{{{u"a b", u"c"}}, u"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<map<u32string, u32string>, u32string>;
        R_TEST(xmap {{{{U"x", U"y"}}, U"z"}}, R"({{x:"y"}:"z"})");
        W_TEST(R"({{x:"y"}:"z"})", xmap {{{{U"x", U"y"}}, U"z"}});
        R_TEST(xmap {{{{U"", U"y"}}, U"z"}}, R"({{:"y"}:"z"})");
        W_TEST(R"({{:"y"}:"z"})", xmap {{{{U"", U"y"}}, U"z"}});
        R_TEST(xmap {{{{U"a\"b", U"c"}}, U"d"}}, R"({{a"b:"c"}:"d"})");
        W_TEST(R"({{a"b:"c"}:"d"})", xmap {{{{U"a\"b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a:b", U"c"}}, U"d"}}, R"({{a\:b:"c"}:"d"})");
        W_TEST(R"({{a\:b:"c"}:"d"})", xmap {{{{U"a:b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a:b", U"c"}}, U"d"}}, R"({{a:b:"c"}:"d"})", json::read_error::unexpected, 4);
        R_TEST(xmap {{{{U"a b", U"c"}}, U"d"}}, R"({{a\ b:"c"}:"d"})");
        W_TEST(R"({{a\ b:"c"}:"d"})", xmap {{{{U"a b", U"c"}}, U"d"}});
        R_TEST(xmap {{{{U"a b", U"c"}}, U"d"}}, R"({{a b:"c"}:"d"})", json::read_error::unexpected, 4);
    }
    {   using xmap = map<vector<string>, string>;
        R_TEST(xmap {{{"x", "y"}, "z"}}, R"({["x","y"]:"z"})");
        W_TEST(R"({["x","y"]:"z"})", xmap {{{"x", "y"}, "z"}});
    }
    // char
    {   using xmap = map<char, char>;
        R_TEST(xmap {{'x', 'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{'x', 'y'}});
        R_TEST(xmap {{'"', 'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{'"', 'x'}});
        R_TEST(xmap {{':', 'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{':', 'x'}});
        //R_TEST(xmap {{':', 'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{' ', 'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{' ', 'x'}});
        R_TEST(xmap {{' ', 'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<wchar_t, wchar_t>;
        R_TEST(xmap {{L'x', L'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{L'x', L'y'}});
        R_TEST(xmap {{L'"', L'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{L'"', L'x'}});
        R_TEST(xmap {{L':', L'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{L':', L'x'}});
        //R_TEST(xmap {{L':', L'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{L' ', L'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{L' ', L'x'}});
        R_TEST(xmap {{L' ', L'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<char16_t, char16_t>;
        R_TEST(xmap {{u'x', u'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{u'x', u'y'}});
        R_TEST(xmap {{u'"', u'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{u'"', u'x'}});
        R_TEST(xmap {{u':', u'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{u':', u'x'}});
        //R_TEST(xmap {{u':', u'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{u' ', u'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{u' ', u'x'}});
        R_TEST(xmap {{u' ', u'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    {   using xmap = map<char32_t, char32_t>;
        R_TEST(xmap {{U'x', U'y'}}, R"({x:"y"})");
        W_TEST(R"({x:"y"})", xmap {{U'x', U'y'}});
        R_TEST(xmap {{U'"', U'x'}}, R"({":"x"})");
        W_TEST(R"({":"x"})", xmap {{U'"', U'x'}});
        R_TEST(xmap {{U':', U'x'}}, R"({\::"x"})");
        W_TEST(R"({\::"x"})", xmap {{U':', U'x'}});
        //R_TEST(xmap {{U':', U'x'}}, R"({::"x"})", json::read_error::unexpected, 2); // okay for char, fixed width
        R_TEST(xmap {{U' ', U'x'}}, R"({\ :"x"})");
        W_TEST(R"({\ :"x"})", xmap {{U' ', U'x'}});
        R_TEST(xmap {{U' ', U'x'}}, R"({ :"x"})", json::read_error::unexpected, 3); // ':' is the key
    }
    // enum
    {   using xmap = map<enum_1, int>;
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c\ c:3,d\:d:4})");
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b\"b:2,c\ c:3,d\:d:4})");
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c c:3,d\:d:4})", json::read_error::unexpected, 14);
        R_TEST(xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}}, R"({aaa:1,b"b:2,c\ c:3,d:d:4})", json::read_error::unexpected, 21);
        W_TEST(R"({aaa:1,b"b:2,c\ c:3,d\:d:4})", xmap {{enum_1::aaa, 1}, {enum_1::bbb, 2}, {enum_1::ccc, 3}, {enum_1::ddd, 4}});
    }
    {   using xmap = map<map<enum_1, int>, int>;
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})");
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b\"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})");
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c c:5}:6,{d\:d:7}:8})", json::read_error::unexpected, 23);
        R_TEST(xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}}, R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d:d:7}:8})", json::read_error::unexpected, 34);
        W_TEST(R"({{aaa:1}:2,{b"b:3}:4,{c\ c:5}:6,{d\:d:7}:8})", xmap {{{{enum_1::aaa, 1}}, 2}, {{{enum_1::bbb, 3}}, 4}, {{{enum_1::ccc, 5}}, 6}, {{{enum_1::ddd, 7}}, 8}});
    }
    // struct
    {   using xmap = map<struct_1, string>;
        R_TEST(xmap {{{"b"}, "c"}}, R"({{a:"b"}:"c"})");
        W_TEST(R"({{a:"b"}:"c"})", xmap {{{"b"}, "c"}});
    }
    {   using xmap = map<map<struct_1, string>, string>;
        R_TEST(xmap {{{{{"b"}, "c"}}, "d"}}, R"({{{a:"b"}:"c"}:"d"})");
        W_TEST(R"({{{a:"b"}:"c"}:"d"})", xmap {{{{{"b"}, "c"}}, "d"}});
    }
    // std::variant
#   ifdef CXON_HAS_LIB_STD_VARIANT
    {   using xmap = map<variant<string, int>, string>;
        R_TEST(xmap {{{"1"}, "2"}}, R"({{0:"1"}:"2"})");
        W_TEST(R"({{0:"1"}:"2"})", xmap {{{"1"}, "2"}});
    }
#   endif
    // std::bitset
    {   using xmap = map<bitset<8>, bitset<8>>;
        R_TEST(xmap {{bitset<8>(85), bitset<8>(170)}}, R"({01010101:"10101010"})");
        W_TEST(R"({01010101:"10101010"})", xmap {{bitset<8>(85), bitset<8>(170)}});
    }
TEST_END()


namespace {
    struct map_div_traits : cxon::json::format_traits {
        struct map : cxon::json::format_traits::map {
            static constexpr char div = '=';
        };
    };
}
TEST_BEG(map_div, cxon::JSON<map_div_traits>, "/std")
    using namespace std;
    {   using xmap = map<string, int>;
        R_TEST(xmap {{"x", 1}}, R"({"x"=1})");
        W_TEST(R"({"x"=1})", xmap {{"x", 1}});
        R_TEST(xmap {{"x:y", 1}}, R"({"x:y"=1})");
        W_TEST(R"({"x:y"=1})", xmap {{"x:y", 1}});
        R_TEST(xmap {{"x=y", 1}}, R"({"x=y"=1})");
        W_TEST(R"({"x=y"=1})", xmap {{"x=y", 1}});
        R_TEST(xmap {}, R"({"x":1})", cxon::json::read_error::unexpected, 4);
    }
TEST_END()

TEST_BEG(map_div_unquoted_quoted_keys, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<map_div_traits>>, "/std")
    using namespace std;
    {   using xmap = map<string, int>;
        R_TEST(xmap {{"x", 1}}, R"({x=1})");
        W_TEST(R"({x=1})", xmap {{"x", 1}});
        R_TEST(xmap {{"x:y", 1}}, R"({x:y=1})");
        W_TEST(R"({x:y=1})", xmap {{"x:y", 1}});
        R_TEST(xmap {{"x=y", 1}}, R"({x\=y=1})");
        W_TEST(R"({x\=y=1})", xmap {{"x=y", 1}});
        R_TEST(xmap {}, R"({x:1})", cxon::json::read_error::unexpected, 5);
        R_TEST(xmap {}, R"({x\:1})", cxon::json::read_error::escape_invalid, 2);
    }
TEST_END()

TEST_BEG(map_div_unquoted_quoted_keys_cov, cxon::JSON<cxon::test::unquoted_quoted_keys_traits<>>, "/std")
    using namespace std;
    {   using xmap = map<string, string>;
        R_TEST(xmap {{"x=y", "z"}}, R"({x\=y:"z"})", json::read_error::escape_invalid, 2);
        W_TEST(R"({x=y:"z"})", xmap {{"x=y", "z"}});
    }
TEST_END()


namespace {
    struct lima_sep_traits : cxon::json::format_traits {
        struct map : cxon::json::format_traits::map {
            static constexpr char sep = ';';
        };
        struct list : cxon::json::format_traits::list {
            static constexpr char sep = ';';
        };
    };
}
TEST_BEG(lima_sep, cxon::JSON<lima_sep_traits>, "/std")
    using namespace std;
    {   using xmap = map<string, string>;
        R_TEST(xmap {{"a", "b"}, {"c", "d"}}, R"({"a":"b";"c":"d"})");
        W_TEST(R"({"a":"b";"c":"d"})", xmap {{"a", "b"}, {"c", "d"}});
    }
    {   using xvec = vector<string>;
        R_TEST(xvec {"a", "b"}, R"(["a";"b"])");
        W_TEST(R"(["a";"b"])", xvec {"a", "b"});
    }
TEST_END()


namespace {
    struct lima_trailing_sep_traits : lima_sep_traits {
        static constexpr bool allow_trailing_separators = true;
    };
}
TEST_BEG(lima_trailing_sep, cxon::JSON<lima_trailing_sep_traits>, "/std")
    using namespace std;
    {   using xmap = map<string, string>;
        R_TEST(xmap {{"a", "b"}, {"c", "d"}}, R"({"a":"b";"c":"d";})");
        W_TEST(R"({"a":"b";"c":"d"})", xmap {{"a", "b"}, {"c", "d"}});
    }
    {   using xvec = vector<string>;
        R_TEST(xvec {"a", "b"}, R"(["a";"b";])");
        W_TEST(R"(["a";"b"])", xvec {"a", "b"});
    }
TEST_END()


namespace {
    struct lima_ws_sep_traits : cxon::json::format_traits {
        struct map : cxon::json::format_traits::map {
            static constexpr char sep = ' ';
        };
        struct list : cxon::json::format_traits::list {
            static constexpr char sep = ' ';
        };
    };
}
TEST_BEG(lima_ws_sep, cxon::JSON<lima_ws_sep_traits>, "/std")
    using namespace std;
    {   using xmap = map<string, string>;
        R_TEST(xmap {{"a", "b"}, {"c", "d"}}, R"({"a":"b" "c":"d"})");
        W_TEST(R"({"a":"b" "c":"d"})", xmap {{"a", "b"}, {"c", "d"}});
    }
    {   using xvec = vector<string>;
        R_TEST(xvec {"a", "b"}, R"(["a" "b"])");
        W_TEST(R"(["a" "b"])", xvec {"a", "b"});
    }
    {   using xvec = vector<map<string, string>>;
        R_TEST(xvec {{{"a", "b"}}, {{"c", "d"}}}, R"([{"a": "b"}, {"c": "d"}])", cxon::json::read_error::unexpected, 11);
    }
TEST_END()


static void memory_deleter(int *p) noexcept {
    delete p;
}

TEST_BEG(memory, cxon::JSON<>, "/std")
    using namespace std;
    // std::unique_ptr<T, D>
        R_TEST(unique_ptr<string>(), "null");
        W_TEST("null", unique_ptr<string>());
        R_TEST(unique_ptr<string>(new string("xXx")), R"("xXx")");
        W_TEST(R"("xXx")", unique_ptr<string>(new string("xXx")));
        R_TEST(unique_ptr<string>(), "nil", json::read_error::unexpected, 0);
        R_TEST(unique_ptr<string>(), "42", json::read_error::unexpected, 0);
        {   auto up = unique_ptr<int, void (*)(int*)>(nullptr, memory_deleter);
                auto fbr = cxon::from_bytes<X>(up, "42");
            TEST_CHECK(fbr && up && *up == 42 && up.get_deleter() == memory_deleter);
        }
    // std::shared_ptr<T>
        R_TEST(shared_ptr<string>(), "null");
        W_TEST("null", shared_ptr<string>());
        R_TEST(shared_ptr<string>(new string("xXx")), R"("xXx")");
        W_TEST(R"("xXx")", shared_ptr<string>(new string("xXx")));
        R_TEST(shared_ptr<string>(), "nil", json::read_error::unexpected, 0);
        R_TEST(shared_ptr<string>(), "42", json::read_error::unexpected, 0);
        {   auto sp = shared_ptr<int>(nullptr, memory_deleter);
                auto fbr = cxon::from_bytes<X>(sp, "42");
            TEST_CHECK(fbr && sp && *sp == 42);
        }
TEST_END()

TEST_BEG(memory_allow_cxx_comments, cxon::JSON<cxon::test::allow_cxx_comments_traits<>>, "/std")
    using namespace std;
    // std::unique_ptr<T, D>
        R_TEST(unique_ptr<string>(), " / null", json::read_error::unexpected, 2);
    // std::shared_ptr<T>
        R_TEST(shared_ptr<string>(), " / null", json::read_error::unexpected, 2);
TEST_END()
