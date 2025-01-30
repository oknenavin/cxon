// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "test.hxx"

#include <limits>
#include <string>
#include <cstdio>

////////////////////////////////////////////////////////////////////////////////

namespace cxon { namespace test {

    template <typename T>
        inline auto to_string(T t)
            -> enable_if_t<std::is_integral<T>::value, std::string>
        {
            return std::to_string(t);
        }

    template <typename> struct fmt;
    template <> struct fmt<float>       { static constexpr char const*const str = "%.*g";   };
    template <> struct fmt<double>      { static constexpr char const*const str = "%.*g";   };
    template <> struct fmt<long double> { static constexpr char const*const str = "%.*Lg";  };

    template <typename T>
        inline auto to_string(T t)
            -> enable_if_t<std::is_floating_point<T>::value, std::string>
        {
            char s[std::numeric_limits<T>::max_digits10 * 2] = { 0 };
            std::size_t const l = sizeof(s) / sizeof(char);
            int const w = std::snprintf(s, l, fmt<T>::str, std::numeric_limits<T>::max_digits10, t);
                CXON_ASSERT(w > 0 && (std::size_t)w < l, "conversion failed");
            return s;
        }

    template <typename T>   constexpr T tmin() { return std::numeric_limits<T>::lowest(); }
    template <typename T>   inline std::string smin() { return to_string(tmin<T>()); }
    template <typename T>   inline std::string omin() { return smin<T>() + '0'; }
    template <typename T>   constexpr T tmax() { return std::numeric_limits<T>::max(); }
    template <typename T>   inline std::string smax() { return to_string(tmax<T>()); }
    template <typename T>   inline std::string omax() { return smax<T>() + '0'; }

}}

TEST_BEG(fundamental, cxon::JSON<>, "/core")
    using namespace test;
    // bool
        R_TEST(true, "true");
        W_TEST("true", true);
        R_TEST(false, "false");
        W_TEST("false", false);
        R_TEST(false, "t", json::read_error::boolean_invalid, 0);
        R_TEST(false, "f", json::read_error::boolean_invalid, 0);
        R_TEST(false, "", json::read_error::boolean_invalid, 0);
    // char
        R_TEST('\0', QS("\\u0000"));
        W_TEST(QS("\\u0000"), '\0');
        R_TEST('\x1f', QS("\\u001f"));
        W_TEST(QS("\\u001f"), '\x1f');
        R_TEST('"', QS("\\\""));
        W_TEST(QS("\\\""), '"');
        R_TEST('\'', QS("'"));
        W_TEST(QS("'"), '\'');
        R_TEST('\x7f', QS("\\u007f"));
        W_TEST(QS("\x7f"), '\x7f');
        R_TEST('\x8f', QS("\\u008f"));
        W_TEST(QS("\x8f"), '\x8f');
        R_TEST('\xff', QS("\\u00ff")); // invalid utf-8
        W_TEST(QS("\xff"), '\xff'); // invalid utf-8
        R_TEST('\0', QS("\xff"), json::read_error::character_invalid, 1); // invalid utf-8
        R_TEST('\0', QS("\\z"), json::read_error::escape_invalid, 1);
        R_TEST('\0', QS("\\u1111"), json::read_error::character_invalid, 1);
        // errors
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<XXON>(c, 'c');
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, 'c');
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[2];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, 'c');
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
    // wchar_t
        R_TEST(L'a', QS("a"));
        W_TEST(QS("a"), L'a');
    // char8_t
#       if defined(__cpp_char8_t)
            R_TEST(u8'\0', QS("\\u0000"));
            W_TEST(QS("\\u0000"), u8'\0');
            R_TEST(u8'\x1f', QS("\\u001f"));
            W_TEST(QS("\\u001f"), u8'\x1f');
            R_TEST(u8'"', QS("\\\""));
            W_TEST(QS("\\\""), u8'"');
            R_TEST(u8'\'', QS("'"));
            W_TEST(QS("'"), u8'\'');
            R_TEST(u8'\x7f', QS("\\u007f"));
            W_TEST(QS("\x7f"), u8'\x7f');
            R_TEST(u8'\0', QS("\xff"), json::read_error::character_invalid, 1); // invalid utf-8
            R_TEST(u8'\0', QS("\\z"), json::read_error::escape_invalid, 1);
            R_TEST(u8'\0', QS("\\u1111"), json::read_error::character_invalid, 1);
