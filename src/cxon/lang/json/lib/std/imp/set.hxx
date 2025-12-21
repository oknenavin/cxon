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

#ifndef CXON_JSON_LIB_STD_IMP_SET_HXX_
#define CXON_JSON_LIB_STD_IMP_SET_HXX_

#include "cxon/lang/common/cio/container.hxx"

namespace cxon { namespace json { namespace imp {

    template <typename X, typename S>
        struct set_element_reader_ {
            template <typename II, typename Cx>
                static bool read(S& t, II& i, II e, Cx& cx) {
                    auto v = alc::create_using_allocator_of<typename S::value_type>(t);
                    return  read_value<X>(v, i, e, cx) &&
                            (t.emplace(std::move(v)), true)
                    ;
                }
        };

    template <typename X, typename S>
        struct set_reader_ {
            template <typename II, typename Cx>
                static bool value(S& s, II& i, II e, Cx& cx) {
                    return cio::cnt::read_list<X>(s, i, e, cx);
                }
        };

    template <typename X, typename S>
        struct set_writer_ {
            template <typename O, typename Cx>
                static bool value(O& o, const S& s, Cx& cx) {
                    return cio::cnt::write_list<X>(o, s, cx);
                }
        };

}}}

#endif // CXON_JSON_LIB_STD_IMP_SET_HXX_
