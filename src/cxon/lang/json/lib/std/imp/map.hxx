// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_IMP_MAP_HXX_
#define CXON_JSON_LIB_STD_IMP_MAP_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon { namespace json { namespace imp {

    template <typename X, typename M>
        struct map_element_reader_ {
            template <typename II, typename Cx>
                static bool read(M& t, II& i, II e, Cx& cx) {
                    typename M::key_type    k = alc::create_using_allocator_of<typename M::key_type>(t);
                    typename M::mapped_type v = alc::create_using_allocator_of<typename M::mapped_type>(t);
                    typename M::value_type *p;
                    return  cio::read_map_key<X>(k, i, e, cx) &&
                            (p = &cnt::emplace(t, std::move(k), std::move(v)), true) &&
                            cio::read_map_val<X>(p->second, i, e, cx)
                    ;
                }
        };

    template <typename X, typename M>
        struct map_element_writer_ {
            template <typename O, typename Cx>
                static bool write(O& o, const typename M::value_type& e, Cx& cx) {
                    return  cio::write_map_key<X>(o, e.first, cx) &&
                            cio::write_map_val<X>(o, e.second, cx)
                    ;
                }
        };

    template <typename X, typename M>
        struct map_reader_ {
            template <typename II, typename Cx>
                static bool value(M& m, II& i, II e, Cx& cx) {
                    return cio::cnt::read_map<X>(m, i, e, cx);
                }
        };

    template <typename X, typename M>
        struct map_writer_ {
            template <typename O, typename Cx>
                static bool value(O& o, const M& m, Cx& cx) {
                    return cio::cnt::write_map<X>(o, m, cx);
                }
        };

}}}

#endif // CXON_JSON_LIB_STD_IMP_MAP_HXX_
