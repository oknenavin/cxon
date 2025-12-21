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

#ifndef CXON_JSON_LIB_STD_BITSET_HXX_
#define CXON_JSON_LIB_STD_BITSET_HXX_

namespace cxon { namespace cio { // type traits
    template <std::size_t N> struct is_string<std::bitset<N>> : std::true_type {};
}}

namespace cxon { namespace json { namespace imp {

    template <typename X, std::size_t N, typename II, typename Cx>
        inline bool read_bits_(std::bitset<N>& t, II& i, II e, Cx& cx) {
            for (std::size_t p = N; p != 0; ) {
                char const c = cio::peek(i, e);
                    if (c != '0' && c != '1') return cx/json::read_error::unexpected;
                t.set(--p, c == '1'), ++i;
            }
            return true;
        }

    template <typename X, std::size_t N, typename O, typename Cx>
        inline bool write_bits_(O& o, const std::bitset<N>& t, Cx& cx) {
            //char bs[N];
            //    for (std::size_t i = 0; i != N; ++i) {
            //        bs[i] = t[N - i - 1] ? '1' : '0';
            //    }
            //return cio::poke<X>(o, bs, N, cx);
            for (std::size_t p = N; p != 0; ) {
                if (!cio::poke<X>(o, t[--p] ? '1' : '0', cx)) return false;
            }
            return true;
        }

}}}

namespace cxon {

    template <typename X, std::size_t N>
        struct read<JSON<X>, std::bitset<N>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::bitset<N>& t, II& i, II e, Cx& cx) {
                    return  cio::consume<Y>(cio::str::delim_be_read<Y, II>, i, e, cx) &&
                                json::imp::read_bits_<Y>(t, i, e, cx) &&
                            cio::consume<Y>(cio::str::delim_en_read<Y, II>, i, e, cx)
                    ;
                }
        };

    template <typename X, std::size_t N>
        struct write<JSON<X>, std::bitset<N>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::bitset<N>& t, Cx& cx) {
                    return  cio::poke<Y>(o, cio::str::delim_be_write<Y, O>, cx) &&
                                json::imp::write_bits_<Y>(o, t, cx) &&
                            cio::poke<Y>(o, cio::str::delim_en_write<Y, O>, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_BITSET_HXX_
