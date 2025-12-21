// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

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
                    return  cio::read_map_key<X>(k, i, e, cx) &&
                            cio::read_map_val<X>(cnt::emplace(t, std::move(k), std::move(v)).second, i, e, cx)
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
