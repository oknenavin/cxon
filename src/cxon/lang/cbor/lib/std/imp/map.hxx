// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_IMP_MAP_HXX_
#define CXON_CBOR_LIB_STD_IMP_MAP_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon { namespace cbor { namespace imp {

    template <typename X, typename M>
        struct map_element_reader_ {
            template <typename II, typename Cx>
                static bool read(M& t, II& i, II e, Cx& cx) {
                    typename M::key_type    k = alc::create_using_allocator_of<typename M::key_type>(t);
                    typename M::mapped_type v = alc::create_using_allocator_of<typename M::mapped_type>(t);
                    typename M::value_type *p;
                    return  read_value<X>(k, i, e, cx) &&
                            (p = &cxon::cnt::emplace(t, std::move(k), std::move(v)), true) &&
                            read_value<X>(p->second, i, e, cx)
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
                    return cbor::cnt::write_map<X>(o, m, cx);
                }
        };

}}}

#endif // CXON_CBOR_LIB_STD_IMP_MAP_HXX_
