// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_BITS_CXON_HXX_
#define CXON_BITS_CXON_HXX_

#include "charconv.hxx"
#include <algorithm>
#include <cmath> // isfinite, ...

namespace cxon { namespace bits { // character classes

    enum : unsigned char {
        CTRL    = 1 << 0,       // control
        REAL    = 1 << 1,       // character
        CXSP    = 1 << 2,       // cxon space
        JSSP    = 1 << 3,       // json space
        DGOC    = 1 << 4,       // octal
        DGDC    = 1 << 5,       // decimal
        DGHX    = 1 << 6,       // hexadecimal
        ALPH    = 1 << 7,       // alpha
        ALNM    = ALPH|DGDC,    // alphanumeric
        C0      = CTRL,
        C1      = REAL,
        C2      = CXSP|CTRL,
        C3      = JSSP|CXSP|CTRL,
        C4      = JSSP|CXSP|REAL,
        C5      = DGOC|DGDC|DGHX|REAL,
        C6      = DGDC|DGHX|REAL,
        C7      = DGHX|ALPH|REAL,
        C8      = ALPH|REAL
    };
    static constexpr unsigned char is_x_[] = { 
        /*  0*/  C0,C0,C0,C0,C0,C0,C0,C0,C0,C3,C3,C2,C2,C3,C0,C0,
        /* 16*/  C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,C0,
        /* 32*/  C4,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /* 48*/  C5,C5,C5,C5,C5,C5,C5,C5,C6,C6,C1,C1,C1,C1,C1,C1,
        /* 64*/  C1,C7,C7,C7,C7,C7,C7,C8,C8,C8,C8,C8,C8,C8,C8,C8,
        /* 80*/  C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C1,C1,C1,C1,C1,
        /* 96*/  C1,C7,C7,C7,C7,C7,C7,C8,C8,C8,C8,C8,C8,C8,C8,C8,
        /*112*/  C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C8,C1,C1,C1,C1,C1,
        /*128*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*144*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*160*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*176*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*192*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*208*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*224*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,
        /*240*/  C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,C1,00
    };

    template <typename X>
        struct is {
            static bool ctrl   (char c) noexcept { return CTRL & is_x_[(unsigned char)c]; }
            static bool real   (char c) noexcept { return REAL & is_x_[(unsigned char)c]; }
            static bool space  (char c) noexcept { return CXSP & is_x_[(unsigned char)c]; }
            static bool digit8 (char c) noexcept { return DGOC & is_x_[(unsigned char)c]; }
            static bool digit10(char c) noexcept { return DGDC & is_x_[(unsigned char)c]; }
            static bool digit16(char c) noexcept { return DGHX & is_x_[(unsigned char)c]; }
            static bool alpha  (char c) noexcept { return ALPH & is_x_[(unsigned char)c]; }
            static bool alnum  (char c) noexcept { return ALNM & is_x_[(unsigned char)c]; }
        };
    template <typename X>
        struct is<JSON<X>> : is<X> {
            static bool space  (char c) noexcept { return JSSP & is_x_[(unsigned char)c]; }
        };

}}  // cxon::bits character classes

namespace cxon { namespace bits { // output write with error handling

    template <typename O>
        inline auto push(O& o, char c) -> enable_if_t<is_output_iterator<O>::value> {
            *o = c, ++o;
        }
    template <typename O>
        inline auto push(O& o, char c) -> enable_if_t<is_back_insertable<O>::value> {
            o.push_back(c);
        }

    template <typename O>
        inline auto push_(option<1>, O& o, const char* s) -> decltype(o.append(s), void()) {
            o.append(s);
        }
    template <typename O>
        inline void push_(option<0>, O& o, const char* s) {
            while (*s) push(o, *s), ++s;
        }
    template <typename O>
        inline void push(O& o, const char* s) {
            push_(option<1>(), o, s);
        }

    template <typename O>
        inline auto push_(option<1>, O& o, const char* s, size_t n) -> decltype(o.append(s, n), void()) {
            o.append(s, n);
        }
    template <typename O>
        inline void push_(option<0>, O& o, const char* s, size_t n) {
            while (n) push(o, *s), ++s, --n;
        }
    template <typename O>
        inline void push(O& o, const char* s, size_t n) {
            push_(option<1>(), o, s, n);
        }

    template <typename O>
        inline auto push_(option<1>, O& o, unsigned n, char c) -> decltype(o.append(n, c), void()) {
            o.append(n, c);
        }
    template <typename O>
        inline void push_(option<0>, O& o, unsigned n, char c) {
            while (n) push(o, c), --n;
        }
    template <typename O>
        inline void push(O& o, unsigned n, char c) {
            push_(option<1>(), o, n, c);
        }

    template <typename O, typename ...P>
        inline auto poke_(option<2>, O& o, P... p) -> decltype((bool)o) {
            return push(o, p...), o;
        }
    template <typename O, typename ...P>
        inline auto poke_(option<1>, O& o, P... p) -> decltype(o.good()) {
            return push(o, p...), o.good();
        }
    template <typename O, typename ...P>
        inline bool poke_(option<0>, O& o, P... p) {
            return push(o, p...), true;
        }
    template <typename O, typename ...P>
        inline bool poke(O& o, P... p) {
            return poke_(option<2>(), o, p...);
        }

    template <typename X, typename O, typename Cx, typename ...P>
        inline bool poke(O& o, Cx& cx, P... p) {
            return poke_(option<2>(), o, p...) || (cx|write_error::output_failure);
        }

}}  // cxon::bits output write with error handling

