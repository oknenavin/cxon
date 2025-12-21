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

#ifndef CXON_CBOR_LIB_STD_UTILITY_HXX_
#define CXON_CBOR_LIB_STD_UTILITY_HXX_

#include "cxon/lang/cbor/compound.hxx"

namespace cxon {

    template <typename X, typename F, typename S>
        struct read<CBOR<X>, std::pair<F, S>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::pair<F, S>& t, II& i, II e, Cx& cx) {
                    return  cbor::tag::read<Y>(i, e, cx) &&
                            cbor::cnt::read_size_eq<Y>(2, i, e, cx) &&
                                read_value<Y>(t.first, i, e, cx) &&
                                read_value<Y>(t.second, i, e, cx)
                    ;
                }
        };

    template <typename X, typename F, typename S>
        struct write<CBOR<X>, std::pair<F, S>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::pair<F, S>& t, Cx& cx) {
                    return  cbor::cnt::write_size<Y>(o, Y::arr, 2, cx) &&
                                write_value<Y>(o, t.first, cx) &&
                                write_value<Y>(o, t.second, cx)
                    ;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_UTILITY_HXX_
