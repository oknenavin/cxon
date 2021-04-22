// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_IMP_MAP_HXX_
#define CXON_CBOR_LIB_STD_IMP_MAP_HXX_

namespace cxon { namespace cbor { namespace imp {

    template <typename X, typename M>
        struct map_element_reader_ {
            template <typename II, typename Cx>
                static bool read(M& m, II& i, II e, Cx& cx) {
                    typename M::key_type k{}; typename M::mapped_type v{}; // TODO: allocator
                    return  read_value<X>(k, i, e, cx) &&
                            read_value<X>(v, i, e, cx) &&
                            (m.emplace(std::move(k), std::move(v)), true)
                    ;
                }
        };

    template <typename X, typename M>
        struct map_element_writer_ {
            template <typename O, typename Cx>
                static bool write(O& o, const typename M::value_type& t, Cx& cx) {
                    return  write_value<X>(o, t.first, cx) &&
                            write_value<X>(o, t.second, cx)
                    ;
                }
        };

    template <typename X, typename M>
        struct map_reader_ {
            template <typename II, typename Cx>
                static bool value(M& m, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<X>(m, i, e, cx);
                }
        };

    template <typename X, typename M>
        struct map_writer_ {
            template <typename O, typename Cx>
                static bool value(O& o, const M& m, Cx& cx) {
                    return cbor::cnt::write_array<X, X::map>(o, m, cx);
                }
        };

}}}

#endif // CXON_CBOR_LIB_STD_IMP_MAP_HXX_
