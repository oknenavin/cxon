// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "../cxon.hxx"
#include "../pretty.hxx"

#include <cstdio>

///////////////////////////////////////////////////////////////////////////////

#define TEST_A      all_
#define TEST_F      err_
#define TEST_V      tst_

#define TEST_INIT()\
    static std::vector<struct tst_struct_*> TEST_V;\
    struct tst_struct_ {\
        tst_struct_() { TEST_V.push_back(this); }\
        virtual void test() const = 0;\
    };\
    static unsigned TEST_A;\
    static unsigned TEST_F;

#define TEST_CAT_(x, y) x##y
#define TEST_CAT(x, y)  TEST_CAT_(x, y)
#define TEST_BEG(...)   static struct : tst_struct_ { using XXON = __VA_ARGS__; void test() const override {
#define TEST_END()      } } const TEST_CAT(TEST_, __COUNTER__){}; 

TEST_INIT()

int main() {
    for (auto t : TEST_V)
        t->test();
    TEST_F ?
        fprintf(stdout, "cxon: %u of %u failed\n", TEST_F, TEST_A) :
        fprintf(stdout, "cxon: %u of %u passed\n", TEST_A, TEST_A)
    ;
    return TEST_F;
}

///////////////////////////////////////////////////////////////////////////////

namespace test {

    template <typename X, typename T>
        inline std::string to_string(const T& t);

    template <typename X, typename T>
        static bool verify_read(const T& ref, const std::string& sbj);
    template <typename X, typename T>
        static bool verify_read(const T& ref, const std::string& sbj, cxon::read_error err, int pos = -1);

    template <typename X, typename T>
        static bool verify_write(const std::string& ref, const T& sbj);
    template <typename X, typename T>
        static bool verify_write(const std::string& ref, const T& sbj, cxon::write_error err);

}   // test

#define R_TEST(ref, ...)\
    if (++TEST_A, !test::verify_read<XXON>(ref, __VA_ARGS__)) {\
        ++TEST_F, fprintf(stderr, "at %s:%li\n", __FILE__, (long)__LINE__), fflush(stderr);\
        CXON_ASSERT(false, "check failed");\
    }
#define W_TEST(ref, ...)\
    if (++TEST_A, !test::verify_write<XXON>(ref, __VA_ARGS__)) {\
        ++TEST_F, fprintf(stderr, "at %s:%li\n", __FILE__, (long)__LINE__), fflush(stderr);\
        CXON_ASSERT(false, "check failed");\
    }

#define QS(s) "\"" s "\""

// tests //////////////////////////////////////////////////////////////////////

namespace base {

    template <typename T>   inline std::string to_string(T t)           { return std::to_string(t); }
    // TODO: using cxon, nok, use std lib
    template <>             inline std::string to_string(float t)       { return test::to_string<cxon::CXON<>>(t); }
    template <>             inline std::string to_string(double t)      { return test::to_string<cxon::CXON<>>(t); }
    template <>             inline std::string to_string(long double t) { return test::to_string<cxon::CXON<>>(t); }

    template <typename T>   constexpr T tmin() { return std::numeric_limits<T>::lowest(); }
    template <typename T>   inline std::string smin() { return to_string(tmin<T>()); }
    template <typename T>   inline std::string omin() { return smin<T>() + '0'; }
    template <typename T>   constexpr T tmax() { return std::numeric_limits<T>::max(); }
    template <typename T>   inline std::string smax() { return to_string(tmax<T>()); }
    template <typename T>   inline std::string omax() { return smax<T>() + '0'; }

    struct force_input_iterator_traits : cxon::format_traits {
        static constexpr bool force_input_iterator = true;
    };

}

TEST_BEG(cxon::CXON<>) // base
    using namespace base;
    // bool
        R_TEST(true, "true");
        W_TEST("true", true);
        R_TEST(false, "false");
        W_TEST("false", false);
        R_TEST(false, "t", cxon::read_error::boolean_invalid, 0);
        R_TEST(false, "f", cxon::read_error::boolean_invalid, 0);
        R_TEST(false, "", cxon::read_error::unexpected, 0);
    // char
        R_TEST('\0', QS("\\0"));
        W_TEST(QS("\\0"), '\0');
        R_TEST('\37', QS("\\37"));
        W_TEST(QS("\\37"), '\37');
        R_TEST('"', QS("\\\""));
        W_TEST(QS("\\\""), '"');
        R_TEST('\'', QS("'"));
        W_TEST(QS("'"), '\'');
        R_TEST('\x7f', QS("\\x7f"));
        W_TEST(QS("\x7f"), '\x7f');
        R_TEST('\x8f', QS("\\x8f"));
        W_TEST(QS("\x8f"), '\x8f');
        R_TEST('\xff', QS("\\xff")); // invalid utf-8
        W_TEST(QS("\xff"), '\xff'); // invalid utf-8
        R_TEST('\0', QS("\xff"), cxon::read_error::character_invalid, 1); // invalid utf-8
        R_TEST('\0', QS("\\u1111"), cxon::read_error::character_invalid, 1);
    // char16_t
        R_TEST(u'a', QS("a"));
        W_TEST(QS("a"), u'a');
        R_TEST(u'\x3B1', QS("\xCE\xB1"));
        W_TEST(QS("\xCE\xB1"), u'\x3B1');
        R_TEST(u'\xFFFF', QS("\xEF\xBF\xBF"));
        W_TEST(QS("\xEF\xBF\xBF"), u'\xFFFF');
        R_TEST(u'\0', QS("\xF0\x90\x80\x80"), cxon::read_error::character_invalid, 1);
    // char32_t
        R_TEST(U'a', QS("a"));
        R_TEST(U'\0', QS("\x80"), cxon::read_error::character_invalid, 1);
        W_TEST(QS("a"), U'a');
        R_TEST(U'\x3B1', QS("\xCE\xB1"));
        R_TEST(U'\0', QS("\xCE"), cxon::read_error::character_invalid, 1);
        W_TEST(QS("\xCE\xB1"), U'\x3B1');
        R_TEST(U'\x389A', QS("\xE3\xA2\x9A"));
        R_TEST(U'\0', QS("\xE3\xA2"), cxon::read_error::character_invalid, 1);
        R_TEST(U'\0', QS("\xE3"), cxon::read_error::character_invalid, 1);
        W_TEST(QS("\xE3\xA2\x9A"), U'\x389A');
        R_TEST(U'\x28440', QS("\xF0\xA8\x91\x80"));
        R_TEST(U'\0', QS("\xF0\xA8\x91"), cxon::read_error::character_invalid, 1);
        R_TEST(U'\0', QS("\xF0\xA8"), cxon::read_error::character_invalid, 1);
        R_TEST(U'\0', QS("\xF0"), cxon::read_error::character_invalid, 1);
        W_TEST(QS("\xF0\xA8\x91\x80"), U'\x28440');
    // wchar_t
        R_TEST(L'a', QS("a"));
        W_TEST(QS("a"), L'a');
    // signed char
        R_TEST((signed char)42, "0b101010");
        R_TEST((signed char)052, "052");
        R_TEST((signed char)0x2A, "0x2A");
        R_TEST(tmin<signed char>(), smin<signed char>());
        W_TEST(smin<signed char>(), tmin<signed char>());
        R_TEST((signed char)0, "0");
        W_TEST("0", (signed char)0);
        R_TEST(tmax<signed char>(), smax<signed char>());
        W_TEST(smax<signed char>(), tmax<signed char>());
        R_TEST(tmin<signed char>(), omin<signed char>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<signed char>(), omax<signed char>(), cxon::read_error::integral_invalid, 0);
    // unsigned char
        R_TEST((unsigned char)42, "0b101010");
        R_TEST((unsigned char)052, "052");
        R_TEST((unsigned char)0x2A, "0x2A");
        R_TEST(tmin<unsigned char>(), smin<unsigned char>());
        W_TEST(smin<unsigned char>(), tmin<unsigned char>());
        R_TEST((unsigned char)0x7F, "0x7F");
        W_TEST("127", (unsigned char)0x7F);
        R_TEST(tmax<unsigned char>(), smax<unsigned char>());
        W_TEST(smax<unsigned char>(), tmax<unsigned char>());
        R_TEST(tmax<unsigned char>(), omax<unsigned char>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<unsigned char>(), omax<unsigned char>(), cxon::read_error::integral_invalid, 0);
    // short
        R_TEST((short)42, "0b101010");
        R_TEST((short)052, "052");
        R_TEST((short)0x2A, "0x2A");
        R_TEST(tmin<short>(), smin<short>());
        W_TEST(smin<short>(), tmin<short>());
        R_TEST((short)0, "0");
        W_TEST("0", (short)0);
        R_TEST(tmax<short>(), smax<short>());
        W_TEST(smax<short>(), tmax<short>());
        R_TEST(tmin<short>(), omin<short>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<short>(), omax<short>(), cxon::read_error::integral_invalid, 0);
    // unsigned short
        R_TEST((unsigned short)42, "0b101010");
        R_TEST((unsigned short)052, "052");
        R_TEST((unsigned short)0x2A, "0x2A");
        R_TEST(tmin<unsigned short>(), smin<unsigned short>());
        W_TEST(smin<unsigned short>(), tmin<unsigned short>());
        R_TEST((unsigned short)0xFF, "0xFF");
        W_TEST("255", (unsigned short)0xFF);
        R_TEST(tmax<unsigned short>(), smax<unsigned short>());
        W_TEST(smax<unsigned short>(), tmax<unsigned short>());
        R_TEST(tmax<unsigned short>(), omax<unsigned short>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<unsigned short>(), omax<unsigned short>(), cxon::read_error::integral_invalid, 0);
    // int
        R_TEST((int)42, "0b101010");
        R_TEST((int)052, "052");
        R_TEST((int)0x2A, "0x2A");
        R_TEST(tmin<int>(), smin<int>());
        W_TEST(smin<int>(), tmin<int>());
        R_TEST((int)0, "0");
        W_TEST("0", (int)0);
        R_TEST(tmax<int>(), smax<int>());
        W_TEST(smax<int>(), tmax<int>());
        R_TEST(tmin<int>(), omin<int>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<int>(), omax<int>(), cxon::read_error::integral_invalid, 0);
    // unsigned int
        R_TEST((unsigned int)42, "0b101010");
        R_TEST((unsigned int)052, "052");
        R_TEST((unsigned int)0x2A, "0x2A");
        R_TEST(tmin<unsigned int>(), smin<unsigned int>());
        W_TEST(smin<unsigned int>(), tmin<unsigned int>());
        R_TEST((unsigned int)0xFFFF, "0xFFFF");
        W_TEST("65535", (unsigned int)0xFFFF);
        R_TEST(tmax<unsigned int>(), smax<unsigned int>());
        W_TEST(smax<unsigned int>(), tmax<unsigned int>());
        R_TEST(tmax<unsigned int>(), omax<unsigned int>(), cxon::read_error::integral_invalid, 0);
    // long
        R_TEST((long)42, "0b101010");
        R_TEST((long)052, "052");
        R_TEST((long)0x2A, "0x2A");
        R_TEST(tmin<long>(), smin<long>());
        W_TEST(smin<long>(), tmin<long>());
        R_TEST((long)0, "0");
        W_TEST("0", (long)0);
        R_TEST(tmax<long>(), smax<long>());
        W_TEST(smax<long>(), tmax<long>());
        R_TEST(tmin<long>(), omin<long>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<long>(), omax<long>(), cxon::read_error::integral_invalid, 0);
    // unsigned long
        R_TEST((unsigned long)42, "0b101010");
        R_TEST((unsigned long)052, "052");
        R_TEST((unsigned long)0x2A, "0x2A");
        R_TEST(tmin<unsigned long>(), smin<unsigned long>());
        W_TEST(smin<unsigned long>(), tmin<unsigned long>());
        R_TEST((unsigned long)0xFFFF, "0xFFFF");
        W_TEST("65535", (unsigned long)0xFFFF);
        R_TEST(tmax<unsigned long>(), smax<unsigned long>());
        W_TEST(smax<unsigned long>(), tmax<unsigned long>());
        R_TEST(tmax<unsigned long>(), omax<unsigned long>(), cxon::read_error::integral_invalid, 0);
    // long long
        R_TEST((long long)42, "0b101010");
        R_TEST((long long)052, "052");
        R_TEST((long long)0x2A, "0x2A");
        R_TEST(tmin<long long>(), smin<long long>());
        W_TEST(smin<long long>(), tmin<long long>());
        R_TEST((long long)0, "0");
        W_TEST("0", (long long)0);
        R_TEST(tmax<long long>(), smax<long long>());
        W_TEST(smax<long long>(), tmax<long long>());
        R_TEST(tmin<long long>(), omin<long long>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<long long>(), omax<long long>(), cxon::read_error::integral_invalid, 0);
    // unsigned long long
        R_TEST((unsigned long long)42, "0b101010");
        R_TEST((unsigned long long)052, "052");
        R_TEST((unsigned long long)0x2A, "0x2A");
        R_TEST(tmin<unsigned long long>(), smin<unsigned long long>());
        W_TEST(smin<unsigned long long>(), tmin<unsigned long long>());
        R_TEST((unsigned long long)0xFFFFFFFF, "0xFFFFFFFF");
        W_TEST("4294967295", (unsigned long long)0xFFFFFFFF);
        R_TEST(tmax<unsigned long long>(), smax<unsigned long long>());
        W_TEST(smax<unsigned long long>(), tmax<unsigned long long>());
        R_TEST(tmax<unsigned long long>(), omax<unsigned long long>(), cxon::read_error::integral_invalid, 0);
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
#   if !defined(__clang__) // some older versions (ex. 3.8) crash
        R_TEST(tmin<long double>(), smin<long double>());
        W_TEST(smin<long double>(), tmin<long double>());
        R_TEST((long double)0, "0");
        W_TEST("0", (long double)0);
        R_TEST(tmax<long double>(), smax<long double>());
        W_TEST(smax<long double>(), tmax<long double>());
