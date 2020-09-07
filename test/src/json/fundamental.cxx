// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "test.hxx"

#include <string>

////////////////////////////////////////////////////////////////////////////////

namespace cxon { namespace test {

    template <typename T>   inline std::string to_string(T t)           { return std::to_string(t); }
    // TODO: using cxon -> nok, use std lib
    template <>             inline std::string to_string(float t)       { return test::to_string<JSON<>>(t); }
    template <>             inline std::string to_string(double t)      { return test::to_string<JSON<>>(t); }
    template <>             inline std::string to_string(long double t) { return test::to_string<JSON<>>(t); }

    template <typename T>   constexpr T tmin() { return std::numeric_limits<T>::lowest(); }
    template <typename T>   inline std::string smin() { return to_string(tmin<T>()); }
    template <typename T>   inline std::string omin() { return smin<T>() + '0'; }
    template <typename T>   constexpr T tmax() { return std::numeric_limits<T>::max(); }
    template <typename T>   inline std::string smax() { return to_string(tmax<T>()); }
    template <typename T>   inline std::string omax() { return smax<T>() + '0'; }

}}

TEST_BEG(cxon::JSON<>)
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
        R_TEST(U'\0', QS("\\udbff\\ue000"), json::read_error::surrogate_invalid, 1); // invalid surrogate
        R_TEST(U'\0', QS("\\udbff\\udbff"), json::read_error::surrogate_invalid, 1); // invalid surrogate
        R_TEST(U'\0', QS("\xF0\xA8\x91"), json::read_error::character_invalid, 1);
        R_TEST(U'\0', QS("\xF0\xA8"), json::read_error::character_invalid, 1);
        R_TEST(U'\0', QS("\xF0"), json::read_error::character_invalid, 1);
        W_TEST(QS("\xF0\xA8\x91\x80"), U'\x28440');
        W_TEST(QS(""), U'\x200000');
    // wchar_t
        R_TEST(L'a', QS("a"));
        W_TEST(QS("a"), L'a');
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
        R_TEST(tmin<long double>(), smin<long double>());
        W_TEST(smin<long double>(), tmin<long double>());
        R_TEST((long double)0, "0");
        W_TEST("0", (long double)0);
        R_TEST(tmax<long double>(), smax<long double>());
        W_TEST(smax<long double>(), tmax<long double>());
    // nullptr_t
        R_TEST(nullptr, "null");
        W_TEST("null", nullptr);
        R_TEST(nullptr, "", json::read_error::unexpected, 0);
        R_TEST(nullptr, "nu", json::read_error::unexpected, 0);
        R_TEST(nullptr, "a", json::read_error::unexpected, 0);
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

TEST_BEG(cxon::JSON<>) // special numbers
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
        R_TEST(  nan<float>(), QS("-nan")/*, json::read_error::floating_point_invalid, 0*/); // TODO: disallow?
        W_TEST(QS("nan"), nan<float>());
        W_TEST(QS("nan"), bfp<float>(0x7fffffff));
        W_TEST(QS("nan"), bfp<float>(0x7f80ffff));
        W_TEST(QS("nan"), bfp<float>(0xffc00000));
        W_TEST(QS("nan"), 0.0 /  zero<float>());
        W_TEST(QS("nan"), 0.0 / -zero<float>());
        W_TEST(QS("nan"), 0.0 *  inf<float>());
        W_TEST(QS("nan"), 0.0 * -inf<float>());
        W_TEST(QS("nan"), inf<float>() + -inf<float>());
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
        R_TEST(  nan<double>(), QS("-nan")/*, json::read_error::floating_point_invalid, 0*/); // TODO: disallow?
        W_TEST(QS("nan"), nan<double>());
        W_TEST(QS("nan"), bfp<double>(0x7fffffffffffffff));
        W_TEST(QS("nan"), bfp<double>(0x7ff0ffffffffffff));
        W_TEST(QS("nan"), bfp<double>(0xfff8000000000000));
        W_TEST(QS("nan"), 0.0 /  zero<double>());
        W_TEST(QS("nan"), 0.0 / -zero<double>());
        W_TEST(QS("nan"), 0.0 *  inf<double>());
        W_TEST(QS("nan"), 0.0 * -inf<double>());
        W_TEST(QS("nan"), inf<double>() + -inf<double>());
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
        R_TEST(  nan<long double>(), QS("-nan")/*, json::read_error::floating_point_invalid, 0*/); // TODO: disallow?
        W_TEST(QS("nan"), nan<long double>());
        //W_TEST( QS("nan"), bfp<long double>(0x));
        //W_TEST( QS("nan"), bfp<long double>(0x));
        //W_TEST( QS("nan"), bfp<long double>(0x));
        W_TEST(QS("nan"), 0.0 /  zero<long double>());
        W_TEST(QS("nan"), 0.0 / -zero<long double>());
        W_TEST(QS("nan"), 0.0 *  inf<long double>());
        W_TEST(QS("nan"), 0.0 * -inf<long double>());
        W_TEST(QS("nan"), inf<long double>() + -inf<long double>());
TEST_END()

TEST_BEG(cxon::JSON<cxon::test::input_iterator_traits>) // special numbers
    using namespace test;
    R_TEST(0.0, "0");
    R_TEST(0.0, "0.0");
    W_TEST("0", 0.0);
    R_TEST(10.0, "10");
    R_TEST(10.0, "10.0");
    R_TEST(10.0e+10, "10.0e+10");
    R_TEST(-inf<double>(), QS("-inf"));
    W_TEST(QS("-inf"), -inf<double>());
    R_TEST(inf<double>(), QS("inf"));
    W_TEST(QS("inf"), inf<double>());
    R_TEST(inf<double>(), QS("+inf"), json::read_error::floating_point_invalid, 1);
    R_TEST(nan<double>(), QS("nan"));
    W_TEST(QS("nan"), nan<double>());
    R_TEST(nan<double>(), QS("-nan"));
    W_TEST(QS("nan"), -nan<double>());
TEST_END()


TEST_BEG(cxon::JSON<cxon::test::input_iterator_traits>) // json number validation
    // floating point
        R_TEST((double)0, "0");
        R_TEST((double)0, "-0");
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
TEST_END()

namespace cxon { namespace test {
    struct strict_number_traits : cxon::json::format_traits {
        struct number {
            static constexpr bool strict = true;
        };
    };
}}

TEST_BEG(cxon::JSON<cxon::test::strict_number_traits>) // json number validation
    // floating point
        W_TEST("0", (double)0);
        R_TEST((double)0, "0");
        R_TEST((double)10, "10");
        R_TEST((double)0, "-0");
        R_TEST((double)0, "0.0");
        R_TEST((double)1000, "1e3");
        R_TEST((double)0, "0e3");
        R_TEST((double)1000, "1000000E-3");
        R_TEST((double)1000, "1e+3");
        R_TEST((double)1000, "1e+03");
        R_TEST((double)0, "", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "+0", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "01", json::read_error::ok, 1);
        R_TEST((double)0, "0.", json::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "0e", json::read_error::floating_point_invalid, 0);
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
        R_TEST( nan<double>(), QS("-nan"));
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
TEST_END()


TEST_BEG(cxon::JSON<>) // json
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
        R_TEST("", QS("\\u"), json::read_error::escape_invalid, 1); // none
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
TEST_END()

namespace cxon { namespace test {
    struct strict_js_traits : cxon::json::format_traits {
        static constexpr bool strict_js = true;
    };
}}

TEST_BEG(cxon::JSON<cxon::test::strict_js_traits>)
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
TEST_END()