namespace cxon { namespace bits { // list read/write helpers

    template <typename X, typename Cr, typename II, typename EA>
        inline void list_read(II& i, II e, EA element_add) {
            // expects non-empty list
            while (element_add() && io::consume<X>(Cr::sep, i, e)) ;
        }

    template <typename X, typename Cr, typename O, typename II, typename Cx, typename L>
        inline void list_write(O& o, II b, II e, Cx& cx, L element_write) {
            if (b != e && element_write(*b)) {
                while (++b != e && io::poke<X>(o, Cr::sep, cx) && element_write(*b)) ;
            }
        }

}}  // cxon::bits list read/write helpers

namespace cxon { namespace bits { // character conversion: read

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

namespace cxon { namespace bits { // character conversion: write

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
                            char b[4]; int const n = bits::utf32_to_utf8(b, c);
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
                            char b[4]; int const n = bits::utf32_to_utf8(b, c);
                            return io::poke<JSON<X>>(o, b, n, cx);
                    }
                    else    return encode<JSON<X>, char>::value(o, char(c), cx);
                }
            template <typename O, typename Cx, typename S = X>
                static auto value(O& o, char32_t c, Cx& cx) -> enable_if_t< S::strict_js, bool> {
                    if (c > 0x7F) {
                            if (c == 0x2028) return io::poke<JSON<X>>(o, "\\u2028", cx);
                            if (c == 0x2029) return io::poke<JSON<X>>(o, "\\u2029", cx);
                            char b[4]; int const n = bits::utf32_to_utf8(b, c);
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

namespace cxon { namespace bits { // number conversion: read

    template <typename T>
        inline auto is_sign(char c) -> enable_if_t<std::is_signed<T>::value, bool> {
            return c == '-';
        }
    template <typename T>
        constexpr auto is_sign(char) -> enable_if_t<std::is_unsigned<T>::value, bool> {
            return false;
        }

#   define CXON_NEXT() { if (f == l) return -1; *f = c, c = io::next(i, e), ++f; }

        template <typename X, typename T>
            struct number_consumer {
                // integral
                template <typename II, typename N = T>
                    static auto consume(char* f, const char* l, II& i, II e)
                        -> enable_if_t<std::is_integral<N>::value, int>
                    {
                        CXON_ASSERT(f && f < l, "unexpected");
                        int b = 2;
                        char c = io::peek(i, e);
                            if (is_sign<T>(c))              CXON_NEXT()
                            if (c == '0')                   goto trap_oct;
                        // trap_dec:
                               if (!is<X>::digit10(c))      return 0;
                            while ( is<X>::digit10(c))      CXON_NEXT()
                            b = 10;                         goto trap_end;
                        trap_oct:
                            ;                               c = io::next(i, e);
                            if (c == 'x')                   goto trap_hex;
                            if (c == 'b')                   goto trap_bin;
                            if (!is<X>::digit8(c)) {        *f = '0', ++f;
                                                            goto trap_end; }
                            while (is<X>::digit8(c))        CXON_NEXT()
                            b = 8;                          goto trap_end;
                        trap_hex:
                            ;                               c = io::next(i, e);
                               if (!is<X>::digit16(c))      return 0;
                            while ( is<X>::digit16(c))      CXON_NEXT()
                            b = 16;                         goto trap_end;
                        trap_bin:
                            ;                               c = io::next(i, e);
                               if (c != '0' && c != '1')    return 0;
                            while (c == '0' || c == '1')    CXON_NEXT()
                        trap_end:
                            return f != l ? (*f = '\0', b) : 0;
                    }
                // floating point
                template <typename II, typename N = T>
                    static auto consume(char* f, const char* l, II& i, II e)
                        -> enable_if_t<std::is_floating_point<N>::value, int>
                    {
                        CXON_ASSERT(f && f < l, "unexpected");
                        char c = io::peek(i, e);
                            if (is_sign<T>(c))          CXON_NEXT()
                            if (c == '.')               goto trap_fraction_0;
                            if (c == 'i')               goto trap_inf;
                            if (c == 'n')               goto trap_nan;
                        //trap_whole:
                               if (!is<X>::digit10(c))  return 0;
                            while ( is<X>::digit10(c))  CXON_NEXT()
                            if (c == '.')               goto trap_fraction_1;
                            if (c == 'e' || c == 'E')   goto trap_exponent;
                            ;                           goto trap_end;
                        trap_fraction_0:
                            ;                           CXON_NEXT()
                               if (!is<X>::digit10(c))  return 0;
                            while ( is<X>::digit10(c))  CXON_NEXT()
                            if (c == 'e' || c == 'E')   goto trap_exponent;
                            ;                           goto trap_end;
                        trap_fraction_1:
                            ;                           CXON_NEXT()
                            while (is<X>::digit10(c))   CXON_NEXT()
                            if (c == 'e' || c == 'E')   goto trap_exponent;
                            ;                           goto trap_end;
                        trap_exponent:
                            ;                           CXON_NEXT()
                            if (c == '-' || c == '+')   CXON_NEXT()
                               if (!is<X>::digit10(c))  return 0;
                            while ( is<X>::digit10(c))  CXON_NEXT()
                            ;                           goto trap_end;
                        trap_inf:
                            CXON_NEXT() if (c != 'n')   return 0;
                            CXON_NEXT() if (c != 'f')   return 0;
                            CXON_NEXT()                 goto trap_end;
                        trap_nan:
                            CXON_NEXT() if (c != 'a')   return 0;
                            CXON_NEXT() if (c != 'n')   return 0;
                            CXON_NEXT()
                        trap_end:
                            return f != l ? (*f = '\0', 1) : 0;
                    }
            };
        template <typename X, typename T>
            struct number_consumer<JSON<X>, T> {
            // integral
            template <typename II, typename N = T>
                static auto consume(char* f, const char* l, II& i, II e)
                    -> enable_if_t<std::is_integral<N>::value, int>
                {
                    CXON_ASSERT(f && f < l, "unexpected");
                    char c = io::peek(i, e);
                        if (is_sign<T>(c))                  CXON_NEXT()
                        if (c == '0') {                     CXON_NEXT()
                                                            goto trap_end; }
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                    trap_end:
                        return f != l ? (*f = '\0', 1) : 0;
                }
            template <typename N = T>
                static auto consume(const char*& i, const char* e)
                    -> enable_if_t<std::is_integral<N>::value, int>
                {
                    io::consume<JSON<X>>(i, e);
                    char c = io::peek(i, e);
                    if (is_sign<T>(c)) goto trap_neg;
                    // trap_pos
                        if (c == '0') return ++i, 1;
                        while (is<JSON<X>>::digit10(c)) c = io::next(i, e);
                        goto trap_end;
                    trap_neg:
                        c = io::next(i, e);
                            if (c == '0' || !is<JSON<X>>::digit10(c)) return 0;
                        while (is<JSON<X>>::digit10(c)) c = io::next(i, e);
                    trap_end:
                        return 1;
                }
            // floating point
            template <typename II, typename N = T>
                static auto consume(char* f, const char* l, II& i, II e)
                    -> enable_if_t<std::is_floating_point<N>::value, int>
                {   // as in RFC7159
                    CXON_ASSERT(f && f < l, "unexpected");
                    char c = io::peek(i, e);
                        if (c == '"')                       goto trap_spec_beg;
                        if (is_sign<T>(c))                  CXON_NEXT()
                        if (c == '0')                       goto trap_zero;
                    //trap_whole:
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                        if (c == '.')                       goto trap_fraction;
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_zero:
                        ;                                   CXON_NEXT()
                        if (c == '.')                       goto trap_fraction;
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_fraction:
                        ;                                   CXON_NEXT()
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_exponent:
                        ;                                   CXON_NEXT()
                        if (c == '-' || c == '+')           CXON_NEXT()
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                        ;                                   goto trap_end;
                    trap_spec_beg:
                        ;                                   CXON_NEXT()
                        if (is_sign<T>(c))                  CXON_NEXT()
                        if (c == 'i')                       goto trap_inf;
                        if (c == 'n')                       goto trap_nan;
                        ;                                   return 0;
                    trap_inf:
                        CXON_NEXT() if (c != 'n')           return 0;
                        CXON_NEXT() if (c != 'f')           return 0;
                        ;                                   goto trap_spec_end;
                    trap_nan:
                        CXON_NEXT() if (c != 'a')           return 0;
                        CXON_NEXT() if (c != 'n')           return 0;
                    trap_spec_end:
                        CXON_NEXT() if (c != '"')           return 0;
                        CXON_NEXT()
                    trap_end:
                        return f != l ? (*f = '\0', 1) : 0;
                }
            template <typename N = T>
                static auto consume(const char*& i, const char* e)
                    -> enable_if_t<std::is_floating_point<N>::value, int>
                {   // as in RFC7159
                    char c = io::peek(i, e);
                        if (c == '"')                       goto trap_spec_beg;
                        if (is_sign<T>(c))                  c = io::next(i, e);
                        if (c == '0')                       goto trap_zero;
                    //trap_whole:
                            if (!is<JSON<X>>::digit10(c))   return 0;
                        while (is<JSON<X>>::digit10(c))     c = io::next(i, e);
                        if (c == '.')                       goto trap_fraction;
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_zero:
                        ;                                   c = io::next(i, e);
                        if (c == '.')                       goto trap_fraction;
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_fraction:
                        ;                                   c = io::next(i, e);
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    c = io::next(i, e);
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_exponent:
                        ;                                   c = io::next(i, e);
                        if (c == '-' || c == '+')           c = io::next(i, e);
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    c = io::next(i, e);
                        ;                                   goto trap_end;
                    trap_spec_beg:
                        ;                                   c = io::next(i, e);
                        if (is_sign<T>(c))                  c = io::next(i, e);
                        if (c == 'i')                       goto trap_inf;
                        if (c == 'n')                       goto trap_nan;
                        ;                                   return 0;
                    trap_inf:
                        c = io::next(i, e); if (c != 'n')   return 0;
                        c = io::next(i, e); if (c != 'f')   return 0;
                        ;                                   goto trap_spec_end;
                    trap_nan:
                        c = io::next(i, e); if (c != 'a')   return 0;
                        c = io::next(i, e); if (c != 'n')   return 0;
                    trap_spec_end:
                        c = io::next(i, e); if (c != '"')   return 0;
                        ++i;
                    trap_end:
                        return 1;
                }
        };

#   undef CXON_NEXT

    template <typename X, typename T>
        struct number_reader {
            // integral
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(32)];
                            int const b = number_consumer<X, T>::consume(s, s + sizeof(s), i, e);
                            return  (b != -1                                                    || (io::rewind(i, o), cx|read_error::overflow)) &&
                                    (b !=  0                                                    || (io::rewind(i, o), cx|read_error::integral_invalid)) &&
                                    (bits::from_chars(s, s + sizeof(s), t, b).ec == std::errc() || (io::rewind(i, o), cx|read_error::integral_invalid))
                            ;
                    }
                // no optimization for const char* because of numeric bases (0, 0b, 0x)
            // floating point
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<std::is_floating_point<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(64)];
                            int const b = number_consumer<X, T>::consume(s, s + sizeof(s), i, e);
                            return  (b != -1                                                    || (io::rewind(i, o), cx|read_error::overflow)) &&
                                    (b !=  0                                                    || (io::rewind(i, o), cx|read_error::floating_point_invalid)) &&
                                    (bits::from_chars(s, s + sizeof(s), t).ec == std::errc()    || (io::rewind(i, o), cx|read_error::floating_point_invalid))
                            ;
                    }
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_floating_point<N>::value, bool>
                    {
                        auto const r = bits::from_chars(i, e, t);
                        return  (r.ec == std::errc() || (cx|read_error::floating_point_invalid)) &&
                                (i = r.ptr, true);
                    }
        };
    template <typename X, typename T>
        struct number_reader<JSON<X>, T> {
            // integral
                // strict
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(32)];
                            int const b = number_consumer<JSON<X>, T>::consume(s, s + sizeof(s), i, e);
                            return  (b != -1                                                    || (io::rewind(i, o), cx|read_error::overflow)) &&
                                    (b !=  0                                                    || (io::rewind(i, o), cx|read_error::integral_invalid)) &&
                                    (bits::from_chars(s, s + sizeof(s), t).ec == std::errc()    || (io::rewind(i, o), cx|read_error::integral_invalid))
                            ;
                    }
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value && X::number::strict, bool>
                    {
                        auto const b = i;
                        if (number_consumer<JSON<X>, T>::consume(i, e)) {
                            if (*b == '0' && i - b == 1) return t = 0, true;
                            auto const r = bits::from_chars(b, i, t);
                            if (r.ec == std::errc() && r.ptr == i) return true;
                        }
                        return i = b, cx|read_error::integral_invalid;
                    }
                // not strict
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value && !X::number::strict, bool>
                    {
                        auto const r = bits::from_chars(i, e, t);
                        return (r.ec == std::errc() && (i = r.ptr, true)) || (cx|read_error::integral_invalid);
                    }
            // floating point
                template <typename N>
                    static charconv::from_chars_result from_chars(const char* b, const char* e, N& t) {
                        if (*b == '"') {
                            if (b[1] == '-') {
                                if (b[2] == 'i') return b[3] == 'n' && b[4] == 'f' && b[5] == '"' ? t = -std::numeric_limits<N>::infinity(),
                                    charconv::from_chars_result{ b + 6, std::errc() } : charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                                return b[2] == 'n' && b[3] == 'a' && b[4] == 'n' && b[5] == '"' ? t =  std::numeric_limits<N>::quiet_NaN(),
                                    charconv::from_chars_result{ b + 6, std::errc() } : charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                            }
                            else {
                                if (b[1] == 'i') return b[2] == 'n' && b[3] == 'f' && b[4] == '"' ? t =  std::numeric_limits<N>::infinity(),
                                    charconv::from_chars_result{ b + 5, std::errc() } : charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                                return b[1] == 'n' && b[2] == 'a' && b[3] == 'n' && b[4] == '"' ? t =  std::numeric_limits<N>::quiet_NaN(),
                                    charconv::from_chars_result{ b + 5, std::errc() } : charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                            }
                        }
                        auto const r = bits::from_chars(b, e, t);
                        return { r.ptr, r.ec };
                    }
                // strict
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<std::is_floating_point<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(64)];
                            int const b = number_consumer<JSON<X>, T>::consume(s, s + sizeof(s), i, e);
                                return  (b != -1                                            || (io::rewind(i, o), cx|read_error::overflow)) &&
                                        (b !=  0                                            || (io::rewind(i, o), cx|read_error::floating_point_invalid)) &&
                                        (from_chars(s, s + sizeof(s), t).ec == std::errc()  || (io::rewind(i, o), cx|read_error::floating_point_invalid))
                                ;
                    }
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_floating_point<N>::value && X::number::strict, bool>
                    {
                        auto const b = i;
                        if (number_consumer<JSON<X>, T>::consume(i, e)) {
                            if (*b == '0' && i - b == 1) return t = 0, true;
                            auto const r = from_chars(b, i, t);
                            if (r.ec == std::errc() && r.ptr == i) return true;
                        }
                        return i = b, cx|read_error::floating_point_invalid;
                    }
                // not strict
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_floating_point<N>::value && !X::number::strict, bool>
                    {
                        auto const r = from_chars(i, e, t);
                        return  (r.ec == std::errc() || (cx|read_error::floating_point_invalid)) &&
                                (i = r.ptr, true)
                        ;
                    }
        };

    template <typename X, typename T, typename II, typename Cx>
        inline bool number_read(T& t, II& i, II e, Cx& cx) {
            io::consume<X>(i, e);
            return number_reader<X, T>::read(t, i, e, cx);
        }

}}  // cxon::bits number conversion: read