#   endif
    // nullptr_t
        R_TEST(nullptr, "null");
        W_TEST("null", nullptr);
        R_TEST(nullptr, "", cxon::read_error::unexpected, 0);
        R_TEST(nullptr, "nu", cxon::read_error::unexpected, 0);
        R_TEST(nullptr, "a", cxon::read_error::unexpected, 0);
    // T[]
        {   int a[] = {1, 2, 3};
            R_TEST(a, "{1,2,3}");
            W_TEST("{1,2,3}", a);
        }
        {   int a[] = {1, 2, 0};
            R_TEST(a, "{1,2}");
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "}", cxon::read_error::unexpected, 0);
            R_TEST(a, "{", cxon::read_error::integral_invalid, 1);
        }
        {   int a[] = {1, 2, 3};
            R_TEST(a, "{1,2,3,4}", cxon::read_error::unexpected, 6);
        }
    // char[]
        R_TEST("", QS(""));
        R_TEST("123", QS("123"));
        {   char a[] = {'1', '2', '3', '\0'};
            R_TEST(a, QS("123"));
            R_TEST(a, QS("123\\0"));
            W_TEST(QS("123"), a);
        }
        {   char a[] = {'1', '2', '3', '\0', '4', '\0'};
            R_TEST(a, QS("123\\0004"));
            W_TEST(QS("123\\04"), a);
        }
        {   char a[] = {'1', '2', '3'};
            R_TEST(a, QS("1234"), cxon::read_error::unexpected, 4);
            R_TEST(a, QS("12\\u2728"), cxon::read_error::unexpected, 3);
        }
        {   char a[] = {'1', '2', '\0'};
            R_TEST(a, "12", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"12", cxon::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), cxon::read_error::escape_invalid, 1);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", cxon::read_error::unexpected, 0);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "}", cxon::read_error::unexpected, 0);
            R_TEST(a, "{", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"", cxon::read_error::unexpected, 1);
        }
    // char16_t[]
        R_TEST(u"", QS(""));
        R_TEST(u"\xD83C\xDF7A\x2764x", QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x")); // \u0001F37A, beer; \u00002764, heart
        R_TEST(u"\x2764\xD83C\xDF7Ax", QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"));
        R_TEST(u"\x2764x\xD83C\xDF7A", QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"));
        {   char16_t a[] = {u'\xD83C', u'\xDF7A', u'\x2764', u'x', u'\0'};
            R_TEST(a, QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x"));
            W_TEST(QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x"), a);
        }
        {   char16_t a[] = {u'\x2764', u'\xD83C', u'\xDF7A', u'x', u'\0'};
            R_TEST(a, QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"));
            W_TEST(QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"), a);
        }
        {   char16_t a[] = {u'\x2764', u'x', u'\xD83C', u'\xDF7A', u'\0'};
            R_TEST(a, QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"));
            W_TEST(QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"), a);
        }
        {   char16_t a[] = {u'1', u'2', u'3', u'\0', u'4', u'\0'};
            R_TEST(a, QS("123\\0004"));
            W_TEST(QS("123\\04"), a);
        }
        {   char16_t a[] = {u'1', u'2', u'3'};
            R_TEST(a, QS("1234"), cxon::read_error::unexpected, 4);
            R_TEST(a, QS("12\xF0\x9F\x8D\xBA"), cxon::read_error::unexpected, 3);
        }
        {   char16_t a[] = {u'1', u'2', u'\0'};
            R_TEST(a, "12", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"12", cxon::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), cxon::read_error::escape_invalid, 1);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", cxon::read_error::unexpected, 0);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "}", cxon::read_error::unexpected, 0);
            R_TEST(a, "{", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"", cxon::read_error::unexpected, 1);
        }
    // char32_t[]
        R_TEST(U"", QS(""));
        R_TEST(U"\x01F37A\x2764x", QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x")); // \u0001F37A, beer; \u00002764, heart
        R_TEST(U"\x2764\x01F37Ax", QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"));
        R_TEST(U"\x2764x\x01F37A", QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"));
        {   char32_t a[] = {U'\x01F37A', U'\x2764', U'x', U'\0'};
            R_TEST(a, QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x"));
            W_TEST(QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x"), a);
        }
        {   char32_t a[] = {U'\x2764', U'\x01F37A', U'x', U'\0'};
            R_TEST(a, QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"));
            W_TEST(QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"), a);
        }
        {   char32_t a[] = {U'\x2764', U'x', U'\x01F37A', U'\0'};
            R_TEST(a, QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"));
            W_TEST(QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"), a);
        }
        {   char32_t a[] = {U'1', U'2', U'3', U'\0', U'4', U'\0'};
            R_TEST(a, QS("123\\0004"));
            W_TEST(QS("123\\04"), a);
        }
        {   char32_t a[] = {U'1', U'2', U'\x1F37A'};
            R_TEST(a, QS("12\xF0\x9F\x8D\xBA"));
            R_TEST(a, QS("1234"), cxon::read_error::unexpected, 4);
        }
        {   char32_t a[] = {U'1', U'2', U'\0'};
            R_TEST(a, "12", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"12", cxon::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), cxon::read_error::escape_invalid, 1);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", cxon::read_error::unexpected, 0);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "}", cxon::read_error::unexpected, 0);
            R_TEST(a, "{", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"", cxon::read_error::unexpected, 1);
        }
    // wchar_t[]
        R_TEST(L"", QS(""));
        {   wchar_t a[] = {L'1', L'2', L'3', L'\0', L'4', L'\0'};
            R_TEST(a, QS("123\\0004"));
            W_TEST(QS("123\\04"), a);
        }
        {   wchar_t a[] = {L'1', L'2', L'3'};
            R_TEST(a, QS("1234"), cxon::read_error::unexpected, 4);
        }
        {   wchar_t a[] = {L'1', L'2', L'\0'};
            R_TEST(a, "12", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"12", cxon::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), cxon::read_error::escape_invalid, 1);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", cxon::read_error::unexpected, 0);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "}", cxon::read_error::unexpected, 0);
            R_TEST(a, "{", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"", cxon::read_error::unexpected, 1);
        }
    // const char*
        R_TEST((const char*)"test", QS("test"));
        R_TEST((const char*)nullptr, "null");
        W_TEST(QS("test"), (const char*)"test");
        W_TEST("null", (const char*)nullptr);
        R_TEST((const char*)nullptr, "nil", cxon::read_error::unexpected, 1);
        R_TEST((const char*)nullptr, "\"nil", cxon::read_error::unexpected, 4);
    // const char16_t*
        R_TEST((const char16_t*)u"test", QS("test"));
        W_TEST(QS("test"), (const char16_t*)u"test");
        R_TEST((const char16_t*)nullptr, "\"", cxon::read_error::unexpected, 1);
    // const char32_t*
        R_TEST((const char32_t*)U"test", QS("test"));
        W_TEST(QS("test"), (const char32_t*)U"test");
        R_TEST((const char32_t*)nullptr, "\"", cxon::read_error::unexpected, 1);
    // const wchar_t*
        R_TEST((const wchar_t*)L"test", QS("test"));
        W_TEST(QS("test"), (const wchar_t*)L"test");
        R_TEST((const wchar_t*)nullptr, "\"", cxon::read_error::unexpected, 1);
    // char*
        R_TEST((char*)"test", QS("test"));
        W_TEST(QS("test"), (char*)"test");
        R_TEST((char*)nullptr, "null");
        W_TEST("null", (char*)nullptr);
    // char16_t*
        R_TEST((char16_t*)u"test", QS("test"));
        W_TEST(QS("test"), (char16_t*)u"test");
    // char32_t*
        R_TEST((char32_t*)U"test", QS("test"));
        W_TEST(QS("test"), (char32_t*)U"test");
    // wchar_t*
        R_TEST((wchar_t*)L"test", QS("test"));
        W_TEST(QS("test"), (wchar_t*)L"test");
    // std::basic_string<char> / std::string
        R_TEST(std::string("test"), QS("test"));
        W_TEST(QS("test"), std::string("test"));
        R_TEST(std::string("te\nst"), QS("te\\nst"));
        W_TEST(QS("te\\nst"), std::string("te\nst"));
        R_TEST(std::string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(std::string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(std::string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<char16_t> / std::u16string
        R_TEST(std::u16string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), std::u16string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(std::u16string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), std::u16string({0x6D4B, 0x8BD5}));
        R_TEST(std::u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), std::u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}));
        R_TEST(std::u16string(u"\xdbff\xdfff"), QS("\\udbff\\udfff")); // surrogate
        W_TEST(QS("\xf4\x8f\xbf\xbf"), std::u16string(u"\xdbff\xdfff")); // surrogate
        R_TEST(std::u16string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(std::u16string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(std::u16string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<char32_t> / std::u32string
        R_TEST(std::u32string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), std::u32string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(std::u32string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), std::u32string({0x6D4B, 0x8BD5}));
        R_TEST(std::u32string({0x0001241D, 0x0001241C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), std::u32string({0x0001241D, 0x0001241C}));
        R_TEST(std::u32string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(std::u32string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(std::u32string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<wchar_t> / std::wstring
        R_TEST(std::wstring({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), std::wstring({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(std::wstring({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), std::wstring({0x6D4B, 0x8BD5}));
        R_TEST(std::wstring(), "a", cxon::read_error::unexpected, 0);
        R_TEST(std::wstring(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(std::wstring(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
TEST_END()

TEST_BEG(cxon::JSON<>) // base
    using namespace base;
    // bool
        R_TEST(true, "true");
        W_TEST("true", true);
        R_TEST(false, "false");
        W_TEST("false", false);
        R_TEST(false, "t", cxon::read_error::boolean_invalid, 0);
        R_TEST(false, "f", cxon::read_error::boolean_invalid, 0);
        R_TEST(false, "", cxon::read_error::unexpected, 0);
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
        R_TEST('\0', QS("\xff"), cxon::read_error::character_invalid, 1); // invalid utf-8
        R_TEST('\0', QS("\\z"), cxon::read_error::escape_invalid, 1);
        R_TEST('\0', QS("\\u1111"), cxon::read_error::character_invalid, 1);
    // char16_t
        R_TEST(u'a', QS("a"));
        W_TEST(QS("a"), u'a');
        R_TEST(u'\x3B1', QS("\xCE\xB1"));
        W_TEST(QS("\xCE\xB1"), u'\x3B1');
        R_TEST(u'\xFFFF', QS("\xEF\xBF\xBF"));
        W_TEST(QS("\xEF\xBF\xBF"), u'\xFFFF');
        R_TEST(u'\0', QS("\xFF"), cxon::read_error::character_invalid, 1);
        R_TEST(u'\0', QS("\xF0\x90\x80\x80"), cxon::read_error::character_invalid, 1);
    // char32_t
        R_TEST(U'a', QS("a"));
        R_TEST(U'\0', QS("\x80"), cxon::read_error::character_invalid, 1);
        W_TEST(QS("a"), U'a');
        R_TEST(U'\x3B1', QS("\xCE\xB1"));
        R_TEST(U'\0', QS("\xCE"), cxon::read_error::character_invalid, 1);
        W_TEST(QS("\xCE\xB1"), U'\x3B1');
        R_TEST(U'\x389A', QS("\xE3\xA2\x9A"));
        R_TEST(U'\0', QS("\xE3\xA2"), cxon::read_error::character_invalid, 1);
        R_TEST(U'\0', QS("\xE3"), cxon::read_error::character_invalid, 1);
        W_TEST(QS("\xE3\xA2\x9A"), U'\x389A');
        R_TEST(U'\x28440', QS("\xF0\xA8\x91\x80"));
        R_TEST(U'\0', QS("\\udbff\\ue000"), cxon::read_error::surrogate_invalid, 1); // invalid surrogate
        R_TEST(U'\0', QS("\\udbff\\udbff"), cxon::read_error::surrogate_invalid, 1); // invalid surrogate
        R_TEST(U'\0', QS("\xF0\xA8\x91"), cxon::read_error::character_invalid, 1);
        R_TEST(U'\0', QS("\xF0\xA8"), cxon::read_error::character_invalid, 1);
        R_TEST(U'\0', QS("\xF0"), cxon::read_error::character_invalid, 1);
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
        R_TEST(tmin<signed char>(), omin<signed char>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<signed char>(), omax<signed char>(), cxon::read_error::integral_invalid, 0);
    // unsigned char
        R_TEST(tmin<unsigned char>(), smin<unsigned char>());
        W_TEST(smin<unsigned char>(), tmin<unsigned char>());
        R_TEST((unsigned char)0x7F, "127");
        W_TEST("127", (unsigned char)0x7F);
        R_TEST(tmax<unsigned char>(), smax<unsigned char>());
        W_TEST(smax<unsigned char>(), tmax<unsigned char>());
        R_TEST(tmax<unsigned char>(), omax<unsigned char>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<unsigned char>(), omax<unsigned char>(), cxon::read_error::integral_invalid, 0);
    // short
        R_TEST(tmin<short>(), smin<short>());
        W_TEST(smin<short>(), tmin<short>());
        R_TEST((short)0, "0");
        W_TEST("0", (short)0);
        R_TEST(tmax<short>(), smax<short>());
        W_TEST(smax<short>(), tmax<short>());
        R_TEST(tmin<short>(), omin<short>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<short>(), omax<short>(), cxon::read_error::integral_invalid, 0);
    // unsigned short
        R_TEST(tmin<unsigned short>(), smin<unsigned short>());
        W_TEST(smin<unsigned short>(), tmin<unsigned short>());
        R_TEST((unsigned short)0xFF, "255");
        W_TEST("255", (unsigned short)0xFF);
        R_TEST(tmax<unsigned short>(), smax<unsigned short>());
        W_TEST(smax<unsigned short>(), tmax<unsigned short>());
        R_TEST(tmax<unsigned short>(), omax<unsigned short>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<unsigned short>(), omax<unsigned short>(), cxon::read_error::integral_invalid, 0);
    // int
        R_TEST(tmin<int>(), smin<int>());
        W_TEST(smin<int>(), tmin<int>());
        R_TEST((int)0, "0");
        W_TEST("0", (int)0);
        R_TEST(tmax<int>(), smax<int>());
        W_TEST(smax<int>(), tmax<int>());
        R_TEST(tmin<int>(), omin<int>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<int>(), omax<int>(), cxon::read_error::integral_invalid, 0);
    // unsigned int
        R_TEST(tmin<unsigned int>(), smin<unsigned int>());
        W_TEST(smin<unsigned int>(), tmin<unsigned int>());
        R_TEST((unsigned int)0xFFFF, "65535");
        W_TEST("65535", (unsigned int)0xFFFF);
        R_TEST(tmax<unsigned int>(), smax<unsigned int>());
        W_TEST(smax<unsigned int>(), tmax<unsigned int>());
        R_TEST(tmax<unsigned int>(), omax<unsigned int>(), cxon::read_error::integral_invalid, 0);
    // long
        R_TEST(tmin<long>(), smin<long>());
        W_TEST(smin<long>(), tmin<long>());
        R_TEST((long)0, "0");
        W_TEST("0", (long)0);
        R_TEST(tmax<long>(), smax<long>());
        W_TEST(smax<long>(), tmax<long>());
        R_TEST(tmin<long>(), omin<long>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<long>(), omax<long>(), cxon::read_error::integral_invalid, 0);
    // unsigned long
        R_TEST(tmin<unsigned long>(), smin<unsigned long>());
        W_TEST(smin<unsigned long>(), tmin<unsigned long>());
        R_TEST((unsigned long)0xFFFF, "65535");
        W_TEST("65535", (unsigned long)0xFFFF);
        R_TEST(tmax<unsigned long>(), smax<unsigned long>());
        W_TEST(smax<unsigned long>(), tmax<unsigned long>());
        R_TEST(tmax<unsigned long>(), omax<unsigned long>(), cxon::read_error::integral_invalid, 0);
    // long long
        R_TEST(tmin<long long>(), smin<long long>());
        W_TEST(smin<long long>(), tmin<long long>());
        R_TEST((long long)0, "0");
        W_TEST("0", (long long)0);
        R_TEST(tmax<long long>(), smax<long long>());
        W_TEST(smax<long long>(), tmax<long long>());
        R_TEST(tmin<long long>(), omin<long long>(), cxon::read_error::integral_invalid, 0);
        R_TEST(tmax<long long>(), omax<long long>(), cxon::read_error::integral_invalid, 0);
    // unsigned long long
        R_TEST(tmin<unsigned long long>(), smin<unsigned long long>());
        W_TEST(smin<unsigned long long>(), tmin<unsigned long long>());
        R_TEST((unsigned long long)0xFFFFFFFF, "4294967295");
        W_TEST("4294967295", (unsigned long long)0xFFFFFFFF);
        R_TEST(tmax<unsigned long long>(), smax<unsigned long long>());
        W_TEST(smax<unsigned long long>(), tmax<unsigned long long>());
        R_TEST(tmax<unsigned long long>(), omax<unsigned long long>(), cxon::read_error::integral_invalid, 0);
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
#   if !defined(__clang__) // some older versions (ex. 3.8) crash
        R_TEST(tmin<long double>(), smin<long double>());
        W_TEST(smin<long double>(), tmin<long double>());
        R_TEST((long double)0, "0");
        W_TEST("0", (long double)0);
        R_TEST(tmax<long double>(), smax<long double>());
        W_TEST(smax<long double>(), tmax<long double>());
#   endif
    // nullptr_t
        R_TEST(nullptr, "null");
        W_TEST("null", nullptr);
        R_TEST(nullptr, "", cxon::read_error::unexpected, 0);
        R_TEST(nullptr, "nu", cxon::read_error::unexpected, 0);
        R_TEST(nullptr, "a", cxon::read_error::unexpected, 0);
    // T[]
        {   int a[] = {1, 2, 3};
            R_TEST(a, "[1,2,3]");
            W_TEST("[1,2,3]", a);
        }
        {   int a[] = {1, 2, 0};
            R_TEST(a, "[1,2]");
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "]", cxon::read_error::unexpected, 0);
            R_TEST(a, "[", cxon::read_error::integral_invalid, 1);
        }
        {   int a[] = {1, 2, 3};
            R_TEST(a, "[1,2,3,4]", cxon::read_error::unexpected, 6);
        }
    // char[]
        R_TEST("", QS(""));
        R_TEST("123", QS("123"));
        {   char a[] = {'1', '2', '3', '\0'};
            R_TEST(a, QS("123"));
            R_TEST(a, QS("123\\u0000"));
            W_TEST(QS("123"), a);
        }
        {   char a[] = {'1', '2', '3', '\0', '4', '\0'};
            R_TEST(a, QS("123\\u00004"));
            W_TEST(QS("123\\u00004"), a);
        }
        {   char a[] = {'1', '2', '3'};
            R_TEST(a, QS("1234"), cxon::read_error::unexpected, 4);
            R_TEST(a, QS("12\\u2728"), cxon::read_error::unexpected, 3);
        }
        {   char a[] = {'1', '2', '\0'};
            R_TEST(a, "12", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"12", cxon::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), cxon::read_error::escape_invalid, 1);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", cxon::read_error::unexpected, 0);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "}", cxon::read_error::unexpected, 0);
            R_TEST(a, "{", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"", cxon::read_error::unexpected, 1);
        }
    // char16_t[]
        R_TEST(u"", QS(""));
        R_TEST(u"\xdbff\xdfff", QS("\\udbff\\udfff")); // surrogate
        W_TEST(QS("\xf4\x8f\xbf\xbf"), u"\xdbff\xdfff"); // surrogate
        R_TEST(u"\xD83C\xDF7A\x2764x", QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x")); // \u0001F37A, beer; \u00002764, heart
        R_TEST(u"\x2764\xD83C\xDF7Ax", QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"));
        R_TEST(u"\x2764x\xD83C\xDF7A", QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"));
        {   char16_t a[] = {u'\xD83C', u'\xDF7A', u'\x2764', u'x', u'\0'};
            R_TEST(a, QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x"));
            W_TEST(QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x"), a);
        }
        {   char16_t a[] = {u'\x2764', u'\xD83C', u'\xDF7A', u'x', u'\0'};
            R_TEST(a, QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"));
            W_TEST(QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"), a);
        }
        {   char16_t a[] = {u'\x2764', u'x', u'\xD83C', u'\xDF7A', u'\0'};
            R_TEST(a, QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"));
            W_TEST(QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"), a);
        }
        {   char16_t a[] = {u'1', u'2', u'3', u'\0', u'4', u'\0'};
            R_TEST(a, QS("123\\u00004"));
            W_TEST(QS("123\\u00004"), a);
        }
        {   char16_t a[] = {u'1', u'2', u'3'};
            R_TEST(a, QS("1234"), cxon::read_error::unexpected, 4);
            R_TEST(a, QS("12\xF0\x9F\x8D\xBA"), cxon::read_error::unexpected, 3);
        }
        {   char16_t a[] = {u'1', u'2', u'\0'};
            R_TEST(a, "12", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"12", cxon::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), cxon::read_error::escape_invalid, 1);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", cxon::read_error::unexpected, 0);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "}", cxon::read_error::unexpected, 0);
            R_TEST(a, "{", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"", cxon::read_error::unexpected, 1);
        }
    // char32_t[]
        R_TEST(U"", QS(""));
        R_TEST(U"\x01F37A\x2764x", QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x")); // \u0001F37A, beer; \u00002764, heart
        R_TEST(U"\x2764\x01F37Ax", QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"));
        R_TEST(U"\x2764x\x01F37A", QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"));
        {   char32_t a[] = {U'\x01F37A', U'\x2764', U'x', U'\0'};
            R_TEST(a, QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x"));
            W_TEST(QS("\xF0\x9F\x8D\xBA\xE2\x9D\xA4x"), a);
        }
        {   char32_t a[] = {U'\x2764', U'\x01F37A', U'x', U'\0'};
            R_TEST(a, QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"));
            W_TEST(QS("\xE2\x9D\xA4\xF0\x9F\x8D\xBAx"), a);
        }
        {   char32_t a[] = {U'\x2764', U'x', U'\x01F37A', U'\0'};
            R_TEST(a, QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"));
            W_TEST(QS("\xE2\x9D\xA4x\xF0\x9F\x8D\xBA"), a);
        }
        {   char32_t a[] = {U'1', U'2', U'3', U'\0', U'4', U'\0'};
            R_TEST(a, QS("123\\u00004"));
            W_TEST(QS("123\\u00004"), a);
        }
        {   char32_t a[] = {U'1', U'2', U'\x1F37A'};
            R_TEST(a, QS("12\xF0\x9F\x8D\xBA"));
            R_TEST(a, QS("1234"), cxon::read_error::unexpected, 4);
        }
        {   char32_t a[] = {U'1', U'2', U'\0'};
            R_TEST(a, "12", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"12", cxon::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), cxon::read_error::escape_invalid, 1);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", cxon::read_error::unexpected, 0);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "}", cxon::read_error::unexpected, 0);
            R_TEST(a, "{", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"", cxon::read_error::unexpected, 1);
        }
    // wchar_t[]
        R_TEST(L"", QS(""));
        {   wchar_t a[] = {L'1', L'2', L'3', L'\0', L'4', L'\0'};
            R_TEST(a, QS("123\\u00004"));
            W_TEST(QS("123\\u00004"), a);
        }
        {   wchar_t a[] = {L'1', L'2', L'3'};
            R_TEST(a, QS("1234"), cxon::read_error::unexpected, 4);
        }
        {   wchar_t a[] = {L'1', L'2', L'\0'};
            R_TEST(a, "12", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"12", cxon::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), cxon::read_error::escape_invalid, 1);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", cxon::read_error::unexpected, 0);
            R_TEST(a, "", cxon::read_error::unexpected, 0);
            R_TEST(a, "}", cxon::read_error::unexpected, 0);
            R_TEST(a, "{", cxon::read_error::unexpected, 0);
            R_TEST(a, "\"", cxon::read_error::unexpected, 1);
        }
    // const char*
        R_TEST((const char*)"test", QS("test"));
        R_TEST((const char*)nullptr, "null");
        W_TEST(QS("test"), (const char*)"test");
        W_TEST("null", (const char*)nullptr);
        R_TEST((const char*)nullptr, "nil", cxon::read_error::unexpected, 1);
        R_TEST((const char*)nullptr, "\"nil", cxon::read_error::unexpected, 4);
    // const char16_t*
        R_TEST((const char16_t*)u"test", QS("test"));
        W_TEST(QS("test"), (const char16_t*)u"test");
        R_TEST((const char16_t*)nullptr, "\"", cxon::read_error::unexpected, 1);
    // const char32_t*
        R_TEST((const char32_t*)U"test", QS("test"));
        W_TEST(QS("test"), (const char32_t*)U"test");
        R_TEST((const char32_t*)nullptr, "\"", cxon::read_error::unexpected, 1);
    // const wchar_t*
        R_TEST((const wchar_t*)L"test", QS("test"));
        W_TEST(QS("test"), (const wchar_t*)L"test");
        R_TEST((const wchar_t*)nullptr, "\"", cxon::read_error::unexpected, 1);
    // char*
        R_TEST((char*)"test", QS("test"));
        R_TEST((char*)nullptr, "null");
        W_TEST(QS("test"), (char*)"test");
        W_TEST("null", (char*)nullptr);
    // char16_t*
        R_TEST((char16_t*)u"test", QS("test"));
        W_TEST(QS("test"), (char16_t*)u"test");
    // char32_t*
        R_TEST((char32_t*)U"test", QS("test"));
        W_TEST(QS("test"), (char32_t*)U"test");
    // wchar_t*
        R_TEST((wchar_t*)L"test", QS("test"));
        W_TEST(QS("test"), (wchar_t*)L"test");
    // std::basic_string<char> / std::string
        R_TEST(std::string("test"), QS("test"));
        W_TEST(QS("test"), std::string("test"));
        R_TEST(std::string("te\nst"), QS("te\\nst"));
        W_TEST(QS("te\\nst"), std::string("te\nst"));
        R_TEST(std::string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(std::string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(std::string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<char16_t> / std::u16string
        R_TEST(std::u16string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), std::u16string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(std::u16string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), std::u16string({0x6D4B, 0x8BD5}));
        R_TEST(std::u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), std::u16string({0xD809, 0xDC1D, 0xD809, 0xDC1C}));
        R_TEST(std::u16string(u"\xdbff\xdfff"), QS("\\udbff\\udfff")); // surrogate
        W_TEST(QS("\xf4\x8f\xbf\xbf"), std::u16string(u"\xdbff\xdfff")); // surrogate
        R_TEST(std::u16string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(std::u16string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(std::u16string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<char32_t> / std::u32string
        R_TEST(std::u32string({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), std::u32string({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(std::u32string({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), std::u32string({0x6D4B, 0x8BD5}));
        R_TEST(std::u32string({0x0001241D, 0x0001241C}), QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"));
        W_TEST(QS("\xF0\x92\x90\x9D\xF0\x92\x90\x9C"), std::u32string({0x0001241D, 0x0001241C}));
        R_TEST(std::u32string(), "a", cxon::read_error::unexpected, 0);
        R_TEST(std::u32string(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(std::u32string(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
    // std::basic_string<wchar_t> / std::wstring
        R_TEST(std::wstring({0x0442, 0x0435, 0x0441, 0x0442}), QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"));
        W_TEST(QS("\xD1\x82\xD0\xB5\xD1\x81\xD1\x82"), std::wstring({0x0442, 0x0435, 0x0441, 0x0442}));
        R_TEST(std::wstring({0x6D4B, 0x8BD5}), QS("\xE6\xB5\x8B\xE8\xAF\x95"));
        W_TEST(QS("\xE6\xB5\x8B\xE8\xAF\x95"), std::wstring({0x6D4B, 0x8BD5}));
        R_TEST(std::wstring(), "a", cxon::read_error::unexpected, 0);
        R_TEST(std::wstring(), "\"", cxon::read_error::unexpected, 1);
        R_TEST(std::wstring(), QS("\\u001"), cxon::read_error::escape_invalid, 1);
TEST_END()


namespace special {

    using namespace base;

    template <typename F> constexpr F inf() { return std::numeric_limits<F>::infinity(); }
    template <typename F> constexpr F nan() { return std::numeric_limits<F>::quiet_NaN(); }

    template <typename F, typename U> inline F bfp(U u) {
        static_assert(sizeof(F) == sizeof(U), "width different");
        union { F f; U u = 0; } a; a.u = u;
        return a.f;
    }

    template <typename F> static F zero() { return 0.0; }

}

TEST_BEG(cxon::CXON<>) // special numbers
    using namespace special;
    // float
        R_TEST( -inf<float>(), omin<float>());
        R_TEST( -inf<float>(), "-inf");
        W_TEST("-inf", -inf<float>());
        R_TEST(  inf<float>(), omax<float>());
        R_TEST(  inf<float>(), "inf");
        R_TEST(  inf<float>(), "+inf", cxon::read_error::floating_point_invalid, 0);
        W_TEST( "inf",  inf<float>());
        R_TEST(  nan<float>(), "nan");
        R_TEST(  nan<float>(), "+nan", cxon::read_error::floating_point_invalid, 0); 
        R_TEST(  nan<float>(), "-nan"/*, cxon::read_error::floating_point_invalid, 0*/); // TODO: disallow?
        W_TEST( "nan", nan<float>());
        W_TEST( "nan", bfp<float>(0x7fffffff));
        W_TEST( "nan", bfp<float>(0x7f80ffff));
        W_TEST( "nan", bfp<float>(0xffc00000));
        W_TEST( "nan", float(0.0 /  zero<float>()));
        W_TEST( "nan", float(0.0 / -zero<float>()));
        W_TEST( "nan", float(0.0 *  inf<float>()));
        W_TEST( "nan", float(0.0 * -inf<float>()));
        W_TEST( "nan", inf<float>() + -inf<float>());
    // double
        R_TEST( -inf<double>(), omin<double>());
        R_TEST( -inf<double>(), "-inf");
        W_TEST("-inf", -inf<double>());
        R_TEST(  inf<double>(), omax<double>());
        R_TEST(  inf<double>(), "inf");
        R_TEST(  inf<double>(), "+inf", cxon::read_error::floating_point_invalid, 0);
        W_TEST( "inf",  inf<double>());
        R_TEST(  nan<double>(), "nan");
        R_TEST(  nan<double>(), "+nan", cxon::read_error::floating_point_invalid, 0); 
        R_TEST(  nan<double>(), "-nan"/*, cxon::read_error::floating_point_invalid, 0*/); // TODO: disallow?
        W_TEST( "nan", nan<double>());
        W_TEST( "nan", bfp<double>(0x7fffffffffffffff));
        W_TEST( "nan", bfp<double>(0x7ff0ffffffffffff));
        W_TEST( "nan", bfp<double>(0xfff8000000000000));
        W_TEST( "nan", 0.0 /  zero<double>());
        W_TEST( "nan", 0.0 / -zero<double>());
        W_TEST( "nan", 0.0 *  inf<double>());
        W_TEST( "nan", 0.0 * -inf<double>());
        W_TEST( "nan", inf<double>() + -inf<double>());
    // long double
#   if !defined(__clang__) // some older versions (ex. 3.8) crash
        R_TEST( -inf<long double>(), omin<long double>());
        R_TEST( -inf<long double>(), "-inf");
        W_TEST("-inf", -inf<long double>());
        R_TEST(  inf<long double>(), omax<long double>());
        R_TEST(  inf<long double>(), "inf");
        R_TEST(  inf<long double>(), "+inf", cxon::read_error::floating_point_invalid, 0);
        W_TEST( "inf",  inf<long double>());
        R_TEST(  nan<long double>(), "nan");
        R_TEST(  nan<long double>(), "+nan", cxon::read_error::floating_point_invalid, 0); 
        R_TEST(  nan<long double>(), "-nan"/*, cxon::read_error::floating_point_invalid, 0*/); // TODO: disallow?
        W_TEST( "nan", nan<long double>());
        //W_TEST( "nan", bfp<long double>(0x));
        //W_TEST( "nan", bfp<long double>(0x));
        //W_TEST( "nan", bfp<long double>(0x));
        W_TEST( "nan", 0.0 /  zero<long double>());
        W_TEST( "nan", 0.0 / -zero<long double>());
        W_TEST( "nan", 0.0 *  inf<long double>());
        W_TEST( "nan", 0.0 * -inf<long double>());
        W_TEST( "nan", inf<long double>() + -inf<long double>());
#   endif
TEST_END()

TEST_BEG(cxon::JSON<>) // special numbers
    using namespace special;
    // float
        R_TEST( -inf<float>(), omin<float>());
        R_TEST( -inf<float>(), QS("-inf"));
        W_TEST(QS("-inf"), -inf<float>());
        R_TEST(  inf<float>(), omax<float>());
        R_TEST(  inf<float>(), QS("inf"));
        R_TEST(  inf<float>(), QS("+inf"), cxon::read_error::floating_point_invalid, 0);
        W_TEST(QS("inf"),  inf<float>());
        R_TEST(  nan<float>(), QS("nan"));
        R_TEST(  nan<float>(), QS("+nan"), cxon::read_error::floating_point_invalid, 0); 
        R_TEST(  nan<float>(), QS("-nan")/*, cxon::read_error::floating_point_invalid, 0*/); // TODO: disallow?
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
        R_TEST( -inf<double>(), omin<double>());
        R_TEST( -inf<double>(), QS("-inf"));
        W_TEST(QS("-inf"), -inf<double>());
        R_TEST(  inf<double>(), omax<double>());
        R_TEST(  inf<double>(), QS("inf"));
        R_TEST(  inf<double>(), QS("+inf"), cxon::read_error::floating_point_invalid, 0);
        W_TEST(QS("inf"),  inf<double>());
        R_TEST(  nan<double>(), QS("nan"));
        R_TEST(  nan<double>(), QS("+nan"), cxon::read_error::floating_point_invalid, 0); 
        R_TEST(  nan<double>(), QS("-nan")/*, cxon::read_error::floating_point_invalid, 0*/); // TODO: disallow?
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
#   if !defined(__clang__) // some older versions (ex. 3.8) crash
        R_TEST( -inf<long double>(), omin<long double>());
        R_TEST( -inf<long double>(), QS("-inf"));
        W_TEST(QS("-inf"), -inf<long double>());
        R_TEST(  inf<long double>(), omax<long double>());
        R_TEST(  inf<long double>(), QS("inf"));
        R_TEST(  inf<long double>(), QS("+inf"), cxon::read_error::floating_point_invalid, 0);
        W_TEST(QS("inf"),  inf<long double>());
        R_TEST(  nan<long double>(), QS("nan"));
        R_TEST(  nan<long double>(), QS("+nan"), cxon::read_error::floating_point_invalid, 0); 
        R_TEST(  nan<long double>(), QS("-nan")/*, cxon::read_error::floating_point_invalid, 0*/); // TODO: disallow?
        W_TEST(QS("nan"), nan<long double>());
        //W_TEST( QS("nan"), bfp<long double>(0x));
        //W_TEST( QS("nan"), bfp<long double>(0x));
        //W_TEST( QS("nan"), bfp<long double>(0x));
        W_TEST(QS("nan"), 0.0 /  zero<long double>());
        W_TEST(QS("nan"), 0.0 / -zero<long double>());
        W_TEST(QS("nan"), 0.0 *  inf<long double>());
        W_TEST(QS("nan"), 0.0 * -inf<long double>());
        W_TEST(QS("nan"), inf<long double>() + -inf<long double>());
#   endif
TEST_END()

TEST_BEG(cxon::CXON<base::force_input_iterator_traits>) // special numbers
    using namespace special;
    R_TEST(0.0, ".0");
    W_TEST("0", 0.0);
    R_TEST(10.0, "10");
    R_TEST(10.0, "10.0");
    R_TEST(10.0e+10, "10.0e+10");
    R_TEST(-inf<double>(), "-inf");
    W_TEST("-inf", -inf<double>());
    R_TEST(inf<double>(), "inf");
    W_TEST("inf", inf<double>());
    R_TEST(inf<double>(), "+inf", cxon::read_error::floating_point_invalid, 0);
    R_TEST(nan<double>(), "nan");
    W_TEST("nan", nan<double>());
TEST_END()

TEST_BEG(cxon::JSON<base::force_input_iterator_traits>) // special numbers
    using namespace special;
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
    R_TEST(inf<double>(), QS("+inf"), cxon::read_error::floating_point_invalid, 1);
    R_TEST(nan<double>(), QS("nan"));
    W_TEST(QS("nan"), nan<double>());
    R_TEST(nan<double>(), QS("-nan"));
    W_TEST(QS("nan"), -nan<double>());
TEST_END()


namespace number {
    struct strict_traits : cxon::format_traits {
        struct number {
            static constexpr bool strict = true;
        };
    };
}

TEST_BEG(cxon::CXON<base::force_input_iterator_traits>) // cxon number validation
    // floating point
        R_TEST((double)0, "0");
        R_TEST((double)10, "010");
        R_TEST((double)0, "-0");
        R_TEST((double)0, "0.0");
        R_TEST((double)0, "0.");
        R_TEST((double)0, ".0");
        R_TEST((double)1000, "1e3");
        R_TEST((double)1000, "1000000E-3");
        R_TEST((double)1000, "1e+3");
        R_TEST((double)1000, "1e+03");
        R_TEST((double)0, "", cxon::read_error::unexpected, 0);
        R_TEST((double)0, "+0", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, ".", cxon::read_error::floating_point_invalid, 1);
        R_TEST((double)0, "-", cxon::read_error::floating_point_invalid, 1);
        R_TEST((double)0, "+", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "e", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "0e", cxon::read_error::floating_point_invalid, 2);
        R_TEST((double)0, std::string(XXON::buffer::max_number + 1, '1'), cxon::read_error::floating_point_invalid, XXON::buffer::max_number);
    // integral
        // bin
        R_TEST((signed)0, "0b0");
        R_TEST((signed)2, "0b10");
        R_TEST((signed)-2, "-0b10");
        R_TEST((unsigned)0, "0b0");
        R_TEST((unsigned)2, "0b10");
        // oct
        R_TEST((signed)0, "0");
        R_TEST((signed)8, "010");
        R_TEST((signed)-8, "-010");
        R_TEST((signed)7, "07");
        R_TEST((signed)-7, "-07");
        R_TEST((unsigned)0, "0");
        R_TEST((unsigned)8, "010");
        // dec
        R_TEST((signed)0, "0");
        R_TEST((signed)10, "10");
        R_TEST((signed)-10, "-10");
        R_TEST((unsigned)0, "0");
        R_TEST((unsigned)10, "10");
        // hex
        R_TEST((signed)0, "0x0");
        R_TEST((signed)16, "0x10");
        R_TEST((signed)-16, "-0x10");
        R_TEST((signed)255, "0xff");
        R_TEST((signed)-255, "-0xff");
        R_TEST((unsigned)0, "0x0");
        R_TEST((unsigned)16, "0x10");
        // invalid
        R_TEST((signed)0, "", cxon::read_error::unexpected, 0);
        R_TEST((signed)0, "+0", cxon::read_error::integral_invalid, 0);
        R_TEST((unsigned)0, "-0", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, ".", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, "-", cxon::read_error::integral_invalid, 1);
        R_TEST((signed)0, "+", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, "e", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, "0b", cxon::read_error::integral_invalid, 2);
        R_TEST((signed)0, "0b2", cxon::read_error::integral_invalid, 2);
        R_TEST((signed)0, "08", cxon::read_error::ok, 1);
        R_TEST((signed)0, "0x", cxon::read_error::integral_invalid, 2);
        R_TEST((signed)0, "0xg", cxon::read_error::integral_invalid, 2);
        R_TEST((signed)0, std::string(XXON::buffer::max_number + 1, '1'), cxon::read_error::integral_invalid, XXON::buffer::max_number);
TEST_END()

TEST_BEG(cxon::JSON<base::force_input_iterator_traits>) // json number validation
    // floating point
        R_TEST((double)0, "0");
        R_TEST((double)0, "-0");
        R_TEST((double)0, "0.0");
        R_TEST((double)1000, "1e3");
        R_TEST((double)1000, "1000000E-3");
        R_TEST((double)1000, "1e+3");
        R_TEST((double)1000, "1e+03");
        R_TEST((double)0, "", cxon::read_error::unexpected, 0);
        R_TEST((double)0, "+0", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "01", cxon::read_error::ok, 1);
        R_TEST((double)0, "0.", cxon::read_error::floating_point_invalid, 2);
        R_TEST((double)0, ".0", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, ".", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "-", cxon::read_error::floating_point_invalid, 1);
        R_TEST((double)0, "+", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "e", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "0e", cxon::read_error::ok, 1);
        R_TEST((double)0, std::string(XXON::buffer::max_number + 1, '1'), cxon::read_error::floating_point_invalid, XXON::buffer::max_number);
    // integral
        W_TEST("0", (signed)0);
        R_TEST((signed)0, "0");
        R_TEST((signed)10, "10");
        R_TEST((signed)-10, "-10");
        R_TEST((signed)0, "", cxon::read_error::unexpected, 0);
        R_TEST((signed)0, "+0", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, ".", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, "-", cxon::read_error::integral_invalid, 1);
        R_TEST((signed)0, "+", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, "e", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, "0b", cxon::read_error::ok, 1);
        R_TEST((signed)0, "0b2", cxon::read_error::ok, 1);
        R_TEST((signed)0, "08", cxon::read_error::ok, 1);
        R_TEST((signed)0, "0x", cxon::read_error::ok, 1);
        R_TEST((signed)0, "0xg", cxon::read_error::ok, 1);
        R_TEST((signed)0, std::string(XXON::buffer::max_number + 1, '1'), cxon::read_error::integral_invalid, XXON::buffer::max_number);
TEST_END()

TEST_BEG(cxon::JSON<number::strict_traits>) // json number validation
    // floating point
        W_TEST("0", (double)0);
        R_TEST((double)0, "0");
        R_TEST((double)10, "10");
        R_TEST((double)0, "-0");
        R_TEST((double)0, "0.0");
        R_TEST((double)1000, "1e3");
        R_TEST((double)1000, "1000000E-3");
        R_TEST((double)1000, "1e+3");
        R_TEST((double)1000, "1e+03");
        R_TEST((double)0, "", cxon::read_error::unexpected, 0);
        R_TEST((double)0, "+0", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "01", cxon::read_error::ok, 1);
        R_TEST((double)0, "0.", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, ".0", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, ".", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "-", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "+", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "e", cxon::read_error::floating_point_invalid, 0);
        R_TEST((double)0, "0e", cxon::read_error::ok, 1);
        using namespace special;
        R_TEST(-inf<double>(), QS("-inf"));
        R_TEST( inf<double>(), QS("inf"));
        R_TEST( inf<double>(), QS("+inf"), cxon::read_error::floating_point_invalid, 0);
        R_TEST( nan<double>(), QS("nan"));
        R_TEST( nan<double>(), QS("-nan"));
        W_TEST(QS("inf"), inf<double>());
        W_TEST(QS("nan"), nan<double>());
    // integral
        W_TEST("0", (signed)0);
        R_TEST((signed)0, "0");
        R_TEST((signed)10, "10");
        R_TEST((signed)-10, "-10");
        R_TEST((signed)0, "", cxon::read_error::unexpected, 0);
        R_TEST((signed)0, "+0", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, ".", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, "-", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, "+", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, "e", cxon::read_error::integral_invalid, 0);
        R_TEST((signed)0, "0b", cxon::read_error::ok, 1);
        R_TEST((signed)0, "0b2", cxon::read_error::ok, 1);
        R_TEST((signed)0, "08", cxon::read_error::ok, 1);
        R_TEST((signed)0, "0x", cxon::read_error::ok, 1);
        R_TEST((signed)0, "0xg", cxon::read_error::ok, 1);
TEST_END()


namespace string_quote {
    struct single_traits : cxon::format_traits {
        struct string {
            static constexpr char beg = '\'';
            static constexpr char end = '\'';
        };
    };
}

TEST_BEG(cxon::CXON<string_quote::single_traits>) // cxon string quoting
    R_TEST(std::u32string(U"\x2Atest\052"), "'\\x2Atest\\052'");
    W_TEST("'test\u002A\\\\'", std::u32string(U"test\U0000002A\\"));
    R_TEST(std::u32string(U"'test'"), "'\\\'test\\\''");
    W_TEST("'\\\'test\\\''", std::u32string(U"'test'"));
    R_TEST(std::u32string(U"\"test\""), "'\"test\"'");
    W_TEST("'\"test\"'", std::u32string(U"\"test\""));
    R_TEST(std::u32string(U"\'"), "'\\''");
    R_TEST(std::u32string(U"\""), "'\\\"'");
    R_TEST(std::u32string(U"?"), "'\\?'");
    R_TEST(std::u32string(U"\\"), "'\\\\'");
    R_TEST(std::u32string(U"\a"), "'\\a'");
    R_TEST(std::u32string(U"\b"), "'\\b'");
    R_TEST(std::u32string(U"\f"), "'\\f'");
    R_TEST(std::u32string(U"\n"), "'\\n'");
    R_TEST(std::u32string(U"\r"), "'\\r'");
    R_TEST(std::u32string(U"\t"), "'\\t'");
    R_TEST(std::u32string(U"\v"), "'\\v'");
    R_TEST(std::u32string(U"\u002A"), "'\\u002A'");
    R_TEST(std::u32string(U"\U0000002A"), "'\\U0000002A'");
    R_TEST(std::u32string(U"\U0010ffff"), "'\\udbff\\udfff'"); // surrogate
    R_TEST(std::u32string(), "'\\udbff\\uffff'", cxon::read_error::surrogate_invalid, 1); // invalid surrogate
    R_TEST(std::u32string(), "'\\z'", cxon::read_error::escape_invalid, 1);
    R_TEST(std::u32string(), "'", cxon::read_error::unexpected, 1);
    R_TEST(std::u32string(), "x", cxon::read_error::unexpected, 0);
    R_TEST(std::u32string(), "'''", cxon::read_error::ok, 2); // trailing '
    R_TEST(std::u32string({0x0442, 0x0435, 0x0441, 0x0442}), "'\xD1\x82\xD0\xB5\xD1\x81\xD1\x82'");
    W_TEST("'\xD1\x82\xD0\xB5\xD1\x81\xD1\x82'", std::u32string({0x0442, 0x0435, 0x0441, 0x0442}));
    R_TEST(std::u32string({0x6D4B, 0x8BD5}), "'\xE6\xB5\x8B\xE8\xAF\x95'");
    W_TEST("'\xE6\xB5\x8B\xE8\xAF\x95'", std::u32string({0x6D4B, 0x8BD5}));
    R_TEST(std::u32string({0x0001241D, 0x0001241C}), "'\xF0\x92\x90\x9D\xF0\x92\x90\x9C'");
    W_TEST("'\xF0\x92\x90\x9D\xF0\x92\x90\x9C'", std::u32string({0x0001241D, 0x0001241C}));
    R_TEST(std::u32string(), "a", cxon::read_error::unexpected, 0);
    R_TEST(std::u32string(), "'\\u001'", cxon::read_error::escape_invalid, 1);
TEST_END()


namespace key {
    template <typename X, bool Q> struct unquoted : X::traits {
        struct map : X::traits::map {
            static constexpr bool unquoted_keys = Q;
        };
    };
    struct less_cstr {
        bool operator ()(const char* k0, const char* k1) const { return (!k0 && k1) || (k0 && k1 && std::strcmp(k0, k1) < 0); }
    };
}

TEST_BEG(cxon::CXON<>) // cxon
    using namespace std;
    // escapes
        R_TEST("\'", QS("\\\'"));
        R_TEST("\"", QS("\\\""));
        R_TEST("\?", QS("\\\?"));
        R_TEST("\\", QS("\\\\"));
        R_TEST("\a", QS("\\a"));
        R_TEST("\b", QS("\\b"));
        R_TEST("\f", QS("\\f"));
        R_TEST("\n", QS("\\n"));
        R_TEST("\r", QS("\\r"));
        R_TEST("\t", QS("\\t"));
        R_TEST("\v", QS("\\v"));
        R_TEST("\7", QS("\\7"));
        R_TEST("\07", QS("\\07"));
        R_TEST("\007", QS("\\007"));
        R_TEST("\077", QS("\\077"));
        R_TEST("\117", QS("\\117"));
        R_TEST("\0007", QS("\\0007")); // one more
        R_TEST("", QS("\\ "), cxon::read_error::escape_invalid, 1); // none
        R_TEST(std::string(), QS("\\"), cxon::read_error::unexpected, 3); // escaped string end
        R_TEST("\x8", QS("\\x8"));
        R_TEST("\x08", QS("\\x08"));
        R_TEST("\x16", QS("\\x16"));
        R_TEST("\x3f", QS("\\x3f"));
        R_TEST("\000f", QS("\\x00f")); // one more
        R_TEST("", QS("\\x"), cxon::read_error::escape_invalid, 1); // none
        R_TEST("\u0008", QS("\\u0008"));
        R_TEST("\0008", QS("\\u00008")); // one more
        R_TEST("", QS("\\u008"), cxon::read_error::escape_invalid, 1); // one less
        R_TEST("", QS("\\u"), cxon::read_error::escape_invalid, 1); // none
        R_TEST("\U00000008", QS("\\U00000008"));
        R_TEST("\0008", QS("\\U000000008")); // one more
        R_TEST("", QS("\\U0000008"), cxon::read_error::escape_invalid, 1); // one less
        R_TEST("", QS("\\U"), cxon::read_error::escape_invalid, 1); // none
        R_TEST("\xf4\x8f\xbf\xbf", QS("\\udbff\\udfff")); // surrogate
        R_TEST("", QS("\\udbff\\ue000"), cxon::read_error::surrogate_invalid, 1); // invalid surrogate
        R_TEST("", QS("\\udbff\\udbff"), cxon::read_error::surrogate_invalid, 1); // invalid surrogate
        R_TEST("", QS("\\udbff"), cxon::read_error::surrogate_invalid, 1); // invalid surrogate
        W_TEST(QS("\\1"), "\1");
        W_TEST(QS("\\2"), "\2");
        W_TEST(QS("\\3"), "\3");
        W_TEST(QS("\\4"), "\4");
        W_TEST(QS("\\5"), "\5");
        W_TEST(QS("\\6"), "\6");
        W_TEST(QS("\\7"), "\7");
        W_TEST(QS("\\b"), "\10");
        W_TEST(QS("\\t"), "\11");
        W_TEST(QS("\\n"), "\12");
        W_TEST(QS("\\v"), "\13");
        W_TEST(QS("\\f"), "\14");
        W_TEST(QS("\\r"), "\15");
        W_TEST(QS("\\16"), "\16");
        W_TEST(QS("\\17"), "\17");
        W_TEST(QS("\\20"), "\20");
        W_TEST(QS("\\21"), "\21");
        W_TEST(QS("\\22"), "\22");
        W_TEST(QS("\\23"), "\23");
        W_TEST(QS("\\24"), "\24");
        W_TEST(QS("\\25"), "\25");
        W_TEST(QS("\\26"), "\26");
        W_TEST(QS("\\27"), "\27");
        W_TEST(QS("\\30"), "\30");
        W_TEST(QS("\\31"), "\31");
        W_TEST(QS("\\32"), "\32");
        W_TEST(QS("\\33"), "\33");
        W_TEST(QS("\\34"), "\34");
        W_TEST(QS("\\35"), "\35");
        W_TEST(QS("\\36"), "\36");
        W_TEST(QS("\\37"), "\37");
        W_TEST(QS("\\\""), "\42");
    // std::tuple<int, double, std::string>
        R_TEST((tuple<int, double, string>{0, 0, "0"}), "{0, 0, \"0\"}");
        W_TEST("{0,0,\"0\"}", (tuple<int, double, string>{0, 0, "0"}));
        R_TEST((tuple<int, double, string>{}), "{0, 0}", cxon::read_error::unexpected, 5);
        R_TEST((tuple<int, double, string>{}), "{0, 0, \"\", 0}", cxon::read_error::unexpected, 9);
        R_TEST((tuple<int, double, string>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((tuple<int, double, string>{}), "}", cxon::read_error::unexpected, 0);
        R_TEST((tuple<int, double, string>{}), "{", cxon::read_error::integral_invalid, 1);
        R_TEST((tuple<int, double, string>{}), "{0, x}", cxon::read_error::floating_point_invalid, 4);
    // std::tuple<int, double>
        R_TEST((tuple<int, double>{}), "{0, 0, \"0\"}", cxon::read_error::unexpected, 5);
        R_TEST((tuple<int, double>{}), "{x}", cxon::read_error::integral_invalid, 1);
    // std::pair<int, std::string>
        R_TEST((pair<int, string>{0, "0"}), "{0, \"0\"}");
        W_TEST("{0,\"0\"}", (pair<int, string>{0, "0"}));
        R_TEST((pair<int, string>{}), "{x}", cxon::read_error::integral_invalid, 1);
        R_TEST((pair<int, string>{}), "{0}", cxon::read_error::unexpected, 2);
        R_TEST((pair<int, string>{}), "{0, \"\", 0}", cxon::read_error::unexpected, 6);
        R_TEST((pair<int, string>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((pair<int, string>{}), "}", cxon::read_error::unexpected, 0);
        R_TEST((pair<int, string>{}), "{", cxon::read_error::integral_invalid, 1);
    // std::valarray<int>
        R_TEST((valarray<int>{1, 2, 3}), "{1, 2, 3}");
        W_TEST("{1,2,3}", (valarray<int>{1, 2, 3}));
        R_TEST((valarray<int>{1, 2, 3, 4}), "{1, 2, 3, 4}");
        R_TEST((valarray<int>{1, 2, 3, 4, 5}), "{1, 2, 3, 4, 5}");
        R_TEST((valarray<int>{1, 2, 3, 4, 5, 6, 7, 8}), "{1, 2, 3, 4, 5, 6, 7, 8}");
        R_TEST((valarray<int>{1, 2, 3, 4, 5, 6, 7, 8, 9}), "{1, 2, 3, 4, 5, 6, 7, 8, 9}");
        R_TEST((valarray<int>()), "", cxon::read_error::unexpected, 0);
        R_TEST((valarray<int>()), "}", cxon::read_error::unexpected, 0);
        R_TEST((valarray<int>()), "{", cxon::read_error::integral_invalid, 1);
        R_TEST((valarray<int>()), "{x", cxon::read_error::integral_invalid, 1);
    // std::array<int, 0>
        R_TEST((array<int, 0>{}), "{}");
        W_TEST("{}", (array<int, 0>{}));
        R_TEST((array<int, 0>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((array<int, 0>{}), "}", cxon::read_error::unexpected, 0);
        R_TEST((array<int, 0>{}), "{", cxon::read_error::unexpected, 1);
    // std::array<int, 3>
        R_TEST((array<int, 3>{{1, 2, 3}}), "{1, 2, 3}");
        W_TEST("{1,2,3}", (array<int, 3>{1, 2, 3}));
        R_TEST((array<int, 4>{{1, 2, 3, 0}}), "{1, 2, 3}");
        R_TEST((array<int, 2>{}), "{1, 2, 3}", cxon::read_error::unexpected, 5);
        R_TEST((array<int, 2>{}), "{x}", cxon::read_error::integral_invalid, 1);
        R_TEST((array<int, 2>{}), "{1, x}", cxon::read_error::integral_invalid, 4);
    // std::queue<int>
        R_TEST((queue<int>{}), "{}");
        W_TEST("{}", (queue<int>{}));
        R_TEST((queue<int>({1, 2, 3})), "{1, 2, 3}");
        W_TEST("{1,2,3}", (queue<int>({1, 2, 3})));
        R_TEST(queue<int>{}, "{1, x}", cxon::read_error::integral_invalid, 4);
    // std::priority_queue<int>
        R_TEST((priority_queue<int>{}), "{}");
        W_TEST("{}", (priority_queue<int>{}));
        R_TEST(priority_queue<int>(less<int>(), {1, 2, 3}), "{1, 2, 3}");
        W_TEST("{3,2,1}", priority_queue<int>(less<int>(), {1, 2, 3}));
        R_TEST(priority_queue<int>{}, "{1, x}", cxon::read_error::integral_invalid, 4);
    // std::stack<int>
        R_TEST((stack<int>{}), "{}");
        W_TEST("{}", (stack<int>{}));
        R_TEST((stack<int>({1, 2, 3})), "{1, 2, 3}");
        W_TEST("{3,2,1}", (stack<int>({1, 2, 3})));
    // std::deque<int>
        R_TEST((deque<int>{}), "{}");
        W_TEST("{}", (deque<int>{}));
        R_TEST((deque<int>({1, 2, 3})), "{1, 2, 3}");
        W_TEST("{1,2,3}", (deque<int>({1, 2, 3})));
    // std::list<int>
        R_TEST((list<int>{}), "{}");
        W_TEST("{}", (list<int>{}));
        R_TEST((list<int>({1, 2, 3})), "{1, 2, 3}");
        W_TEST("{1,2,3}", (list<int>({1, 2, 3})));
    // std::vector<int>
        R_TEST((vector<int>{}), "{}");
        W_TEST("{}", (vector<int>{}));
        R_TEST((vector<int>({1, 2, 3})), "{1, 2, 3}");
        W_TEST("{1,2,3}", (vector<int>({1, 2, 3})));
        R_TEST((vector<int>()), "", cxon::read_error::unexpected, 0);
        R_TEST((vector<int>()), "}", cxon::read_error::unexpected, 0);
        R_TEST((vector<int>()), "{", cxon::read_error::integral_invalid, 1);
        R_TEST((vector<float>()), "", cxon::read_error::unexpected, 0);
        R_TEST((vector<float>()), "}", cxon::read_error::unexpected, 0);
        R_TEST((vector<float>()), "{", cxon::read_error::floating_point_invalid, 1);
    // std::set<int>;
        R_TEST((set<int>{}), "{}");
        W_TEST("{}", (set<int>{}));
        R_TEST((set<int>({1, 2, 3})), "{1, 1, 2, 3}");
        W_TEST("{1,2,3}", (set<int>({1, 2, 3})));
        R_TEST(set<int>{}, "{1, x}", cxon::read_error::integral_invalid, 4);
    // std::multiset<int>;
        R_TEST((multiset<int>{}), "{}");
        W_TEST("{}", (multiset<int>{}));
        R_TEST((multiset<int>({1, 1, 2, 3})), "{1, 1, 2, 3}");
        W_TEST("{1,1,2,3}", (multiset<int>({1, 1, 2, 3})));
    // std::unordered_set<int>;
        R_TEST((unordered_set<int>{}), "{}");
        W_TEST("{}", (unordered_set<int>{}));
        R_TEST((unordered_set<int>({1, 2, 3})), "{1, 1, 2, 3}");
        W_TEST("{1}", (unordered_set<int>({1})));
    // std::unordered_multiset<int>;
        R_TEST((unordered_multiset<int>{}), "{}");
        W_TEST("{}", (unordered_multiset<int>{}));
        R_TEST((unordered_multiset<int>({1, 1, 2, 3})), "{1, 1, 2, 3}");
        W_TEST("{1,1}", (unordered_multiset<int>({1, 1})));
    // std::vector<std::list<int>>
        R_TEST((vector<list<int>>{}), "{}");
        W_TEST("{}", (vector<list<int>>{}));
        R_TEST((vector<list<int>>{{}}), "{{}}");
        W_TEST("{{}}", (vector<list<int>>{{}}));
        R_TEST((vector<list<int>>{{1, 2, 3}, {3, 2, 1}}), "{{1, 2, 3}, {3, 2, 1}}");
        W_TEST("{{1,2,3},{3,2,1}}", (vector<list<int>>{{1, 2, 3}, {3, 2, 1}}));
    // std::list<std::vector<int>>
        R_TEST((list<vector<int>>{}), "{}");
        W_TEST("{}", (list<vector<int>>{}));
        R_TEST((list<vector<int>>{{}}), "{{}}");
        W_TEST("{{}}", (list<vector<int>>{{}}));
        R_TEST((list<vector<int>>{{1, 2, 3}, {3, 2, 1}}), "{{1, 2, 3}, {3, 2, 1}}");
        W_TEST("{{1,2,3},{3,2,1}}", (list<vector<int>>{{1, 2, 3}, {3, 2, 1}}));
TEST_END()


TEST_BEG(cxon::CXON<key::unquoted<cxon::CXON<>, false>>)
    using namespace std;
    // std::map<int, std::string>
        R_TEST((map<int, string>{}), "{}");
        W_TEST("{}", (map<int, string>{}));
        R_TEST((map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}), "{1 : \"1\" , 1 : \"1\" , 2 : \"2\" , 3 : \"3\"}");
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}));
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, const char*>{{1, "1"}, {2, "2"}, {3, "3"}}));
        R_TEST((map<int, string>{}), "{x}", cxon::read_error::integral_invalid, 1);
        R_TEST((map<int, string>{}), "{1: x}", cxon::read_error::unexpected, 4);
    // std::map<std::string, int>
        R_TEST((map<string, int>{}), "{}");
        W_TEST("{}", (map<string, int>{}));
        R_TEST((map<string, int>{{"1", 1}, {"2", 2}, {" 3 ", 3}}), "{\"1\": 1, \"1\": 1, \"2\": 2, \" 3 \": 3}");
        W_TEST("{\"1\":1,\"2\":2,\"3\":3}", (map<string, int>{{"1", 1}, {"2", 2}, {"3", 3}}));
        W_TEST("{\"1\":1,\"2\":2,\"3\":3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<int, int>
        R_TEST((map<int, int>{}), "{}");
        W_TEST("{}", (map<int, int>{}));
        R_TEST((map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,2:2,3:3}", (map<int, int>{{1, 1}, {2, 2}, {3, 3}}));
    // std::multimap<int, int>
        R_TEST((multimap<int, int>{}), "{}");
        W_TEST("{}", (multimap<int, int>{}));
        R_TEST((multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,1:1,2:2,3:3}", (multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}));
    // std::unordered_map<int, int>
        R_TEST((unordered_map<int, int>{}), "{}");
        W_TEST("{}", (unordered_map<int, int>{}));
        R_TEST((unordered_map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1}", (unordered_map<int, int>{{1, 1}, {1, 1}}));
    // std::unordered_multimap<int, int>
        R_TEST((unordered_multimap<int, int>{}), "{}");
        W_TEST("{}", (unordered_multimap<int, int>{}));
        R_TEST((unordered_multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,1:1}", (unordered_multimap<int, int>{{1, 1}, {1, 1}}));
TEST_END()

TEST_BEG(cxon::CXON<key::unquoted<cxon::CXON<>, true>>)
    using namespace std;
    // std::map<int, std::string>
        R_TEST((map<int, string>{}), "{}");
        W_TEST("{}", (map<int, string>{}));
        R_TEST((map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}), "{1 : \"1\" , 1 : \"1\" , 2 : \"2\" , 3 : \"3\"}");
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}));
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, const char*>{{1, "1"}, {2, "2"}, {3, "3"}}));
        R_TEST((map<int, string>{}), "{x}", cxon::read_error::integral_invalid, 1);
        R_TEST((map<int, string>{}), "{1: x}", cxon::read_error::unexpected, 4);
    // std::map<std::string, int>
        R_TEST((map<string, int>{}), "{}");
        W_TEST("{}", (map<string, int>{}));
        R_TEST((map<string, int>{{"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<string, int>{{"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<string, int>{{"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<string, int>{{"1", 1}, {"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<string, int>{{"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<string, int>{{"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<string, int>{{"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<string, int>{{"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<string, int>{{"1", 1}, {"2", 2}, {"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
        W_TEST("{null:0,1:1,2:2}", (map<const char*, int, key::less_cstr>{{nullptr, 0}, {"1", 1}, {"2", 2}}));
    // std::map<std::u16string, int>
        R_TEST((map<u16string, int>{}), "{}");
        W_TEST("{}", (map<u16string, int>{}));
        R_TEST((map<u16string, int>{{u"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<u16string, int>{{u"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<u16string, int>{{u"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<u16string, int>{{u"1", 1}, {u"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<u16string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<u16string, int>{{u"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<u16string, int>{{u"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<u16string, int>{{u"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<u16string, int>{{u"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<u16string, int>{{u"1", 1}, {u"2", 2}, {u"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<std::u32string, int>
        R_TEST((map<u32string, int>{}), "{}");
        W_TEST("{}", (map<u32string, int>{}));
        R_TEST((map<u32string, int>{{U"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<u32string, int>{{U"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<u32string, int>{{U"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<u32string, int>{{U"1", 1}, {U"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<u32string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<u32string, int>{{U"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<u32string, int>{{U"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<u32string, int>{{U"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<u32string, int>{{U"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<u32string, int>{{U"1", 1}, {U"2", 2}, {U"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<std::wstring, int>
        R_TEST((map<wstring, int>{}), "{}");
        W_TEST("{}", (map<wstring, int>{}));
        R_TEST((map<wstring, int>{{L"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<wstring, int>{{L"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<wstring, int>{{L"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<wstring, int>{{L"1", 1}, {L"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<wstring, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<wstring, int>{{L"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<wstring, int>{{L"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<wstring, int>{{L"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<wstring, int>{{L"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<wstring, int>{{L"1", 1}, {L"2", 2}, {L"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<int, int>
        R_TEST((map<int, int>{}), "{}");
        W_TEST("{}", (map<int, int>{}));
        R_TEST((map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,2:2,3:3}", (map<int, int>{{1, 1}, {2, 2}, {3, 3}}));
TEST_END()


TEST_BEG(cxon::JSON<key::unquoted<cxon::JSON<>, false>>)
    using namespace std;
    // std::map<int, std::string>
        R_TEST((map<int, string>{}), "{}");
        W_TEST("{}", (map<int, string>{}));
        R_TEST((map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}), "{\"1\" : \"1\" , \"1\" : \"1\" , \"2\" : \"2\" , \"3\" : \"3\"}");
        W_TEST("{\"1\":\"1\",\"2\":\"2\",\"3\":\"3\"}", (map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}));
        W_TEST("{\"1\":\"1\",\"2\":\"2\",\"3\":\"3\"}", (map<int, const char*>{{1, "1"}, {2, "2"}, {3, "3"}}));
        R_TEST((map<int, string>{}), "{\"x\"}", cxon::read_error::integral_invalid, 2);
        R_TEST((map<int, string>{}), "{\"1\": x}", cxon::read_error::unexpected, 6);
    // std::map<std::string, int>
        R_TEST((map<string, int>{}), "{}");
        W_TEST("{}", (map<string, int>{}));
        R_TEST((map<string, int>{{"1", 1}, {"2", 2}, {" 3 ", 3}}), "{\"1\": 1, \"1\": 1, \"2\": 2, \" 3 \": 3}");
        W_TEST("{\"1\":1,\"2\":2,\"3\":3}", (map<string, int>{{"1", 1}, {"2", 2}, {"3", 3}}));
        W_TEST("{\"1\":1,\"2\":2,\"3\":3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<int, int>
        R_TEST((map<int, int>{}), "{}");
        W_TEST("{}", (map<int, int>{}));
        R_TEST((map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{\"1\" : 1 , \"1\" : 1 , \"2\" : 2 , \"3\" : 3}");
        W_TEST("{\"1\":1,\"2\":2,\"3\":3}", (map<int, int>{{1, 1}, {2, 2}, {3, 3}}));
    // std::multimap<int, int>
        R_TEST((multimap<int, int>{}), "{}");
        W_TEST("{}", (multimap<int, int>{}));
        R_TEST((multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{\"1\" : 1 , \"1\" : 1 , \"2\" : 2 , \"3\" : 3}");
        W_TEST("{\"1\":1,\"1\":1,\"2\":2,\"3\":3}", (multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}));
    // std::unordered_map<int, int>
        R_TEST((unordered_map<int, int>{}), "{}");
        W_TEST("{}", (unordered_map<int, int>{}));
        R_TEST((unordered_map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{\"1\" : 1 , \"1\" : 1 , \"2\" : 2 , \"3\" : 3}");
        W_TEST("{\"1\":1}", (unordered_map<int, int>{{1, 1}, {1, 1}}));
    // std::unordered_multimap<int, int>
        R_TEST((unordered_multimap<int, int>{}), "{}");
        W_TEST("{}", (unordered_multimap<int, int>{}));
        R_TEST((unordered_multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{\"1\" : 1 , \"1\" : 1 , \"2\" : 2 , \"3\" : 3}");
        W_TEST("{\"1\":1,\"1\":1}", (unordered_multimap<int, int>{{1, 1}, {1, 1}}));
TEST_END()

TEST_BEG(cxon::JSON<key::unquoted<cxon::JSON<>, true>>)
    using namespace std;
    // std::map<int, std::string>
        R_TEST((map<int, string>{}), "{}");
        W_TEST("{}", (map<int, string>{}));
        R_TEST((map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}), "{1 : \"1\" , 1 : \"1\" , 2 : \"2\" , 3 : \"3\"}");
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, string>{{1, "1"}, {2, "2"}, {3, "3"}}));
        W_TEST("{1:\"1\",2:\"2\",3:\"3\"}", (map<int, const char*>{{1, "1"}, {2, "2"}, {3, "3"}}));
        R_TEST((map<int, string>{}), "{x}", cxon::read_error::integral_invalid, 1);
        R_TEST((map<int, string>{}), "{1: x}", cxon::read_error::unexpected, 4);
    // std::map<std::string, int>
        R_TEST((map<string, int>{}), "{}");
        W_TEST("{}", (map<string, int>{}));
        R_TEST((map<string, int>{{"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<string, int>{{"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<string, int>{{"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<string, int>{{"1", 1}, {"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<string, int>{{"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<string, int>{{"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<string, int>{{"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<string, int>{{"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<string, int>{{"1", 1}, {"2", 2}, {"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<std::u16string, int>
        R_TEST((map<u16string, int>{}), "{}");
        W_TEST("{}", (map<u16string, int>{}));
        R_TEST((map<u16string, int>{{u"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<u16string, int>{{u"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<u16string, int>{{u"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<u16string, int>{{u"1", 1}, {u"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<u16string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<u16string, int>{{u"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<u16string, int>{{u"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<u16string, int>{{u"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<u16string, int>{{u"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<u16string, int>{{u"1", 1}, {u"2", 2}, {u"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<std::u32string, int>
        R_TEST((map<u32string, int>{}), "{}");
        W_TEST("{}", (map<u32string, int>{}));
        R_TEST((map<u32string, int>{{U"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<u32string, int>{{U"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<u32string, int>{{U"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<u32string, int>{{U"1", 1}, {U"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<u32string, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<u32string, int>{{U"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<u32string, int>{{U"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<u32string, int>{{U"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<u32string, int>{{U"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<u32string, int>{{U"1", 1}, {U"2", 2}, {U"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<std::wstring, int>
        R_TEST((map<wstring, int>{}), "{}");
        W_TEST("{}", (map<wstring, int>{}));
        R_TEST((map<wstring, int>{{L"1:1", 1}}), "{1\\:1: 1}"); // ':'
        R_TEST((map<wstring, int>{{L"1\"1", 1}}), "{1\"1: 1}"); // '"'
        R_TEST((map<wstring, int>{{L"1 1", 1}}), "{1\\ 1: 1}"); // ' '
        R_TEST((map<wstring, int>{{L"1", 1}, {L"2", 1}}), "{1: 1, \"2\": 1}"); // mix
        R_TEST((map<wstring, int>{}), "{\0: 1}", cxon::read_error::unexpected, 1);
        W_TEST("{1\\:1:1}", (map<wstring, int>{{L"1:1", 1}})); // ':'
        W_TEST("{1\"1:1}", (map<wstring, int>{{L"1\"1", 1}})); // '"'
        W_TEST("{1'1:1}", (map<wstring, int>{{L"1\'1", 1}})); // '\''
        W_TEST("{1\\ 1:1}", (map<wstring, int>{{L"1 1", 1}})); // ' '
        W_TEST("{1:1,2:2,3:3}", (map<wstring, int>{{L"1", 1}, {L"2", 2}, {L"3", 3}}));
        W_TEST("{1:1,2:2,3:3}", (map<const char*, int, key::less_cstr>{{"1", 1}, {"2", 2}, {"3", 3}}));
    // std::map<int, int>
        R_TEST((map<int, int>{}), "{}");
        W_TEST("{}", (map<int, int>{}));
        R_TEST((map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,2:2,3:3}", (map<int, int>{{1, 1}, {2, 2}, {3, 3}}));
    // std::multimap<int, int>
        R_TEST((multimap<int, int>{}), "{}");
        W_TEST("{}", (multimap<int, int>{}));
        R_TEST((multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,1:1,2:2,3:3}", (multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}));
    // std::unordered_map<int, int>
        R_TEST((unordered_map<int, int>{}), "{}");
        W_TEST("{}", (unordered_map<int, int>{}));
        R_TEST((unordered_map<int, int>{{1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1}", (unordered_map<int, int>{{1, 1}}));
    // std::unordered_multimap<int, int>
        R_TEST((unordered_multimap<int, int>{}), "{}");
        W_TEST("{}", (unordered_multimap<int, int>{}));
        R_TEST((unordered_multimap<int, int>{{1, 1}, {1, 1}, {2, 2}, {3, 3}}), "{1 : 1 , 1 : 1 , 2 : 2 , 3 : 3}");
        W_TEST("{1:1,1:1}", (unordered_multimap<int, int>{{1, 1}, {1, 1}}));
TEST_END()


namespace js {
    struct strict_traits : cxon::json_format_traits {
        static constexpr bool strict_js = true;
    };
}

TEST_BEG(cxon::JSON<>) // json
    using namespace std;
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
        R_TEST('\0', QS("\\u1111"), cxon::read_error::character_invalid, 1);
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
        R_TEST("", QS("\\u008"), cxon::read_error::escape_invalid, 1); // one less
        R_TEST("", QS("\\u"), cxon::read_error::escape_invalid, 1); // none
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

TEST_BEG(cxon::JSON<js::strict_traits>)
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

TEST_BEG(cxon::JSON<>)
    using namespace std;
    // std::nullptr_t
        R_TEST(nullptr, "null");
        W_TEST("null", nullptr);
    // std::tuple<int, double, std::string>
        R_TEST((tuple<int, double, string>{0, 0, "0"}), "[0, 0, \"0\"]");
        W_TEST("[0,0,\"0\"]", (tuple<int, double, string>{0, 0, "0"}));
        R_TEST((tuple<int, double, string>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((tuple<int, double, string>{}), "]", cxon::read_error::unexpected, 0);
        R_TEST((tuple<int, double, string>{}), "[", cxon::read_error::integral_invalid, 1);
        R_TEST((tuple<int, double, string>{}), "[0, 0]", cxon::read_error::unexpected, 5);
        R_TEST((tuple<int, double, string>{}), "[0, 0, \"\", 0]", cxon::read_error::unexpected, 9);
    // std::tuple<int, double>
        R_TEST((tuple<int, double>{}), "[0, 0, \"0\"]", cxon::read_error::unexpected, 5);
        W_TEST("[0,0]", (tuple<int, double>{0, 0}));
    // std::pair<int, std::string>
        R_TEST((pair<int, string>{0, "0"}), "[0, \"0\"]");
        W_TEST("[0,\"0\"]", (pair<int, string>{0, "0"}));
        R_TEST((pair<int, string>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((pair<int, string>{}), "]", cxon::read_error::unexpected, 0);
        R_TEST((pair<int, string>{}), "[", cxon::read_error::integral_invalid, 1);
        R_TEST((pair<int, string>{}), "[x]", cxon::read_error::integral_invalid, 1);
        R_TEST((pair<int, string>{}), "[0]", cxon::read_error::unexpected, 2);
        R_TEST((pair<int, string>{}), "[0, \"\", 0]", cxon::read_error::unexpected, 6);
    // std::valarray<int>
        R_TEST((valarray<int>{1, 2, 3}), "[1, 2, 3]");
        W_TEST("[1,2,3]", (valarray<int>{1, 2, 3}));
        R_TEST((valarray<int>{1, 2, 3, 4}), "[1, 2, 3, 4]");
        R_TEST((valarray<int>{1, 2, 3, 4, 5}), "[1, 2, 3, 4, 5]");
        R_TEST((valarray<int>{1, 2, 3, 4, 5, 6, 7, 8}), "[1, 2, 3, 4, 5, 6, 7, 8]");
        R_TEST((valarray<int>{1, 2, 3, 4, 5, 6, 7, 8, 9}), "[1, 2, 3, 4, 5, 6, 7, 8, 9]");
        R_TEST((valarray<int>()), "", cxon::read_error::unexpected, 0);
        R_TEST((valarray<int>()), "]", cxon::read_error::unexpected, 0);
        R_TEST((valarray<int>()), "[", cxon::read_error::integral_invalid, 1);
        R_TEST((valarray<int>()), "[x", cxon::read_error::integral_invalid, 1);
    // std::array<int, 0>
        R_TEST((array<int, 0>{}), "[]");
        W_TEST("[]", (array<int, 0>{}));
        R_TEST((array<int, 0>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((array<int, 0>{}), "]", cxon::read_error::unexpected, 0);
        R_TEST((array<int, 0>{}), "[", cxon::read_error::unexpected, 1);
    // std::array<int, 3>
        R_TEST((array<int, 3>{{1, 2, 3}}), "[1, 2, 3]");
        W_TEST("[1,2,3]", (array<int, 3>{{1, 2, 3}}));
        R_TEST((array<int, 4>{{1, 2, 3, 0}}), "[1, 2, 3]");
        W_TEST("[1,2,3,4]", (array<int, 4>{{1, 2, 3, 4}}));
        R_TEST((array<int, 2>{}), "[1, 2, 3]", cxon::read_error::unexpected, 5);
        W_TEST("[1,2]", (array<int, 2>{{1, 2}}));
    // std::queue<int>
        R_TEST((queue<int>{}), "[]");
        W_TEST("[]", (queue<int>{}));
        R_TEST((queue<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (queue<int>({1, 2, 3})));
    // std::priority_queue<int>
        R_TEST((priority_queue<int>{}), "[]");
        W_TEST("[]", (priority_queue<int>{}));
        R_TEST(priority_queue<int>(less<int>(), {1, 2, 3}), "[1, 2, 3]");
        W_TEST("[3,2,1]", priority_queue<int>(less<int>(), {1, 2, 3}));
    // std::stack<int>
        R_TEST((stack<int>{}), "[]");
        W_TEST("[]", (stack<int>{}));
        R_TEST((stack<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[3,2,1]", (stack<int>({1, 2, 3})));
    // std::deque<int>
        R_TEST((deque<int>{}), "[]");
        W_TEST("[]", (deque<int>{}));
        R_TEST((deque<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (deque<int>({1, 2, 3})));
    // std::list<int>
        R_TEST((list<int>{}), "[]");
        W_TEST("[]", (list<int>{}));
        R_TEST((list<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (list<int>({1, 2, 3})));
        R_TEST((list<int>{}), "", cxon::read_error::unexpected, 0);
        R_TEST((list<int>{}), "]", cxon::read_error::unexpected, 0);
        R_TEST((list<int>{}), "{", cxon::read_error::unexpected, 0);
        R_TEST((list<int>{}), "[", cxon::read_error::integral_invalid, 1);
        W_TEST("[1,2,3]", (list<float>({1, 2, 3})));
        R_TEST((list<float>{}), "[", cxon::read_error::floating_point_invalid, 1);
    // std::vector<int>
        R_TEST((vector<int>{}), "[]");
        W_TEST("[]", (vector<int>{}));
        R_TEST((vector<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (vector<int>({1, 2, 3})));
    // std::set<int>
        R_TEST((set<int>{}), "[]");
        W_TEST("[]", (set<int>{}));
        R_TEST((set<int>({1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1,2,3]", (set<int>({1, 2, 3})));
    // std::multiset<int>;
        R_TEST((multiset<int>{}), "[]");
        W_TEST("[]", (multiset<int>{}));
        R_TEST((multiset<int>({1, 1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1,1,2,3]", (multiset<int>({1, 1, 2, 3})));
    // std::unordered_set<int>
        R_TEST((unordered_set<int>{}), "[]");
        W_TEST("[]", (unordered_set<int>{}));
        R_TEST((unordered_set<int>({1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1]", (unordered_set<int>({1})));
    // std::unordered_multiset<int>;
        R_TEST((unordered_multiset<int>{}), "[]");
        W_TEST("[]", (unordered_multiset<int>{}));
        R_TEST((unordered_multiset<int>({1, 1, 2, 3})), "[1, 1, 2, 3]");
        W_TEST("[1,1]", (unordered_multiset<int>({1, 1})));
    // std::vector<std::list<int>>
        R_TEST((vector<list<int>>{}), "[]");
        W_TEST("[]", (vector<list<int>>{}));
        R_TEST((vector<list<int>>{{}}), "[[]]");
        W_TEST("[[]]", (vector<list<int>>{{}}));
        R_TEST((vector<list<int>>{{1, 2, 3}, {3, 2, 1}}), "[[1, 2, 3], [3, 2, 1]]");
        W_TEST("[[1,2,3],[3,2,1]]", (vector<list<int>>{{1, 2, 3}, {3, 2, 1}}));
    // std::list<std::vector<int>>
        R_TEST((list<vector<int>>{}), "[]");
        W_TEST("[]", (list<vector<int>>{}));
        R_TEST((list<vector<int>>{{}}), "[[]]");
        W_TEST("[[]]", (list<vector<int>>{{}}));
        R_TEST((list<vector<int>>{{1, 2, 3}, {3, 2, 1}}), "[[1, 2, 3], [3, 2, 1]]");
        W_TEST("[[1,2,3],[3,2,1]]", (list<vector<int>>{{1, 2, 3}, {3, 2, 1}}));
TEST_END()


enum Enum1 { one, two, three, four };

CXON_ENUM(Enum1,
    CXON_ENUM_VALUE_ASIS(one),
    CXON_ENUM_VALUE_NAME("Two (2)", two),
    CXON_ENUM_VALUE_ASIS(three)
)

TEST_BEG(cxon::CXON<>) // enum
    R_TEST(Enum1::one, "one");
    W_TEST("one", Enum1::one);
    R_TEST(Enum1::two, "Two (2)");
    W_TEST("Two (2)", Enum1::two);
    R_TEST(Enum1::one, "", cxon::read_error::unexpected, 0);
    R_TEST(Enum1::one, "o", cxon::read_error::unexpected, 0);
    R_TEST(Enum1::one, "{", cxon::read_error::unexpected, 1);
    R_TEST(Enum1::one, "{}", cxon::read_error::unexpected, 0);
    R_TEST(Enum1::one, "{{}}", cxon::read_error::unexpected, 0);
    R_TEST(Enum1::one, "{\"}", cxon::read_error::unexpected, 3);
    R_TEST(Enum1::one, "{\"\"}", cxon::read_error::unexpected, 0);
    R_TEST(Enum1::one, "{\"\\x\"}", cxon::read_error::unexpected, 0);
#   ifdef NDEBUG
        W_TEST("", Enum1::four);
#   endif
TEST_END()

TEST_BEG(cxon::JSON<>)
    R_TEST(Enum1::one, QS("one"));
    W_TEST(QS("one"), Enum1::one);
    R_TEST(Enum1::two, QS("Two (2)"));
    W_TEST(QS("Two (2)"), Enum1::two);
    R_TEST(Enum1::one, QS("noe"), cxon::read_error::unexpected, 0);
#   ifdef NDEBUG
        W_TEST("", Enum1::four);
#   endif
TEST_END()


struct Struct1 {
    int a;
    Enum1 b;
    int c;
    Struct1(int a = 0, Enum1 b = Enum1::one, int c = 0) : a(a), b(b), c(c) {}
    bool operator ==(const Struct1& t) const { return a == t.a && b == t.b && c == t.c; }
};

struct Struct2 {
    int a;
    int b;
    Struct2(int a = 0, int b = 0) : a(a), b(b) {}
    bool operator ==(const Struct2& t) const { return a == t.a && b == t.b; }
};

CXON_STRUCT_READ(Struct1,
    CXON_STRUCT_FIELD_NAME("A", a),
    CXON_STRUCT_FIELD_ASIS(b)
)
CXON_STRUCT_WRITE(Struct1,
    CXON_STRUCT_FIELD_NAME("A", a),
    CXON_STRUCT_FIELD_ASIS(b)
)

CXON_STRUCT(Struct2,
    CXON_STRUCT_FIELD_ASIS(a),
    CXON_STRUCT_FIELD_NAME("B", b)
)

TEST_BEG(cxon::CXON<>) // struct macros
    R_TEST(Struct1(0, Enum1::one), "{A: 0, b: one}");
    R_TEST(Struct1(0, Enum1::two), "{b: Two (2), A: 0}");
    R_TEST(Struct1(0, Enum1::three), "{b: three}");
    W_TEST("{A:0,b:one}", Struct1(0, Enum1::one));
    R_TEST(Struct2(0, 1), "{a: 0, B: 1}");
    W_TEST("{a:0,B:1}", Struct2(0, 1));
    R_TEST(Struct2(), "{\"x\": 1}", cxon::read_error::unexpected, 1);
    R_TEST(Struct1(0, Enum1::one), "{A: 0, b: eno}", cxon::read_error::unexpected, 10);
    R_TEST(Struct1(0, Enum1::one), "{A: 0, x: eno}", cxon::read_error::unexpected, 7);
    R_TEST(Struct2(), "", cxon::read_error::unexpected, 0);
    R_TEST(Struct2(), "}", cxon::read_error::unexpected, 0);
    R_TEST(Struct2(), "{", cxon::read_error::unexpected, 1);
TEST_END()

TEST_BEG(cxon::JSON<>)
    R_TEST(Struct1(0, Enum1::one), "{\"A\": 0, \"b\": \"one\"}");
    R_TEST(Struct1(0, Enum1::two), "{\"b\": \"Two (2)\", \"A\": 0}");
    R_TEST(Struct1(0, Enum1::three), "{\"b\": \"three\"}");
    W_TEST("{\"A\":0,\"b\":\"one\"}", Struct1(0, Enum1::one));
    R_TEST(Struct2(0, 1), "{\"a\": 0, \"B\": 1}");
    R_TEST(Struct2(), "{\"x\": 1}", cxon::read_error::unexpected, 1);
    W_TEST("{\"a\":0,\"B\":1}", Struct2(0, 1));
    R_TEST(Struct1(), "{\"x\": 1}", cxon::read_error::unexpected, 1);
    R_TEST(Struct1(), "{\"b\": \"one\", \"A\": x}", cxon::read_error::integral_invalid, 18);
TEST_END()


struct Struct3 {
    int a;
    Struct3* b;
    Struct3(int a = 0, Struct3* b = nullptr) : a(a), b(b) {}
    bool operator ==(const Struct3& t) const { return a == t.a && ((!b && !t.b) || (b && t.b && *b == *t.b)); }
};

CXON_STRUCT(Struct3,
    CXON_STRUCT_FIELD_ASIS(a),
    CXON_STRUCT_FIELD_ASIS(b)
)

TEST_BEG(cxon::CXON<>)
    R_TEST(Struct3(1, new Struct3(2, nullptr)), "{a: 1, b: {a: 2}}");
    R_TEST(Struct3(1, nullptr), "{a: 1, b: null}");
    W_TEST("{a:1,b:{a:2,b:null}}", Struct3(1, new Struct3(2, nullptr)));
    R_TEST(Struct3(), "{a: 1, x: nil}", cxon::read_error::unexpected, 7);
    R_TEST(Struct3(), "{a: 1, b: nil}", cxon::read_error::unexpected, 10);
    R_TEST(Struct3(), "{a: 1, b: {a: x}}", cxon::read_error::integral_invalid, 14);
TEST_END()

TEST_BEG(cxon::JSON<>)
    R_TEST(Struct3(1, new Struct3(2, nullptr)), "{\"a\": 1, \"b\": {\"a\": 2}}");
    R_TEST(Struct3(1, nullptr), "{\"a\": 1, \"b\": null}");
    W_TEST("{\"a\":1,\"b\":{\"a\":2,\"b\":null}}", Struct3(1, new Struct3(2, nullptr)));
    R_TEST(Struct3(), "{\"a\": 1, \"x\": nil}", cxon::read_error::unexpected, 9);
    R_TEST(Struct3(), "{\"a\": 1, \"b\": nil}", cxon::read_error::unexpected, 14);
    R_TEST(Struct3(), "{\"a\": 1, \"b\": {\"a\": x}}", cxon::read_error::integral_invalid, 20);
TEST_END()


struct Struct4 {
    Struct4(int a = 0) : a(a) {}
    bool operator ==(const Struct4& t) const { return a == t.a; }

    template <typename X, typename II>
        static auto read_value(Struct4& t, II& b, II e, cxon::rctx<X>& ctx) -> cxon::enable_for_t<X, cxon::CXON, bool> {
            return cxon::read_value<X>(t.a, b, e, ctx);
        }
    template <typename X, typename II>
        static auto read_value(Struct4& t, II& b, II e, cxon::rctx<X>& ctx) -> cxon::enable_for_t<X, cxon::JSON, bool> {
            return cxon::read_value<X>(t.a, b, e, ctx);
        }
    template <typename X, typename OI>
        static bool write_value(OI& o, const Struct4& t, cxon::wctx<X>& ctx) {
            return cxon::write_value<X>(o, t.a, ctx);
        }

private:
    int a;
};

TEST_BEG(cxon::CXON<>) // static method
    R_TEST(Struct4(1), "1");
    W_TEST("3", Struct4(3));
TEST_END()

TEST_BEG(cxon::JSON<>)
    R_TEST(Struct4(1), "1");
    W_TEST("3", Struct4(3));
TEST_END()


struct Struct5 {
    Struct5(int a = 0) : a(a) {}
    bool operator ==(const Struct5& t) const { return a == t.a; }

    template <typename X, typename II>
        bool read_value(II& b, II e, cxon::rctx<X>& ctx) {
            return cxon::read_value<X>(a, b, e, ctx);
        }
    template <typename X, typename OI>
        auto write_value(OI& o, typename cxon::wctx<X>& ctx) const -> cxon::enable_for_t<X, cxon::CXON, bool> {
            return cxon::write_value<X>(o, a, ctx);
        }
    template <typename X, typename OI>
        auto write_value(OI& o, cxon::wctx<X>& ctx) const -> cxon::enable_for_t<X, cxon::JSON, bool> {
            return cxon::write_value<X>(o, a, ctx);
        }

private:
    int a;
};

TEST_BEG(cxon::CXON<>) // method
    R_TEST(Struct5(1), "1");
    W_TEST("3", Struct5(3));
TEST_END()

TEST_BEG(cxon::JSON<>)
    R_TEST(Struct5(1), "1");
    W_TEST("3", Struct5(3));
TEST_END()


struct Struct6 {
    int a;
    Struct6(int a = 0) : a(a) {}
    bool operator ==(const Struct6& t) const { return a == t.a; }
};

namespace cxon {
    template <typename X, typename II>
        inline  enable_for_t<X, CXON, bool> read_value(Struct6& t, II& b, II e, rctx<X>& ctx) {
            return read_value<X>(t.a, b, e, ctx);
        }
    template <typename X, typename II>
        inline enable_for_t<X, JSON, bool> read_value(Struct6& t, II& b, II e, rctx<X>& ctx) {
            return read_value<X>(t.a, b, e, ctx);
        }
    template <typename X, typename OI>
        inline bool write_value(OI& o, const Struct6& t, wctx<X>& ctx) {
            return write_value<X>(o, t.a, ctx);
        }
}

TEST_BEG(cxon::CXON<>) // function
    R_TEST(Struct6(1), "1");
    W_TEST("3", Struct6(3));
TEST_END()

TEST_BEG(cxon::JSON<>)
    R_TEST(Struct6(1), "1");
    W_TEST("3", Struct6(3));
TEST_END()


struct Struct7 {
    Struct7(int a = 0, int b = 0) : a(a), b(b) {}
    bool operator ==(const Struct7& t) const { return a == t.a && b == t.b; }

    CXON_STRUCT_MEMBER(Struct7,
        CXON_STRUCT_FIELD_ASIS(a),
        CXON_STRUCT_FIELD_ASIS(b)
    )

private:
    int a;
    int b;
};

TEST_BEG(cxon::CXON<>) // macros inside
    R_TEST(Struct7(1, 2), "{a: 1, b: 2}");
    R_TEST(Struct7(3, 0), "{a: 3}");
    R_TEST(Struct7(0, 6), "{b: 6}");
    W_TEST("{a:9,b:10}", Struct7(9, 10));
    R_TEST(Struct7(), "{x: 0}", cxon::read_error::unexpected, 1);
TEST_END()

TEST_BEG(cxon::JSON<>) // macros inside
    R_TEST(Struct7(1, 2), "{\"a\": 1, \"b\": 2}");
    R_TEST(Struct7(3, 0), "{\"a\": 3}");
    R_TEST(Struct7(0, 6), "{\"b\": 6}");
    W_TEST("{\"a\":9,\"b\":10}", Struct7(9, 10));
    R_TEST(Struct7(), "{\"x\": 0}", cxon::read_error::unexpected, 1);
TEST_END()


struct Struct8 {
    Struct8(int a = 0, int b = 0) : a(a), b(b) {}
    bool operator ==(const Struct8& t) const { return a == t.a && b == t.b; }

    template <typename X, typename II>
        static bool read_value(Struct8& t, II& i, II e, cxon::rctx<X>& ctx) {
            using namespace cxon::structs;
            static constexpr auto f = make_fields(
                make_field("a", &Struct8::a),
                make_field("b", &Struct8::b)
            );
            return read_fields<X>(t, f, i, e, ctx);
        }
    template <typename X, typename OI>
        static bool write_value(OI& o, const Struct8& t, cxon::wctx<X>& ctx) {
            using namespace cxon::structs;
            static constexpr auto f = make_fields(
                make_field("a", &Struct8::a),
                make_field("b", &Struct8::b)
            );
            return write_fields<X>(o, t, f, ctx);
        }

private:
    int a;
    int b;
};

TEST_BEG(cxon::CXON<>)
    R_TEST(Struct8(1, 2), "{a: 1, \"b\": 2}");
    R_TEST(Struct8(1, 2), "{a: 1, x: 2}", cxon::read_error::unexpected, 7);
    W_TEST("{a:3,b:4}", Struct8(3, 4));
TEST_END()

TEST_BEG(cxon::JSON<>)
    R_TEST(Struct8(1, 2), "{\"a\": 1, \"b\": 2}");
    R_TEST(Struct8(1, 2), "{\"a\": 1, \"x\": 2}", cxon::read_error::unexpected, 9);
    W_TEST("{\"a\":3,\"b\":4}", Struct8(3, 4));
TEST_END()


struct Struct9 {
    static int a;
    static int const b;

    bool operator ==(const Struct9&) const { return true; }

    CXON_STRUCT_READ_MEMBER(Struct9,
        CXON_STRUCT_FIELD_ASIS(a)
    )
    CXON_STRUCT_WRITE_MEMBER(Struct9,
        CXON_STRUCT_FIELD_ASIS(a),
        CXON_STRUCT_FIELD_ASIS(b)
    )
};
int Struct9::a = 0;
int const Struct9::b = 3;

TEST_BEG(cxon::CXON<>) // static field
    R_TEST(Struct9(), "{}");
    W_TEST("{a:0,b:3}", Struct9());
    R_TEST(Struct9(), "{a: 1}");
    W_TEST("{a:1,b:3}", Struct9());
    R_TEST(Struct9(), "{}");
    W_TEST("{a:1,b:3}", Struct9());
    R_TEST(Struct9(), "{a: 3}");
    W_TEST("{a:3,b:3}", Struct9());
    R_TEST(Struct9(), "{a: 1, x: 3}", cxon::read_error::unexpected, 7);
TEST_END()

TEST_BEG(cxon::JSON<>) // static field
    R_TEST(Struct9(), "{\"a\":0}");
    W_TEST("{\"a\":0,\"b\":3}", Struct9());
    R_TEST(Struct9(), "{\"a\": 1}");
    W_TEST("{\"a\":1,\"b\":3}", Struct9());
    R_TEST(Struct9(), "{}");
    W_TEST("{\"a\":1,\"b\":3}", Struct9());
    R_TEST(Struct9(), "{\"a\": 3}");
    W_TEST("{\"a\":3,\"b\":3}", Struct9());
    R_TEST(Struct9(), "{\"a\": 1, \"x\": 3}", cxon::read_error::unexpected, 9);
TEST_END()


struct Struct10 {
    int a;
    int b;
    Struct10(int a = 0, int b = 0) : a(a), b(b) {}
    bool operator ==(const Struct10& t) const { return a == t.a && b == t.b; }
};

CXON_STRUCT_READ(Struct10,
    CXON_STRUCT_FIELD_ASIS(a),
    CXON_STRUCT_FIELD_SKIP("skip1"),
    CXON_STRUCT_FIELD_SKIP("skip2"),
    CXON_STRUCT_FIELD_SKIP("skip3"),
    CXON_STRUCT_FIELD_SKIP("skip4"),
    CXON_STRUCT_FIELD_SKIP("skip5"),
    CXON_STRUCT_FIELD_SKIP("skip6")
)
CXON_STRUCT_WRITE(Struct10,
    CXON_STRUCT_FIELD_ASIS(a),
    CXON_STRUCT_FIELD_NAME("* \"':*", b)
)

TEST_BEG(cxon::CXON<>) // skip field
    R_TEST(Struct10(1), "{skip1: true, a: 1}");
    R_TEST(Struct10(1), "{skip2: 1, a: 1}");
    R_TEST(Struct10(1), "{skip3: \"2\", a: 1}");
    R_TEST(Struct10(1), "{skip3: \"\\x\", a: 1}");
    R_TEST(Struct10(1), "{skip4: {3, 4}, a: 1}");
    R_TEST(Struct10(1), "{skip4: {{}}, a: 1}");
    R_TEST(Struct10(1), "{skip6: {\"}\": 5}, a: 1}");
    R_TEST(Struct10(), "{x: 1}", cxon::read_error::unexpected, 1);
    R_TEST(Struct10(), "{x\\ x: 1}", cxon::read_error::unexpected, 1);
    R_TEST(Struct10(), "{skip6: {", cxon::read_error::unexpected, 9);
    R_TEST(Struct10(), "{skip6: \"", cxon::read_error::unexpected, 9);
    W_TEST("{a:1,*\\ \"'\\:*:2}", Struct10(1, 2));
TEST_END()

TEST_BEG(cxon::JSON<>) // skip field
    R_TEST(Struct10(1), "{\"skip1\": true, \"a\": 1}");
    R_TEST(Struct10(1), "{\"skip2\": 1, \"a\": 1}");
    R_TEST(Struct10(1), "{\"skip3\": \"2\", \"a\": 1}");
    R_TEST(Struct10(1), "{\"skip3\": \"\\x\", \"a\": 1}");
    R_TEST(Struct10(1), "{\"skip4\": [3, 4], \"a\": 1}");
    R_TEST(Struct10(1), "{\"skip4\": [[]], \"a\": 1}");
    R_TEST(Struct10(1), "{\"skip5\": {5, 6}, \"a\": 1}");
    R_TEST(Struct10(1), "{\"skip5\": {{}}, \"a\": 1}");
    R_TEST(Struct10(1), "{\"skip6\": {\"}\": 7}, \"a\": 1}");
    R_TEST(Struct10(1), "{\"skip6\": {\"]\": 8}, \"a\": 1}");
    R_TEST(Struct10(1), "{\"skip6\": [\"]\", 9], \"a\": 1}");
    R_TEST(Struct10(1), "{\"skip6\": [\"}\", 10], \"a\": 1}");
    R_TEST(Struct10(), "{\"x\": 1}", cxon::read_error::unexpected, 1);
    R_TEST(Struct10(), "{\"skip1\": {", cxon::read_error::unexpected, 11);
    R_TEST(Struct10(), "{\"skip1\": [", cxon::read_error::unexpected, 11);
    R_TEST(Struct10(), "{\"skip1\": \"", cxon::read_error::unexpected, 11);
    W_TEST("{\"a\":1,\"* \\\"':*\":2}", Struct10(1, 2));
TEST_END()


TEST_BEG(cxon::CXON<>) // interface/read
    // iterator
    {   ++TEST_A;
        int r; char const i[] = "1";
        if (!cxon::from_chars(r, std::begin(i), std::end(i)) || r != 1) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    {   ++TEST_A;
        int r; std::string const i = "1";
        if (!cxon::from_chars(r, std::begin(i), std::end(i)) || r != 1) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    {   ++TEST_A;
        int r; std::vector<char> const i = {'1'};
        if (!cxon::from_chars(r, std::begin(i), std::end(i)) || r != 1) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    // container
    {   ++TEST_A;
        int r; char const i[] = "1";
        if (!cxon::from_chars(r, i) || r != 1) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    {   ++TEST_A;
        int r; std::string const i = "1";
        if (!cxon::from_chars(r, i) || r != 1) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    {   ++TEST_A;
        int r; std::vector<char> const i = {'1'};
        if (!cxon::from_chars(r, i) || r != 1) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
TEST_END()

TEST_BEG(cxon::CXON<>) // interface/write
    // output iterator
    {   ++TEST_A;
        std::string r; std::string const e = QS("1");
        if (!cxon::to_chars<XXON>(std::back_inserter(r), "1") || r != e) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    {   ++TEST_A;
        std::string r; std::string const e = "1";
        if (!cxon::to_chars<XXON>(std::back_inserter(r), 1) || r != e) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    {   ++TEST_A;
        std::string r; std::string const e = "true";
        if (!cxon::to_chars<XXON>(std::back_inserter(r), true) || r != e) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    // range
    {   ++TEST_A;
        char o[16]; char const e[] = QS("1");
        auto const r = cxon::to_chars<XXON>(std::begin(o), std::end(o), "1");
        if (!r || std::strncmp(o, e, r.end - std::begin(o)) != 0) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
        {   ++TEST_A;
            char o[1];
            auto const r = cxon::to_chars<XXON>(std::begin(o), std::end(o), "1");
            if (r.ec != cxon::write_error::output_failure) {
                ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
                CXON_ASSERT(false, "check failed");
            }
        }
    {   ++TEST_A;
        char o[16]; char const e[] = "1";
        auto const r = cxon::to_chars<XXON>(std::begin(o), std::end(o), 1);
        if (!r || std::strncmp(o, e, r.end - std::begin(o)) != 0) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
        {   ++TEST_A;
            char o[1];
            auto const r = cxon::to_chars<XXON>(std::begin(o), std::end(o), 42);
            if (r.ec != cxon::write_error::output_failure) {
                ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
                CXON_ASSERT(false, "check failed");
            }
        }
    {   ++TEST_A;
        char o[16]; char const e[] = "true";
        auto const r = cxon::to_chars<XXON>(std::begin(o), std::end(o), true);
        if (!r || std::strncmp(o, e, r.end - std::begin(o)) != 0) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
        {   ++TEST_A;
            char o[1];
            auto const r = cxon::to_chars<XXON>(std::begin(o), std::end(o), true);
            if (r.ec != cxon::write_error::output_failure) {
                ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
                CXON_ASSERT(false, "check failed");
            }
        }
    // container/std::string (push_back, append)
    {   ++TEST_A;
        std::string r; std::string const e = QS("1");
        if (!cxon::to_chars<XXON>(r, "1") || r != e) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    {   ++TEST_A;
        std::string r; std::string const e = "1";
        if (!cxon::to_chars<XXON>(r, 1) || r != e) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    {   ++TEST_A;
        std::string r; std::string const e = "true";
        if (!cxon::to_chars<XXON>(r, true) || r != e) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    // container/std::vector (push_back)
    {   ++TEST_A;
        std::vector<char> r; std::vector<char> const e = {'"', '1', '"'};
        if (!cxon::to_chars<XXON>(r, "1") || r != e) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    {   ++TEST_A;
        std::vector<char> r; std::vector<char> const e = {'1'};
        if (!cxon::to_chars<XXON>(r, 1) || r != e) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    {   ++TEST_A;
        std::vector<char> r; std::vector<char> const e = {'t', 'r', 'u', 'e'};
        if (!cxon::to_chars<XXON>(r, true) || r != e) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
TEST_END()

TEST_BEG(cxon::CXON<>) // errors
    using namespace cxon;
    {   std::error_condition ec;
            ec = read_error::ok;
                CXON_ASSERT(ec.category() == read_error_category::value, "check failed");
                CXON_ASSERT(std::strcmp(ec.category().name(), "cxon/read") == 0, "check failed");
                CXON_ASSERT(ec.message() == "no error", "check failed");
            ec = read_error::unexpected;
                CXON_ASSERT(ec.message() == "unexpected input", "check failed");
            ec = read_error::character_invalid;
                CXON_ASSERT(ec.message() == "invalid character", "check failed");
            ec = read_error::integral_invalid;
                CXON_ASSERT(ec.message() == "invalid integral or value out of range", "check failed");
            ec = read_error::floating_point_invalid;
                CXON_ASSERT(ec.message() == "invalid floating point", "check failed");
            ec = read_error::boolean_invalid;
                CXON_ASSERT(ec.message() == "invalid boolean", "check failed");
            ec = read_error::escape_invalid;
                CXON_ASSERT(ec.message() == "invalid escape sequence", "check failed");
            ec = read_error::surrogate_invalid;
                CXON_ASSERT(ec.message() == "invalid surrogate", "check failed");
#           ifdef NDEBUG
                ec = read_error(255);
                    CXON_ASSERT(ec.message() == "unknown error", "check failed");
#           endif
    }
    {   std::error_condition ec;
            ec = write_error::ok;
                CXON_ASSERT(ec.category() == write_error_category::value, "check failed");
                CXON_ASSERT(std::strcmp(ec.category().name(), "cxon/write") == 0, "check failed");
                CXON_ASSERT(ec.message() == "no error", "check failed");
            ec = write_error::output_failure;
                CXON_ASSERT(ec.message() == "output cannot be written", "check failed");
            ec = write_error::argument_invalid;
                CXON_ASSERT(ec.message() == "invalid argument", "check failed");
#           ifdef NDEBUG
                ec = write_error(255);
                    CXON_ASSERT(ec.message() == "unknown error", "check failed");
#           endif
    }
TEST_END()

TEST_BEG(cxon::CXON<>) // pretty
    {   ++TEST_A;
        using map = std::map<std::string, std::vector<int>>;
        char const p[] =
            "{\n"
            "\teven: {\n"
            "\t\t2,\n"
            "\t\t4,\n"
            "\t\t6\n"
            "\t},\n"
            "\todd: {\n"
            "\t\t1,\n"
            "\t\t3,\n"
            "\t\t5\n"
            "\t}\n"
            "}"
        ;
        std::string s;
            cxon::to_chars<XXON>(cxon::make_indenter<XXON>(s), map{{"even", {2, 4, 6}}, {"odd", {1, 3, 5}}});
        if (s != p) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
    {   ++TEST_A;
        char const p[] =
            "{\n"
            "\tala: \"ba\\\"la\",\n"
            "\tbl\\ ah: \"blah\"\n"
            "}"
        ;
        std::string s = "{ala: \"ba\\\"la\", bl\\ ah: \"blah\"}";
            s = cxon::pretty<XXON>(s);
        if (s != p) {
            ++TEST_F, fprintf(stderr, "must pass, but failed: at %s:%li\n", __FILE__, (long)__LINE__);
            CXON_ASSERT(false, "check failed");
        }
    }
TEST_END()

////////////////////////////////////////////////////////////////////// tests //

namespace test {

    using namespace cxon;

    template <typename X, typename T>
        inline std::string to_string(const T& t) {
            std::string s;
                to_chars<X>(s, t);
            return s;
        }

    template <typename ...>
        struct match;

    template <typename T>
        struct match<T> {
            template <typename U = T>
                static auto values(const U& t0, const U& t1) -> enable_if_t<!std::is_floating_point<U>::value, bool> {
                    return t0 == t1;
                }
            template <typename U = T>
                static auto values(const U& t0, const U& t1) -> enable_if_t< std::is_floating_point<U>::value, bool> {
                    return !std::isgreater(t0, t1) && !std::isless(t0, t1);
                }
        };
    template <>
        struct match<const char*> {
            static bool values(const char* t0, const char* t1) {
                return t0 == t1 || (t0 && std::strcmp(t0, t1) == 0);
            }
        };
    template <>
        struct match<char*> {
            static bool values(const char* t0, const char* t1) {
                return t0 == t1 || (t0 && std::strcmp(t0, t1) == 0);
            }
        };
    template <>
        struct match<const char16_t*> {
            static bool values(const char16_t* t0, const char16_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <>
        struct match<char16_t*> {
            static bool values(const char16_t* t0, const char16_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <>
        struct match<const char32_t*> {
            static bool values(const char32_t* t0, const char32_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <>
        struct match<char32_t*> {
            static bool values(const char32_t* t0, const char32_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <>
        struct match<const wchar_t*> {
            static bool values(const wchar_t* t0, const wchar_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <>
        struct match<wchar_t*> {
            static bool values(const wchar_t* t0, const wchar_t* t1) {
                while (*t0 && *t1 && *t0 == *t1) ++t0, ++t1;
                return *t0 == *t1;
            }
        };
    template <typename T, size_t N>
        struct match<T[N]> {
            static bool values(const T (&t0)[N], const T (&t1)[N]) {
                return std::equal(std::begin(t0), std::end(t0), std::begin(t1), [] (const T& t0, const T& t1) { return match<T>::values(t0, t1); });
            }
        };
    template <typename T>
        struct match<std::valarray<T>> {
            static bool values(const std::valarray<T>& t0, const std::valarray<T>& t1) {
                return std::equal(std::begin(t0), std::end(t0), std::begin(t1), [] (const T& t0, const T& t1) { return match<T>::values(t0, t1); });
            }
        };
    template <typename T>
        struct match<std::priority_queue<T>> {
            static bool values(const std::priority_queue<T>& t0, const std::priority_queue<T>& t1) {
                auto r0 = t0; auto r1 = t1;
                    while (!r0.empty() && !r1.empty()) {
                        if (!match<T>::values(r0.top(), r1.top())) return false;
                        r0.pop(); r1.pop();
                    }
                return r0.empty() && r1.empty();
            }
        };

    template <typename II>
        struct force_input_iterator {
            using iterator_category = std::input_iterator_tag;
            using value_type        = typename II::value_type;
            using difference_type   = typename II::difference_type;
            using pointer           = typename II::pointer;
            using reference         = typename II::reference;

            II ii_;

            force_input_iterator(II ii) : ii_(ii) {}

            bool operator ==(const force_input_iterator& i) const { return ii_ == i.ii_; }
            bool operator !=(const force_input_iterator& i) const { return ii_ != i.ii_; }
            force_input_iterator& operator ++() { return ++ii_, *this; }
            reference operator *() { return *ii_; }
        };
    template <typename II>
        inline force_input_iterator<II> make_force_input_iterator(II ii) {
            return force_input_iterator<II>(ii);
        }

    template <typename, typename = void>
        struct is_force_input_iterator : std::false_type {};
    template <typename X>
        struct is_force_input_iterator<X, enable_if_t<X::force_input_iterator>> : std::true_type {};

    template <typename X, typename T>
        inline auto from_string(T& t, const std::string& c)
            -> enable_if_t< is_force_input_iterator<X>::value, from_chars_result<decltype(c.begin())>>
        {
            auto b = make_force_input_iterator(c.begin()), e = make_force_input_iterator(c.end());
            auto const r =  from_chars<X>(t, b, e);
            return { r.ec, r.end.ii_ };
        }
    template <typename X, typename T>
        inline auto from_string(T& t, const std::string& c)
            -> enable_if_t<!is_force_input_iterator<X>::value, from_chars_result<decltype(c.begin())>>
        {
            return from_chars<X>(t, c);
        }

    template <typename X, typename T>
        static bool verify_read(const T& ref, const std::string& sbj) {
            T res{};
                auto const r = from_string<X>(res, sbj);
            if (!r || r.end != sbj.end() || !match<T>::values(res, ref)) {
                return fprintf(stderr, "must pass, but failed: "), false;
            }
            return true;
        }
    template <typename X, typename T>
        static bool verify_read(const T&, const std::string& sbj, cxon::read_error err, int pos) {
            T res{};
                auto const r = from_string<X>(res, sbj);
            if (r.ec.value() != (int)err || (pos != -1 && std::distance(sbj.begin(), r.end) != pos)) {
                return fprintf(stderr, "must fail, but passed: "), false;
            }
            return true;
        }

    template <typename X, typename T>
        static bool verify_write(const std::string& ref, const T& sbj) {
            std::string res;
                auto const r = cxon::to_chars<X>(res, sbj);
            if (!r || ref != res) {
                return fprintf(stderr, "must pass, but failed: "), false;
            }
            return true;
        }
    template <typename X, typename T>
        static bool verify_write(const std::string& ref, const T& sbj, cxon::write_error err) {
            std::string res;
                auto const r = cxon::to_chars<X>(res, sbj);
            if (r.ec.value() != (int)err) {
                return fprintf(stderr, "must fail, but passed: "), false;
            }
            return true;
        }

}   // test
