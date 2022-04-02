// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "test.hxx"

#include <string>

////////////////////////////////////////////////////////////////////////////////

TEST_BEG(arrays, cxon::JSON<>, "/core")
    // T[]
        {   int a[] = {1, 2, 3};
            R_TEST(a, "[1,2,3]");
            W_TEST("[1,2,3]", a);
        }
        {   int a[] = {1, 2, 0};
            R_TEST(a, "[1,2]");
            R_TEST(a, "", json::read_error::unexpected, 0);
            R_TEST(a, "]", json::read_error::unexpected, 0);
            R_TEST(a, "[", json::read_error::integral_invalid, 1);
        }
        {   int a[] = {1, 2, 3};
            R_TEST(a, "[1,2,3,4]", json::read_error::overflow, 0);
        }
    // char[]
        R_TEST("", QS(""));
        W_TEST(QS(""), "");
        R_TEST("123", QS("123"));
        {   char a[] = {'1', '2', '3', '\0'};
            R_TEST(a, QS("123"));
            R_TEST(a, QS("123\\u0000"));
            W_TEST(QS("123"), a);
            R_TEST(a, QS("x\xFFy"), json::read_error::character_invalid, 2);
        }
        {   char a[] = {'1', '2', '3', '\0', '4', '\0'};
            R_TEST(a, QS("123\\u00004"));
            W_TEST(QS("123\\u00004"), a);
        }
        {   char a[] = {'1', '2', '3'};
            R_TEST(a, QS("1234"), json::read_error::overflow, 1);
            R_TEST(a, QS("12\\u2728"), json::read_error::overflow, 3);
        }
        {   char a[] = {'1', '2', '\0'};
            R_TEST(a, "12", json::read_error::unexpected, 0);
            R_TEST(a, "\"12", json::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), json::read_error::escape_invalid, 1);
            R_TEST(a, "", json::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", json::read_error::unexpected, 0);
            R_TEST(a, "", json::read_error::unexpected, 0);
            R_TEST(a, "}", json::read_error::unexpected, 0);
            R_TEST(a, "{", json::read_error::unexpected, 0);
            R_TEST(a, "\"", json::read_error::unexpected, 1);
        }
    // wchar_t[]
        R_TEST(L"", QS(""));
        W_TEST(QS(""), L"");
        {   wchar_t a[] = {L'1', L'2', L'3', L'\0', L'4', L'\0'};
            R_TEST(a, QS("123\\u00004"));
            W_TEST(QS("123\\u00004"), a);
        }
        {   wchar_t a[] = {L'1', L'2', L'3'};
            R_TEST(a, QS("1234"), json::read_error::overflow, 5);
        }
        {   wchar_t a[] = {L'1', L'2', L'\0'};
            R_TEST(a, "12", json::read_error::unexpected, 0);
            R_TEST(a, "\"12", json::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), json::read_error::escape_invalid, 1);
            R_TEST(a, "", json::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", json::read_error::unexpected, 0);
            R_TEST(a, "", json::read_error::unexpected, 0);
            R_TEST(a, "}", json::read_error::unexpected, 0);
            R_TEST(a, "{", json::read_error::unexpected, 0);
            R_TEST(a, "\"", json::read_error::unexpected, 1);
        }
    // char8_t[]
#       if __cplusplus > 201703L /* C++20 */
            R_TEST(u8"", QS(""));
            W_TEST(QS(""), u8"");
            R_TEST(u8"123", QS("123"));
            {   char8_t a[] = {'1', '2', '3', '\0'};
                R_TEST(a, QS("123"));
                R_TEST(a, QS("123\\u0000"));
                W_TEST(QS("123"), a);
            }
            {   char8_t a[] = {'1', '2', '3', '\0', '4', '\0'};
                R_TEST(a, QS("123\\u00004"));
                W_TEST(QS("123\\u00004"), a);
            }
            {   char8_t a[] = {'1', '2', '3'};
                R_TEST(a, QS("1234"), json::read_error::overflow, 1);
                R_TEST(a, QS("12\\u2728"), json::read_error::overflow, 3);
            }
            {   char8_t a[] = {'1', '2', '\0'};
                R_TEST(a, "12", json::read_error::unexpected, 0);
                R_TEST(a, "\"12", json::read_error::unexpected, 3);
                R_TEST(a, QS("\\u001"), json::read_error::escape_invalid, 1);
                R_TEST(a, "", json::read_error::unexpected, 0);
                R_TEST(a, "{1,2,0}", json::read_error::unexpected, 0);
                R_TEST(a, "", json::read_error::unexpected, 0);
                R_TEST(a, "}", json::read_error::unexpected, 0);
                R_TEST(a, "{", json::read_error::unexpected, 0);
                R_TEST(a, "\"", json::read_error::unexpected, 1);
            }
