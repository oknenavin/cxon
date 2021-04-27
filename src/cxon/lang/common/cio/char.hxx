// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_CHAR_HXX_
#define CXON_CIO_CHAR_HXX_

#include "cio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace chr { // character conversion: read

    template <typename T>
        using is_char_8 = std::integral_constant<
            bool, is_char<T>::value && sizeof(T) == sizeof(char)
        >;
    template <typename T>
        using is_char_16 = std::integral_constant<
            bool, std::is_same<T, char16_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char16_t))
        >;
    template <typename T>
        using is_char_32 = std::integral_constant<
            bool, std::is_same<T, char32_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char32_t))
        >;


    template <typename X, typename II, typename Cx>
        inline char32_t esc_to_utf32(II& i, II e, Cx& cx);

    template <typename X, typename II, typename Cx>
        inline auto utf8_to_utf32(II& i, II e, Cx& cx)
            -> enable_if_t<is_char<typename std::iterator_traits<II>::value_type>::value, char32_t>;

    template <typename T>
        inline auto utf32_to_utf8(T (&t)[4], char32_t c32) noexcept
            -> enable_if_t<is_char_8<T>::value, int>;

    template <typename II>
        inline int utf8_check(II i, II e);

}}}

namespace cxon { namespace cio { namespace chr { // character conversion: write

    template <typename X, typename O, typename T, typename Cx>
        inline auto encode_range(O& o, const T* f, const T* l, Cx& cx)
            -> enable_if_t<is_char<T>::value, bool>;

