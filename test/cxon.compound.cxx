// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "cxon.hxx"

#include <string>

///////////////////////////////////////////////////////////////////////////////

TEST_BEG(cxon::CXON<>)
    // T[]
        {   int a[] = {1, 2, 3};
            R_TEST(a, "{1,2,3}");
            W_TEST("{1,2,3}", a);
        }
        {   int a[] = {1, 2, 0};
            R_TEST(a, "{1,2}");
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "}", chio::read_error::unexpected, 0);
            R_TEST(a, "{", chio::read_error::integral_invalid, 1);
        }
        {   int a[] = {1, 2, 3};
            R_TEST(a, "{1,2,3,4}", chio::read_error::overflow, 0);
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
            R_TEST(a, QS("1234"), chio::read_error::overflow, 0);
            R_TEST(a, QS("12\\u2728"), chio::read_error::overflow, 0);
        }
        {   char a[] = {'1', '2', '\0'};
            R_TEST(a, "12", chio::read_error::unexpected, 0);
            R_TEST(a, "\"12", chio::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), chio::read_error::escape_invalid, 1);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", chio::read_error::unexpected, 0);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "}", chio::read_error::unexpected, 0);
            R_TEST(a, "{", chio::read_error::unexpected, 0);
            R_TEST(a, "\"", chio::read_error::unexpected, 1);
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
            R_TEST(a, QS("1234"), chio::read_error::overflow, 0);
            R_TEST(a, QS("12\xF0\x9F\x8D\xBA"), chio::read_error::overflow, 0);
        }
        {   char16_t a[] = {u'1', u'2', u'\0'};
            R_TEST(a, "12", chio::read_error::unexpected, 0);
            R_TEST(a, "\"12", chio::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), chio::read_error::escape_invalid, 1);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", chio::read_error::unexpected, 0);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "}", chio::read_error::unexpected, 0);
            R_TEST(a, "{", chio::read_error::unexpected, 0);
            R_TEST(a, "\"", chio::read_error::unexpected, 1);
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
            R_TEST(a, QS("1234"), chio::read_error::overflow, 0);
        }
        {   char32_t a[] = {U'1', U'2', U'\0'};
            R_TEST(a, "12", chio::read_error::unexpected, 0);
            R_TEST(a, "\"12", chio::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), chio::read_error::escape_invalid, 1);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", chio::read_error::unexpected, 0);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "}", chio::read_error::unexpected, 0);
            R_TEST(a, "{", chio::read_error::unexpected, 0);
            R_TEST(a, "\"", chio::read_error::unexpected, 1);
        }
    // wchar_t[]
        R_TEST(L"", QS(""));
        {   wchar_t a[] = {L'1', L'2', L'3', L'\0', L'4', L'\0'};
            R_TEST(a, QS("123\\0004"));
            W_TEST(QS("123\\04"), a);
        }
        {   wchar_t a[] = {L'1', L'2', L'3'};
            R_TEST(a, QS("1234"), chio::read_error::overflow, 0);
        }
        {   wchar_t a[] = {L'1', L'2', L'\0'};
            R_TEST(a, "12", chio::read_error::unexpected, 0);
            R_TEST(a, "\"12", chio::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), chio::read_error::escape_invalid, 1);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", chio::read_error::unexpected, 0);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "}", chio::read_error::unexpected, 0);
            R_TEST(a, "{", chio::read_error::unexpected, 0);
            R_TEST(a, "\"", chio::read_error::unexpected, 1);
        }
    // const char*
        R_TEST((const char*)"test", QS("test"));
        R_TEST((const char*)nullptr, "null");
        W_TEST(QS("test"), (const char*)"test");
        W_TEST("null", (const char*)nullptr);
        R_TEST((const char*)nullptr, "nil", chio::read_error::unexpected, 1);
        R_TEST((const char*)nullptr, "\"nil", chio::read_error::unexpected, 4);
    // const char16_t*
        R_TEST((const char16_t*)u"test", QS("test"));
        W_TEST(QS("test"), (const char16_t*)u"test");
        R_TEST((const char16_t*)nullptr, "\"", chio::read_error::unexpected, 1);
    // const char32_t*
        R_TEST((const char32_t*)U"test", QS("test"));
        W_TEST(QS("test"), (const char32_t*)U"test");
        R_TEST((const char32_t*)nullptr, "\"", chio::read_error::unexpected, 1);
    // const wchar_t*
        R_TEST((const wchar_t*)L"test", QS("test"));
        W_TEST(QS("test"), (const wchar_t*)L"test");
        R_TEST((const wchar_t*)nullptr, "\"", chio::read_error::unexpected, 1);
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
TEST_END()

