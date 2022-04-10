// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_BITSET_HXX_
#define CXON_JSON_LIB_STD_BITSET_HXX_

namespace cxon { namespace cio { namespace key {
    template <std::size_t N> struct is_quoted<std::bitset<N>> : std::true_type {};
}}}

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
                    return  cio::consume<Y>(Y::string::template del_read<II>, i, e, cx) &&
                                json::imp::read_bits_<Y>(t, i, e, cx) &&
                            cio::consume<Y>(Y::string::template del_read<II>, i, e, cx)
                    ;
                }
        };

    template <typename X, std::size_t N>
        struct write<JSON<X>, std::bitset<N>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::bitset<N>& t, Cx& cx) {
                    return  cio::poke<Y>(o, Y::string::template del_write<O>, cx) &&
                                json::imp::write_bits_<Y>(o, t, cx) &&
                            cio::poke<Y>(o, Y::string::template del_write<O>, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_BITSET_HXX_