    template <typename X, typename T, typename O, typename Cx>
        inline auto write(O& o, T t, Cx& cx)
            -> enable_if_t<is_char<T>::value, bool>;

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace chr {

    namespace imp {

        inline char32_t hex_to_utf32_(const char* b, const char* e) noexcept {
            static constexpr char32_t hex_to_utf32_[] = { 
                00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
                00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
                00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
                00, 1, 2, 3, 4, 5, 6, 7, 8, 9,00,00,00,00,00,00,
                00,10,11,12,13,14,15,00,00,00,00,00,00,00,00,00,
                00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
                00,10,11,12,13,14,15,00,00,00,00,00,00,00,00,00
            };
            CXON_ASSERT(b && b < e, "unexpected");
            char32_t c = 0;
                for ( ; b != e; ++b) c = (c << 4) | hex_to_utf32_[(unsigned char)*b];
            return c;
        }

#       define CXON_ASS_U(t) if (!is<X>::digit16(next(i, e))) return 0xFFFFFFFF; t = peek(i, e)
            template <typename X, typename II>
                inline char32_t esc_to_utf32_(II& i, II e) {
                    switch (peek(i, e)) {
                        case '\"': return ++i, U'\"';
                        case '\\': return ++i, U'\\';
                        case '/' : return ++i, U'/';
                        case 'b' : return ++i, U'\b';
                        case 'f' : return ++i, U'\f';
                        case 'n' : return ++i, U'\n';
                        case 'r' : return ++i, U'\r';
                        case 't' : return ++i, U'\t';
                        case 'u' : {
                            char h[4];
                                CXON_ASS_U(h[0]); CXON_ASS_U(h[1]); CXON_ASS_U(h[2]); CXON_ASS_U(h[3]);
                            return ++i, hex_to_utf32_(h, h + sizeof(h));
                        }
                        default: return 0xFFFFFFFF;
                    }
                }
#       undef CXON_ASS_U

    }

    template <typename X, typename II, typename Cx>
        inline char32_t esc_to_utf32(II& i, II e, Cx& cx) {
            char32_t const c32 = imp::esc_to_utf32_<X>(i, e);
                if (c32 == 0xFFFFFFFF) return cx/X::read_error::escape_invalid, 0xFFFFFFFF;
            if (c32 < 0xD800 || c32 > 0xDBFF) return c32;
            // surrogate
                if (peek(i, e) != '\\') return cx/X::read_error::surrogate_invalid, 0xFFFFFFFF;
            char32_t const s32 = (++i, imp::esc_to_utf32_<X>(i, e));
                if (s32 < 0xDC00 || s32 > 0xDFFF)
                    return (s32 == 0xFFFFFFFF ? cx/X::read_error::escape_invalid : cx/X::read_error::surrogate_invalid), 0xFFFFFFFF;
            return char32_t(0x10000 + (((c32 - 0xD800) << 10) | (s32 - 0xDC00)));
        }

#   define CXON_EXPECT(c) if (!(c)) return cx/X::read_error::character_invalid, 0xFFFFFFFF
        template <typename X, typename II, typename Cx>
            inline auto utf8_to_utf32(II& i, II e, Cx& cx)
                -> enable_if_t<is_char<typename std::iterator_traits<II>::value_type>::value, char32_t>
            {
                char32_t const c0 = peek(i, e);
                if (c0 != '\\') {
                    if ((c0 & 0x80) == 0)
                        return ++i, c0;
                    if ((c0 & 0xE0) == 0xC0) {
                        char32_t const c1 = next(i, e); CXON_EXPECT((c1 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x1F) << 6) | (c1 & 0x3F);
                    }
                    if ((c0 & 0xF0) == 0xE0) {
                        char32_t const c1 = next(i, e); CXON_EXPECT((c1 & 0xC0) == 0x80);
                        char32_t const c2 = next(i, e); CXON_EXPECT((c2 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
                    }
                    if ((c0 & 0xF8) == 0xF0) {
                        char32_t const c1 = next(i, e); CXON_EXPECT((c1 & 0xC0) == 0x80);
                        char32_t const c2 = next(i, e); CXON_EXPECT((c2 & 0xC0) == 0x80);
                        char32_t const c3 = next(i, e); CXON_EXPECT((c3 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                    }
                    CXON_EXPECT(false);
                }
                return esc_to_utf32<X>(++i, e, cx);
            }
#   undef CXON_EXPECT

    template <typename T>
        inline auto utf32_to_utf8(T (&t)[4], char32_t c32) noexcept
            -> enable_if_t<is_char_8<T>::value, int>
        {
            static constexpr unsigned long const ms[] = { 0xBFUL, 0x00UL, 0xC0UL, 0xE0UL, 0xF0UL, 0xF8UL, 0xFCUL };
                if (c32 >= 0xD800UL && c32 <= 0xDBFFUL) return 0; // surrogate
            int const bs = c32 >= 0x80UL ? c32 >= 0x800UL ? c32 >= 0x10000UL ? c32 >= 0x110000UL ? 0 : 4 : 3 : 2 : 1;
                switch (bs) {
                    case 4: t[3] = (T)((c32 | 0x80UL) & 0xBFUL); c32 >>= 6; CXON_FALLTHROUGH;
                    case 3: t[2] = (T)((c32 | 0x80UL) & 0xBFUL); c32 >>= 6; CXON_FALLTHROUGH;
                    case 2: t[1] = (T)((c32 | 0x80UL) & 0xBFUL); c32 >>= 6; CXON_FALLTHROUGH;
                    case 1: t[0] = (T)( c32 | ms[bs]          );
                }
            return bs;
            //if (c32 < 0x80)  // 0XXX XXXX
            //    return t[0] = char(c32), 1;
            //if (c32 < 0x800) { // 110XXXXX
            //    t[0] = char(0xC0 | (c32 >> 6));
            //    t[1] = char(0x80 | (0x3F & c32));
            //    return 2;
            //}
            //if (c32 < 0x10000) { // 1110XXXX
            //    // error: 0xFFFE || 0xFFFF // not a char?
            //        if (c32 >= 0xD800 && c32 <= 0xDBFF) return 0;
            //    t[0] = char(0xE0 | (c32 >> 12));
            //    t[1] = char(0x80 | (0x3F & (c32 >> 6)));
            //    t[2] = char(0x80 | (0x3F & c32));
            //    return 3;
            //}
            //if (c32 < 0x110000) { // 11110XXX
            //    t[0] = char(0xF0 | (c32 >> 18));
            //    t[1] = char(0x80 | (0x3F & (c32 >> 12)));
            //    t[2] = char(0x80 | (0x3F & (c32 >> 6)));
            //    t[3] = char(0x80 | (0x3F & c32));
            //    return 4;
            //}
            //return 0;
        }

    template <typename II>
        inline int utf8_check(II i, II e) {
            auto const c0 = *i;
            if ((c0 & 0x80) == 0)
                return 0;
            if ((c0 & 0xE0) == 0xC0) {
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                return 1;
            }
            if ((c0 & 0xF0) == 0xE0) {
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                return 2;
            }
            if ((c0 & 0xF8) == 0xF0) {
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                if ((cio::next(i, e) & 0xC0) != 0x80) return 4;
                return 3;
            }
            return 4;
        }

}}}

namespace cxon { namespace cio { namespace chr {

    namespace imp {

        template <typename X, typename T>
            struct encode_;

        template <typename X>
            struct encode_<X, char> {
                template <typename O, typename Cx>
                    static bool value(O& o, char c, Cx& cx) {
                        static constexpr char const*const encode_[] = {
                            "\\u0000", "\\u0001", "\\u0002", "\\u0003", "\\u0004", "\\u0005", "\\u0006", "\\u0007",
                            "\\b"    , "\\t"    , "\\n"    , "\\u000b", "\\f"    , "\\r"    , "\\u000e", "\\u000f",
                            "\\u0010", "\\u0011", "\\u0012", "\\u0013", "\\u0014", "\\u0015", "\\u0016", "\\u0017",
                            "\\u0018", "\\u0019", "\\u001a", "\\u001b", "\\u001c", "\\u001d", "\\u001e", "\\u001f",
                            " "      , "!"      , "\\\""   , "#"      , "$"      , "%"      , "&"      , "'"
                        };
                        switch (c) {
                            case  0: case  1: case  2: case  3: case  4: case  5: case  6: case  7:
                            case  8: case  9: case 10: case 11: case 12: case 13: case 14: case 15:
                            case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23:
                            case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31:
                            case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39:
                                        return poke<X>(o, encode_[(unsigned char)c], cx);
                            case '\\':  return poke<X>(o, "\\\\", cx);
                            default:    return poke<X>(o, c, cx);
                        }
                    }
                template <typename O, typename Cx>
                    static bool range(O& o, const char* f, const char* l, Cx& cx) {
                        char const* b = f;
                        for ( ; f != l; ++f) {
                            if (should_escape_(*f)) {
                                if (b != f)
                                    if (!poke<X>(o, b, f - b, cx)) return false;
                                if (!value(o, *f, cx)) return false;
                                b = f + 1;
                            }
                            else CXON_IF_CONSTEXPR (X::strict_js) {
                                if (*f == '\xE2' && l - f > 2) {
                                    if (f[1] == '\x80') {
                                        if (f[2] == '\xA8' || f[2] == '\xA9') {
                                            if (b != f)
                                                if (!poke<X>(o, b, f - b, cx)) return false;
                                            if (!poke<X>(o, f[2] == '\xA8' ? "\\u2028" : "\\u2029", 6, cx)) return false;
                                            f += 2, b = f + 1;
                                        }
                                    }
                                }
                            }
                        }
                        return b == f || poke<X>(o, b, f - b, cx);
                    }
                private:
                    static bool should_escape_(char c) noexcept {
                        static constexpr char se_[] = {
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                        };
                        return se_[(unsigned char)c] == '\1';
                    }
            };

        template <typename X>
            struct encode_<X, wchar_t> {
                template <typename O, typename Cx, typename T = wchar_t>
                    static auto value(O& o, T c, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char16_t), bool> {
                        return encode_<X, char16_t>::value(o, char16_t(c), cx);
                    }
                template <typename O, typename Cx, typename T = wchar_t>
                    static auto value(O& o, T c, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char32_t), bool> {
                        return encode_<X, char32_t>::value(o, char32_t(c), cx);
                    }
                template <typename O, typename Cx, typename T = wchar_t>
                    static auto range(O& o, const T* f, const T* l, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char16_t), bool> {
                        return encode_<X, char16_t>::range(o, (const char16_t*)f, (const char16_t*)l, cx);
                    }
                template <typename O, typename Cx, typename T = wchar_t>
                    static auto range(O& o, const T* f, const T* l, Cx& cx) -> enable_if_t<sizeof(T) == sizeof(char32_t), bool> {
                        return encode_<X, char32_t>::range(o, (const char32_t*)f, (const char32_t*)l, cx);
                    }
            };

#       if __cplusplus > 201703L /* C++20 */
            template <typename X>
                struct encode_<X, char8_t> {
                    template <typename O, typename Cx>
                        static bool value(O& o, char8_t c, Cx& cx) {
                            return encode_<X, char>::value(o, c, cx);
                        }
                    template <typename O, typename Cx>
                        static bool range(O& o, const char8_t* f, const char8_t* l, Cx& cx) {
                            return encode_<X, char>::range(o, (const char*)f, (const char*)l, cx);
                        }
                };
#       endif

        template <typename X>
            struct encode_<X, char16_t> {
                template <typename O, typename Cx>
                    static bool value(O& o, char16_t c, Cx& cx) {
                        CXON_ASSERT(c < 0xD800 || c > 0xDBFF, "unexpected surrogate");
                        return encode_<X, char32_t>::value(o, c, cx);
                    }
                template <typename O, typename Cx>
                    static bool range(O& o, const char16_t* i, const char16_t* e, Cx& cx) {
                        for ( ; i != e && value_(o, i, e, cx); ++i)
                            ;
                        return i == e;
                    }
                private:
                    template <typename O, typename Cx>
                        static bool value_(O& o, const char16_t*& i, const char16_t* e, Cx& cx) {
                            char32_t c32 = *i;
                                if (c32 >= 0xD800 && c32 <= 0xDBFF) { // surrogate
                                    ++i;                        CXON_ASSERT(i != e, "invalid surrogate");
                                    char32_t const s32 = *i;    CXON_ASSERT(s32 >= 0xDC00 && s32 <= 0xDFFF, "invalid surrogate");
                                    c32 = char32_t(0x10000 + (((c32 - 0xD800) << 10) | (s32 - 0xDC00)));
                                }
                            return encode_<X, char32_t>::value(o, c32, cx);
                        }
            };

        template <typename X>
            struct encode_<X, char32_t> {
                template <typename O, typename Cx, typename S = X>
                    static auto value(O& o, char32_t c, Cx& cx) -> enable_if_t<!S::strict_js, bool> {
                        if (c > 0x7F) {
                                char b[4]; int const n = utf32_to_utf8(b, c);
                                return poke<X>(o, b, n, cx);
                        }
                        else    return encode_<X, char>::value(o, char(c), cx);
                    }
                template <typename O, typename Cx, typename S = X>
                    static auto value(O& o, char32_t c, Cx& cx) -> enable_if_t< S::strict_js, bool> {
                        if (c > 0x7F) {
                                if (c == 0x2028) return poke<X>(o, "\\u2028", 6, cx);
                                if (c == 0x2029) return poke<X>(o, "\\u2029", 6, cx);
                                char b[4]; int const n = utf32_to_utf8(b, c);
                                return poke<X>(o, b, n, cx);
                        }
                        else    return encode_<X, char>::value(o, char(c), cx);
                    }
                template <typename O, typename Cx>
                    static bool range(O& o, const char32_t* i, const char32_t* e, Cx& cx) {
                        for ( ; i != e && value(o, *i, cx); ++i)
                            ;
                        return i == e;
                    }
            };

    }

    template <typename X, typename O, typename T, typename Cx>
        inline auto encode_range(O& o, const T* f, const T* l, Cx& cx) -> enable_if_t<is_char<T>::value, bool> {
            return imp::encode_<X, T>::range(o, f, l, cx);
        }

    template <typename X, typename T, typename O, typename Cx>
        inline auto write(O& o, T t, Cx& cx) -> enable_if_t<is_char<T>::value, bool> {
            return  poke<X>(o, X::string::beg, cx) &&
                        imp::encode_<X, T>::value(o, t, cx) &&
                    poke<X>(o, X::string::end, cx)
            ;
        }

}}}

#endif // CXON_CIO_CHAR_HXX_