TEST_BEG(cxon::JSON<>)
    // T[]
        {   int a[] = {1, 2, 3};
            R_TEST(a, "[1,2,3]");
            W_TEST("[1,2,3]", a);
        }
        {   int a[] = {1, 2, 0};
            R_TEST(a, "[1,2]");
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "]", chio::read_error::unexpected, 0);
            R_TEST(a, "[", chio::read_error::integral_invalid, 1);
        }
        {   int a[] = {1, 2, 3};
            R_TEST(a, "[1,2,3,4]", chio::read_error::overflow, 0);
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
            R_TEST(a, QS("1234"), chio::read_error::overflow, 0);
            R_TEST(a, QS("12\\u2728"), chio::read_error::overflow, 0);
        }
        {   char a[] = {'1', '2', '\0'};
            R_TEST(a, "12", chio::read_error::unexpected, 0);
            R_TEST(a, "\"12", chio::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), chio::read_error::escape_invalid, 1);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", chio::read_error::unexpected, 0);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "}", chio::read_error::unexpected, 0);
            R_TEST(a, "{", chio::read_error::unexpected, 0);
            R_TEST(a, "\"", chio::read_error::unexpected, 1);
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
            R_TEST(a, QS("1234"), chio::read_error::overflow, 0);
            R_TEST(a, QS("12\xF0\x9F\x8D\xBA"), chio::read_error::overflow, 0);
        }
        {   char16_t a[] = {u'1', u'2', u'\0'};
            R_TEST(a, "12", chio::read_error::unexpected, 0);
            R_TEST(a, "\"12", chio::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), chio::read_error::escape_invalid, 1);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", chio::read_error::unexpected, 0);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "}", chio::read_error::unexpected, 0);
            R_TEST(a, "{", chio::read_error::unexpected, 0);
            R_TEST(a, "\"", chio::read_error::unexpected, 1);
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
            R_TEST(a, QS("1234"), chio::read_error::overflow, 0);
        }
        {   char32_t a[] = {U'1', U'2', U'\0'};
            R_TEST(a, "12", chio::read_error::unexpected, 0);
            R_TEST(a, "\"12", chio::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), chio::read_error::escape_invalid, 1);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", chio::read_error::unexpected, 0);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "}", chio::read_error::unexpected, 0);
            R_TEST(a, "{", chio::read_error::unexpected, 0);
            R_TEST(a, "\"", chio::read_error::unexpected, 1);
        }
    // wchar_t[]
        R_TEST(L"", QS(""));
        {   wchar_t a[] = {L'1', L'2', L'3', L'\0', L'4', L'\0'};
            R_TEST(a, QS("123\\u00004"));
            W_TEST(QS("123\\u00004"), a);
        }
        {   wchar_t a[] = {L'1', L'2', L'3'};
            R_TEST(a, QS("1234"), chio::read_error::overflow, 0);
        }
        {   wchar_t a[] = {L'1', L'2', L'\0'};
            R_TEST(a, "12", chio::read_error::unexpected, 0);
            R_TEST(a, "\"12", chio::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), chio::read_error::escape_invalid, 1);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", chio::read_error::unexpected, 0);
            R_TEST(a, "", chio::read_error::unexpected, 0);
            R_TEST(a, "}", chio::read_error::unexpected, 0);
            R_TEST(a, "{", chio::read_error::unexpected, 0);
            R_TEST(a, "\"", chio::read_error::unexpected, 1);
        }
    // const char*
        R_TEST((const char*)"test", QS("test"));
        R_TEST((const char*)nullptr, "null");
        W_TEST(QS("test"), (const char*)"test");
        W_TEST("null", (const char*)nullptr);
        R_TEST((const char*)nullptr, "nil", chio::read_error::unexpected, 1);
        R_TEST((const char*)nullptr, "\"nil", chio::read_error::unexpected, 4);
    // const char16_t*
        R_TEST((const char16_t*)u"test", QS("test"));
        W_TEST(QS("test"), (const char16_t*)u"test");
        R_TEST((const char16_t*)nullptr, "\"", chio::read_error::unexpected, 1);
    // const char32_t*
        R_TEST((const char32_t*)U"test", QS("test"));
        W_TEST(QS("test"), (const char32_t*)U"test");
        R_TEST((const char32_t*)nullptr, "\"", chio::read_error::unexpected, 1);
    // const wchar_t*
        R_TEST((const wchar_t*)L"test", QS("test"));
        W_TEST(QS("test"), (const wchar_t*)L"test");
        R_TEST((const wchar_t*)nullptr, "\"", chio::read_error::unexpected, 1);
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
    R_TEST(Enum1::one, "", chio::read_error::unexpected, 0);
    R_TEST(Enum1::one, "o", chio::read_error::unexpected, 0);
    R_TEST(Enum1::one, "{", chio::read_error::unexpected, 1);
    R_TEST(Enum1::one, "{}", chio::read_error::unexpected, 0);
    R_TEST(Enum1::one, "{{}}", chio::read_error::unexpected, 0);
    R_TEST(Enum1::one, "{\"}", chio::read_error::unexpected, 3);
    R_TEST(Enum1::one, "{\"\"}", chio::read_error::unexpected, 0);
    R_TEST(Enum1::one, "{\"\\x\"}", chio::read_error::unexpected, 0);
    W_TEST("", Enum1::four, chio::write_error::argument_invalid);
