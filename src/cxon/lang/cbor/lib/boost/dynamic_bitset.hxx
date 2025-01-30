// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_BOOST_DYNAMIC_BITSET_HXX_
#define CXON_CBOR_LIB_BOOST_DYNAMIC_BITSET_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon {

    namespace cbor { namespace imp {

        template <typename X, typename B, typename A, typename II, typename Cx>
            inline bool read_bitset_(boost::dynamic_bitset<B, A>& t, II& i, II e, Cx& cx) {
                switch (bio::peek(i, e) & X::mjr) {
                    case X::bstr: {
                        std::size_t N;
                        if (!cbor::cnt::read_size<X>(N, i, e, cx))
                            return false;
                        if (i == e)
                            return cx/cbor::read_error::unexpected;
                        bio::byte const S = *i;
                        t.resize(t.size() + --N * 8);
                            for (++i; N != 0 && i != e; --N, ++i) {
                                bio::byte const b = *i; t <<= 8;
                                t.set(7, b & 0x80); t.set(6, b & 0x40); t.set(5, b & 0x20); t.set(4, b & 0x10);
                                t.set(3, b & 0x08); t.set(2, b & 0x04); t.set(1, b & 0x02); t.set(0, b & 0x01);
                            }
                        t.resize(t.size() - S);
                        return N == 0 || cx/cbor::read_error::unexpected;
                    }
                    default:
                        return cx/cbor::read_error::unexpected;
                }
            }

        template <typename X, typename O, typename B, typename A, typename Cx>
            inline bool write_bitset_(O& o, const boost::dynamic_bitset<B, A>& t, Cx& cx) {
                auto const N = t.size() / 8 + (t.size() % 8 ? 1 : 0);
                if (!cbor::cnt::write_size<X>(o, X::bstr, N + 1, cx))
                    return false;
                if (!bio::poke<X>(o, bio::byte(N * 8 - t.size()), cx))
                    return false;
                bio::byte b = 0;
                for (std::size_t i = t.size(); i != 0; --i) {
                    b |= t[i - 1] << ((i - 1) % 8);
                    if ((i - 1) % 8 == 0) {
                        if (!bio::poke<X>(o, b, cx))
                            return false;
                        b = 0;
                    }
                }
                return true;
            }

    }}

    template <typename X, typename B, typename A>
        struct read<CBOR<X>, boost::dynamic_bitset<B, A>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(boost::dynamic_bitset<B, A>& t, II& i, II e, Cx& cx) {
                    return  cbor::tag::read<Y>(i, e, cx) &&
                            cbor::imp::read_bitset_<Y>(t, i, e, cx)
                    ;
                }
        };

    template <typename X, typename B, typename A>
        struct write<CBOR<X>, boost::dynamic_bitset<B, A>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const boost::dynamic_bitset<B, A>& t, Cx& cx) {
                    return cbor::imp::write_bitset_<Y>(o, t, cx);
                }
        };

}

#endif // CXON_CBOR_LIB_BOOST_DYNAMIC_BITSET_HXX_
