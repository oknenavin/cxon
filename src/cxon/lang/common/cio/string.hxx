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

#ifndef CXON_CIO_STRING_HXX_
#define CXON_CIO_STRING_HXX_

#include "cio.hxx"
#include "char.hxx"
#include "cxon/lang/common/allocator.hxx"
#include "cxon/lang/common/container.hxx"
#include <algorithm>

#if CXON_USE_SIMD_SSE2
#   ifdef _MSC_VER
#       include <intrin.h>
#   endif
#endif

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace str { // char arrays: read

    template <typename X, typename C, typename II, typename Cx>
        inline auto string_read(C& c, II& i, II e, Cx& cx)      -> std::enable_if_t<is_char<typename C::value_type>::value, bool>;

    template <typename X, typename T, typename II, typename Cx>
        inline auto array_read(T* f, T* l, II& i, II e, Cx& cx) -> std::enable_if_t<is_char<T>::value, bool>;

    template <typename X, typename T, typename II, typename Cx>
        inline auto pointer_read(T*& t, II& i, II e, Cx& cx)    -> std::enable_if_t<is_char<T>::value, bool>;

}}}

namespace cxon { namespace cio { namespace str { // char arrays: write

    template <typename X, typename O, typename T, typename Cx>
        inline auto array_write(O& o, const T* t, const T* te, Cx& cx)      -> std::enable_if_t<is_char<T>::value, bool>;

    template <typename X, typename O, typename T, typename Cx>
        inline auto pointer_write(O& o, const T* t, std::size_t s, Cx& cx)  -> std::enable_if_t<is_char<T>::value, bool>;
    template <typename X, typename O, typename T, typename Cx>
        inline auto pointer_write(O& o, const T* t, Cx& cx)                 -> std::enable_if_t<is_char<T>::value, bool>;

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace str { namespace imp {
    template <typename X>
        using is_raw_ = has_traits<X, str::raw_traits>;
}}}}

namespace cxon { namespace cio { namespace str {

    namespace imp {

        template <typename C, typename T = typename C::value_type>
            inline auto char_append_(C& c, char32_t c32) -> std::enable_if_t<chr::is_char_8<T>::value, bool> {
                T b[4]; int const n = chr::utf32_to_utf8(b, c32); CXON_ASSERT(n > 0 && n <=4, "unexpected");
                return cxon::cnt::append(c, &b[0], &b[0] + n);
            }
        template <typename C, typename T = typename C::value_type>
            inline auto char_append_(C& c, char32_t c32) -> std::enable_if_t<chr::is_char_16<T>::value, bool> {
                if (c32 > 0xFFFF) {
                    c32 -= 0x10000;
                    return cxon::cnt::append(c, T(0xD800 | (c32 >> 10))) && cxon::cnt::append(c, T(0xDC00 | (c32 & 0x3FF)));
                }
                return cxon::cnt::append(c, T(c32));
            }
        template <typename C, typename T = typename C::value_type>
            inline auto char_append_(C& c, char32_t c32) -> std::enable_if_t<chr::is_char_32<T>::value, bool> {
                return cxon::cnt::append(c, T(c32));
            }

        template <typename X, typename C, typename II, typename Cx>
            inline bool char_read_(C& c, II& i, II e, Cx& cx) {
                II const o = i;
                    char32_t const c32 = chr::utf8_to_utf32<X>(i, e, cx);
                        if (c32 == chr::bad_utf32) return rewind(i, o), false;
                return char_append_(c, c32) || (rewind(i, o), cx/X::read_error::overflow);
            }

        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
            inline auto string_read_tail_(C& c, II& i, II e, Cx& cx)
                -> std::enable_if_t<!chr::is_char_8<T>::value || !std::is_pointer<II>::value, bool>
            {
                while (i != e) {
                    CXON_IF_CONSTEXPR (!is_unquoted_key_context<X>::value) {
                        if (delim_en_check<X>(*i)) return delim_en_read<X>(i, e);
                    }
                    CXON_IF_CONSTEXPR (is_unquoted_key_context<X>::value || !X::assume_no_escapes) {
                        if (*i == '\\') {
                            II const o = i; ++i;
                            CXON_IF_CONSTEXPR (is_unquoted_key_context<X>::value) {
                                if (i != e && *i == X::string::del)
                                    return ++i, true;
                            }
                            char32_t c32 = chr::esc_to_utf32<X>(i, e, cx);
                                if (c32 == chr::bad_utf32) return rewind(i, o), false;
                            if (!char_append_(c, c32))
                                return rewind(i, o), cx/X::read_error::overflow;
                            continue;
                        }
                    }
                    CXON_IF_CONSTEXPR (X::validate_string_escapes) {
                        if (chr::is<X>::ctrl(*i)) return cx/X::read_error::unexpected;
                    }
                    if (!char_read_<X>(c, i, e, cx)) return false;
                }
                return cx/X::read_error::unexpected;
            }