#       endif
    // char16_t
        R_TEST(u'a', QS("a"));
        W_TEST(QS("a"), u'a');
        R_TEST(u'\x3B1', QS("\xCE\xB1"));
        W_TEST(QS("\xCE\xB1"), u'\x3B1');
        R_TEST(u'\xFFFF', QS("\xEF\xBF\xBF"));
        W_TEST(QS("\xEF\xBF\xBF"), u'\xFFFF');
        R_TEST(u'\0', QS("\xFF"), json::read_error::character_invalid, 1);
        R_TEST(u'\0', QS("\xF0\x90\x80\x80"), json::read_error::character_invalid, 1);
    // char32_t
        R_TEST(U'a', QS("a"));
        R_TEST(U'\0', QS("\x80"), json::read_error::character_invalid, 1);
        W_TEST(QS("a"), U'a');
        R_TEST(U'\x3B1', QS("\xCE\xB1"));
        R_TEST(U'\0', QS("\xCE"), json::read_error::character_invalid, 1);
        W_TEST(QS("\xCE\xB1"), U'\x3B1');
        R_TEST(U'\x389A', QS("\xE3\xA2\x9A"));
        R_TEST(U'\0', QS("\xE3\xA2"), json::read_error::character_invalid, 1);
        R_TEST(U'\0', QS("\xE3"), json::read_error::character_invalid, 1);
        W_TEST(QS("\xE3\xA2\x9A"), U'\x389A');
        R_TEST(U'\x28440', QS("\xF0\xA8\x91\x80"));
        {   char u[4];
            TEST_CHECK(cio::chr::utf32_to_utf8(u, 0xDBFF) == 0);
        }
        R_TEST(U'\0', QS("\\udbff"), json::read_error::surrogate_invalid, 1); // invalid surrogate
        R_TEST(U'\0', QS("\\udbff\\ue000"), json::read_error::surrogate_invalid, 1); // invalid surrogate
        R_TEST(U'\0', QS("\\udbff\\udbff"), json::read_error::surrogate_invalid, 1); // invalid surrogate
        R_TEST(U'\0', QS("\xF0\xA8\x91"), json::read_error::character_invalid, 1);
        R_TEST(U'\0', QS("\xF0\xA8"), json::read_error::character_invalid, 1);
        R_TEST(U'\0', QS("\xF0"), json::read_error::character_invalid, 1);
        W_TEST(QS("\xF0\xA8\x91\x80"), U'\x28440');
        W_TEST(QS(""), U'\x200000');
    // signed char
        R_TEST(tmin<signed char>(), smin<signed char>());
        W_TEST(smin<signed char>(), tmin<signed char>());
        R_TEST((signed char)0, "0");
        W_TEST("0", (signed char)0);
        R_TEST(tmax<signed char>(), smax<signed char>());
        W_TEST(smax<signed char>(), tmax<signed char>());
        R_TEST(tmin<signed char>(), omin<signed char>(), json::read_error::integral_invalid, 0);
        R_TEST(tmax<signed char>(), omax<signed char>(), json::read_error::integral_invalid, 0);
    // unsigned char
        R_TEST(tmin<unsigned char>(), smin<unsigned char>());
        W_TEST(smin<unsigned char>(), tmin<unsigned char>());
        R_TEST((unsigned char)0x7F, "127");
        W_TEST("127", (unsigned char)0x7F);
        R_TEST(tmax<unsigned char>(), smax<unsigned char>());
        W_TEST(smax<unsigned char>(), tmax<unsigned char>());
        R_TEST(tmax<unsigned char>(), omax<unsigned char>(), json::read_error::integral_invalid, 0);
        R_TEST(tmax<unsigned char>(), omax<unsigned char>(), json::read_error::integral_invalid, 0);
    // short
        R_TEST(tmin<short>(), smin<short>());
        W_TEST(smin<short>(), tmin<short>());
        R_TEST((short)0, "0");
        W_TEST("0", (short)0);
        R_TEST(tmax<short>(), smax<short>());
        W_TEST(smax<short>(), tmax<short>());
        R_TEST(tmin<short>(), omin<short>(), json::read_error::integral_invalid, 0);
        R_TEST(tmax<short>(), omax<short>(), json::read_error::integral_invalid, 0);
    // unsigned short
        R_TEST(tmin<unsigned short>(), smin<unsigned short>());
        W_TEST(smin<unsigned short>(), tmin<unsigned short>());
        R_TEST((unsigned short)0xFF, "255");
        W_TEST("255", (unsigned short)0xFF);
        R_TEST(tmax<unsigned short>(), smax<unsigned short>());
        W_TEST(smax<unsigned short>(), tmax<unsigned short>());
        R_TEST(tmax<unsigned short>(), omax<unsigned short>(), json::read_error::integral_invalid, 0);
        R_TEST(tmax<unsigned short>(), omax<unsigned short>(), json::read_error::integral_invalid, 0);
    // int
        R_TEST(tmin<int>(), smin<int>());
        W_TEST(smin<int>(), tmin<int>());
        R_TEST((int)0, "0");
        W_TEST("0", (int)0);
        R_TEST(tmax<int>(), smax<int>());
        W_TEST(smax<int>(), tmax<int>());
        R_TEST(tmin<int>(), omin<int>(), json::read_error::integral_invalid, 0);
        R_TEST(tmax<int>(), omax<int>(), json::read_error::integral_invalid, 0);
    // unsigned int
        R_TEST(tmin<unsigned int>(), smin<unsigned int>());
        W_TEST(smin<unsigned int>(), tmin<unsigned int>());
        R_TEST((unsigned int)0xFFFF, "65535");
        W_TEST("65535", (unsigned int)0xFFFF);
        R_TEST(tmax<unsigned int>(), smax<unsigned int>());
        W_TEST(smax<unsigned int>(), tmax<unsigned int>());
        R_TEST(tmax<unsigned int>(), omax<unsigned int>(), json::read_error::integral_invalid, 0);
    // long
        R_TEST(tmin<long>(), smin<long>());
        W_TEST(smin<long>(), tmin<long>());
        R_TEST((long)0, "0");
        W_TEST("0", (long)0);
        R_TEST(tmax<long>(), smax<long>());
        W_TEST(smax<long>(), tmax<long>());
        R_TEST(tmin<long>(), omin<long>(), json::read_error::integral_invalid, 0);
        R_TEST(tmax<long>(), omax<long>(), json::read_error::integral_invalid, 0);
    // unsigned long
        R_TEST(tmin<unsigned long>(), smin<unsigned long>());
        W_TEST(smin<unsigned long>(), tmin<unsigned long>());
        R_TEST((unsigned long)0xFFFF, "65535");
        W_TEST("65535", (unsigned long)0xFFFF);
        R_TEST(tmax<unsigned long>(), smax<unsigned long>());
        W_TEST(smax<unsigned long>(), tmax<unsigned long>());
        R_TEST(tmax<unsigned long>(), omax<unsigned long>(), json::read_error::integral_invalid, 0);
    // long long
        R_TEST(tmin<long long>(), smin<long long>());
        W_TEST(smin<long long>(), tmin<long long>());
        R_TEST((long long)0, "0");
        W_TEST("0", (long long)0);
        R_TEST(tmax<long long>(), smax<long long>());
        W_TEST(smax<long long>(), tmax<long long>());
        R_TEST(tmin<long long>(), omin<long long>(), json::read_error::integral_invalid, 0);
        R_TEST(tmax<long long>(), omax<long long>(), json::read_error::integral_invalid, 0);
    // unsigned long long
        R_TEST(tmin<unsigned long long>(), smin<unsigned long long>());
        W_TEST(smin<unsigned long long>(), tmin<unsigned long long>());
        R_TEST((unsigned long long)0xFFFFFFFF, "4294967295");
        W_TEST("4294967295", (unsigned long long)0xFFFFFFFF);
        R_TEST(tmax<unsigned long long>(), smax<unsigned long long>());
        W_TEST(smax<unsigned long long>(), tmax<unsigned long long>());
        R_TEST(tmax<unsigned long long>(), omax<unsigned long long>(), json::read_error::integral_invalid, 0);
    // float
        R_TEST(tmin<float>(), smin<float>());
        W_TEST(smin<float>(), tmin<float>());
        R_TEST((float)0, "0");
        W_TEST("0", (float)0);
        R_TEST(tmax<float>(), smax<float>());
        W_TEST(smax<float>(), tmax<float>());
    // double
        R_TEST(tmin<double>(), smin<double>());
        W_TEST(smin<double>(), tmin<double>());
        R_TEST((double)0, "0");
        W_TEST("0", (double)0);
        R_TEST(tmax<double>(), smax<double>());
        W_TEST(smax<double>(), tmax<double>());
    // long double
        //R_TEST(tmin<long double>(), smin<long double>());
        W_TEST(smin<long double>(), tmin<long double>());
        R_TEST((long double)0, "0");
        W_TEST("0", (long double)0);
        //R_TEST(tmax<long double>(), smax<long double>());
        W_TEST(smax<long double>(), tmax<long double>());
    // nullptr_t
        R_TEST(nullptr, "null");
        W_TEST("null", nullptr);
        R_TEST(nullptr, " null");
        R_TEST(nullptr, "", json::read_error::unexpected, 0);
        R_TEST(nullptr, "nu", json::read_error::unexpected, 0);
        R_TEST(nullptr, "a", json::read_error::unexpected, 0);
    // errors
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, 3.14);
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                c.push_back('x');
                auto r = cxon::to_bytes<XXON>(c, std::numeric_limits<double>::infinity());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, std::numeric_limits<double>::infinity());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[1];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, std::numeric_limits<double>::quiet_NaN());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
        {   char b[4];
            auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
                auto r = cxon::to_bytes<XXON>(c, std::numeric_limits<double>::quiet_NaN());
            TEST_CHECK(r.ec == json::write_error::output_failure);
        }
