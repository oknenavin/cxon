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

#ifndef CXON_CIO_SIMD_HXX_
#define CXON_CIO_SIMD_HXX_

#include "cio.hxx"

#if CXON_USE_SIMD_SSE2
#   include <emmintrin.h>
#endif

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace simd {

    template <typename X, typename P8>
        inline P8 find_first_read_break(P8 b, P8 e);
    template <typename X, typename P8>
        inline P8 find_first_write_break(P8 b, P8 e);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace simd { namespace imp {

#   if CXON_USE_SIMD_SSE2
        inline int find_first_set_(unsigned int value)
        {   // returns the number of trailing 0-bits in x, starting at the least significant bit position. if x is 0, the result is undefined
#           ifdef _MSC_VER
                CXON_ASSERT(value, "unexpected invocaion");
                unsigned long f;
                    _BitScanForward(&f, value);
                return f;
#           else
                return __builtin_ffs(value) - 1;
#           endif
        }
#   endif

    template <typename X>
        using is_raw_ = has_traits<X, str::raw_traits>;
    template <typename X>
        using has_quotes_ = bool_constant<is_unquoted_key_context<X>::value || !is_quoted_key_context<X>::value>;

    template <typename X>
        inline auto check_read_break_condition_(char c) -> std::enable_if_t<!is_raw_<X>::value &&  has_quotes_<X>::value, bool>
        { return c == X::string::del || c == '\\' || c <= '\x1F'; }
    template <typename X>
        inline auto check_read_break_condition_(char c) -> std::enable_if_t<!is_raw_<X>::value && !has_quotes_<X>::value, bool>
        { return c == X::map::div || c == ' ' || c == '\\' || c <= '\x1F'; }
    template <typename X>
        inline auto check_read_break_condition_(char c) -> std::enable_if_t< is_raw_<X>::value &&  has_quotes_<X>::value, bool>
        { return c == X::string::del; }
    template <typename X>
        inline auto check_read_break_condition_(char c) -> std::enable_if_t< is_raw_<X>::value && !has_quotes_<X>::value, bool>
        { return c == X::map::div || c == ' ' || c == '\n' || c == '\t' || c == '\r'; }

    template <typename X>
        inline auto check_write_break_condition_(unsigned c) -> std::enable_if_t<!X::produce_strict_javascript &&  has_quotes_<X>::value, bool>
        { return c == X::string::del || c == '\\' || c <= 0x1F; }
    template <typename X>
        inline auto check_write_break_condition_(unsigned c) -> std::enable_if_t<!X::produce_strict_javascript && !has_quotes_<X>::value, bool>
        { return c == X::map::div || c == ' ' || c == '\\' || c <= 0x1F; }
    template <typename X>
        inline auto check_write_break_condition_(unsigned c) -> std::enable_if_t< X::produce_strict_javascript &&  has_quotes_<X>::value, bool>
        { return c == X::string::del || c == '\\' || c <= 0x1F || c == 0xE2; }
    template <typename X>
        inline auto check_write_break_condition_(unsigned c) -> std::enable_if_t< X::produce_strict_javascript && !has_quotes_<X>::value, bool>
        { return c == X::map::div || c == ' ' || c == '\\' || c <= 0x1F || c == 0xE2; }

}}}}

namespace cxon { namespace cio { namespace simd {

    template <typename X, typename P8>
        inline P8 find_first_read_break(P8 b, P8 e)
        {   CXON_ASSERT(b < e, "unexpected invocaion");
#           if CXON_USE_SIMD_SSE2
                if (e - b >= 16) {
                    __m128i const vq = _mm_set1_epi8(X::string::del);
                    __m128i const vd = _mm_set1_epi8(X::map::div);
                    __m128i const vs = _mm_set1_epi8(' ');
                    __m128i const vb = _mm_set1_epi8('\\');
                    __m128i const vc = _mm_set1_epi8('\x1F');
                    do {
                        __m128i const   vi = _mm_loadu_si128((__m128i const*)b);
                        __m128i         rs;
                                    CXON_IF_CONSTEXPR (imp::has_quotes_<X>::value) {
                                        rs =                  _mm_cmpeq_epi8(vi, vq);                       // quotes
                                    }
                                    else {
                                        rs =                  _mm_cmpeq_epi8(vi, vd);                       // key/value delimiter
                                        rs = _mm_or_si128(rs, _mm_cmpeq_epi8(vi, vs));                      // space
                                    }
                                    CXON_IF_CONSTEXPR (!imp::is_raw_<X>::value) {
                                        rs = _mm_or_si128(rs, _mm_cmpeq_epi8(vi, vb));                      // escape
                                        rs = _mm_or_si128(rs, _mm_cmplt_epi8(vi, vc));                      // control & multi-bytes characters
                                    }
                                    else {
                                        rs = _mm_or_si128(rs, _mm_cmpeq_epi8(vi, _mm_min_epu8(vi, vc)));    // control characters
                                    }
                        if (int const   mk = _mm_movemask_epi8(rs)) {
                            return b + imp::find_first_set_(mk);
                        }
                        b += 16;
                    }   while (e - b >= 16);
                }
#           endif
            for ( ; b != e; ++b) {
                if (imp::check_read_break_condition_<X>(*b))
                    return b;
            }
            return e;
        }

    template <typename X, typename P8>
        inline P8 find_first_write_break(P8 b, P8 e)
        {   CXON_ASSERT(b < e, "unexpected invocaion");
#           if CXON_USE_SIMD_SSE2
                if (e - b >= 16) {
                    __m128i const vq = _mm_set1_epi8(X::string::del);
                    __m128i const vd = _mm_set1_epi8(X::map::div);
                    __m128i const vs = _mm_set1_epi8(' ');
                    __m128i const vb = _mm_set1_epi8('\\');
                    __m128i const vc = _mm_set1_epi8('\x1F');
                    __m128i const ve = _mm_set1_epi8('\xE2');
                    do {
                        __m128i const   vi = _mm_loadu_si128((__m128i const*)b);
                        __m128i         rs;
                                    CXON_IF_CONSTEXPR (imp::has_quotes_<X>::value) {
                                        rs =                  _mm_cmpeq_epi8(vi, vq);                       // quotes
                                    }
                                    else {
                                        rs =                  _mm_cmpeq_epi8(vi, vd);                       // key/value delimiter
                                        rs = _mm_or_si128(rs, _mm_cmpeq_epi8(vi, vs));                      // space
                                    }
                                        rs = _mm_or_si128(rs, _mm_cmpeq_epi8(vi, vb));                      // escape
                                        rs = _mm_or_si128(rs, _mm_cmpeq_epi8(vi, _mm_min_epu8(vi, vc)));    // control characters (<= 0x1F)
                                    CXON_IF_CONSTEXPR (X::produce_strict_javascript) {
                                        rs = _mm_or_si128(rs, _mm_cmpeq_epi8(vi, ve));                      // U+2028 LINE SEPARATOR and U+2029 PARAGRAPH SEPARATOR
                                    }
                        if (int const   mk = _mm_movemask_epi8(rs)) {
                            return b + imp::find_first_set_(mk);
                        }
                        b += 16;
                    }   while (e - b >= 16);
                }
#           endif
            for ( ; b != e; ++b) {
                if (imp::check_write_break_condition_<X>((unsigned char)*b))
                    return b;
            }
            return e;
        }

}}}

#endif // CXON_CIO_SIMD_HXX_
