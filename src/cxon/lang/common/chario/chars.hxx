// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHARIO_CHARS_HXX_
#define CXON_CHARIO_CHARS_HXX_

#include "chario.hxx"
#include "charclass.hxx"

namespace cxon { namespace chars { // character conversion: read
    
    namespace io = cxon::chario;
    using namespace cxon::charclass;

    template <typename II, typename IsX>
        inline unsigned consume(char* f, const char* l, II& i, II e, IsX is_x) {
            unsigned n = 0;
                for (char c = io::peek(i, e); f != l && is_x(c); c = io::next(i, e), ++f, ++n) *f = c;
            return n;
        }

    inline char32_t oct_to_utf32(const char* b, const char* e) noexcept {
        static constexpr char32_t oct_to_utf32_[] = { 
            /*  0*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 16*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 32*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 48*/ 00, 1, 2, 3, 4, 5, 6, 7,00,00,00,00,00,00,00,00
        };
        CXON_ASSERT(b && b < e, "unexpected");
        char32_t c = 0; 
            for ( ; b != e; ++b) c = (c << 3) | oct_to_utf32_[(unsigned char)*b];
        return c; 
    }

    inline char32_t hex_to_utf32(const char* b, const char* e) noexcept {
        static constexpr char32_t hex_to_utf32_[] = { 
            /*  0*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 16*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 32*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 48*/ 00, 1, 2, 3, 4, 5, 6, 7, 8, 9,00,00,00,00,00,00,
            /* 64*/ 00,10,11,12,13,14,15,00,00,00,00,00,00,00,00,00,
            /* 80*/ 00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            /* 96*/ 00,10,11,12,13,14,15,00,00,00,00,00,00,00,00,00
        };
        CXON_ASSERT(b && b < e, "unexpected");
        char32_t c = 0; 
            for ( ; b != e; ++b) c = (c << 4) | hex_to_utf32_[(unsigned char)*b];
        return c; 
    }

#   define CXON_ASS_U(t) if (!is<X>::digit16(io::next(i, e))) return 0xFFFFFFFF; t = io::peek(i, e)
        template <typename X>
            struct esc_to {
                template <typename II>
                    static char32_t utf32(II& i, II e) {
                        switch (io::peek(i, e)) {
                            case '\'': return ++i, '\'';
                            case '\"': return ++i, '\"';
                            case '\?': return ++i, '\?';
                            case '\\': return ++i, '\\';
                            case 'a' : return ++i, '\a';
                            case 'b' : return ++i, '\b';
                            case 'f' : return ++i, '\f';
                            case 'n' : return ++i, '\n';
                            case 'r' : return ++i, '\r';
                            case 't' : return ++i, '\t';
                            case 'v' : return ++i, '\v';
                            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                                char h[3]; // arbitrary length
                                    unsigned const w = consume(h, h + sizeof(h), i, e, is<X>::digit8);
                                        if (!w) return 0xFFFFFFFF;
                                return oct_to_utf32(h, h + w);
                            }
                            case 'x' : {
                                char h[2]; // arbitrary length
                                    unsigned const w = consume(h, h + sizeof(h), ++i, e, is<X>::digit16);
                                        if (!w) return 0xFFFFFFFF;
                                return hex_to_utf32(h, h + w);
                            }
                            case 'u' : {
                                char h[4];
                                    CXON_ASS_U(h[0]); CXON_ASS_U(h[1]); CXON_ASS_U(h[2]); CXON_ASS_U(h[3]);
                                return ++i, hex_to_utf32(h, h + sizeof(h));
                            }
                            case 'U' : {
                                char h[8];
                                    CXON_ASS_U(h[0]); CXON_ASS_U(h[1]); CXON_ASS_U(h[2]); CXON_ASS_U(h[3]);
                                    CXON_ASS_U(h[4]); CXON_ASS_U(h[5]); CXON_ASS_U(h[6]); CXON_ASS_U(h[7]);
                                return ++i, hex_to_utf32(h, h + sizeof(h));
                            }
                            default: return 0xFFFFFFFF;
                        }
                    }
            };
#   undef CXON_ASS_U

#   define CXON_ASS_U(t) if (!is<JSON<X>>::digit16(io::next(i, e))) return 0xFFFFFFFF; t = io::peek(i, e)
        template <typename X>
            struct esc_to<JSON<X>> {
                template <typename II>
                    static char32_t utf32(II& i, II e) {
                        switch (io::peek(i, e)) {
                            case '\"': return ++i, '\"';
                            case '\\': return ++i, '\\';
                            case '/' : return ++i, '/';
                            case 'b' : return ++i, '\b';
                            case 'f' : return ++i, '\f';
                            case 'n' : return ++i, '\n';
                            case 'r' : return ++i, '\r';
                            case 't' : return ++i, '\t';
                            case 'u' : {
                                char h[4];
                                    CXON_ASS_U(h[0]); CXON_ASS_U(h[1]); CXON_ASS_U(h[2]); CXON_ASS_U(h[3]);
                                return ++i, hex_to_utf32(h, h + sizeof(h));
                            }
                            default: return 0xFFFFFFFF;
                        }
                    }
        };
#   undef CXON_ASS_U