#       endif
    // char16_t[]
        R_TEST(u"", QS(""));
        W_TEST(QS(""), u"");
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
            R_TEST(a, QS("1234"), json::read_error::overflow, 5);
            R_TEST(a, QS("12\xF0\x9F\x8D\xBA"), json::read_error::overflow, 7);
            R_TEST(a, QS("12\\udbff\\udfff"), json::read_error::overflow, 3);
        }
        {   char16_t a[] = {u'1', u'2', u'\0'};
            R_TEST(a, "12", json::read_error::unexpected, 0);
            R_TEST(a, "\"12", json::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), json::read_error::escape_invalid, 1);
            R_TEST(a, "", json::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", json::read_error::unexpected, 0);
            R_TEST(a, "", json::read_error::unexpected, 0);
            R_TEST(a, "}", json::read_error::unexpected, 0);
            R_TEST(a, "{", json::read_error::unexpected, 0);
            R_TEST(a, "\"", json::read_error::unexpected, 1);
        }
    // char32_t[]
        R_TEST(U"", QS(""));
        W_TEST(QS(""), U"");
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
            R_TEST(a, QS("1234"), json::read_error::overflow, 5);
        }
        {   char32_t a[] = {U'1', U'2', U'\0'};
            R_TEST(a, "12", json::read_error::unexpected, 0);
            R_TEST(a, "\"12", json::read_error::unexpected, 3);
            R_TEST(a, QS("\\u001"), json::read_error::escape_invalid, 1);
            R_TEST(a, "", json::read_error::unexpected, 0);
            R_TEST(a, "{1,2,0}", json::read_error::unexpected, 0);
            R_TEST(a, "", json::read_error::unexpected, 0);
            R_TEST(a, "}", json::read_error::unexpected, 0);
            R_TEST(a, "{", json::read_error::unexpected, 0);
            R_TEST(a, "\"", json::read_error::unexpected, 1);
        }
TEST_END()


TEST_BEG(pointers, cxon::JSON<>, "/core")
    // T*
        {   int x = 42;
            R_TEST(&x, "42");
            R_TEST((int*)nullptr, "null");
            W_TEST("42", &x);
            W_TEST("null", (int*)nullptr);
            R_TEST((int*)nullptr, "nil", json::read_error::unexpected, 0);
        }
    // const T*
        {   int const x = 42;
            R_TEST(&x, "42");
            R_TEST((const int*)nullptr, "null");
            W_TEST("42", &x);
            W_TEST("null", (const int*)nullptr);
            R_TEST((const int*)nullptr, "nil", json::read_error::unexpected, 0);
        }
    // const char*
        R_TEST((const char*)"test", QS("test"));
        R_TEST((const char*)nullptr, "null");
        W_TEST(QS("test"), (const char*)"test");
        W_TEST("null", (const char*)nullptr);
        R_TEST((const char*)nullptr, "nil", json::read_error::unexpected, 1);
        R_TEST((const char*)nullptr, "\"nil", json::read_error::unexpected, 4);
    // const wchar_t*
        R_TEST((const wchar_t*)L"test", QS("test"));
        W_TEST(QS("test"), (const wchar_t*)L"test");
        R_TEST((const wchar_t*)nullptr, "\"", json::read_error::unexpected, 1);
    // const char8_t*
#       if __cplusplus > 201703L /* C++20 */
            R_TEST((const char8_t*)"test", QS("test"));
            R_TEST((const char8_t*)nullptr, "null");
            W_TEST(QS("test"), (const char8_t*)u8"test");
            W_TEST("null", (const char8_t*)nullptr);
            R_TEST((const char8_t*)nullptr, "nil", json::read_error::unexpected, 1);
            R_TEST((const char8_t*)nullptr, "\"nil", json::read_error::unexpected, 4);