TEST_END()


namespace cxon { namespace test {

    template <typename F> constexpr F inf() { return std::numeric_limits<F>::infinity(); }
    template <typename F> constexpr F nan() { return std::numeric_limits<F>::quiet_NaN(); }

    template <typename F, typename U> inline F bfp(U u) {
        static_assert(sizeof(F) == sizeof(U), "width different");
        union { F f; U u = 0; } a; a.u = u;
        return a.f;
    }

    template <typename F> static F zero() { return 0.0; }

}}

TEST_BEG(special_numbers, cxon::JSON<>, "/core") // special numbers
    using namespace test;
    // float
        R_TEST( -inf<float>(), omin<float>(), json::read_error::floating_point_invalid, 0);
        R_TEST( -inf<float>(), QS("-inf"));
        W_TEST(QS("-inf"), -inf<float>());
        R_TEST(  inf<float>(), omax<float>(), json::read_error::floating_point_invalid, 0);
        R_TEST(  inf<float>(), QS("inf"));
        R_TEST(  inf<float>(), QS("+inf"), json::read_error::floating_point_invalid, 0);
        W_TEST(QS("inf"),  inf<float>());
        R_TEST(  nan<float>(), QS("nan"));
        R_TEST(  nan<float>(), QS("+nan"), json::read_error::floating_point_invalid, 0); 
        R_TEST( -nan<float>(), QS("-nan")/*, json::read_error::floating_point_invalid, 0*/); // TODO: disallow?
        W_TEST(QS("nan"), nan<float>());
        W_TEST(QS("nan"), bfp<float>(0x7fffffff));
        W_TEST(QS("nan"), bfp<float>(0x7f80ffff));
        W_TEST(QS("-nan"), bfp<float>(0xffc00000));
        //W_TEST(QS("nan"), 0.0 /  zero<float>());
        //W_TEST(QS("nan"), 0.0 / -zero<float>());
        //W_TEST(QS("nan"), 0.0 *  inf<float>());
        //W_TEST(QS("nan"), 0.0 * -inf<float>());
        //W_TEST(QS("nan"), inf<float>() + -inf<float>());
        // errors
        R_TEST(0.f, "\"i", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"in", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"inf", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"xxxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"ixxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"inxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"infx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-i", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-in", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-inf", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-xxxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-ixxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-inxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-infx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"n", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"na", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"nan", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"xxxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"nxxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"naxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"nanx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-n", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-na", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-nan", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-xxxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-nxxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-naxx", json::read_error::floating_point_invalid, 0);
        R_TEST(0.f, "\"-nanx", json::read_error::floating_point_invalid, 0);
    // double
        R_TEST( -inf<double>(), omin<double>(), json::read_error::floating_point_invalid, 0);
        R_TEST( -inf<double>(), QS("-inf"));
        W_TEST(QS("-inf"), -inf<double>());
        R_TEST(  inf<double>(), omax<double>(), json::read_error::floating_point_invalid, 0);
        R_TEST(  inf<double>(), QS("inf"));
        R_TEST(  inf<double>(), QS("+inf"), json::read_error::floating_point_invalid, 0);
        W_TEST(QS("inf"),  inf<double>());
        R_TEST(  nan<double>(), QS("nan"));
        R_TEST(  nan<double>(), QS("+nan"), json::read_error::floating_point_invalid, 0); 
        R_TEST( -nan<double>(), QS("-nan")/*, json::read_error::floating_point_invalid, 0*/); // TODO: disallow?
        W_TEST(QS("nan"), nan<double>());
        W_TEST(QS("nan"), bfp<double>(0x7fffffffffffffff));
        W_TEST(QS("nan"), bfp<double>(0x7ff0ffffffffffff));
        W_TEST(QS("-nan"), bfp<double>(0xfff8000000000000));
        //W_TEST(QS("nan"), 0.0 /  zero<double>());
        //W_TEST(QS("nan"), 0.0 / -zero<double>());
        //W_TEST(QS("nan"), 0.0 *  inf<double>());
        //W_TEST(QS("nan"), 0.0 * -inf<double>());
        //W_TEST(QS("nan"), inf<double>() + -inf<double>());
    // long double
        R_TEST( -inf<long double>(), omin<long double>(), json::read_error::floating_point_invalid, 0);
        R_TEST( -inf<long double>(), QS("-inf"));
        W_TEST(QS("-inf"), -inf<long double>());
        R_TEST(  inf<long double>(), omax<long double>(), json::read_error::floating_point_invalid, 0);
        R_TEST(  inf<long double>(), QS("inf"));
        R_TEST(  inf<long double>(), QS("+inf"), json::read_error::floating_point_invalid, 0);
        W_TEST(QS("inf"),  inf<long double>());
        R_TEST(  nan<long double>(), QS("nan"));
        R_TEST(  nan<long double>(), QS("+nan"), json::read_error::floating_point_invalid, 0); 
        R_TEST( -nan<long double>(), QS("-nan")/*, json::read_error::floating_point_invalid, 0*/); // TODO: disallow?
        W_TEST(QS("nan"), nan<long double>());
        //W_TEST( QS("nan"), bfp<long double>(0x));
        //W_TEST( QS("nan"), bfp<long double>(0x));
        //W_TEST( QS("nan"), bfp<long double>(0x));
        //W_TEST(QS("nan"), 0.0 /  zero<long double>());
        //W_TEST(QS("nan"), 0.0 / -zero<long double>());
        //W_TEST(QS("nan"), 0.0 *  inf<long double>());
        //W_TEST(QS("nan"), 0.0 * -inf<long double>());
        //W_TEST(QS("nan"), inf<long double>() + -inf<long double>());
TEST_END()

TEST_BEG(special_numbers_input_iterator, cxon::JSON<cxon::test::input_iterator_traits<>>, "/core") // special numbers
    using namespace test;
    R_TEST(0.0, "0");
    R_TEST(0.0, "0.0");
    W_TEST("0", 0.0);
    R_TEST(10.0, "10");
    R_TEST(10.0, "10.0");
    R_TEST(10.0e+10, "10.0e+10");
    R_TEST(10.0e-10, "10.0e-10");
    R_TEST(10.0e+10, "10.0E+10");
    R_TEST(10.0e-10, "10.0E-10");
    R_TEST(-inf<double>(), QS("-inf"));
    W_TEST(QS("-inf"), -inf<double>());
    R_TEST(inf<double>(), QS("inf"));
    W_TEST(QS("inf"), inf<double>());
    R_TEST(inf<double>(), QS("+inf"), json::read_error::floating_point_invalid, 1);
    R_TEST(nan<double>(), QS("nan"));
    W_TEST(QS("nan"), nan<double>());
    R_TEST(-nan<double>(), QS("-nan"));
    W_TEST(QS("-nan"), -nan<double>());
TEST_END()


TEST_BEG(special_numbers_javascript_nans, cxon::JSON<cxon::test::allow_javascript_nans_traits<>>, "/core") // special numbers
    using namespace test;
    // float
        R_TEST(-inf<float>(), "-Infinity");
        W_TEST("-Infinity", -inf<float>());
        R_TEST( inf<float>(), " Infinity");
        W_TEST("Infinity", inf<float>());
        R_TEST( nan<float>(), " NaN");
        W_TEST("NaN", nan<float>());
        R_TEST(-nan<float>(), "-NaN");
        W_TEST("-NaN", -nan<float>());
        R_TEST( inf<float>(), "+Infinity", json::read_error::floating_point_invalid, 0);
        R_TEST( nan<float>(), "+NaN", json::read_error::floating_point_invalid, 0);
    // double
        R_TEST(-inf<double>(), "-Infinity");
        W_TEST("-Infinity", -inf<double>());
        R_TEST( inf<double>(), " Infinity");
        W_TEST("Infinity", inf<double>());
        R_TEST( nan<double>(), " NaN");
        W_TEST("NaN", nan<double>());
        R_TEST(-nan<double>(), "-NaN");
        W_TEST("-NaN", -nan<double>());
        R_TEST( inf<double>(), "+Infinity", json::read_error::floating_point_invalid, 0);
        R_TEST( nan<double>(), "+NaN", json::read_error::floating_point_invalid, 0);
    // long double
        R_TEST(-inf<long double>(), "-Infinity");
        W_TEST("-Infinity", -inf<long double>());
        R_TEST( inf<long double>(), " Infinity");
        W_TEST("Infinity", inf<long double>());
        R_TEST( nan<long double>(), " NaN");
        W_TEST("NaN", nan<long double>());
        R_TEST(-nan<long double>(), "-NaN");
        W_TEST("-NaN", -nan<long double>());
        R_TEST( inf<long double>(), "+Infinity", json::read_error::floating_point_invalid, 0);
        R_TEST( nan<long double>(), "+NaN", json::read_error::floating_point_invalid, 0);
    // errors
        R_TEST( inf<double>(), "Ix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Inx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Infx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Infix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Infinx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Infinix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Infinitx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Infinitx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Infinix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Infinx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Infix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Infx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Inx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Ix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Nx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Nax", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Nax", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Nx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-xxx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "xxx", json::read_error::floating_point_invalid, 0);
TEST_END()


TEST_BEG(special_numbers_javascript_nans_input_iterator, cxon::JSON<cxon::test::allow_javascript_nans_traits<cxon::test::input_iterator_traits<>>>, "/core") // special numbers
    using namespace test;
    // float
        R_TEST(-inf<float>(), "-Infinity");
        W_TEST("-Infinity", -inf<float>());
        R_TEST( inf<float>(), " Infinity");
        W_TEST("Infinity", inf<float>());
        R_TEST( nan<float>(), " NaN");
        W_TEST("NaN", nan<float>());
        R_TEST(-nan<float>(), "-NaN");
        W_TEST("-NaN", -nan<float>());
        R_TEST( inf<float>(), "+Infinity", json::read_error::floating_point_invalid, 0);
        R_TEST( nan<float>(), "+NaN", json::read_error::floating_point_invalid, 0);
    // double
        R_TEST(-inf<double>(), "-Infinity");
        W_TEST("-Infinity", -inf<double>());
        R_TEST( inf<double>(), " Infinity");
        W_TEST("Infinity", inf<double>());
        R_TEST( nan<double>(), " NaN");
        W_TEST("NaN", nan<double>());
        R_TEST(-nan<double>(), "-NaN");
        W_TEST("-NaN", -nan<double>());
        R_TEST( inf<double>(), "+Infinity", json::read_error::floating_point_invalid, 0);
        R_TEST( nan<double>(), "+NaN", json::read_error::floating_point_invalid, 0);
    // long double
        R_TEST(-inf<long double>(), "-Infinity");
        W_TEST("-Infinity", -inf<long double>());
        R_TEST( inf<long double>(), " Infinity");
        W_TEST("Infinity", inf<long double>());
        R_TEST( nan<long double>(), " NaN");
        W_TEST("NaN", nan<long double>());
        R_TEST(-nan<long double>(), "-NaN");
        W_TEST("-NaN", -nan<long double>());
        R_TEST( inf<long double>(), "+Infinity", json::read_error::floating_point_invalid, 0);
        R_TEST( nan<long double>(), "+NaN", json::read_error::floating_point_invalid, 0);
    // errors
        R_TEST( inf<double>(), "Ix", json::read_error::floating_point_invalid, 1);
        R_TEST( inf<double>(), "Inx", json::read_error::floating_point_invalid, 2);
        R_TEST( inf<double>(), "Infx", json::read_error::floating_point_invalid, 3);
        R_TEST( inf<double>(), "Infix", json::read_error::floating_point_invalid, 4);
        R_TEST( inf<double>(), "Infinx", json::read_error::floating_point_invalid, 5);
        R_TEST( inf<double>(), "Infinix", json::read_error::floating_point_invalid, 6);
        R_TEST( inf<double>(), "Infinitx", json::read_error::floating_point_invalid, 7);
        R_TEST( inf<double>(), "-Infinitx", json::read_error::floating_point_invalid, 8);
        R_TEST( inf<double>(), "-Infinix", json::read_error::floating_point_invalid, 7);
        R_TEST( inf<double>(), "-Infinx", json::read_error::floating_point_invalid, 6);
        R_TEST( inf<double>(), "-Infix", json::read_error::floating_point_invalid, 5);
        R_TEST( inf<double>(), "-Infx", json::read_error::floating_point_invalid, 4);
        R_TEST( inf<double>(), "-Inx", json::read_error::floating_point_invalid, 3);
        R_TEST( inf<double>(), "-Ix", json::read_error::floating_point_invalid, 2);
        R_TEST( inf<double>(), "Nx", json::read_error::floating_point_invalid, 1);
        R_TEST( inf<double>(), "Nax", json::read_error::floating_point_invalid, 2);
        R_TEST( inf<double>(), "-Nax", json::read_error::floating_point_invalid, 3);
        R_TEST( inf<double>(), "-Nx", json::read_error::floating_point_invalid, 2);
        R_TEST( inf<double>(), "-xxx", json::read_error::floating_point_invalid, 1);
        R_TEST( inf<double>(), "xxx", json::read_error::floating_point_invalid, 0);
TEST_END()


TEST_BEG(json_number_validation_1, cxon::JSON<cxon::test::input_iterator_traits<>>, "/core") // json number validation
    // floating point
        R_TEST((double)0, "0");
        R_TEST((double)-0., "-0");
        R_TEST((double)0, "0.0");
        R_TEST((double)1000, "1e3");
        R_TEST((double)1000, "1000000E-3");
        R_TEST((double)1000, "1e+3");
        R_TEST((double)1000, "1e+03");
        R_TEST((double)0, "0e+03");
        R_TEST((double)0, "", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "+0", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "01", json::read_error::ok, 1);
        R_TEST((double)0, "0.", json::read_error::floating_point_invalid, 2);
        R_TEST((double)0, "0e", json::read_error::floating_point_invalid, 2);
        R_TEST((double)0, ".0", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, ".", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "-", json::read_error::floating_point_invalid, 1);
        R_TEST((double)0, "+", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "e", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, std::string(64 + 1, '1'), json::read_error::overflow, 64); // cxon::json::num_len_max
    // integral
        W_TEST("0", (signed)0);
        R_TEST((signed)0, "0");
        R_TEST((signed)-0, "-0");
        R_TEST((signed)10, "10");
        R_TEST((signed)-10, "-10");
        R_TEST((signed)0, "", json::read_error::integral_invalid, 0);
        R_TEST((signed)0, "+0", json::read_error::integral_invalid, 0);
        R_TEST((signed)0, ".", json::read_error::integral_invalid, 0);
        R_TEST((signed)0, "-", json::read_error::integral_invalid, 1);
        R_TEST((signed)0, "+", json::read_error::integral_invalid, 0);
        R_TEST((signed)0, "e", json::read_error::integral_invalid, 0);
        R_TEST((signed)0, "0b", json::read_error::ok, 1);
        R_TEST((signed)0, "0b2", json::read_error::ok, 1);
        R_TEST((signed)0, "08", json::read_error::ok, 1);
        R_TEST((signed)0, "0x", json::read_error::ok, 1);
        R_TEST((signed)0, "0xg", json::read_error::ok, 1);
        R_TEST((signed)0, std::string(32 + 1, '1'), json::read_error::overflow, 32); // cxon::json::num_len_max
    // errors
#   define CHECK_ERROR(ot, in, bf, ec) { auto r = test::from_string<XXON>(ot, in, cxon::json::num_len_max::set<bf>()); TEST_CHECK(ec); }
    {   signed t;
        CHECK_ERROR(t, " 0", 1, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, " 0", 2, r)
        CHECK_ERROR(t, "-0", 1, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "-0", 2, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "-0", 3, r)
    }
    {   double t;
        CHECK_ERROR(t, "0", 1, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "0", 2, r)
        CHECK_ERROR(t, "-0", 1, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "0.", 1, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "0.", 2, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "0.0", 2, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "0.x", 3, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "0.0", 3, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "0.0", 4, r)
        CHECK_ERROR(t, "0e", 1, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "0e0", 2, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "0E0", 2, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "0e+", 2, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "0E+", 2, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "0.0x", 4, r && *r.end == 'x')
        CHECK_ERROR(t, "\"-", 1, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "\"i", 1, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "\"in", 2, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "\"ix", 2, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "\"inf", 3, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "\"inx", 3, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "\"n", 1, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "\"na", 2, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "\"nx", 2, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "\"nan", 3, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "\"nax", 3, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "\"nan\"", 4, r.ec == json::read_error::overflow)
        CHECK_ERROR(t, "\"nan", 4, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "\"nan\"", 5, r.ec == json::read_error::overflow)
    }
#   undef CHECK_ERROR
TEST_END()

namespace {
    struct strict_number_traits : cxon::json::format_traits {
        struct number : cxon::json::format_traits::number {
            static constexpr bool strict = true;
        };
    };
}

TEST_BEG(json_number_validation_2, cxon::JSON<strict_number_traits>, "/core") // json number validation
    // floating point
        W_TEST("0", (double)0);
        R_TEST((double)0, "0");
        R_TEST((double)10, "10");
        R_TEST((double)0, "-0");
        R_TEST((double)0, "0.0");
        R_TEST((double)1000, "1e3");
        R_TEST((double)0, "0e3");
        R_TEST((double)1000, "1000000E-3");
        R_TEST((double)1e+3, "1e+3");
        R_TEST((double)1e-3, "1e-3");
        R_TEST((double)1e+3, "1e+03");
        R_TEST((double)1e-3, "1e-03");
        R_TEST((double)1e+3, "1E+3");
        R_TEST((double)1e-3, "1E-3");
        R_TEST((double)1e-3, "1E-03");
        R_TEST((double)1e+3, "1.0e+3");
        R_TEST((double)1e-3, "1.0e-3");
        R_TEST((double)1e+3, "1.0e+03");
        R_TEST((double)1e-3, "1.0e-03");
        R_TEST((double)1e+3, "1.0E+3");
        R_TEST((double)1e-3, "1.0E-3");
        R_TEST((double)1e-3, "1.0E-03");
        R_TEST((double)0, "1.", json::read_error::floating_point_invalid, 0);
        R_TEST((double)1, "1.0");
        R_TEST((double)0, "", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "+0", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "01", json::read_error::ok, 1);
        R_TEST((double)0, "0.", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "0e", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "0E", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, ".0", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, ".", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "-", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "+", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "e", json::read_error::floating_point_invalid, 0);
        using namespace test;
        R_TEST(-inf<double>(), QS("-inf"));
        R_TEST( inf<double>(), QS("inf"));
        R_TEST( inf<double>(), QS("+inf"), json::read_error::floating_point_invalid, 0);
        R_TEST( nan<double>(), QS("nan"));
        R_TEST(-nan<double>(), QS("-nan"));
        W_TEST(QS("inf"), inf<double>());
        W_TEST(QS("nan"), nan<double>());
    // integral
        W_TEST("0", (signed)0);
        R_TEST((signed)0, "0");
        R_TEST((signed)10, "10");
        R_TEST((signed)-10, "-10");
        R_TEST((signed)0, "", json::read_error::integral_invalid, 0);
        R_TEST((signed)0, "+0", json::read_error::integral_invalid, 0);
        R_TEST((signed)0, ".", json::read_error::integral_invalid, 0);
        R_TEST((signed)0, "-", json::read_error::integral_invalid, 0);
        R_TEST((signed)0, "+", json::read_error::integral_invalid, 0);
        R_TEST((signed)0, "e", json::read_error::integral_invalid, 0);
        R_TEST((signed)0, "0b", json::read_error::ok, 1);
        R_TEST((signed)0, "0b2", json::read_error::ok, 1);
        R_TEST((signed)0, "08", json::read_error::ok, 1);
        R_TEST((signed)0, "0x", json::read_error::ok, 1);
        R_TEST((signed)0, "0xg", json::read_error::ok, 1);
    // errors
#   define CHECK_ERROR(ot, in, bf, ec) { auto r = test::from_string<XXON>(ot, in, cxon::json::num_len_max::set<bf>()); TEST_CHECK(ec); }
    {   double t;
        CHECK_ERROR(t, "0.", 2, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "0.x", 3, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "0.0x", 4, r && *r.end == 'x')
        CHECK_ERROR(t, "\"ix", 2, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "\"inx", 3, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "\"nx", 2, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "\"nax", 3, r.ec == json::read_error::floating_point_invalid)
        CHECK_ERROR(t, "\"nan", 4, r.ec == json::read_error::floating_point_invalid)
    }
#   undef CHECK_ERROR
TEST_END()


TEST_BEG(special_numbers_javascript_nans_strict, cxon::JSON<cxon::test::allow_javascript_nans_traits<strict_number_traits>>, "/core") // special numbers
    using namespace test;
    // float
        R_TEST(-inf<float>(), "-Infinity");
        W_TEST("-Infinity", -inf<float>());
        R_TEST( inf<float>(), " Infinity");
        W_TEST("Infinity", inf<float>());
        R_TEST( nan<float>(), " NaN");
        W_TEST("NaN", nan<float>());
        R_TEST(-nan<float>(), "-NaN");
        W_TEST("-NaN", -nan<float>());
        R_TEST( inf<float>(), "+Infinity", json::read_error::floating_point_invalid, 0);
        R_TEST( nan<float>(), "+NaN", json::read_error::floating_point_invalid, 0);
    // double
        R_TEST(-inf<double>(), "-Infinity");
        W_TEST("-Infinity", -inf<double>());
        R_TEST( inf<double>(), " Infinity");
        W_TEST("Infinity", inf<double>());
        R_TEST( nan<double>(), " NaN");
        W_TEST("NaN", nan<double>());
        R_TEST(-nan<double>(), "-NaN");
        W_TEST("-NaN", -nan<double>());
        R_TEST( inf<double>(), "+Infinity", json::read_error::floating_point_invalid, 0);
        R_TEST( nan<double>(), "+NaN", json::read_error::floating_point_invalid, 0);
    // long double
        R_TEST(-inf<long double>(), "-Infinity");
        W_TEST("-Infinity", -inf<long double>());
        R_TEST( inf<long double>(), " Infinity");
        W_TEST("Infinity", inf<long double>());
        R_TEST( nan<long double>(), " NaN");
        W_TEST("NaN", nan<long double>());
        R_TEST(-nan<long double>(), "-NaN");
        W_TEST("-NaN", -nan<long double>());
        R_TEST( inf<long double>(), "+Infinity", json::read_error::floating_point_invalid, 0);
        R_TEST( nan<long double>(), "+NaN", json::read_error::floating_point_invalid, 0);
    // errors
        R_TEST( inf<double>(), "x", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Ix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Inx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Infx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Infix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Infinx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Infinix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Infinitx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Infinitx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Infinix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Infinx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Infix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Infx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Inx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Ix", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Nx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "Nax", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Nax", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-Nx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "-xxx", json::read_error::floating_point_invalid, 0);
        R_TEST( inf<double>(), "xxx", json::read_error::floating_point_invalid, 0);
TEST_END()


TEST_BEG(json_escapes, cxon::JSON<>, "/core") // json
    // char
        R_TEST('\0', QS("\\u0000"));
        W_TEST(QS("\\u0000"), '\0');
        R_TEST('\x37', QS("\\u0037"));
        W_TEST(QS("7"), '\x37');
        R_TEST('"', QS("\\\""));
        W_TEST(QS("\\\""), '"');
        R_TEST('\'', QS("'"));
        W_TEST(QS("'"), '\'');
        R_TEST('\x7f', QS("\\u007f"));
        W_TEST(QS("\x7f"), '\x7f');
        R_TEST('\x8f', QS("\\u008f"));
        W_TEST(QS("\x8f"), '\x8f');
        R_TEST('\xff', QS("\\u00ff")); // invalid utf-8
        W_TEST(QS("\xff"), '\xff'); // invalid utf-8
        R_TEST('\0', QS("\\u1111"), json::read_error::character_invalid, 1);
    // escapes
        R_TEST("\"", QS("\\\""));
        R_TEST("\\", QS("\\\\"));
        R_TEST("/", QS("\\/"));
        R_TEST("\b", QS("\\b"));
        R_TEST("\f", QS("\\f"));
        R_TEST("\n", QS("\\n"));
        R_TEST("\r", QS("\\r"));
        R_TEST("\t", QS("\\t"));
        R_TEST("\u0008", QS("\\u0008"));
        R_TEST("\0008", QS("\\u00008")); // one more
        R_TEST("", QS("\\u008"), json::read_error::escape_invalid, 1); // one less
        R_TEST("", "\"\\u", json::read_error::escape_invalid, 1); // none
        R_TEST("", "\"\\uX", json::read_error::escape_invalid, 1); // none
        R_TEST("", "\"\\u0", json::read_error::escape_invalid, 1); // none
        R_TEST("", "\"\\u0X", json::read_error::escape_invalid, 1); // none
        R_TEST("", "\"\\u00", json::read_error::escape_invalid, 1); // none
        R_TEST("", "\"\\u00X", json::read_error::escape_invalid, 1); // none
        R_TEST("", "\"\\u000", json::read_error::escape_invalid, 1); // none
        R_TEST("", "\"\\u000X", json::read_error::escape_invalid, 1); // none
        R_TEST("", "\"\\ ", json::read_error::escape_invalid, 1); // none
        R_TEST("", "\"\\:", json::read_error::escape_invalid, 1); // none
        R_TEST("", "\"\\'", json::read_error::escape_invalid, 1); // none
        W_TEST(QS("\\u0001"), "\1");
        W_TEST(QS("\\u0002"), "\2");
        W_TEST(QS("\\u0003"), "\3");
        W_TEST(QS("\\u0004"), "\4");
        W_TEST(QS("\\u0005"), "\5");
        W_TEST(QS("\\u0006"), "\6");
        W_TEST(QS("\\u0007"), "\7");
        W_TEST(QS("\\b"), "\10");
        W_TEST(QS("\\t"), "\11");
        W_TEST(QS("\\n"), "\12");
        W_TEST(QS("\\u000b"), "\13");
        W_TEST(QS("\\f"), "\14");
        W_TEST(QS("\\r"), "\15");
        W_TEST(QS("\\u000e"), "\16");
        W_TEST(QS("\\u000f"), "\17");
        W_TEST(QS("\\u0010"), "\20");
        W_TEST(QS("\\u0011"), "\21");
        W_TEST(QS("\\u0012"), "\22");
        W_TEST(QS("\\u0013"), "\23");
        W_TEST(QS("\\u0014"), "\24");
        W_TEST(QS("\\u0015"), "\25");
        W_TEST(QS("\\u0016"), "\26");
        W_TEST(QS("\\u0017"), "\27");
        W_TEST(QS("\\u0018"), "\30");
        W_TEST(QS("\\u0019"), "\31");
        W_TEST(QS("\\u001a"), "\32");
        W_TEST(QS("\\u001b"), "\33");
        W_TEST(QS("\\u001c"), "\34");
        W_TEST(QS("\\u001d"), "\35");
        W_TEST(QS("\\u001e"), "\36");
        W_TEST(QS("\\u001f"), "\37");
        W_TEST(QS("\\\""), "\42");
        W_TEST(QS("\\\\"), "\134");
        W_TEST(QS(" "), L" ");
        W_TEST(QS(":"), L":");
        W_TEST(QS("'"), L"'");
TEST_END()

TEST_BEG(json_escapes_input_iterator, cxon::JSON<cxon::test::input_iterator_traits<>>, "/core") // json
    // char
        R_TEST('\0', QS("\\u0000"));
        W_TEST(QS("\\u0000"), '\0');
        R_TEST('\x37', QS("\\u0037"));
        W_TEST(QS("7"), '\x37');
        R_TEST('"', QS("\\\""));
        W_TEST(QS("\\\""), '"');
        R_TEST('\'', QS("'"));
        W_TEST(QS("'"), '\'');
        R_TEST('\x7f', QS("\\u007f"));
        W_TEST(QS("\x7f"), '\x7f');
        R_TEST('\x8f', QS("\\u008f"));
        W_TEST(QS("\x8f"), '\x8f');
        R_TEST('\xff', QS("\\u00ff")); // invalid utf-8
        W_TEST(QS("\xff"), '\xff'); // invalid utf-8
        R_TEST('\0', QS("\\u1111"), json::read_error::character_invalid, 7);
    // escapes
        R_TEST("\"", QS("\\\""));
        R_TEST("\\", QS("\\\\"));
        R_TEST("/", QS("\\/"));
        R_TEST("\b", QS("\\b"));
        R_TEST("\f", QS("\\f"));
        R_TEST("\n", QS("\\n"));
        R_TEST("\r", QS("\\r"));
        R_TEST("\t", QS("\\t"));
        R_TEST("\u0008", QS("\\u0008"));
        R_TEST("\0008", QS("\\u00008")); // one more
        R_TEST("", QS("\\u008"), json::read_error::escape_invalid, 6); // one less
        R_TEST("", "\"\\u", json::read_error::escape_invalid, 3); // none
        R_TEST("", "\"\\uX", json::read_error::escape_invalid, 3); // none
        R_TEST("", "\"\\u0", json::read_error::escape_invalid, 4); // none
        R_TEST("", "\"\\u0X", json::read_error::escape_invalid, 4); // none
        R_TEST("", "\"\\u00", json::read_error::escape_invalid, 5); // none
        R_TEST("", "\"\\u00X", json::read_error::escape_invalid, 5); // none
        R_TEST("", "\"\\u000", json::read_error::escape_invalid, 6); // none
        R_TEST("", "\"\\u000X", json::read_error::escape_invalid, 6); // none
        R_TEST("", "\"\\ ", json::read_error::escape_invalid, 2); // none
        R_TEST("", "\"\\:", json::read_error::escape_invalid, 2); // none
        R_TEST("", "\"\\'", json::read_error::escape_invalid, 2); // none
        W_TEST(QS("\\u0001"), "\1");
        W_TEST(QS("\\u0002"), "\2");
        W_TEST(QS("\\u0003"), "\3");
        W_TEST(QS("\\u0004"), "\4");
        W_TEST(QS("\\u0005"), "\5");
        W_TEST(QS("\\u0006"), "\6");
        W_TEST(QS("\\u0007"), "\7");
        W_TEST(QS("\\b"), "\10");
        W_TEST(QS("\\t"), "\11");
        W_TEST(QS("\\n"), "\12");
        W_TEST(QS("\\u000b"), "\13");
        W_TEST(QS("\\f"), "\14");
        W_TEST(QS("\\r"), "\15");
        W_TEST(QS("\\u000e"), "\16");
        W_TEST(QS("\\u000f"), "\17");
        W_TEST(QS("\\u0010"), "\20");
        W_TEST(QS("\\u0011"), "\21");
        W_TEST(QS("\\u0012"), "\22");
        W_TEST(QS("\\u0013"), "\23");
        W_TEST(QS("\\u0014"), "\24");
        W_TEST(QS("\\u0015"), "\25");
        W_TEST(QS("\\u0016"), "\26");
        W_TEST(QS("\\u0017"), "\27");
        W_TEST(QS("\\u0018"), "\30");
        W_TEST(QS("\\u0019"), "\31");
        W_TEST(QS("\\u001a"), "\32");
        W_TEST(QS("\\u001b"), "\33");
        W_TEST(QS("\\u001c"), "\34");
        W_TEST(QS("\\u001d"), "\35");
        W_TEST(QS("\\u001e"), "\36");
        W_TEST(QS("\\u001f"), "\37");
        W_TEST(QS("\\\""), "\42");
        W_TEST(QS("\\\\"), "\134");
        W_TEST(QS(" "), L" ");
        W_TEST(QS(":"), L":");
        W_TEST(QS("'"), L"'");
TEST_END()

namespace cxon { namespace test {
    struct produce_strict_javascript_traits : cxon::json::format_traits {
        static constexpr bool produce_strict_javascript = true;
    };
}}

TEST_BEG(produce_strict_javascript_traits, cxon::JSON<cxon::test::produce_strict_javascript_traits>, "/core")
    W_TEST(QS("x"), "x");
    W_TEST(QS("\\u2028"), "\xE2\x80\xA8");
    W_TEST(QS("\\u2029"), "\xE2\x80\xA9");
    W_TEST(QS("\xE2\x80"), "\xE2\x80");
    W_TEST(QS("x\\u2028"), "x\xE2\x80\xA8");
    W_TEST(QS("x\\u2029"), "x\xE2\x80\xA9");
    W_TEST(QS("x"), u8"x");
    W_TEST(QS("\\u2028"), u8"\u2028");
    W_TEST(QS("\\u2029"), u8"\u2029");
    W_TEST(QS("x"), u"x");
    W_TEST(QS("\\u2028"), u"\u2028");
    W_TEST(QS("\\u2029"), u"\u2029");
    W_TEST(QS("x"), U"x");
    W_TEST(QS("\\u2028"), U"\u2028");
    W_TEST(QS("\\u2029"), U"\u2029");
    W_TEST(QS("x"), L"x");
    W_TEST(QS("\\u2028"), L"\u2028");
    W_TEST(QS("\\u2029"), L"\u2029");
    W_TEST(QS("\xE2\x80\xA7"), u8"\u2027");
    W_TEST(QS("\xE2\x81\xA7"), "\xE2\x81\xA7");
    W_TEST(QS("\xf4\x8f\xbf\xbf"), u"\xdbff\xdfff"); // surrogate
    // errors
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            c.push_back('x');
            auto r = cxon::to_bytes<XXON>(c, "s\xE2\x80\xA8");
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[1];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, "s\xE2\x80\xA8");
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[2];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, "s\xE2\x80\xA8");
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[3];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, "s\xE2\x80\xA8");
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[4];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, "s\xE2\x80\xA8");
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[5];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, "s\xE2\x80\xA8");
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[6];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, "s\xE2\x80\xA8");
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[7];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, "s\xE2\x80\xA8");
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
    {   char b[8];
        auto c = cxon::cnt::make_range_container(std::begin(b), std::end(b));
            auto r = cxon::to_bytes<XXON>(c, "s\xE2\x80\xA8");
        TEST_CHECK(r.ec == json::write_error::output_failure);
    }
TEST_END()