TEST_END()

TEST_BEG(cxon::CXON<test::input_iterator_traits>) // enum
    R_TEST(Enum1::one, "one");
    W_TEST("one", Enum1::one);
    R_TEST(Enum1::two, "Two (2)");
    W_TEST("Two (2)", Enum1::two);
    R_TEST(Enum1::one, "", chio::read_error::unexpected, 0);
    R_TEST(Enum1::one, "o", chio::read_error::unexpected, 1);
    R_TEST(Enum1::one, "{", chio::read_error::unexpected, 1);
    R_TEST(Enum1::one, "{}", chio::read_error::unexpected, 2);
    R_TEST(Enum1::one, "{{}}", chio::read_error::unexpected, 4);
    R_TEST(Enum1::one, "{\"}", chio::read_error::unexpected,3);
    R_TEST(Enum1::one, "{\"\"}", chio::read_error::unexpected, 4);
    R_TEST(Enum1::one, "{\"\\x\"}", chio::read_error::unexpected, 6);
    W_TEST("", Enum1::four, chio::write_error::argument_invalid);
TEST_END()

TEST_BEG(cxon::JSON<>)
    R_TEST(Enum1::one, QS("one"));
    W_TEST(QS("one"), Enum1::one);
    R_TEST(Enum1::two, QS("Two (2)"));
    W_TEST(QS("Two (2)"), Enum1::two);
    R_TEST(Enum1::one, QS("noe"), chio::read_error::unexpected, 0);
    W_TEST("", Enum1::four, chio::write_error::argument_invalid);
TEST_END()