namespace cxon { namespace bits { // number conversion: write

    template <typename X>
        struct opqt {
            static constexpr std::nullptr_t const beg = nullptr;
            static constexpr std::nullptr_t const end = nullptr;
        };
    template <typename X>
        struct opqt<JSON<X>> : JSON<X>::string {};

    template <typename X, typename T, typename O, typename Cx>
        inline auto number_write(O& o, const T& t, Cx& cx) -> enable_if_t<std::is_integral<T>::value, bool> {
            char s[std::numeric_limits<T>::digits10 + 3];
            auto const r = bits::to_chars(s, s + sizeof(s) / sizeof(char), t);
            return (r.ec == std::errc() || (cx|write_error::argument_invalid)) &&
                    io::poke<X>(o, s, r.ptr - s, cx)
            ;
        }

    template <typename X, typename T, typename O, typename Cx>
        inline auto number_write(O& o, const T& t, Cx& cx) -> enable_if_t<std::is_floating_point<T>::value, bool> {
            if (std::isinf(t)) {
                if (!io::poke<X>(o, opqt<X>::beg, cx)) return false;
                if (std::signbit(t) && !io::poke<X>(o, '-', cx)) return false;
                return io::poke<X>(o, "inf", cx) && io::poke<X>(o, opqt<X>::end, cx);
            }
            if (std::isnan(t))
                return io::poke<X>(o, opqt<X>::beg, cx) && io::poke<X>(o, "nan", cx) && io::poke<X>(o, opqt<X>::end, cx);
            CXON_ASSERT(std::isfinite(t), "unexpected");
            char s[std::numeric_limits<T>::max_digits10 * 2];
            auto const r = bits::to_chars(
                s, s + sizeof(s) / sizeof(char), t, fp_precision::constant<prms_type<Cx>>(std::numeric_limits<T>::max_digits10)
            );
            return (r.ec == std::errc() || (cx|write_error::argument_invalid)) &&
                    io::poke<X>(o, s, r.ptr - s, cx)
            ;
        }

}}  // cxon::bits number conversion: write

