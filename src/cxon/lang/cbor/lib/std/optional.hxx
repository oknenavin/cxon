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

#ifndef CXON_CBOR_LIB_STD_OPTIONAL_HXX_
#define CXON_CBOR_LIB_STD_OPTIONAL_HXX_

namespace cxon {

    template <typename X, typename T>
        struct read<CBOR<X>, std::optional<T>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::optional<T>& t, II& i, II e, Cx& cx) {
                    return cbor::tag::read<Y>(i, e, cx) && (
                        (bio::peek(i, e) == Y::und && (bio::get(i, e), t = std::nullopt, true)) ||
                        (read_value<Y>(t.emplace(), i, e, cx))
                    );
                }
        };

    template <typename X, typename T>
        struct write<CBOR<X>, std::optional<T>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::optional<T>& t, Cx& cx) {
                    return t.has_value() ?
                        write_value<Y>(o, t.value(), cx) :
                        bio::poke<Y>(o, Y::und, cx)
                    ;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_OPTIONAL_HXX_
