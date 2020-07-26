// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_BITSET_HXX_
#define CXON_JSON_LIB_STD_BITSET_HXX_

namespace cxon { namespace bits {

    template <size_t N> struct is_quoted<std::bitset<N>> : std::true_type {};

    template <typename X, size_t N, typename II, typename Cx>
        inline bool read_bits(std::bitset<N>& t, II& i, II e, Cx& cx) {
            for (size_t p = N; p != 0; ) {
                char const c = io::peek(i, e);
                    if (c != '0' && c != '1') return cx|read_error::unexpected;
                t.set(--p, c == '1'), io::next(i, e);
            }
            return true;
        }

    template <typename X, size_t N, typename O, typename Cx>
        inline bool write_bits(O& o, const std::bitset<N>& t, Cx& cx) {
            for (size_t p = N; p != 0; ) {
                if (!io::poke<X>(o, t[--p] ? '1' : '0', cx)) return false;
            }
            return true;
        }

}}  // cxon::bits

namespace cxon {

    template <typename X, size_t N>
        struct read<X, std::bitset<N>> {
            template <typename II, typename Cx>
                static bool value(std::bitset<N>& t, II& i, II e, Cx& cx) {
                    return bits::read_bits<X>(t, i, e, cx);
                }
        };
    template <typename X, size_t N>
        struct read<JSON<X>, std::bitset<N>> {
            template <typename II, typename Cx>
                static bool value(std::bitset<N>& t, II& i, II e, Cx& cx) {
                    return  bits::consume_str<JSON<X>>::beg(i, e, cx) &&
                                bits::read_bits<JSON<X>>(t, i, e, cx) &&
                            bits::consume_str<JSON<X>>::end(i, e, cx)
                    ;
                }
        };

    template <typename X, size_t N>
        struct write<X, std::bitset<N>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::bitset<N>& t, Cx& cx) {
                    return bits::write_bits<X>(o, t, cx);
                }
        };
    template <typename X, size_t N>
        struct write<JSON<X>, std::bitset<N>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::bitset<N>& t, Cx& cx) {
                    return  io::poke<JSON<X>>(o, bits::str<JSON<X>>::beg, cx) &&
                                bits::write_bits<JSON<X>>(o, t, cx) &&
                            io::poke<JSON<X>>(o, bits::str<JSON<X>>::end, cx)
                    ;
                }
        };

}   // cxon

#endif // CXON_JSON_LIB_STD_BITSET_HXX_