namespace cxon { namespace bits { // key quoting

    template <typename T> struct is_quoted : std::false_type {};
#   define CXON_QUOTED(T)\
        template <size_t N>         struct is_quoted<T[N]>                          : std::true_type  {};\
        template <size_t N>         struct is_quoted<const T[N]>                    : std::true_type  {};\
        template <>                 struct is_quoted<T*>                            : std::true_type  {};\
        template <>                 struct is_quoted<const T*>                      : std::true_type  {};\
        template <typename ...R>    struct is_quoted<std::basic_string<T, R...>>    : std::true_type  {};
        CXON_QUOTED(char)
        CXON_QUOTED(char16_t)
        CXON_QUOTED(char32_t)
        CXON_QUOTED(wchar_t)
#   undef CXON_QUOTED

    template <typename S> struct UQKEY : S { using X = S; };

}}  // cxon::bits key quoting

namespace cxon { namespace bits { // string quoting: read

    template <typename X>
        struct is_str {
            static           bool end(char c) noexcept  { return c == X::string::end; }
            static constexpr bool esc(char)             { return false; }
        };
    template <typename X, template <typename> class S>
        struct is_str<S<UQKEY<X>>> {
            static bool end(char c) noexcept { return is<S<X>>::space(c) || c == S<X>::map::div; }
            static bool esc(char c) noexcept { return end(c); }
        };

