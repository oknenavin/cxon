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

#ifndef CXON_CBOR_LIB_STD_ARRAY_HXX_
#define CXON_CBOR_LIB_STD_ARRAY_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon {

    template <typename X, typename T, std::size_t N>
        struct read<CBOR<X>, std::array<T, N>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::array<T, N>& t, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<Y>(t.begin(), t.end(), i, e, cx);
                }
        };

    template <typename X, typename T, std::size_t N>
        struct write<CBOR<X>, std::array<T, N>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::array<T, N>& t, Cx& cx) {
                    return cbor::cnt::write_array<Y>(o, t, cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_ARRAY_HXX_