#       endif
    // const char16_t*
        R_TEST((const char16_t*)u"test", QS("test"));
        W_TEST(QS("test"), (const char16_t*)u"test");
        R_TEST((const char16_t*)nullptr, "\"", json::read_error::unexpected, 1);
    // const char32_t*
        R_TEST((const char32_t*)U"test", QS("test"));
        W_TEST(QS("test"), (const char32_t*)U"test");
        R_TEST((const char32_t*)nullptr, "\"", json::read_error::unexpected, 1);
    // char*
        R_TEST((char*)"test", QS("test"));
        R_TEST((char*)nullptr, "null");
        W_TEST(QS("test"), (char*)"test");
        W_TEST("null", (char*)nullptr);
    // wchar_t*
        R_TEST((wchar_t*)L"test", QS("test"));
        W_TEST(QS("test"), (wchar_t*)L"test");
    // char8_t*
#       if __cplusplus > 201703L /* C++20 */
            R_TEST((char8_t*)"test", QS("test"));
            R_TEST((char8_t*)nullptr, "null");
            W_TEST(QS("test"), (char8_t*)u8"test");
            W_TEST("null", (char8_t*)nullptr);
#       endif
    // char16_t*
        R_TEST((char16_t*)u"test", QS("test"));
        W_TEST(QS("test"), (char16_t*)u"test");
    // char32_t*
        R_TEST((char32_t*)U"test", QS("test"));
        W_TEST(QS("test"), (char32_t*)U"test");
TEST_END()


namespace {
    enum Enum1 { one, two, three, four };
}

CXON_JSON_ENM(Enum1,
    CXON_JSON_ENM_VALUE_ASIS(one),
    CXON_JSON_ENM_VALUE_NAME("Two (2)", two),
    CXON_JSON_ENM_VALUE_ASIS(three)
)

TEST_BEG(enum, cxon::JSON<>, "/core")
    R_TEST(Enum1::one, QS("one"));
    W_TEST(QS("one"), Enum1::one);
    R_TEST(Enum1::two, QS("Two (2)"));
    W_TEST(QS("Two (2)"), Enum1::two);
    R_TEST(Enum1::one, QS("noe"), json::read_error::unexpected, 0);
    R_TEST(Enum1::one, "one", json::read_error::unexpected, 0);
    W_TEST("", Enum1::four, json::write_error::argument_invalid);
TEST_END()

TEST_BEG(enum_input_iterator, cxon::JSON<cxon::test::input_iterator_traits>, "/core")
    R_TEST(Enum1::one, QS("one"));
    W_TEST(QS("one"), Enum1::one);
    R_TEST(Enum1::two, QS("Two (2)"));
    W_TEST(QS("Two (2)"), Enum1::two);
    R_TEST(Enum1::one, QS("noe"), json::read_error::unexpected, 5);
    R_TEST(Enum1::one, "one", json::read_error::unexpected, 0);
    W_TEST("", Enum1::four, json::write_error::argument_invalid);
TEST_END()


namespace {

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

}

CXON_JSON_CLS_READ(Struct1,
    CXON_JSON_CLS_FIELD_NAME("A", a),
    CXON_JSON_CLS_FIELD_ASIS(b)
)
CXON_JSON_CLS_WRITE(Struct1,
    CXON_JSON_CLS_FIELD_NAME("A", a),
    CXON_JSON_CLS_FIELD_ASIS(b)
)

CXON_JSON_CLS(Struct2,
    CXON_JSON_CLS_FIELD_ASIS(a),
    CXON_JSON_CLS_FIELD_NAME("B", b)
)

TEST_BEG(struct_1, cxon::JSON<>, "/core")
    R_TEST(Struct1(0, Enum1::one), "{\"A\": 0, \"b\": \"one\"}");
    R_TEST(Struct1(0, Enum1::two), "{\"b\": \"Two (2)\", \"A\": 0}");
    R_TEST(Struct1(0, Enum1::three), "{\"b\": \"three\"}");
    W_TEST("{\"A\":0,\"b\":\"one\"}", Struct1(0, Enum1::one));
    R_TEST(Struct2(0, 1), "{\"a\": 0, \"B\": 1}");
    R_TEST(Struct2(), "{\"x\": 1}", json::read_error::unexpected, 1);
    W_TEST("{\"a\":0,\"B\":1}", Struct2(0, 1));
    R_TEST(Struct1(), "{\"x\": 1}", json::read_error::unexpected, 1);
    R_TEST(Struct1(), "{\"b\": \"one\", \"A\": x}", json::read_error::integral_invalid, 18);
    R_TEST(Struct1(0, Enum1::one), "{\"A\": 0, \"b\": \"eno\"}", json::read_error::unexpected, 14);
    R_TEST(Struct1(0, Enum1::one), "{\"A\": 0, \"x\": \"eno\"}", json::read_error::unexpected, 9);
    R_TEST(Struct2(), "", json::read_error::unexpected, 0);
    R_TEST(Struct2(), "}", json::read_error::unexpected, 0);
    R_TEST(Struct2(), "{", json::read_error::unexpected, 1);