    template <typename X>
        struct consume_str {
            template <typename II, typename Cx> static bool      beg(II& i, II e, Cx& cx) { return io::consume<X>(X::string::beg, i, e, cx); }
            template <typename II, typename Cx> static bool      end(II& i, II e, Cx& cx) { return io::consume<X>(X::string::end, i, e, cx); }
            template <typename II, typename Cx> static char32_t  chr(II& i, II e, Cx& cx) {
                return  str_to_utf32<X>(i, e, cx);
            }
        };
    template <typename X, template <typename> class S>
        struct consume_str<S<UQKEY<X>>> {
            template <typename II, typename Cx> static constexpr bool    beg(II&, II, Cx&)          { return true; }
            template <typename II, typename Cx> static constexpr bool    end(II&, II, Cx&)          { return true; }
            template <typename II, typename Cx> static char32_t          chr(II& i, II e, Cx& cx)   {
                if (io::peek(i, e) == '\\') {
                        char const c = io::next(i, e);
                        return!is_str<S<UQKEY<X>>>::esc(c) ? esc_to_utf32<S<X>>(i, e, cx) : (++i, char32_t(c));
                }
                return  str_to_utf32<S<X>>(i, e, cx);
            }
        };

}}  // cxon::bits string quoting: read

namespace cxon { namespace bits { // string quoting: write

    template <typename X>
        struct str {
            static constexpr char                   beg = X::string::beg;
            static constexpr char                   end = X::string::end;
        };
    template <typename X, template <typename> class S>
        struct str<S<UQKEY<X>>> {
            static constexpr std::nullptr_t         beg = nullptr;
            static constexpr std::nullptr_t         end = nullptr;
        };

}}  // cxon::bits string quoting: write

namespace cxon { namespace bits { // char arrays: read

