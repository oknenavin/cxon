// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_CHAR_HXX_
#define CXON_CIO_CHAR_HXX_

#include "cio.hxx"
#include <limits>

#if CXON_USE_SIMD_SSE2
#   include <emmintrin.h>
#endif

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


    static constexpr char32_t bad_utf32 = std::numeric_limits<char32_t>::max();

    template <typename X, typename II, typename Cx>
        inline char32_t esc_to_utf32(II& i, II e, Cx& cx);

    template <typename X, typename II, typename Cx>
        inline auto utf8_to_utf32(II& i, II e, Cx& cx)
            -> enable_if_t<is_char_8<typename std::iterator_traits<II>::value_type>::value, char32_t>;

    template <typename T>
        inline auto utf32_to_utf8(T (&t)[4], char32_t c32) noexcept
            -> enable_if_t<is_char_8<T>::value, int>;

    template <typename II>
        CXON_INLAY int utf8_check(II i, II e);

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

        static constexpr char32_t hex_to_dec_[] = { // outside u_to_ for C++11
            00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            00, 1, 2, 3, 4, 5, 6, 7, 8, 9,00,00,00,00,00,00,
            00,10,11,12,13,14,15,00,00,00,00,00,00,00,00,00,
            00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,
            00,10,11,12,13,14,15,00,00,00,00,00,00,00,00,00
        };
        template <typename X>
            struct u_to_ {
                template <typename II>
                    static auto dec(II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, char32_t> {
#                       define CXON_NEXT_HEX() if (++i == e || !is<X>::digit16(*i)) return bad_utf32
                            char32_t c;
                                CXON_NEXT_HEX(); c = hex_to_dec_[(unsigned char)*i];
                                CXON_NEXT_HEX(); c = hex_to_dec_[(unsigned char)*i] | (c << 4);
                                CXON_NEXT_HEX(); c = hex_to_dec_[(unsigned char)*i] | (c << 4);
                                CXON_NEXT_HEX(); c = hex_to_dec_[(unsigned char)*i] | (c << 4);
                            return ++i, c;
#                       undef CXON_NEXT_HEX
                    }
                template <typename II>
                    static auto dec(II& i, II e) -> enable_if_t< is_random_access_iterator<II>::value, char32_t> {
                        if (e - i < 5 || !is<X>::digit16(i[1]) || !is<X>::digit16(i[2]) || !is<X>::digit16(i[3]) || !is<X>::digit16(i[4]))
                            return bad_utf32;
                        char32_t const c =  (hex_to_dec_[(unsigned char)i[1]] << 12) |
                                            (hex_to_dec_[(unsigned char)i[2]] <<  8) |
                                            (hex_to_dec_[(unsigned char)i[3]] <<  4) |
                                            (hex_to_dec_[(unsigned char)i[4]] <<  0)
                        ;
                        return i += 5, c;
                    }
            };

        template <typename X, typename II>
            inline char32_t esc_to_utf32_(II& i, II e) {
                switch (peek(i, e)) {
                    case '"' :  return ++i, U'"';
                    case '\\':  return ++i, U'\\';
                    case '/' :  return ++i, U'/';
                    case 'b' :  return ++i, U'\b';
                    case 'f' :  return ++i, U'\f';
                    case 'n' :  return ++i, U'\n';
                    case 'r' :  return ++i, U'\r';
                    case 't' :  return ++i, U'\t';
                    case 'u' :  return u_to_<X>::dec(i, e);
                    case ' ' :
                        CXON_IF_CONSTEXPR (is_quoted_key_context<X>::value)
                                return ++i, U' ';
                        else    return bad_utf32;
                    case ':' :
                        CXON_IF_CONSTEXPR (is_quoted_key_context<X>::value && X::map::div == ':')
                                return ++i, U':';
                        else    return bad_utf32;
                    case '=' :
                        CXON_IF_CONSTEXPR (is_quoted_key_context<X>::value && X::map::div == '=')
                                return ++i, U'=';
                        else    return bad_utf32;
                    case '\'':
                        CXON_IF_CONSTEXPR (X::string::del == '\'')
                                return ++i, U'\'';
                        else    return bad_utf32;
                    default  :  return bad_utf32;
                }
            }

    }

    template <typename X, typename II, typename Cx>
        inline char32_t esc_to_utf32(II& i, II e, Cx& cx) {
            char32_t const c32 = imp::esc_to_utf32_<X>(i, e);
                if (c32 == bad_utf32) return cx/X::read_error::escape_invalid, bad_utf32;
            if (c32 < 0xD800 || c32 > 0xDBFF) return c32;
            // surrogate
                if (peek(i, e) != '\\') return cx/X::read_error::surrogate_invalid, bad_utf32;
            char32_t const s32 = (++i, imp::esc_to_utf32_<X>(i, e));
                if (s32 < 0xDC00 || s32 > 0xDFFF)
                    return (s32 == bad_utf32 ? cx/X::read_error::escape_invalid : cx/X::read_error::surrogate_invalid), bad_utf32;
            return char32_t(0x10000 + (((c32 - 0xD800) << 10) | (s32 - 0xDC00)));
        }

    namespace imp {

        template <typename II>
            inline auto head_(II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, unsigned char> { return next(i, e); }
        template <typename II>
            inline auto head_(II& i, II  ) -> enable_if_t< is_random_access_iterator<II>::value, unsigned char> { return *++i; }
        template <typename II>
            inline auto tail_(II& i, II e) -> enable_if_t<!is_random_access_iterator<II>::value, unsigned char> { return i != e ? peek(++i, e) : '\xFF'; }
        template <typename II>
            inline auto tail_(II& i, II  ) -> enable_if_t< is_random_access_iterator<II>::value, unsigned char> { return *++i; }

#       define CXON_EXPECT(c) if (!(c)) return cx/X::read_error::character_invalid, bad_utf32

            template <typename X, typename II, typename Cx>
                inline auto utf8_to_utf32_(II& i, II e, Cx& cx)
                    -> enable_if_t< X::validate_string_encoding, char32_t>
                {   static_assert(is_char_8<typename std::iterator_traits<II>::value_type>::value, "unexpected");
                    CXON_ASSERT(i != e, "unexpected");
                    char32_t const  c0 = (unsigned char)*i;
                    char32_t        c1, c2, c3;
                    if (c0 <= 0x7F)
                        return ++i, c0;
                    if (c0 >= 0xC2 && c0 <= 0xDF) {
                        c1 = head_(i, e);       CXON_EXPECT(c1 >= 0x80 && c1 <= 0xBF);
                        return ++i, ((c0 & 0x1F) << 6) | (c1 & 0x3F);
                    }
                    if (c0 == 0xE0) {
                        c1 = head_(i, e);       CXON_EXPECT(c1 >= 0xA0 && c1 <= 0xBF);
                        c2 = tail_(i, e);       CXON_EXPECT(c2 >= 0x80 && c2 <= 0xBF);
                        return ++i, ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
                    }
                    if (c0 >= 0xE1 && c0 <= 0xEC) {
                        c1 = head_(i, e);       CXON_EXPECT(c1 >= 0x80 && c1 <= 0xBF);
                        c2 = tail_(i, e);       CXON_EXPECT(c2 >= 0x80 && c2 <= 0xBF);
                        return ++i, ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
                    }
                    if (c0 == 0xED) {
                        c1 = head_(i, e);       CXON_EXPECT(c1 >= 0x80 && c1 <= 0x9F);
                        c2 = tail_(i, e);       CXON_EXPECT(c2 >= 0x80 && c2 <= 0xBF);
                        return ++i, ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
                    }
                    if (c0 >= 0xEE && c0 <= 0xEF) {
                        c1 = head_(i, e);       CXON_EXPECT(c1 >= 0x80 && c1 <= 0xBF);
                        c2 = tail_(i, e);       CXON_EXPECT(c2 >= 0x80 && c2 <= 0xBF);
                        return ++i, ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
                    }
                    if (c0 == 0xF0) {
                        c1 = head_(i, e);       CXON_EXPECT(c1 >= 0x90 && c1 <= 0xBF);
                        c2 = tail_(i, e);       CXON_EXPECT(c2 >= 0x80 && c2 <= 0xBF);
                        c3 = tail_(i, e);       CXON_EXPECT(c3 >= 0x80 && c3 <= 0xBF);
                        return ++i, ((c0 & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                    }
                    if (c0 >= 0xF1 && c0 <= 0xF3) {
                        c1 = head_(i, e);       CXON_EXPECT(c1 >= 0x80 && c1 <= 0xBF);
                        c2 = tail_(i, e);       CXON_EXPECT(c2 >= 0x80 && c2 <= 0xBF);
                        c3 = tail_(i, e);       CXON_EXPECT(c3 >= 0x80 && c3 <= 0xBF);
                        return ++i, ((c0 & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                    }
                    if (c0 == 0xF4) {
                        c1 = head_(i, e);       CXON_EXPECT(c1 >= 0x80 && c1 <= 0x8F);
                        c2 = tail_(i, e);       CXON_EXPECT(c2 >= 0x80 && c2 <= 0xBF);
                        c3 = tail_(i, e);       CXON_EXPECT(c3 >= 0x80 && c3 <= 0xBF);
                        return ++i, ((c0 & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                    }
                    CXON_EXPECT(false);
                }
            template <typename X, typename II, typename Cx>
                inline auto utf8_to_utf32_(II& i, II e, Cx& cx)
                    -> enable_if_t<!X::validate_string_encoding, char32_t>
                {   static_assert(is_char_8<typename std::iterator_traits<II>::value_type>::value, "unexpected");
                    CXON_ASSERT(i != e, "unexpected");
                    char32_t const  c0 = (unsigned char)*i;
                    char32_t        c1, c2, c3;
                    if ((c0 & 0x80) == 0)
                        return ++i, c0;
                    if ((c0 & 0xE0) == 0xC0) {
                        c1 = head_(i, e);       CXON_EXPECT((c1 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x1F) << 6) | (c1 & 0x3F);
                    }
                    if ((c0 & 0xF0) == 0xE0) {
                        c1 = head_(i, e);       CXON_EXPECT((c1 & 0xC0) == 0x80);
                        c2 = tail_(i, e);       CXON_EXPECT((c2 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
                    }
                    if ((c0 & 0xF8) == 0xF0) {
                        c1 = head_(i, e);       CXON_EXPECT((c1 & 0xC0) == 0x80);
                        c2 = tail_(i, e);       CXON_EXPECT((c2 & 0xC0) == 0x80);
                        c3 = tail_(i, e);       CXON_EXPECT((c3 & 0xC0) == 0x80);
                        return ++i, ((c0 & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                    }
                    CXON_EXPECT(false);
                }

#       undef CXON_EXPECT

    }
    template <typename X, typename II, typename Cx>
        inline auto utf8_to_utf32(II& i, II e, Cx& cx)
            -> enable_if_t<is_char_8<typename std::iterator_traits<II>::value_type>::value, char32_t>
        {
            return peek(i, e) != '\\' ?
                imp::utf8_to_utf32_<X>(i, e, cx) :
                esc_to_utf32<X>(++i, e, cx)
            ;
        }

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
        CXON_INLAY int utf8_check(II i, II e) {
            // http://www.unicode.org/versions/Unicode6.0.0/ch03.pdf
            // p41, Table 3-7. Well-Formed UTF-8 Byte Sequences
            CXON_ASSERT(i != e, "unexpected");
            unsigned const  c0 = (unsigned char)*i;
            unsigned        c1, c2, c3;
            // 1
            //if (c0 < 0x80)
            //    return 1;
            // 2
            if (c0 < 0xE0) {
                CXON_IF_CONSTEXPR (is_random_access_iterator<II>::value)
                    if (e - i < 2) return 0;
                if (c0 >= 0xC2 && c0 <= 0xDF) {
                    c1 = imp::head_(i, e);
                    if (c1 >= 0x80 && c1 <= 0xBF)
                        return 2;
                }
            }
            // 3
            else if (c0 < 0xF0) {
                CXON_IF_CONSTEXPR (is_random_access_iterator<II>::value)
                    if (e - i < 3) return 0;
                if (c0 == 0xE0) {
                    c1 = imp::head_(i, e), c2 = imp::tail_(i, e);
                    if (c1 >= 0xA0 && c1 <= 0xBF && c2 >= 0x80 && c2 <= 0xBF)
                        return 3;
                }
                else if (c0 >= 0xE1 && c0 <= 0xEC) {
                    c1 = imp::head_(i, e), c2 = imp::tail_(i, e);
                    if (c1 >= 0x80 && c1 <= 0xBF && c2 >= 0x80 && c2 <= 0xBF)
                        return 3;
                }
                else if (c0 == 0xED) {
                    c1 = imp::head_(i, e), c2 = imp::tail_(i, e);
                    if (c1 >= 0x80 && c1 <= 0x9F && c2 >= 0x80 && c2 <= 0xBF)
                        return 3;
                }
                else if (c0 >= 0xEE && c0 <= 0xEF) {
                    c1 = imp::head_(i, e), c2 = imp::tail_(i, e);
                    if (c1 >= 0x80 && c1 <= 0xBF && c2 >= 0x80 && c2 <= 0xBF)
                        return 3;
                }
            }
            // 4
            else if (c0 < 0xF8) {
                CXON_IF_CONSTEXPR (is_random_access_iterator<II>::value)
                    if (e - i < 4) return 0;
                if (c0 == 0xF0) {
                    c1 = imp::head_(i, e), c2 = imp::tail_(i, e), c3 = imp::tail_(i, e);
                    if (c1 >= 0x90 && c1 <= 0xBF && c2 >= 0x80 && c2 <= 0xBF && c3 >= 0x80 && c3 <= 0xBF)
                        return 4;
                }
                else if (c0 >= 0xF1 && c0 <= 0xF3) {
                    c1 = imp::head_(i, e), c2 = imp::tail_(i, e), c3 = imp::tail_(i, e);
                    if (c1 >= 0x80 && c1 <= 0xBF && c2 >= 0x80 && c2 <= 0xBF && c3 >= 0x80 && c3 <= 0xBF)
                        return 4;
                }
                else if (c0 == 0xF4) {
                    c1 = imp::head_(i, e), c2 = imp::tail_(i, e), c3 = imp::tail_(i, e);
                    if (c1 >= 0x80 && c1 <= 0x8F && c2 >= 0x80 && c2 <= 0xBF && c3 >= 0x80 && c3 <= 0xBF)
                        return 4;
                }
            }
            return 0;
        }

}}}

namespace cxon { namespace cio { namespace chr {

    namespace imp {

        static constexpr char should_escape_not_quoted_key_context_[] = {
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        };
        template <typename X>
            constexpr auto should_escape_(char c) noexcept -> enable_if_t<!is_quoted_key_context<X>::value, bool> {
                return should_escape_not_quoted_key_context_[(unsigned char)c] == '\1';
            }
        static constexpr char should_escape_quoted_key_context_[] = {
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        };
        template <typename X>
            constexpr auto should_escape_(char c) noexcept -> enable_if_t< is_quoted_key_context<X>::value, bool> {
                return should_escape_quoted_key_context_[(unsigned char)c] == '\1';
            }

        template <typename X, typename T>
            struct encode_;

        template <typename X>
            struct encode_<X, char> {
                template <typename O, typename Cx>
                    static bool value(O& o, char c, Cx& cx) {
                        static constexpr char const*const esc_[] = {
                            "\\u0000", "\\u0001", "\\u0002", "\\u0003", "\\u0004", "\\u0005", "\\u0006", "\\u0007",
                                "\\b",     "\\t",     "\\n", "\\u000b",     "\\f",     "\\r", "\\u000e", "\\u000f",
                            "\\u0010", "\\u0011", "\\u0012", "\\u0013", "\\u0014", "\\u0015", "\\u0016", "\\u0017",
                            "\\u0018", "\\u0019", "\\u001a", "\\u001b", "\\u001c", "\\u001d", "\\u001e", "\\u001f"
                        };
                        static constexpr unsigned len_[] = { 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 6, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 };
                        static_assert(X::string::del == '"' || X::string::del == '\'', "expecting single or double quotes as a string delimiter");
                        static_assert(X::map::div == ':' || X::map::div == '=', "expecting colon or equal sign as a key/value divider");
                        switch (c) {
                            case  0: case  1: case  2: case  3: case  4: case  5: case  6: case  7:
                            case  8: case  9: case 10: case 11: case 12: case 13: case 14: case 15:
                            case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23:
                            case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31:
                                        return poke<X>(o, esc_[(unsigned char)c], len_[(unsigned char)c], cx);
                            case ' ' :
                                CXON_IF_CONSTEXPR (is_quoted_key_context<X>::value)
                                        return poke<X>(o, "\\ ", 2, cx);
                                else    return poke<X>(o, ' ', cx);
                            case '"':
                                CXON_IF_CONSTEXPR (is_unquoted_key_context<X>::value)
                                        return poke<X>(o, "\\u0022", 6, cx);
                                CXON_IF_CONSTEXPR (is_quoted_key_context<X>::value || X::string::del == '\'')
                                        return poke<X>(o, '"', cx);
                                else    return poke<X>(o, "\\\"", 2, cx);
                            case '\'':
                                CXON_IF_CONSTEXPR (is_unquoted_key_context<X>::value)
                                        return poke<X>(o, '\'', cx);
                                CXON_IF_CONSTEXPR (is_quoted_key_context<X>::value || X::string::del == '"')
                                        return poke<X>(o, '\'', cx);
                                else    return poke<X>(o, "\\'", 2, cx);
                            case ':' :
                                CXON_IF_CONSTEXPR (is_quoted_key_context<X>::value && X::map::div == ':')
                                        return poke<X>(o, "\\:", 2, cx);
                                else    return poke<X>(o, ':', cx);
                            case '=' :
                                CXON_IF_CONSTEXPR (is_quoted_key_context<X>::value && X::map::div == '=')
                                        return poke<X>(o, "\\=", 2, cx);
                                else    return poke<X>(o, '=', cx);
                            case '\\':  return poke<X>(o, "\\\\", 2, cx);
                            default  :  return poke<X>(o, c, cx);
                        }
                    }
                template <typename Y = X>
                    struct use_simd_ : bool_constant<CXON_USE_SIMD_SSE2 && !is_key_context<Y>::value && !Y::produce_strict_javascript> {};
                template <typename O, typename Cx, typename Y = X>
                    static auto range(O& o, const char* f, const char* l, Cx& cx)
                        -> enable_if_t<( is_unquoted_key_context<X>::value || !Y::assume_no_escapes) && !use_simd_<Y>::value, bool>
                    {
                        char const* a = f;
                        for ( ; f != l; ++f) {
                            if (should_escape_<Y>(*f)) {
                                if (a != f && !poke<Y>(o, a, f, cx))    return false;
                                if (!value(o, *f, cx))                  return false;
                                a = f + 1;
                            }
                            else CXON_IF_CONSTEXPR (Y::produce_strict_javascript) {
                                if (*f == '\xE2' && l - f > 2) {
                                    if (f[1] == '\x80') {
                                        if (f[2] == '\xA8' || f[2] == '\xA9') {
                                            if (a != f)
                                                if (!poke<Y>(o, a, f, cx)) return false;
                                            if (!poke<Y>(o, f[2] == '\xA8' ? "\\u2028" : "\\u2029", 6, cx)) return false;
                                            f += 2, a = f + 1;
                                        }
                                    }
                                }
                            }
                        }
                        return a == f || poke<Y>(o, a, f, cx);
                    }
                template <typename O, typename Cx, typename Y = X>
                    static auto range(O& o, const char* f, const char* l, Cx& cx)
                        -> enable_if_t<( is_unquoted_key_context<X>::value || !Y::assume_no_escapes) &&  use_simd_<Y>::value, bool>
                    {   // TODO: assumes that the input 16 byte aligned, but it may not be the case - preprocess the unaligned prefix
                        static_assert(X::string::del == '"' || X::string::del == '\'', "expecting single or double quotes as a string delimiter");
#                       if CXON_USE_SIMD_SSE2
                            char const *a = f;

                            if (l - f >= 16) {
                                __m128i const v1 = _mm_set1_epi8('"');
                                __m128i const v2 = _mm_set1_epi8('\'');
                                __m128i const v3 = _mm_set1_epi8('\\');
                                __m128i const v4 = _mm_set1_epi8(0x1F);
                                std::size_t n = l - f;
                                while (n >= 16) {
                                    __m128i const v0 = _mm_loadu_si128((__m128i const*)f);
                                    __m128i r = X::string::del == '\"' ?
                                                                _mm_cmpeq_epi8(v0, v1) :
                                                                _mm_cmpeq_epi8(v0, v2) ;
                                            r = _mm_or_si128(r, _mm_cmpeq_epi8(v0, v3));
                                            r = _mm_or_si128(r, _mm_cmpeq_epi8(v0, _mm_min_epu8(v0, v4)));
                                    if (int const m = _mm_movemask_epi8(r)) {
                                        int e = 0;
                                            for (auto* g = f + 16; f != g; ++f, ++e) {
                                                if ((1 << e) & m) {
                                                    if (a != f && !poke<Y>(o, a, f, cx))    return false;
                                                    if (!value(o, *f, cx))                  return false;
                                                    a = f + 1;
                                                }
                                            }
                                        if (a != f && !poke<Y>(o, a, f, cx)) return false;
                                        a = f, f -= 16;
                                    }
                                    f += 16, n -= 16;
                                }
                                if (a != f) {
                                    if (!poke<Y>(o, a, f, cx)) return false;
                                    a = f;
                                }
                            }

                            for ( ; f != l; ++f) {
                                if (should_escape_<Y>(*f)) {
                                    if (a != f && !poke<Y>(o, a, f, cx))    return false;
                                    if (!value(o, *f, cx))                  return false;
                                    a = f + 1;
                                }
                            }

                            return a == f || poke<Y>(o, a, f, cx);
#                       else
                            return cx/X::read_error::unexpected;
#                       endif
                    }
                template <typename O, typename Cx, typename Y = X>
                    static auto range(O& o, const char* f, const char* l, Cx& cx)
                        -> enable_if_t<!is_unquoted_key_context<X>::value &&  Y::assume_no_escapes, bool>
                    {
                        return poke<Y>(o, f, l, cx);
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

#       if defined(__cpp_char8_t)
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
                    static auto value(O& o, char32_t c, Cx& cx) -> enable_if_t<!S::produce_strict_javascript, bool> {
                        if (c > 0x7F) {
                                char b[4]; int const n = utf32_to_utf8(b, c);
                                return poke<X>(o, b, n, cx);
                        }
                        else    return encode_<X, char>::value(o, char(c), cx);
                    }
                template <typename O, typename Cx, typename S = X>
                    static auto value(O& o, char32_t c, Cx& cx) -> enable_if_t< S::produce_strict_javascript, bool> {
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
            return  poke<X>(o, str::delim_be_write<X, O>, cx) &&
                        imp::encode_<X, T>::value(o, t, cx) &&
                    poke<X>(o, str::delim_en_write<X, O>, cx)
            ;
        }

}}}

#endif // CXON_CIO_CHAR_HXX_
