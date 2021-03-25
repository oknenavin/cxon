// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_BITSET_HXX_
#define CXON_JSON_LIB_STD_BITSET_HXX_

namespace cxon { namespace cio { namespace key {
    template <size_t N> struct is_quoted<std::bitset<N>> : std::true_type {};
}}}

namespace cxon { namespace json { namespace bits {

    template <typename X, size_t N, typename II, typename Cx>
        inline bool read_bits(std::bitset<N>& t, II& i, II e, Cx& cx) {
            for (size_t p = N; p != 0; ) {
                char const c = cio::peek(i, e);
                    if (c != '0' && c != '1') return cx/json::read_error::unexpected;
                t.set(--p, c == '1'), cio::next(i, e);
            }
            return true;
        }

    template <typename X, size_t N, typename O, typename Cx>
        inline bool write_bits(O& o, const std::bitset<N>& t, Cx& cx) {
            for (size_t p = N; p != 0; ) {
                if (!cio::poke<X>(o, t[--p] ? '1' : '0', cx)) return false;
            }
            return true;
        }

}}}

namespace cxon {

    template <typename X, size_t N>
        struct read<JSON<X>, std::bitset<N>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::bitset<N>& t, II& i, II e, Cx& cx) {
                    return  cio::consume<X>(X::string::beg, i, e, cx) &&
                                json::bits::read_bits<J>(t, i, e, cx) &&
                            cio::consume<X>(X::string::end, i, e, cx)
                    ;
                }
        };

    template <typename X, size_t N>
        struct write<JSON<X>, std::bitset<N>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::bitset<N>& t, Cx& cx) {
                    return  cio::poke<J>(o, J::string::beg, cx) &&
                                json::bits::write_bits<J>(o, t, cx) &&
                            cio::poke<J>(o, J::string::end, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_BITSET_HXX_