    template <typename X, typename II, typename Cx> // TODO: common with std::basic_string?
        inline bool array_char_read(char*& t, const char* te, II& i, II ie, Cx& cx) {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return io::rewind(i, o), false;
                char b[4]; int const n = utf32_to_utf8(b, c32);
                    if (n == 0 || t + n > te) return cx|read_error::overflow;
                std::copy_n(b, n, t);
            return t += n, true;
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto array_char_read(T*& t, const T* te, II& i, II ie, Cx& cx)
            -> enable_if_t<std::is_same<T, char16_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char16_t)), bool>
        {
            II const o = i;
                char32_t c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return io::rewind(i, o), false;
                if (c32 > 0xFFFF) {
                    c32 -= 0x10000;
                    *t = T(0xD800 | (c32 >> 10));   if (++t == te) return cx|read_error::overflow;
                    *t = T(0xDC00 | (c32 & 0x3FF));
                }
                else {
                    *t = T(c32);
                }
            return ++t, true;
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto array_char_read(T*& t, const T*, II& i, II ie, Cx& cx)
            -> enable_if_t<std::is_same<T, char32_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char32_t)), bool>
        {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return io::rewind(i, o), false;
            return *t = T(c32), ++t, true;
        }

    template <typename X, typename T, typename II, typename Cx>
        inline bool array_read(T* t, const T* te, II& i, II ie, Cx& cx) {
            II const o = i;
                if (!consume_str<X>::beg(i, ie, cx)) return false;
                    while (t < te) {
                        if (!is<X>::real(io::peek(i, ie)))          return consume_str<X>::end(i, ie, cx);
                        if (is_str<X>::end(io::peek(i, ie)))        return *t = '\0', consume_str<X>::end(i, ie, cx);
                        if (!array_char_read<X>(t, te, i, ie, cx))  return cx.ec == read_error::overflow && (io::rewind(i, o), false);
                    }
                return (is_str<X>::end(io::peek(i, ie)) || (io::rewind(i, o), cx|read_error::overflow)) &&
                        consume_str<X>::end(i, ie, cx)
                ;
        }

    template <typename X>
        struct array {
            template <typename T, typename II, typename Cx>
                static bool read(T* t, const T* te, II& i, II ie, Cx& cx) {
                    io::consume<X>(i, ie);
                    return array_read<X>(t, te, i, ie, cx);
                }
        };
    template <typename X, template <typename> class S>
        struct array<S<UQKEY<X>>> {
            template <typename T, typename II, typename Cx>
                static bool read(T* t, const T* te, II& i, II ie, Cx& cx) {
                    io::consume<S<X>>(i, ie);
                    return io::peek(i, ie) == S<X>::string::beg ?
                        array_read<S<X>>(t, te, i, ie, cx) :
                        array_read<S<UQKEY<X>>>(t, te, i, ie, cx)
                    ;
                }
        };

    template <typename X, typename T, typename II, typename Cx>
        inline bool pointer_read(T*& t, II& i, II e, Cx& cx) {
            io::consume<X>(i, e);
            if (io::peek(i, e) == *X::id::nil && io::consume<X>(X::id::nil, i, e)) return true;
                if (!consume_str<X>::beg(i, e, cx)) return false;
            auto ax = allocator::value(cx.ps, std::allocator<T>());
            typename std::allocator_traits<decltype(ax)>::template rebind_alloc<T> at;
                using al = std::allocator_traits<decltype(at)>;
            T *b = al::allocate(at, 4), *p = b; T* be = b + 4;
            for ( ; ; ) {
                if (p + 4 > be) {
                    T *const n = al::allocate(at, 2 * (be - b));
                        std::copy_n(b, p - b, n);
                    p = n + (p - b);
                    al::deallocate(at, b, be - b);
                    be = n + 2 * (be - b), b = n;
                }
                if (!is<X>::real(io::peek(i, e)))           goto err;
                if (is_str<X>::end(io::peek(i, e)))         return *p = '\0', t = b, consume_str<X>::end(i, e, cx);
                if (!array_char_read<X>(p, be, i, e, cx))   goto err;
            }
            err: return al::deallocate(at, b, be - b), cx|read_error::unexpected;
        }

}}  // cxon::bits char arrays: read

namespace cxon { namespace bits { // char arrays: write

    template <typename T>
        inline size_t ptrlen(const T* t) noexcept {
            const T* e = t;
                while (*e) ++e;
            return e - t;
        }

    template <typename X, typename O, typename T, typename Cx>
        inline bool array_write(O& o, const T* t, const T* te, Cx& cx) {
            if (!io::poke<X>(o, X::string::beg, cx)) return false;
                if (*(te - 1) == T(0)) --te;
            return encode<X, T>::range(o, t, te, cx) && io::poke<X>(o, X::string::end, cx);
        }

    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, size_t s, Cx& cx) {
            return  io::poke<X>(o, X::string::beg, cx) &&
                        encode<X, T>::range(o, t, t + s, cx) &&
                    io::poke<X>(o, X::string::end, cx)
            ;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, Cx& cx) {
            return t ?
                pointer_write<X>(o, t, ptrlen(t), cx) :
                io::poke<X>(o, X::id::nil, cx)
            ;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool uqkey_pointer_write(O& o, const T* t, size_t s, Cx& cx) {
            for (auto e = t + s; t != e; ++t) {
                switch (*t) {
                    case ' ':           if (!io::poke<X>(o, '\\', cx) || !io::poke<X>(o, ' ', cx))          return false;   break;
                    case '"':           if (!io::poke<X>(o, '"', cx))                                       return false;   break;
                    case '\'':          if (!io::poke<X>(o, '\'', cx))                                      return false;   break;
                    case X::map::div:   if (!io::poke<X>(o, '\\', cx) || !io::poke<X>(o, X::map::div, cx))  return false;   break;
                    default:            if (!encode<X, T>::value(o, t, e, cx))                              return false;
                }
            }
            return true;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool uqkey_pointer_write(O& o, const T* t, Cx& cx) {
            return t ?
                uqkey_pointer_write<X>(o, t, ptrlen(t), cx) :
                io::poke<X>(o, X::id::nil, cx)
            ;
        }

    template <typename X>
        struct pointer {
            template <typename O, typename T, typename Cx>
                static bool write(O& o, const T* t, Cx& cx) {
                    return pointer_write<X>(o, t, cx);
                }
        };
    template <typename X, template <typename> class S>
        struct pointer<S<bits::UQKEY<X>>> {
            template <typename O, typename T, typename Cx>
                static bool write(O& o, const T* t, Cx& cx) {
                    return uqkey_pointer_write<S<X>>(o, t, cx);
                }
        };

}}  // cxon::bits char arrays: write

namespace cxon { namespace bits { // key: read

