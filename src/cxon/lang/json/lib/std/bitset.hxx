// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_BITSET_HXX_
#define CXON_JSON_LIB_STD_BITSET_HXX_

namespace cxon { namespace chio { namespace bits {
    template <size_t N> struct is_quoted<std::bitset<N>> : std::true_type {};
}}}

namespace cxon { namespace bits {

    template <typename X, size_t N, typename II, typename Cx>
        inline bool read_bits(std::bitset<N>& t, II& i, II e, Cx& cx) {
            for (size_t p = N; p != 0; ) {
                char const c = chio::peek(i, e);
                    if (c != '0' && c != '1') return cx|chio::read_error::unexpected;
                t.set(--p, c == '1'), chio::next(i, e);
            }
            return true;
        }

    template <typename X, size_t N, typename O, typename Cx>
        inline bool write_bits(O& o, const std::bitset<N>& t, Cx& cx) {
            for (size_t p = N; p != 0; ) {
                if (!chio::poke<X>(o, t[--p] ? '1' : '0', cx)) return false;
            }
            return true;
        }

}}  // cxon::bits

namespace cxon {

    template <typename X, size_t N>
        struct read<X, std::bitset<N>> {
            template <typename II, typename Cx>
                static bool value(std::bitset<N>& t, II& i, II e, Cx& cx) {
                    return  chio::strs::consume_str<X>::beg(i, e, cx) &&
                                bits::read_bits<X>(t, i, e, cx) &&
                            chio::strs::consume_str<X>::end(i, e, cx)
                    ;
                }
        };

    template <typename X, size_t N>
        struct write<X, std::bitset<N>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::bitset<N>& t, Cx& cx) {
                    return  chio::poke<X>(o, chio::strs::str<X>::beg, cx) &&
                                bits::write_bits<X>(o, t, cx) &&
                            chio::poke<X>(o, chio::strs::str<X>::end, cx)
                    ;
                }
        };

}   // cxon

#endif // CXON_JSON_LIB_STD_BITSET_HXX_