TEST_BEG(cxon::JSON<test::input_iterator_traits>)
    R_TEST(Enum1::one, QS("one"));
    W_TEST(QS("one"), Enum1::one);
    R_TEST(Enum1::two, QS("Two (2)"));
    W_TEST(QS("Two (2)"), Enum1::two);
    R_TEST(Enum1::one, QS("noe"), chio::read_error::unexpected, 5);
    W_TEST("", Enum1::four, chio::write_error::argument_invalid);
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
    R_TEST(Struct2(), "{\"x\": 1}", chio::read_error::unexpected, 1);
    R_TEST(Struct1(0, Enum1::one), "{A: 0, b: eno}", chio::read_error::unexpected, 10);
    R_TEST(Struct1(0, Enum1::one), "{A: 0, x: eno}", chio::read_error::unexpected, 7);
    R_TEST(Struct2(), "", chio::read_error::unexpected, 0);
    R_TEST(Struct2(), "}", chio::read_error::unexpected, 0);
    R_TEST(Struct2(), "{", chio::read_error::unexpected, 1);
TEST_END()

TEST_BEG(cxon::JSON<>)
    R_TEST(Struct1(0, Enum1::one), "{\"A\": 0, \"b\": \"one\"}");
    R_TEST(Struct1(0, Enum1::two), "{\"b\": \"Two (2)\", \"A\": 0}");
    R_TEST(Struct1(0, Enum1::three), "{\"b\": \"three\"}");
    W_TEST("{\"A\":0,\"b\":\"one\"}", Struct1(0, Enum1::one));
    R_TEST(Struct2(0, 1), "{\"a\": 0, \"B\": 1}");
    R_TEST(Struct2(), "{\"x\": 1}", chio::read_error::unexpected, 1);
    W_TEST("{\"a\":0,\"B\":1}", Struct2(0, 1));
    R_TEST(Struct1(), "{\"x\": 1}", chio::read_error::unexpected, 1);
    R_TEST(Struct1(), "{\"b\": \"one\", \"A\": x}", chio::read_error::integral_invalid, 18);
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
    R_TEST(Struct3(), "{a: 1, x: nil}", chio::read_error::unexpected, 7);
    R_TEST(Struct3(), "{a: 1, b: nil}", chio::read_error::unexpected, 10);
    R_TEST(Struct3(), "{a: 1, b: {a: x}}", chio::read_error::integral_invalid, 14);
TEST_END()

TEST_BEG(cxon::JSON<>)
    R_TEST(Struct3(1, new Struct3(2, nullptr)), "{\"a\": 1, \"b\": {\"a\": 2}}");
    R_TEST(Struct3(1, nullptr), "{\"a\": 1, \"b\": null}");
    W_TEST("{\"a\":1,\"b\":{\"a\":2,\"b\":null}}", Struct3(1, new Struct3(2, nullptr)));
    R_TEST(Struct3(), "{\"a\": 1, \"x\": nil}", chio::read_error::unexpected, 9);
    R_TEST(Struct3(), "{\"a\": 1, \"b\": nil}", chio::read_error::unexpected, 14);
    R_TEST(Struct3(), "{\"a\": 1, \"b\": {\"a\": x}}", chio::read_error::integral_invalid, 20);
TEST_END()


struct Struct4 {
    Struct4(int a = 0) : a(a) {}
    bool operator ==(const Struct4& t) const { return a == t.a; }