    template <typename X> struct key_read;

    template <typename X, template <typename> class S>
        struct key_read<S<X>> {
            template <typename T, typename II, typename Cx, typename E = S<X>>
                static auto value(T& t, II& i, II e, Cx& cx)
                    -> enable_if_t< is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                {
                    return read_value<S<UQKEY<X>>>(t, i, e, cx);
                }
            template <typename T, typename II, typename Cx, typename E = S<X>>
                static auto value(T& t, II& i, II e, Cx& cx)
                    -> enable_if_t<!is_quoted<T>::value || !E::map::unquoted_keys, bool>
                {
                    return read_value<E>(t, i, e, cx);
                }
        };
    template <typename X>
        struct key_read<JSON<X>> {
            template <typename T, typename II, typename Cx, typename E = JSON<X>>
                static auto value(T& t, II& i, II e, Cx& cx)
                    -> enable_if_t< is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                {
                    return read_value<JSON<UQKEY<X>>>(t, i, e, cx);
                }
            template <typename T, typename II, typename Cx, typename E = JSON<X>>
                static auto value(T& t, II& i, II e, Cx& cx)
                    -> enable_if_t< is_quoted<T>::value && !E::map::unquoted_keys, bool>
                {
                    return read_value<E>(t, i, e, cx);
                }
            template <typename T, typename II, typename Cx, typename E = JSON<X>>
                static auto value(T& t, II& i, II e, Cx& cx)
                    -> enable_if_t<!is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                {
                    return read_value<E>(t, i, e, cx);
                }
            template <typename T, typename II, typename Cx, typename E = JSON<X>>
                static auto value(T& t, II& i, II e, Cx& cx)
                    -> enable_if_t<!is_quoted<T>::value && !E::map::unquoted_keys, bool>
                {
                    return  io::consume<E>(E::string::beg, i, e, cx) &&
                                read_value<E>(t, i, e, cx) &&
                            io::consume<E>(E::string::end, i, e, cx)
                    ;
                }
        };

}}  // cxon::bits key: read

namespace cxon { namespace bits { // key: write

    template <typename X> struct key_write;

    template <typename X, template <typename> class S>
        struct key_write<S<X>> {
            template <typename T, typename O, typename Cx, typename E = S<X>>
                static auto value(O& o, const T& t, Cx& cx)
                    -> enable_if_t< is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                {
                    return write_value<S<UQKEY<X>>>(o, t, cx);
                }
            template <typename T, typename O, typename Cx, typename E = S<X>>
                static auto value(O& o, const T& t, Cx& cx)
                    -> enable_if_t<!is_quoted<T>::value || !E::map::unquoted_keys, bool>
                {
                    return write_value<E>(o, t, cx);
                }
        };
    template <typename X>
        struct key_write<JSON<X>> {
            template <typename T, typename O, typename Cx, typename E = JSON<X>>
                static auto value(O& o, const T& t, Cx& cx)
                    -> enable_if_t< is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                {
                    return write_value<JSON<UQKEY<X>>>(o, t, cx);
                }
            template <typename T, typename O, typename Cx, typename E = JSON<X>>
                static auto value(O& o, const T& t, Cx& cx)
                    -> enable_if_t< is_quoted<T>::value && !E::map::unquoted_keys, bool>
                {
                    return write_value<E>(o, t, cx);
                }
            template <typename T, typename O, typename Cx, typename E = JSON<X>>
                static auto value(O& o, const T& t, Cx& cx)
                    -> enable_if_t<!is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                {
                    return write_value<E>(o, t, cx);
                }
            template <typename T, typename O, typename Cx, typename E = JSON<X>>
                static auto value(O& o, const T& t, Cx& cx)
                    -> enable_if_t<!is_quoted<T>::value && !E::map::unquoted_keys, bool>
                {
                    return  io::poke<E>(o, E::string::beg, cx) &&
                                write_value<E>(o, t, cx) &&
                            io::poke<E>(o, E::string::end, cx)
                    ;
                }
        };

}}  // cxon::bits key: write

namespace cxon { namespace bits { // std::basic_string read

