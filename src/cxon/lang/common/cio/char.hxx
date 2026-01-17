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

#ifndef CXON_CIO_CHAR_HXX_
#define CXON_CIO_CHAR_HXX_

#include "cio.hxx"
#include "simd.hxx"
#include <limits>

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
            -> std::enable_if_t<is_char_8<typename std::iterator_traits<II>::value_type>::value, char32_t>;

    template <typename T>
        inline auto utf32_to_utf8(T (&t)[4], char32_t c32) noexcept
            -> std::enable_if_t<is_char_8<T>::value, int>;

    template <typename II>
        inline int utf8_check(II i, II e);

}}}

namespace cxon { namespace cio { namespace chr { // character conversion: write

    template <typename X, typename O, typename T, typename Cx>
        inline auto encode_range(O& o, const T* f, const T* l, Cx& cx)
            -> std::enable_if_t<is_char<T>::value, bool>;

    template <typename X, typename T, typename O, typename Cx>
        inline auto write(O& o, T t, Cx& cx)
            -> std::enable_if_t<is_char<T>::value, bool>;

        template <typename X>
            constexpr bool should_escape(char c) noexcept;

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace chr { namespace imp {

    // http://www.unicode.org/versions/Unicode6.0.0/ch03.pdf
    //
    // Table 3-6. UTF-8 Bit Distribution
    // +---------------------------+----------+----------+----------+----------+
    // |Scalar Value               | 1        | 2        | 3        | 4        |
    // +---------------------------+----------+----------+----------+----------+
    // |00000000 0xxxxxxx          | 0xxxxxxx |          |          |          |
    // |00000yyy yyxxxxxx          | 110yyyyy | 10xxxxxx |          |          |
    // |zzzzyyyy yyxxxxxx          | 1110zzzz | 10yyyyyy | 10xxxxxx |          |
    // |000uuuuu zzzzyyyy yyxxxxxx | 11110uuu | 10uuzzzz | 10yyyyyy | 10xxxxxx |
    // +---------------------------+----------+----------+----------+----------+
    //
    // Table 3-7. Well-Formed UTF-8 Byte Sequences
    // +--------------------+--------+--------+--------+--------+
    // | Code Points        | 1      | 2      | 3      | 4      |
    // +--------------------+--------+--------+--------+--------+
    // | U+0000..  U+007F   | 00..7F |        |        |        |
    // | U+0080..  U+07FF   | C2..DF | 80..BF |        |        |
    // | U+0800..  U+0FFF   | E0     | A0..BF | 80..BF |        |
    // | U+1000..  U+CFFF   | E1..EC | 80..BF | 80..BF |        |
    // | U+D000..  U+D7FF   | ED     | 80..9F | 80..BF |        |
    // | U+E000..  U+FFFF   | EE..EF | 80..BF | 80..BF |        |
    // | U+10000.. U+3FFFF  | F0     | 90..BF | 80..BF | 80..BF |
    // | U+40000.. U+FFFFF  | F1..F3 | 80..BF | 80..BF | 80..BF |
    // | U+100000..U+10FFFF | F4     | 80..8F | 80..BF | 80..BF |
    // +--------------------+--------+--------+--------+--------+

    // https://bjoern.hoehrmann.de/utf-8/decoder/dfa/
    // Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
    // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
    // TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    // THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    // CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    // IN THE SOFTWARE.
    static constexpr unsigned char utf8_decode_class_[] = {
      // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
      //12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12, // 0
      //12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12, // 1
      //12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12, // 2
      //12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12, // 3
      //12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12, // 4
      //12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12, // 5
      //12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12, // 6
      //12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12, // 7
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
         9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, // 9
         7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, // A
         7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, // B
         8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // C
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // D
        10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, // E
        11, 6, 6, 6, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8  // F
    };
    static constexpr unsigned char utf8_decode_state_[] = { // 24/2, 36/3, 48/3, 60/3, 72/4, 84/4, 96/4
      // 0  1  2  3  4  5  6  7  8  9
         0,12,24,36,60,96,84,12,12,12, //  0
        48,72,12,12,12,12,12,12,12,12, //  1
        12,12,12,12,12, 0,12,12,12,12, //  2
        12, 0,12, 0,12,12,12,24,12,12, //  3
        12,12,12,24,12,24,12,12,12,12, //  4
        12,12,12,12,12,24,12,12,12,12, //  5
        12,24,12,12,12,12,12,12,12,24, //  6
        12,12,12,12,12,12,12,12,12,36, //  7
        12,36,12,12,12,36,12,12,12,12, //  8
        12,36,12,36,12,12,12,36,12,12, //  9
        12,12,12,12,12,12,12,12        // 10
    };

    inline unsigned utf8_decode_check_(unsigned state, unsigned char byte) {
        return byte > 0x7F ? utf8_decode_state_[state + utf8_decode_class_[byte & 0x7F]] : 12;
    }

    inline unsigned utf8_decode_code_(unsigned state, char32_t& code, unsigned char byte) {
        unsigned const type = byte > 0x7F ? utf8_decode_class_[byte & 0x7F] : 0;
        code = (state != 0) ? (byte & 0x3F) | (code << 6) : (0xFF >> type) & byte;
        return utf8_decode_state_[state + type];
    }

}}}}

namespace cxon { namespace cio { namespace chr {

