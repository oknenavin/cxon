// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_BITSET_HXX_
#define CXON_CBOR_LIB_STD_BITSET_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon {

    namespace cbor { namespace imp {

        template <typename X, size_t N, typename II, typename Cx>
            inline auto read_bitset_(std::bitset<N>& t, II& i, II e, Cx& cx) -> enable_if_t<N >= 8, bool> {
                switch (bio::peek(i, e) & X::mjr) {
                    case X::bstr: {
                        if (!cbor::cnt::read_size_eq<X>(N / 8 + size_t(!!(N % 8)), i, e, cx))
                            return false;
                        if (i == e)
                            return cx/cbor::read_error::unexpected;
                        if (N % 8)
                            t |= std::bitset<N> {*i++};
                        size_t n = N / 8;
                            for ( ; n != 0 && i != e; --n, ++i) {
                                bio::byte const b = *i; t <<= 8;
                                t.set(7, b & 0x80); t.set(6, b & 0x40); t.set(5, b & 0x20); t.set(4, b & 0x10);
                                t.set(3, b & 0x08); t.set(2, b & 0x04); t.set(1, b & 0x02); t.set(0, b & 0x01);
                            }
                        return n == 0 || cx/cbor::read_error::unexpected;
                    }
                    default:
                        return cx/cbor::read_error::unexpected;
                }
            }
        template <typename X, size_t N, typename II, typename Cx>
            inline auto read_bitset_(std::bitset<N>& t, II& i, II e, Cx& cx) -> enable_if_t<N  < 8, bool> {
                switch (bio::peek(i, e) & X::mjr) {
                    case X::bstr: {
                        if (!cbor::cnt::read_size_eq<X>(N / 8 + size_t(!!(N % 8)), i, e, cx))
                            return false;
                        if (i == e)
                            return cx/cbor::read_error::unexpected;
                        return t |= std::bitset<N> {*i++}, true;
                    }
                    default:
                        return cx/cbor::read_error::unexpected;
                }
            }

    }}

    template <typename X, size_t N>
        struct read<CBOR<X>, std::bitset<N>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::bitset<N>& t, II& i, II e, Cx& cx) {
                    return  cbor::tag::read<Y>(i, e, cx) &&
                            cbor::imp::read_bitset_<Y>(t, i, e, cx)
                    ;
                }
        };

    template <typename X, size_t N>
        struct write<CBOR<X>, std::bitset<N>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::bitset<N>& t, Cx& cx) {
                    if (!cbor::cnt::write_size<Y>(o, Y::bstr, N / 8 + size_t(!!(N % 8)), cx))
                        return false;

                    bio::byte b = 0;
                    for (size_t i = t.size(); i != 0; --i) {
                        b |= t[i - 1] << ((i - 1) % 8);
                        if ((i - 1) % 8 == 0) {
                            if (!bio::poke<Y>(o, b, cx))
                                return false;
                            b = 0;
                        }
                    }
                    return true;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_BITSET_HXX_