    template <typename X, typename II, typename Cx>
        inline char32_t esc_to_utf32(II& i, II e, Cx& cx) {
            char32_t const c32 = esc_to<X>::utf32(i, e);
                if (c32 == 0xFFFFFFFF) return cx|read_error::escape_invalid, 0xFFFFFFFF;
            if (c32 < 0xD800 || c32 > 0xDBFF) return c32;
            // surrogate
                if (io::peek(i, e) != '\\') return cx|read_error::surrogate_invalid, 0xFFFFFFFF;
            char32_t const s32 = (++i, esc_to<X>::utf32(i, e));
                if (s32 < 0xDC00 || s32 > 0xDFFF)
                    return (s32 == 0xFFFFFFFF ? cx|read_error::escape_invalid : cx|read_error::surrogate_invalid), 0xFFFFFFFF;
            return char32_t(0x10000 + (((c32 - 0xD800) << 10) | (s32 - 0xDC00)));
        }

#   define CXON_EXPECT(c) if (!(c)) return cx|read_error::character_invalid, 0xFFFFFFFF

        template <typename X, typename II, typename Cx>
            static char32_t str_to_utf32(II& i, II e, Cx& cx) {
                char32_t const c0 = io::peek(i, e);
                if (c0 != '\\') {
                    if ((c0 & 0x80) == 0)
                        return ++i, c0;
                    if ((c0 & 0xE0) == 0xC0) {
                        char32_t const c1 = io::next(i, e); CXON_EXPECT((c1 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x1F) << 6) | (c1 & 0x3F);
                    }
                    if ((c0 & 0xF0) == 0xE0) {
                        char32_t const c1 = io::next(i, e); CXON_EXPECT((c1 & 0xC0) == 0x80);
                        char32_t const c2 = io::next(i, e); CXON_EXPECT((c2 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
                    }
                    if ((c0 & 0xF8) == 0xF0) {
                        char32_t const c1 = io::next(i, e); CXON_EXPECT((c1 & 0xC0) == 0x80);
                        char32_t const c2 = io::next(i, e); CXON_EXPECT((c2 & 0xC0) == 0x80);
                        char32_t const c3 = io::next(i, e); CXON_EXPECT((c3 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                    }
                    CXON_EXPECT(false);
                }
                return esc_to_utf32<X>(++i, e, cx);
            }

#   undef CXON_EXPECT

    template <typename = void> // instantiate only if used
        static int utf32_to_utf8(char (&t)[4], char32_t c32) noexcept {
            if (c32 < 0x80)  // 0XXX XXXX
                return t[0] = char(c32), 1;
            if (c32 < 0x800) { // 110XXXXX
                t[0] = char(0xC0 | (c32 >> 6));
                t[1] = char(0x80 | (0x3F & c32));
                return 2;
            }
            if (c32 < 0x10000) { // 1110XXXX
                // error: 0xFFFE || 0xFFFF // not a char?
                    if (c32 >= 0xD800 && c32 <= 0xDBFF) return 0;
                t[0] = char(0xE0 | (c32 >> 12));
                t[1] = char(0x80 | (0x3F & (c32 >> 6)));
                t[2] = char(0x80 | (0x3F & c32));
                return 3;
            }
            if (c32 < 0x200000) { // 11110XXX
                    if (c32 > 0x10FFFF) return 0;
                t[0] = char(0xF0 | (c32 >> 18));
                t[1] = char(0x80 | (0x3F & (c32 >> 12)));
                t[2] = char(0x80 | (0x3F & (c32 >> 6)));
                t[3] = char(0x80 | (0x3F & c32));
                return 4;
            }
            return 0;
        }

}}  // cxon::bits character conversion: read

namespace cxon { namespace chars { // character conversion: write

    template <typename X, typename T>
        struct encode;

    template <typename X>
        struct encode<X, char> {
            template <typename O, typename Cx>
                static bool value(O& o, char c, Cx& cx) {
                    static constexpr char const* encode_[] = {
                        /*  0*/  "\\0"  , "\\1"  , "\\2"  , "\\3"  , "\\4"  , "\\5"  , "\\6"  , "\\7",
                        /*  8*/  "\\b"  , "\\t"  , "\\n"  , "\\v"  , "\\f"  , "\\r"  , "\\16" , "\\17",
                        /* 16*/  "\\20" , "\\21" , "\\22" , "\\23" , "\\24" , "\\25" , "\\26" , "\\27",
                        /* 24*/  "\\30" , "\\31" , "\\32" , "\\33" , "\\34" , "\\35" , "\\36" , "\\37",
                        /* 32*/  " "    , "!"    , "\\\"" , "#"    , "$"    , "%"    , "&"    , "'"
                    };
                    if ('"' != X::string::end && X::string::end == c)   return io::poke<X>(o, '\\', cx) && io::poke<X>(o, c, cx);
                    else if (X::string::end != '"' && '"' == c)         return io::poke<X>(o, c, cx);
                    else if (0 <= c && c <= 39)                         return io::poke<X>(o, encode_[(unsigned char)c], cx);
                    else if (c == '\\')                                 return io::poke<X>(o, "\\\\", cx);
                    else                                                return io::poke<X>(o, c, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool value(O& o, II i, II, Cx& cx) {
                    return value(o, *i, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool range(O& o, II i, II e, Cx& cx) {
                    for ( ; i != e && value(o, *i, cx); ++i) ;
                    return i == e;
                }
        };
    template <typename X>
        struct encode<JSON<X>, char> {
            template <typename O, typename Cx>
                static bool value(O& o, char c, Cx& cx) {
                    static constexpr char const*const encode_[] = {
                        /*  0*/  "\\u0000", "\\u0001", "\\u0002", "\\u0003", "\\u0004", "\\u0005", "\\u0006", "\\u0007",
                        /*  8*/  "\\b"    , "\\t"    , "\\n"    , "\\u000b", "\\f"    , "\\r"    , "\\u000e", "\\u000f",
                        /* 16*/  "\\u0010", "\\u0011", "\\u0012", "\\u0013", "\\u0014", "\\u0015", "\\u0016", "\\u0017",
                        /* 24*/  "\\u0018", "\\u0019", "\\u001a", "\\u001b", "\\u001c", "\\u001d", "\\u001e", "\\u001f",
                        /* 32*/  " "      , "!"      , "\\\""   , "#"      , "$"      , "%"      , "&"      , "'"
                    };
                    if (0 <= c && c <= 39)  return io::poke<JSON<X>>(o, encode_[(unsigned char)c], cx);
                    else if (c == '\\')     return io::poke<JSON<X>>(o, "\\\\", cx);
                    else                    return io::poke<JSON<X>>(o, c, cx);
                }
            template <typename O, typename II, typename Cx, typename S = X>
                static auto value(O& o, II i, II, Cx& cx)       -> enable_if_t<!S::strict_js, bool> {
                    return value(o, *i, cx);
                }
            template <typename O, typename II, typename Cx, typename S = X>
                // escape U+2028 LINE SEPARATOR and U+2029 PARAGRAPH SEPARATOR are invalid JavaScript
                static auto value(O& o, II& i, II e, Cx& cx)    -> enable_if_t< S::strict_js, bool> {
                    if (*i == '\xE2') {
                        ++i; CXON_ASSERT(i != e, "unexpected");
                        if (*i == '\x80') {
                            ++i; CXON_ASSERT(i != e, "unexpected");
                            switch (*i) {
                                case '\xA8':    return io::poke<JSON<X>>(o, "\\u2028", cx);
                                case '\xA9':    return io::poke<JSON<X>>(o, "\\u2029", cx);
                                default:        return value(o, '\xE2', cx) && value(o, '\x80', cx) && value(o, *i, cx); } }
                        else                    return value(o, '\xE2', cx) && value(o, *i, cx); }
                    else                        return value(o, *i, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool range(O& o, II i, II e, Cx& cx) {
                    for ( ; i != e && value(o, i, e, cx); ++i) ;
                    return i == e;
                }
        };

    template <typename X>
        struct encode<X, char16_t> {
            template <typename O, typename Cx>
                static bool value(O& o, char16_t c, Cx& cx) {
                    CXON_ASSERT(c < 0xD800 || c > 0xDBFF, "unexpected surrogate");
                    return encode<X, char32_t>::value(o, c, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool value(O& o, II& i, II e, Cx& cx) {
                    char32_t c32 = *i;
                        if (c32 >= 0xD800 && c32 <= 0xDBFF) { // surrogate
                            ++i;                        CXON_ASSERT(i != e, "invalid surrogate");
                            char32_t const s32 = *i;    CXON_ASSERT(s32 >= 0xDC00 && s32 <= 0xDFFF, "invalid surrogate");
                            c32 = char32_t(0x10000 + (((c32 - 0xD800) << 10) | (s32 - 0xDC00)));
                        }
                    return encode<X, char32_t>::value(o, c32, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool range(O& o, II i, II e, Cx& cx) {
                    for ( ; i != e && value(o, i, e, cx); ++i) ;
                    return i == e;
                }
        };

    template <typename X>
        struct encode<X, char32_t> {
            template <typename O, typename Cx>
                static bool value(O& o, char32_t c, Cx& cx) {
                    if (c > 0x7F) {
                            char b[4]; int const n = utf32_to_utf8(b, c);
                            return io::poke<X>(o, b, n, cx);
                    }
                    else    return encode<X, char>::value(o, char(c), cx);
                }
            template <typename O, typename II, typename Cx>
                static bool value(O& o, II i, II, Cx& cx) {
                    return value(o, *i, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool range(O& o, II i, II e, Cx& cx) {
                    for ( ; i != e && value(o, *i, cx); ++i) ;
                    return i == e;
                }
        };
    template <typename X>
        struct encode<JSON<X>, char32_t> {
            template <typename O, typename Cx, typename S = X>
                static auto value(O& o, char32_t c, Cx& cx) -> enable_if_t<!S::strict_js, bool> {
                    if (c > 0x7F) {
                            char b[4]; int const n = utf32_to_utf8(b, c);
                            return io::poke<JSON<X>>(o, b, n, cx);
                    }
                    else    return encode<JSON<X>, char>::value(o, char(c), cx);
                }
            template <typename O, typename Cx, typename S = X>
                static auto value(O& o, char32_t c, Cx& cx) -> enable_if_t< S::strict_js, bool> {
                    if (c > 0x7F) {
                            if (c == 0x2028) return io::poke<JSON<X>>(o, "\\u2028", cx);
                            if (c == 0x2029) return io::poke<JSON<X>>(o, "\\u2029", cx);
                            char b[4]; int const n = utf32_to_utf8(b, c);
                            return io::poke<JSON<X>>(o, b, n, cx);
                    }
                    else    return encode<JSON<X>, char>::value(o, char(c), cx);
                }
            template <typename O, typename II, typename Cx>
                static bool value(O& o, II i, II, Cx& cx) {
                    return value(o, *i, cx);
                }
            template <typename O, typename II, typename Cx>
                static bool range(O& o, II i, II e, Cx& cx) {
                    for ( ; i != e && value(o, *i, cx); ++i) ;
                    return i == e;
                }
        };

    template <typename X>
        struct encode<X, wchar_t> {
            template <typename O, typename Cx, typename T = wchar_t>
                static auto value(O& o, T c, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char16_t), bool> {
                    return encode<X, char16_t>::value(o, char16_t(c), cx);
                }
            template <typename O, typename Cx, typename T = wchar_t>
                static auto value(O& o, T c, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char32_t), bool> {
                    return encode<X, char32_t>::value(o, char32_t(c), cx);
                }
            template <typename O, typename Cx, typename T = wchar_t, typename II>
                static auto value(O& o, II i, II e, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char16_t), bool> {
                    return encode<X, char16_t>::value(o, i, e, cx);
                }
            template <typename O, typename Cx, typename T = wchar_t, typename II>
                static auto value(O& o, II i, II e, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char32_t), bool> {
                    return encode<X, char32_t>::value(o, i, e, cx);
                }
            template <typename O, typename Cx, typename T = wchar_t, typename II>
                static auto range(O& o, II i, II e, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char16_t), bool> {
                    return encode<X, char16_t>::range(o, i, e, cx);
                }
            template <typename O, typename Cx, typename T = wchar_t, typename II>
                static auto range(O& o, II i, II e, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char32_t), bool> {
                    return encode<X, char32_t>::range(o, i, e, cx);
                }
        };

    template <typename X, typename T, typename O, typename Cx>
        inline bool character_write(O& o, T t, Cx& cx) {
            return  io::poke<X>(o, X::string::beg, cx) &&
                        encode<X, T>::value(o, t, cx) &&
                    io::poke<X>(o, X::string::end, cx)
            ;
        }

}}  // cxon::bits character conversion: write

#endif // CXON_CHARIO_CHARS_HXX_