    namespace imp {

        static constexpr char32_t hex_to_dec_[] = {
             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
             0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
             0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
             0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };
        template <typename X, typename II>
            static auto u_to_dec_(II& i, II e)
                -> std::enable_if_t<!is_random_access_iterator<II>::value, char32_t>
            {
#               define CXON_NEXT_HEX() if (++i == e || !is<X>::digit16(*i)) return bad_utf32
                    char32_t c;
                        CXON_NEXT_HEX(); c = hex_to_dec_[(unsigned char)*i];
                        CXON_NEXT_HEX(); c = hex_to_dec_[(unsigned char)*i] | (c << 4);
                        CXON_NEXT_HEX(); c = hex_to_dec_[(unsigned char)*i] | (c << 4);
                        CXON_NEXT_HEX(); c = hex_to_dec_[(unsigned char)*i] | (c << 4);
                    return ++i, c;
#               undef CXON_NEXT_HEX
            }
        template <typename X, typename II>
            static auto u_to_dec_(II& i, II e)
                -> std::enable_if_t< is_random_access_iterator<II>::value, char32_t>
            {
                if (e - i < 5 || !is<X>::digit16(i[1]) || !is<X>::digit16(i[2]) || !is<X>::digit16(i[3]) || !is<X>::digit16(i[4]))
                    return bad_utf32;
                char32_t const c =
                    (hex_to_dec_[(unsigned char)i[1]] << 12) |
                    (hex_to_dec_[(unsigned char)i[2]] <<  8) |
                    (hex_to_dec_[(unsigned char)i[3]] <<  4) |
                    (hex_to_dec_[(unsigned char)i[4]] <<  0)
                ;
                return i += 5, c;
            }

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
                    case 'u' :  return u_to_dec_<X>(i, e);
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
        template <typename X, typename II, typename Cx>
            inline char32_t utf8_to_utf32_(II& i, II e, Cx& cx)
            {   static_assert(is_char_8<typename std::iterator_traits<II>::value_type>::value, "unexpected");
                CXON_ASSERT(i != e, "unexpected");
                char32_t c;
                    auto s = utf8_decode_code_(0, c, *i);
                        if (s > 12 && ++i != e) {
                            s = imp::utf8_decode_code_(s, c, *i);
                            if (s > 12 && ++i != e) {
                                s = imp::utf8_decode_code_(s, c, *i);
                                if (s > 12 && ++i != e) {
                                    s = imp::utf8_decode_code_(s, c, *i);
                                }
                            }
                        }
                return s == 0 ? (++i, c) : (cx/X::read_error::character_invalid, bad_utf32);
            }
    }
    template <typename X, typename II, typename Cx>
        inline auto utf8_to_utf32(II& i, II e, Cx& cx)
            -> std::enable_if_t<is_char_8<typename std::iterator_traits<II>::value_type>::value, char32_t>
        {
            return peek(i, e) != '\\' ?
                imp::utf8_to_utf32_<X>(i, e, cx) :
                esc_to_utf32<X>(++i, e, cx)
            ;
        }

    template <typename T>
        inline auto utf32_to_utf8(T (&t)[4], char32_t c32) noexcept
            -> std::enable_if_t<is_char_8<T>::value, int>
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
        }

    template <typename II>
        inline int utf8_check(II b, II e) {
            auto s = imp::utf8_decode_check_(0, *b);
                if (s != 12 && ++b != e) {
                    s = imp::utf8_decode_check_(s, *b);         if (s == 0) return 2;
                    if (s != 12 && ++b != e) {
                        s = imp::utf8_decode_check_(s, *b);     if (s == 0) return 3;
                        if (s != 12 && ++b != e) {
                            s = imp::utf8_decode_check_(s, *b); if (s == 0) return 4;
                        }
                    }
                }
            return 0;
        }

}}}