TEST_END()


namespace {

    struct Struct3 {
        int a;
        Struct3* b;
        Struct3(int a = 0, Struct3* b = nullptr) : a(a), b(b) {}
        bool operator ==(const Struct3& t) const { return a == t.a && ((!b && !t.b) || (b && t.b && *b == *t.b)); }
    };

}

CXON_JSON_CLS(Struct3,
    CXON_JSON_CLS_FIELD_ASIS(a),
    CXON_JSON_CLS_FIELD_ASIS(b)
)

TEST_BEG(struct_3, cxon::JSON<>, "/core")
    R_TEST(Struct3(1, new Struct3(2, nullptr)), "{\"a\": 1, \"b\": {\"a\": 2}}");
    R_TEST(Struct3(1, nullptr), "{\"a\": 1, \"b\": null}");
    W_TEST("{\"a\":1,\"b\":{\"a\":2,\"b\":null}}", Struct3(1, new Struct3(2, nullptr)));
    R_TEST(Struct3(), "{\"a\": 1, \"x\": nil}", json::read_error::unexpected, 9);
    R_TEST(Struct3(), "{\"a\": 1, \"b\": nil}", json::read_error::unexpected, 14);
    R_TEST(Struct3(), "{\"a\": 1, \"b\": {\"a\": x}}", json::read_error::integral_invalid, 20);
TEST_END()


namespace {

    struct Struct4 {
        Struct4(int a = 0) : a(a) {}
        bool operator ==(const Struct4& t) const { return a == t.a; }

        template <typename X, typename II, typename C>
            static auto read_value(Struct4& t, II& b, II e, C& ctx) -> cxon::enable_for_t<X, cxon::JSON> {
                return cxon::read_value<X>(t.a, b, e, ctx);
            }
        template <typename X, typename OI, typename C>
            static bool write_value(OI& o, const Struct4& t, C& ctx) {
                return cxon::write_value<X>(o, t.a, ctx);
            }

    private:
        int a;
    };

}

TEST_BEG(struct_4, cxon::JSON<>, "/core")
    R_TEST(Struct4(1), "1");
    W_TEST("3", Struct4(3));
TEST_END()


namespace {

    struct Struct5 {
        Struct5(int a = 0) : a(a) {}
        bool operator ==(const Struct5& t) const { return a == t.a; }

        template <typename X, typename II, typename C>
            bool read_value(II& b, II e, C& ctx) {
                return cxon::read_value<X>(a, b, e, ctx);
            }
        template <typename X, typename OI, typename C>
            auto write_value(OI& o, C& ctx) const -> cxon::enable_for_t<X, cxon::JSON> {
                return cxon::write_value<X>(o, a, ctx);
            }

    private:
        int a;
    };

}

TEST_BEG(struct_5, cxon::JSON<>, "/core")
    R_TEST(Struct5(1), "1");
    W_TEST("3", Struct5(3));
TEST_END()


namespace {

    struct Struct6 {
        int a;
        Struct6(int a = 0) : a(a) {}
        bool operator ==(const Struct6& t) const { return a == t.a; }
    };

}

namespace cxon {
    template <typename X, typename II, typename C>
        inline enable_for_t<X, JSON> read_value(Struct6& t, II& b, II e, C& ctx) {
            return read_value<X>(t.a, b, e, ctx);
        }
    template <typename X, typename OI, typename C>
        inline bool write_value(OI& o, const Struct6& t, C& ctx) {
            return write_value<X>(o, t.a, ctx);
        }
}

