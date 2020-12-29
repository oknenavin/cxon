// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_BITS_MAP_HXX_
#define CXON_CBOR_LIB_STD_BITS_MAP_HXX_

namespace cxon { namespace cbor { namespace bits {

    template <typename X, typename M>
        struct map_element_reader {
            template <typename II, typename Cx>
                static bool read(M& m, II& i, II e, Cx& cx) {
                    typename M::key_type k{}; typename M::mapped_type v{};
                    return  read_value<X>(k, i, e, cx) &&
                            read_value<X>(v, i, e, cx) &&
                            (m.emplace(std::move(k), std::move(v)), true)
                    ;
                }
        };

    template <typename X, typename M>
        struct map_element_writer {
            template <typename O, typename Cx>
                static bool write(O& o, const typename M::value_type& t, Cx& cx) {
                    return  write_value<X>(o, t.first, cx) &&
                            write_value<X>(o, t.second, cx)
                    ;
                }
        };

}}}

#endif // CXON_CBOR_LIB_STD_BITS_MAP_HXX_