    template <typename X, typename II, typename C>
        static auto read_value(Struct4& t, II& b, II e, C& ctx) -> cxon::enable_for_t<X, cxon::CXON, bool> {
            return cxon::read_value<X>(t.a, b, e, ctx);
        }
    template <typename X, typename II, typename C>
        static auto read_value(Struct4& t, II& b, II e, C& ctx) -> cxon::enable_for_t<X, cxon::JSON, bool> {
            return cxon::read_value<X>(t.a, b, e, ctx);
        }
    template <typename X, typename OI, typename C>
        static bool write_value(OI& o, const Struct4& t, C& ctx) {
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

    template <typename X, typename II, typename C>
        bool read_value(II& b, II e, C& ctx) {
            return cxon::read_value<X>(a, b, e, ctx);
        }
    template <typename X, typename OI, typename C>
        auto write_value(OI& o, C& ctx) const -> cxon::enable_for_t<X, cxon::CXON, bool> {
            return cxon::write_value<X>(o, a, ctx);
        }
    template <typename X, typename OI, typename C>
        auto write_value(OI& o, C& ctx) const -> cxon::enable_for_t<X, cxon::JSON, bool> {
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
    template <typename X, typename II, typename C>
        inline  enable_for_t<X, CXON, bool> read_value(Struct6& t, II& b, II e, C& ctx) {
            return read_value<X>(t.a, b, e, ctx);
        }
    template <typename X, typename II, typename C>
        inline enable_for_t<X, JSON, bool> read_value(Struct6& t, II& b, II e, C& ctx) {
            return read_value<X>(t.a, b, e, ctx);
        }
    template <typename X, typename OI, typename C>
        inline bool write_value(OI& o, const Struct6& t, C& ctx) {
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
    R_TEST(Struct7(), "{x: 0}", chio::read_error::unexpected, 1);
TEST_END()

TEST_BEG(cxon::JSON<>) // macros inside
    R_TEST(Struct7(1, 2), "{\"a\": 1, \"b\": 2}");
    R_TEST(Struct7(3, 0), "{\"a\": 3}");
    R_TEST(Struct7(0, 6), "{\"b\": 6}");
    W_TEST("{\"a\":9,\"b\":10}", Struct7(9, 10));
    R_TEST(Struct7(), "{\"x\": 0}", chio::read_error::unexpected, 1);
TEST_END()


struct Struct8 {
    Struct8(int a = 0, int b = 0) : a(a), b(b) {}
    bool operator ==(const Struct8& t) const { return a == t.a && b == t.b; }

    template <typename X, typename II, typename C>
        static bool read_value(Struct8& t, II& i, II e, C& ctx) {
            using namespace cxon::chio::structs;
            static constexpr auto f = make_fields(
                make_field("a", &Struct8::a),
                make_field("b", &Struct8::b)
            );
            return read_fields<X>(t, f, i, e, ctx);
        }
    template <typename X, typename OI, typename C>
        static bool write_value(OI& o, const Struct8& t, C& ctx) {
            using namespace cxon::chio::structs;
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
    R_TEST(Struct8(1, 2), "{a: 1, x: 2}", chio::read_error::unexpected, 7);
    W_TEST("{a:3,b:4}", Struct8(3, 4));
TEST_END()

TEST_BEG(cxon::JSON<>)
    R_TEST(Struct8(1, 2), "{\"a\": 1, \"b\": 2}");
    R_TEST(Struct8(1, 2), "{\"a\": 1, \"x\": 2}", chio::read_error::unexpected, 9);
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
    R_TEST(Struct9(), "{a: 1, x: 3}", chio::read_error::unexpected, 7);
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
    R_TEST(Struct9(), "{\"a\": 1, \"x\": 3}", chio::read_error::unexpected, 9);
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
    R_TEST(Struct10(), "{x: 1}", chio::read_error::unexpected, 1);
    R_TEST(Struct10(), "{x\\ x: 1}", chio::read_error::unexpected, 1);
    R_TEST(Struct10(), "{skip6: {", chio::read_error::unexpected, 9);
    R_TEST(Struct10(), "{skip6: \"", chio::read_error::unexpected, 9);
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
    R_TEST(Struct10(), "{\"x\": 1}", chio::read_error::unexpected, 1);
    R_TEST(Struct10(), "{\"skip1\": {", chio::read_error::unexpected, 11);
    R_TEST(Struct10(), "{\"skip1\": [", chio::read_error::unexpected, 11);
    R_TEST(Struct10(), "{\"skip1\": \"", chio::read_error::unexpected, 11);
    W_TEST("{\"a\":1,\"* \\\"':*\":2}", Struct10(1, 2));
TEST_END()