namespace cxon { namespace cio { namespace chr {

    namespace imp {

        static constexpr char should_escape_not_quoted_key_context_[] = {
        //  0 1 2 3 4 5 6 7 8 9 A B C D E F
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 1
            0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0, // 2
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 3
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 4
            0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0, // 5
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 6
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  // 7
        };
        template <typename X>
            constexpr auto should_escape_(char c) noexcept -> std::enable_if_t<!is_quoted_key_context<X>::value, bool> {
                return ((unsigned char)c & 0x80) == 0 && should_escape_not_quoted_key_context_[(unsigned char)c] == '\1';
            }
        static constexpr char should_escape_quoted_key_context_[] = {
        //  0 1 2 3 4 5 6 7 8 9 A B C D E F
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 1
            1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0, // 2
            0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0, // 3
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 4
            0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0, // 5
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 6
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  // 7
        };
        template <typename X>
            constexpr auto should_escape_(char c) noexcept -> std::enable_if_t< is_quoted_key_context<X>::value, bool> {
                return ((unsigned char)c & 0x80) == 0 && should_escape_quoted_key_context_[(unsigned char)c] == '\1';
            }
    }
    template <typename X>
        constexpr bool should_escape(char c) noexcept {
            return imp::should_escape_<X>(c);
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
                         return should_escape<X>(c) ?
                            value_(o, c, cx) :
                            poke<X>(o, c, cx)
                        ;
                    }

                template <typename O, typename Cx, typename Y = X>
                    static auto range(O& o, const char* f, const char* l, Cx& cx)
                        -> std::enable_if_t< is_unquoted_key_context<X>::value || !Y::assume_no_escapes, bool>
                    {
                        char const* a = f;
                            while (f != l) {
                                f = simd::find_first_write_break<X>(f, l);
                                    if (f == l) break;
                                CXON_IF_CONSTEXPR (Y::produce_strict_javascript) {
                                    if (*f == '\xE2' && l - f > 2) {
                                        if (f[1] == '\x80') {
                                            if (f[2] == '\xA8' || f[2] == '\xA9') {
                                                if (a != f)
                                                    if (!poke<Y>(o, a, f, cx)) return false;
                                                if (!poke<Y>(o, f[2] == '\xA8' ? "\\u2028" : "\\u2029", 6, cx)) return false;
                                                f += 3, a = f;
                                                continue;
                                            }
                                        }
                                    }
                                }
                                if (!poke<Y>(o, a, f, cx) || !value(o, *f, cx))
                                    return false;
                                a = ++f;
                            }
                        return a == f || poke<Y>(o, a, f, cx);
                    }

                template <typename O, typename Cx, typename Y = X>
                    static auto range(O& o, const char* f, const char* l, Cx& cx)
                        -> std::enable_if_t<!is_unquoted_key_context<X>::value &&  Y::assume_no_escapes, bool>
                    {
                        return poke<Y>(o, f, l, cx);
                    }

                private:
                    template <typename O, typename Cx>
                        static bool value_(O& o, char c, Cx& cx) {
                            static_assert(X::string::del == '"' || X::string::del == '\'', "expecting single or double quotes as a string delimiter");
                            static_assert(X::map::div == ':' || X::map::div == '=', "expecting colon or equal sign as a key/value divider");

                            static constexpr struct { char const* c; std::size_t l; } esc_[] = {
                                { "\\u0000", 6 }, { "\\u0001", 6 }, { "\\u0002", 6 }, { "\\u0003", 6 }, { "\\u0004", 6 }, { "\\u0005", 6 }, { "\\u0006", 6 }, { "\\u0007", 6 },
                                {     "\\b", 2 }, {     "\\t", 2 }, {     "\\n", 2 }, { "\\u000b", 6 }, {     "\\f", 2 }, {     "\\r", 2 }, { "\\u000e", 6 }, { "\\u000f", 6 },
                                { "\\u0010", 6 }, { "\\u0011", 6 }, { "\\u0012", 6 }, { "\\u0013", 6 }, { "\\u0014", 6 }, { "\\u0015", 6 }, { "\\u0016", 6 }, { "\\u0017", 6 }, 
                                { "\\u0018", 6 }, { "\\u0019", 6 }, { "\\u001a", 6 }, { "\\u001b", 6 }, { "\\u001c", 6 }, { "\\u001d", 6 }, { "\\u001e", 6 }, { "\\u001f", 6 }
                            };

                            switch (c) {
                                case ' ':
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
                                case ':':
                                    CXON_IF_CONSTEXPR (is_quoted_key_context<X>::value && X::map::div == ':')
                                            return poke<X>(o, "\\:", 2, cx);
                                    else    return poke<X>(o, ':', cx);
                                case '=':
                                    CXON_IF_CONSTEXPR (is_quoted_key_context<X>::value && X::map::div == '=')
                                            return poke<X>(o, "\\=", 2, cx);
                                    else    return poke<X>(o, '=', cx);
                                case '\\':
                                    return poke<X>(o, "\\\\", 2, cx);
                                default:
                                    CXON_ASSERT((unsigned char)c <= 0x1F, "unexpected");
                                    auto const& e = esc_[(unsigned char)c];
                                    return poke<X>(o, e.c, e.l, cx);
                            }
                        }
            };