    template <typename X, typename ...R, typename II, typename Cx> // TODO: common with arrays?
        inline bool basic_string_char_read(std::basic_string<char, R...>& t, II& i, II e, Cx& cx) {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return io::rewind(i, o), false;
                char b[4]; t.append(b, utf32_to_utf8(b, c32));
            return true;
        }
    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline auto basic_string_char_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<T, char16_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char16_t)), bool>
        {
            II const o = i;
                char32_t c32 = consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return io::rewind(i, o), false;
                if (c32 > 0xFFFF) {
                    c32 -= 0x10000;
                    t.push_back(char16_t(0xD800 | (c32 >> 10)));
                    t.push_back(char16_t(0xDC00 | (c32 & 0x3FF)));
                }
                else {
                    t.push_back(char16_t(c32));
                }
            return true;
        }
    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline auto basic_string_char_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<T, char32_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char32_t)), bool>
        {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return io::rewind(i, o), false;
            return t.push_back(T(c32)), true;
        }

    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline bool basic_string_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
            if (!consume_str<X>::beg(i, e, cx)) return false;
                for (char c = io::peek(i, e); is<X>::real(c); c = io::peek(i, e)) {
                    if (is_str<X>::end(c))                          return consume_str<X>::end(i, e, cx);
                    if (!basic_string_char_read<X>(t, i, e, cx))    return false;
                }
            return cx|read_error::unexpected;
        }

}}  // cxon::bits std::basic_string read

namespace cxon { namespace unquoted { namespace bits { // unquoted value

    template <typename X>
        struct value {
            template <typename BA, typename II>
                static bool skip(BA& o, II& i, II e) {
                    if (!o.add(*i)) return false;
                    for (++i; i != e; ++i) {
                        switch (*i)  {
                            case '\\':              if (!o.add(*i)) return false;
                                                    if (++i == e)   return false;
                                                    break;
                            case X::string::end:    return o.add(*i);
                        }
                    }
                    return false;
                }
            template <char CB, char CE, typename BA, typename II>
                static bool skip(BA& o, II& i, II e) {
                    if (!o.add(*i)) return false;
                    unsigned r = 0;
                    for (++i; i != e; ++i) {
                        switch (*i) {
                            case CB:                if (!o.add(*i))     return false;
                                                    ++r; break;
                            case CE:                if (!o.add(*i))     return false;
                                                    if (r == 0)         return true;
                                                    --r; break;
                            case X::string::beg:    if (!skip(o, i, e)) return false;
                        }
                    }
                    return false;
                }
            template <typename BA, typename II>
                static bool read(BA o, II& i, II e) {
                    for ( ; i != e; ++i) {
                        char const c = *i;
                             if (c == X::map::beg)      { if (!skip<map<X>::beg, map<X>::end>(o, i, e))     return false; }
                        else if (c == X::list::beg)     { if (!skip<list<X>::beg, list<X>::end>(o, i, e))   return false; }
                        else if (c == X::string::beg)   { if (!skip(o, i, e))                               return false; }
                        else if (c == X::map::sep)                                                          break;
                        else if (c == X::map::end)                                                          break;
                        else if (c == X::list::end)                                                         break;
                        else                            { if (!o.add(*i))                                   return false; }
                    }
                    return o.add('\0');
                }
        };

    template <typename T, typename Cx>
        struct array_adder {
            T *f, *l;
            Cx& cx;
            template <size_t N>
                array_adder(T (&t)[N], Cx& cx) : f(t), l(t + N), cx(cx) {}
            bool add(T e) { return f != l ? (*f = e, ++f, true) : cx|read_error::overflow; }
        };

    template <typename T>
        struct black_adder {
            constexpr bool add(T) const { return true; }
        };

}}} // cxon::unquoted::bits unquoted value

namespace cxon { namespace enums { namespace bits { // enum reader/writer construction helpers

    template <typename X>
        struct read {
            template <size_t N, typename II, typename Cx>
                static bool value(char (&t)[N], II& i, II e, Cx& cx) {
                    return unquoted::read_value<X>(t, i, e, cx);
                }
        };
    template <typename X>
        struct read<JSON<X>> {
            template <size_t N, typename II, typename Cx>
                static bool value(char (&t)[N], II& i, II e, Cx& cx) {
                    return cxon::read_value<JSON<X>>(t, i, e, cx);
                }
        };

}}} // cxon::enum::bits enum reader/writer construction helpers

namespace cxon { namespace structs { namespace bits { // structured types reader/writer construction helpers

    template <typename X, typename S, typename ...>
        struct read {
            template <typename II, typename Cx>
                static constexpr bool fields(S&, const char*, const fields<>&, II&, II, Cx&) {
                    return false;
                }
        };
    template <typename X, typename S, typename H, typename ...T>
        struct read<X, S, H, T...> {
            template <typename II, typename Cx>
                static bool fields(S& t, const char* name, const fields<H, T...>& f, II& i, II e, Cx& cx) {
                    return std::strcmp(f.field.name, name) == 0 ?
                        read_field<X>(t, f.field, i, e, cx) :
                        read<X, S, T...>::fields(t, name, f.next, i, e, cx)
                    ;
                }
        };

    template <typename X, typename S, typename H, typename ...T>
        struct write {
            template <typename O, typename Cx>
                static bool fields(O& o, const S& t, const fields<H, T...>& f, Cx& cx) {
                    return  write_field<X>(o, t, f.field, cx) && io::poke<X>(o, X::map::sep, cx) &&
                            write<X, S, T...>::fields(o, t, f.next, cx)
                    ;
                }
        };
    template <typename X, typename S, typename F>
        struct write<X, S, F> {
            template <typename O, typename Cx>
                static bool fields(O& o, const S& t, const fields<F>& f, Cx& cx) {
                    return write_field<X>(o, t, f.field, cx);
                }
        };

}}} // cxon::structs::bits structured types reader/writer construction helpers

#endif // CXON_BITS_CXON_HXX_