        namespace imp // str::raw_traits
        {
#           if CXON_USE_SIMD_SSE2
                int inline find_first_set_(unsigned int value)
                {   // returns the number of trailing 0-bits in x, starting at the least significant bit position. if x is 0, the result is undefined
#                   ifdef _MSC_VER
                        CXON_ASSERT(value, "unexpected invocaion");
                        unsigned long f;
                            _BitScanForward(&f, value);
                        return f;
#                   else
                        return __builtin_ffs(value) - 1;
#                   endif
                }
#           endif

            template <typename X>
                using has_quotes_ = bool_constant<is_unquoted_key_context<X>::value || !is_quoted_key_context<X>::value>;

            template <typename X>
                inline auto check_break_condition_(unsigned char c) -> std::enable_if_t<!is_raw_<X>::value &&  has_quotes_<X>::value, bool>
                { return c >= 0x80 || c == X::string::del || c == '\\' || c <= 0x1F; }
            template <typename X>
                inline auto check_break_condition_(unsigned char c) -> std::enable_if_t<!is_raw_<X>::value && !has_quotes_<X>::value, bool>
                { return c >= 0x80 || c == X::map::div || c == ' ' || c == '\\' || c <= 0x1F; }
            template <typename X>
                inline auto check_break_condition_(unsigned char c) -> std::enable_if_t< is_raw_<X>::value &&  has_quotes_<X>::value, bool>
                { return c == X::string::del; }
            template <typename X>
                inline auto check_break_condition_(unsigned char c) -> std::enable_if_t< is_raw_<X>::value && !has_quotes_<X>::value, bool>
                { return c == X::map::div || c == ' '; }

            template <typename X>
                inline const char* find_first_break_(const char* b, const char* e)
                {   CXON_ASSERT(b < e, "unexpected invocaion");
#                   if CXON_USE_SIMD_SSE2 // TODO actually SSE4.1 because of _mm_min_epu8
                        if (e - b >= 16) {
                            __m128i const vq = _mm_set1_epi8(X::string::del);
                            __m128i const vd = _mm_set1_epi8(X::map::div);
                            __m128i const vs = _mm_set1_epi8(' ');
                            __m128i const vb = _mm_set1_epi8('\\');
                            __m128i const vc = _mm_set1_epi8('\x1F');
                            __m128i const vm = _mm_set1_epi8('\x80');
                            do {
                                __m128i const   vi = _mm_loadu_si128((__m128i const*)b);
                                __m128i         rs;
                                            CXON_IF_CONSTEXPR (has_quotes_<X>::value) {
                                                rs =                  _mm_cmpeq_epi8(vi,                  vq);
                                            }
                                            else {
                                                rs =                  _mm_cmpeq_epi8(vi,                  vd);
                                                rs = _mm_or_si128(rs, _mm_cmpeq_epi8(vi,                  vs));
                                            }
                                            CXON_IF_CONSTEXPR (!is_raw_<X>::value) {
                                                rs = _mm_or_si128(rs, _mm_cmpeq_epi8(vi,                  vb));
                                                rs = _mm_or_si128(rs, _mm_cmpeq_epi8(vi, _mm_min_epu8(vi, vc)));
                                                rs = _mm_or_si128(rs, _mm_and_si128 (vi,                  vm));
                                            }
                                int const       mk = _mm_movemask_epi8(rs);
                                if (mk) {
                                    int const i = find_first_set_(mk);
                                    return b + i;
                                }
                                b += 16;
                            }   while (e - b >= 16);
                        }
#                   endif
                    for ( ; b != e; ++b) {
                        if (check_break_condition_<X>(*b))
                            return b;
                    }
                    return e;
                }
        }
        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
            inline auto string_read_tail_(C& c, II& i, II e, Cx& cx)
                -> std::enable_if_t< chr::is_char_8<T>::value &&  std::is_pointer<II>::value, bool>
            {
                II l = i;
                    while (i != e) {
                        i = (II)imp::find_first_break_<X>(i, e);
                        if (i == e)
                            return cx/X::read_error::unexpected;
                        CXON_IF_CONSTEXPR (!is_unquoted_key_context<X>::value) {
                            if (delim_en_check<X>(*i)) {
                                if (l == i || cxon::cnt::append(c, l, i))
                                    return delim_en_read<X>(i, e);
                                return rewind(i, l), cx/X::read_error::overflow;
                            }
                        }
                        CXON_IF_CONSTEXPR (!is_raw_<X>::value && X::validate_string_encoding) {
                            if ((unsigned char)*i > 0x7F) {
                                int bs = chr::utf8_check(i, e);
                                if (bs == 0)
                                    return cx/X::read_error::character_invalid;
                                while ((bs = chr::utf8_check(i += bs, e)))
                                    ;
                                continue;
                            }
                        }
                        CXON_IF_CONSTEXPR (!is_raw_<X>::value && (is_unquoted_key_context<X>::value || !X::assume_no_escapes)) {
                            if (*i == '\\') {
                                if (l != i && !cxon::cnt::append(c, l, i))
                                    return rewind(i, l), cx/X::read_error::overflow;
                                CXON_IF_CONSTEXPR (is_unquoted_key_context<X>::value) {
                                    II f = i;
                                        if (++i != e && *i == X::string::del)
                                            return ++i, true;
                                    i = f;
                                }
                                II o = i;
                                    char32_t const c32 = chr::esc_to_utf32<X>(++i, e, cx);
                                        if (c32 == chr::bad_utf32) return rewind(i, o), false;
                                    if (!char_append_(c, c32))
                                        return rewind(i, o), cx/X::read_error::overflow;
                                l = i;
                                continue;
                            }
                        }
                        CXON_IF_CONSTEXPR (!is_raw_<X>::value && X::validate_string_escapes) {
                            if (chr::is<X>::ctrl(*i))
                                return cx/X::read_error::unexpected;
                        }
                        ++i;
                    }
                return cx/X::read_error::unexpected;
            }