TEST_BEG(struct_6, cxon::JSON<>, "/core")
    R_TEST(Struct6(1), "1");
    W_TEST("3", Struct6(3));
TEST_END()


namespace {

    struct Struct7 {
        Struct7(int a = 0, int b = 0) : a(a), b(b) {}
        bool operator ==(const Struct7& t) const { return a == t.a && b == t.b; }

        CXON_JSON_CLS_MEMBER(Struct7,
            CXON_JSON_CLS_FIELD_ASIS(a),
            CXON_JSON_CLS_FIELD_ASIS(b)
        )

    private:
        int a;
        int b;
    };

}

TEST_BEG(struct_7, cxon::JSON<>, "/core") // macros inside
    R_TEST(Struct7(1, 2), "{\"a\": 1, \"b\": 2}");
    R_TEST(Struct7(3, 0), "{\"a\": 3}");
    R_TEST(Struct7(0, 6), "{\"b\": 6}");
    W_TEST("{\"a\":9,\"b\":10}", Struct7(9, 10));
    R_TEST(Struct7(), "{\"x\": 0}", json::read_error::unexpected, 1);
TEST_END()


namespace {

    struct Struct8 {
        Struct8(int a = 0, int b = 0) : a(a), b(b) {}
        bool operator ==(const Struct8& t) const { return a == t.a && b == t.b; }

        template <typename X, typename II, typename C>
            static bool read_value(Struct8& t, II& i, II e, C& ctx) {
                using namespace cxon::json::cls;
                static auto const f = make_fields(
                    make_field<Struct8>("a", &Struct8::a),
                    make_field<Struct8>("b", &Struct8::b)
                );
                return read_fields<X>(t, f, i, e, ctx);
            }
        template <typename X, typename OI, typename C>
            static bool write_value(OI& o, const Struct8& t, C& ctx) {
                using namespace cxon::json::cls;
                static auto const f = make_fields(
                    make_field<Struct8>("a", &Struct8::a),
                    make_field<Struct8>("b", &Struct8::b)
                );
                return write_fields<X>(o, t, f, ctx);
            }

    private:
        int a;
        int b;
    };

}

TEST_BEG(struct_8, cxon::JSON<>, "/core")
    R_TEST(Struct8(1, 2), "{\"a\": 1, \"b\": 2}");
    R_TEST(Struct8(1, 2), "{\"a\": 1, \"x\": 2}", json::read_error::unexpected, 9);
    W_TEST("{\"a\":3,\"b\":4}", Struct8(3, 4));
TEST_END()


namespace {

    struct Struct9 {
        static int a;
        static int const b;

        bool operator ==(const Struct9&) const { return true; }

        CXON_JSON_CLS_READ_MEMBER(Struct9,
            CXON_JSON_CLS_FIELD_ASIS(a)
        )
        CXON_JSON_CLS_WRITE_MEMBER(Struct9,
            CXON_JSON_CLS_FIELD_ASIS(a),
            CXON_JSON_CLS_FIELD_ASIS(b)
        )
    };
    int Struct9::a = 0;
    int const Struct9::b = 3;

}

TEST_BEG(struct_9, cxon::JSON<>, "/core") // static field
    R_TEST(Struct9(), "{\"a\":0}");
    W_TEST("{\"a\":0,\"b\":3}", Struct9());
    R_TEST(Struct9(), "{\"a\": 1}");
    W_TEST("{\"a\":1,\"b\":3}", Struct9());
    R_TEST(Struct9(), "{}");
    W_TEST("{\"a\":1,\"b\":3}", Struct9());
    R_TEST(Struct9(), "{\"a\": 3}");
    W_TEST("{\"a\":3,\"b\":3}", Struct9());
    R_TEST(Struct9(), "{\"a\": 1, \"x\": 3}", json::read_error::unexpected, 9);
TEST_END()


namespace {

    struct Struct10 {
        int a;
        int b;
        Struct10(int a = 0, int b = 0) : a(a), b(b) {}
        bool operator ==(const Struct10& t) const { return a == t.a && b == t.b; }
    };

}