        template <typename X>
            struct encode_<X, wchar_t> {
                template <typename O, typename Cx, typename T = wchar_t>
                    static auto value(O& o, T c, Cx& cx) -> std::enable_if_t<sizeof(T) == sizeof(char16_t), bool> {
                        return encode_<X, char16_t>::value(o, char16_t(c), cx);
                    }
                template <typename O, typename Cx, typename T = wchar_t>
                    static auto value(O& o, T c, Cx& cx) -> std::enable_if_t<sizeof(T) == sizeof(char32_t), bool> {
                        return encode_<X, char32_t>::value(o, char32_t(c), cx);
                    }
                template <typename O, typename Cx, typename T = wchar_t>
                    static auto range(O& o, const T* f, const T* l, Cx& cx) -> std::enable_if_t<sizeof(T) == sizeof(char16_t), bool> {
                        return encode_<X, char16_t>::range(o, (const char16_t*)f, (const char16_t*)l, cx);
                    }
                template <typename O, typename Cx, typename T = wchar_t>
                    static auto range(O& o, const T* f, const T* l, Cx& cx) -> std::enable_if_t<sizeof(T) == sizeof(char32_t), bool> {
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
                        return encode_<X, char32_t>::value(o, to_c32_(c), cx);
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
                            char32_t c32 = to_c32_(*i);
                                if (c32 >= 0xD800 && c32 <= 0xDBFF) { // surrogate
                                    ++i;                                CXON_ASSERT(i != e, "invalid surrogate");
                                    char32_t const s32 = to_c32_(*i);   CXON_ASSERT(s32 >= 0xDC00 && s32 <= 0xDFFF, "invalid surrogate");
                                    c32 = char32_t(0x10000 + (((c32 - 0xD800) << 10) | (s32 - 0xDC00)));
                                }
                            return encode_<X, char32_t>::value(o, c32, cx);
                        }
                        static char32_t to_c32_(char16_t c16) noexcept {
                            return static_cast<char32_t>(static_cast<typename std::make_unsigned<char16_t>::type>(c16));
                        }
            };

        template <typename X>
            struct encode_<X, char32_t> {
                template <typename O, typename Cx, typename S = X>
                    static auto value(O& o, char32_t c, Cx& cx) -> std::enable_if_t<!S::produce_strict_javascript, bool> {
                        if (c > 0x7F) {
                                char b[4]; int const n = utf32_to_utf8(b, c);
                                return poke<X>(o, b, n, cx);
                        }
                        else    return encode_<X, char>::value(o, char(c), cx);
                    }
                template <typename O, typename Cx, typename S = X>
                    static auto value(O& o, char32_t c, Cx& cx) -> std::enable_if_t< S::produce_strict_javascript, bool> {
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
        inline auto encode_range(O& o, const T* f, const T* l, Cx& cx) -> std::enable_if_t<is_char<T>::value, bool> {
            return imp::encode_<X, T>::range(o, f, l, cx);
        }

    template <typename X, typename T, typename O, typename Cx>
        inline auto write(O& o, T t, Cx& cx) -> std::enable_if_t<is_char<T>::value, bool> {
            return  poke<X>(o, str::delim_be_write<X, O>, cx) &&
                        imp::encode_<X, T>::value(o, t, cx) &&
                    poke<X>(o, str::delim_en_write<X, O>, cx)
            ;
        }

}}}

#endif // CXON_CIO_CHAR_HXX_