        template <typename X, typename C, typename II, typename Cx>
            inline bool string_read_head_(C&, II& i, II e, Cx& cx) {
                return consume<X>(delim_be_read<X, II>, i, e, cx);
            }

    }

    template <typename X, typename C, typename II, typename Cx>
        inline auto string_read(C& c, II& i, II e, Cx& cx)
             -> std::enable_if_t<is_char<typename C::value_type>::value, bool>
        {
            return  imp::string_read_head_<X>(c, i, e, cx) &&
                    imp::string_read_tail_<X>(c, i, e, cx)
            ;
        }

    template <typename X, typename T, typename II, typename Cx>
        inline auto array_read(T* f, T* l, II& i, II e, Cx& cx)
            -> std::enable_if_t<is_char<T>::value, bool>
        {
            auto c = cxon::cnt::make_range_container(f, l);
            return consume<X>(i, e, cx) && string_read<X>(c, i, e, cx) && (cxon::cnt::append(c, {}), true);
        }

    template <typename X, typename T, typename II, typename Cx>
        inline auto pointer_read(T*& t, II& i, II e, Cx& cx)
            -> std::enable_if_t<is_char<T>::value, bool>
        {
            if (!consume<X>(i, e, cx))
                return false;
            if (peek(i, e) == *X::id::nil && consume<X>(X::id::nil, i, e))
                return true;
            auto c = cxon::cnt::make_pointer_container<X, T>(cx);
            return string_read<X>(c, i, e, cx) && (c.push_back({}), t = c.release(), true);
        }

}}}

namespace cxon { namespace cio { namespace str {

    template <typename X, typename O, typename T, typename Cx>
        inline auto array_write(O& o, const T* f, const T* l, Cx& cx)
             -> std::enable_if_t<is_char<T>::value, bool>
        {
            if (!poke<X>(o, delim_be_write<X, O>, cx)) return false;
                if (*(l - 1) == T(0)) --l;
            return chr::encode_range<X>(o, f, l, cx) && poke<X>(o, delim_en_write<X, O>, cx);
        }

    namespace imp { // str::raw_traits
        template <typename X, typename O, typename T, typename Cx>
            inline auto pointer_write_(O& o, const T* t, std::size_t s, Cx& cx)
                 -> std::enable_if_t<!is_raw_<X>::value, bool>
            {
                return  poke<X>(o, delim_be_write<X, O>, cx) &&
                            chr::encode_range<X>(o, t, t + s, cx) &&
                        poke<X>(o, delim_en_write<X, O>, cx)
                ;
            }
        template <typename X, typename O, typename T, typename Cx>
            inline auto pointer_write_(O& o, const T* t, std::size_t s, Cx& cx)
                 -> std::enable_if_t< is_raw_<X>::value, bool>
            {
                return  poke<X>(o, delim_be_write<X, O>, cx) &&
                            poke<X>(o, t, s, cx) &&
                        poke<X>(o, delim_en_write<X, O>, cx)
                ;
            }
    }
    template <typename X, typename O, typename T, typename Cx>
        inline auto pointer_write(O& o, const T* t, std::size_t s, Cx& cx)
             -> std::enable_if_t<is_char<T>::value, bool>
        {
            return imp::pointer_write_<X>(o, t, s, cx);
        }
    template <typename X, typename O, typename T, typename Cx>
        inline auto pointer_write(O& o, const T* t, Cx& cx)
            -> std::enable_if_t<is_char<T>::value, bool>
        {
            return t ?
                pointer_write<X>(o, t, std::char_traits<T>::length(t), cx) :
                poke<X>(o, X::id::nil, cx)
            ;
        }

}}}

#endif // CXON_CIO_STRING_HXX_