CXON_JSON_CLS_READ(Struct10,
    CXON_JSON_CLS_FIELD_ASIS(a),
    CXON_JSON_CLS_FIELD_SKIP("skip1"),
    CXON_JSON_CLS_FIELD_SKIP("skip2"),
    CXON_JSON_CLS_FIELD_SKIP("skip3"),
    CXON_JSON_CLS_FIELD_SKIP("skip4"),
    CXON_JSON_CLS_FIELD_SKIP("skip5"),
    CXON_JSON_CLS_FIELD_SKIP("skip6")
)
CXON_JSON_CLS_WRITE(Struct10,
    CXON_JSON_CLS_FIELD_ASIS(a),
    CXON_JSON_CLS_FIELD_NAME("* \"':*", b)
)

TEST_BEG(struct_10, cxon::JSON<>, "/core") // skip field
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
    R_TEST(Struct10(1), "{\"skip6\": [1, 2, 3], \"a\": 1}");
    R_TEST(Struct10(), "{\"x\": 1}", json::read_error::unexpected, 1);
    R_TEST(Struct10(), "{\"skip1\": {", json::read_error::unexpected, 11);
    R_TEST(Struct10(), "{\"skip1\": [", json::read_error::unexpected, 11);
    R_TEST(Struct10(), "{\"skip1\": \"", json::read_error::unexpected, 11);
    W_TEST("{\"a\":1,\"* \\\"':*\":2}", Struct10(1, 2));
TEST_END()


namespace {

    struct Struct11 {
        using sink = cxon::cio::val::sink<std::string>;
        int a;
        sink b;
        Struct11(int a = 0, sink b = sink{}) : a(a), b(b) {}
        bool operator ==(const Struct11& t) const { return a == t.a && b.value == t.b.value; }
    };

}

CXON_JSON_CLS_READ(Struct11,
    CXON_JSON_CLS_FIELD_ASIS(a),
    CXON_JSON_CLS_FIELD_ASIS(b)
)
CXON_JSON_CLS_WRITE(Struct11,
    CXON_JSON_CLS_FIELD_ASIS_DFLT(a, self.a == 0),
    CXON_JSON_CLS_FIELD_ASIS(b)
)

TEST_BEG(struct_11, cxon::JSON<>, "/core") // skip field
    R_TEST(Struct11(1, {" [1, 2, 3]"}), "{\"a\": 1, \"b\": [1, 2, 3]}");
    R_TEST(Struct11(1, {" \"1, 2, 3\""}), "{\"a\": 1, \"b\": \"1, 2, 3\"}");
    R_TEST(Struct11(1, {" {\"x\": 1, \"y\": 2}"}), "{\"a\": 1, \"b\": {\"x\": 1, \"y\": 2}}");
    W_TEST("{\"a\":1,\"b\":[1, 2, 3]}", Struct11(1, {"[1, 2, 3]"}));
    W_TEST("{\"b\":[1, 2, 3]}", Struct11(0, {"[1, 2, 3]"}));
TEST_END()


namespace {

    struct Struct12 {
        int x;
        int y;
        int z;
        bool operator ==(const Struct12& t) const { return x == t.x && y == t.y && z == t.z; }
    };

}

CXON_JSON_CLS(Struct12,
    CXON_JSON_CLS_FIELD_ASIS_DFLT(x, self.x == 0),
    CXON_JSON_CLS_FIELD_ASIS_DFLT(y, self.y == 0),
    CXON_JSON_CLS_FIELD_ASIS_DFLT(z, self.z == 0)
)

TEST_BEG(struct_12, cxon::JSON<>, "/core") // defaults
    W_TEST("{\"x\":1,\"y\":1,\"z\":1}", Struct12 {1, 1, 1});
    W_TEST("{\"y\":1,\"z\":1}", Struct12 {0, 1, 1});
    W_TEST("{\"x\":1,\"z\":1}", Struct12 {1, 0, 1});
    W_TEST("{\"x\":1,\"y\":1}", Struct12 {1, 1, 0});
    W_TEST("{\"x\":1}", Struct12 {1, 0, 0});
    W_TEST("{\"y\":1}", Struct12 {0, 1, 0});
    W_TEST("{\"z\":1}", Struct12 {0, 0, 1});
    W_TEST("{}", Struct12 {0, 0, 0});
TEST_END()


TEST_BEG(sink, cxon::JSON<>, "/core") // sink
    {   char o[2];
        auto const r = to_bytes(std::begin(o), std::end(o), cio::val::sink<std::string>{"\01\02\03"});
        TEST_CHECK(!r && r.ec == json::write_error::output_failure);
    }
TEST_END()
