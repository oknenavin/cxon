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

#ifndef CXON_CBOR_LIB_STD_CHRONO_HXX_
#define CXON_CBOR_LIB_STD_CHRONO_HXX_

namespace cxon {

    template <typename X, typename R, typename P>
        struct read<CBOR<X>, std::chrono::duration<R, P>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::chrono::duration<R, P>& t, II& i, II e, Cx& cx) {
                    R c{};
                    return read_value<Y>(c, i, e, cx) && (t = std::chrono::duration<R, P>(c), true);
                }
        };

    template <typename X, typename R, typename P>
        struct write<CBOR<X>, std::chrono::duration<R, P>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::chrono::duration<R, P>& t, Cx& cx) {
                    return write_value<Y>(o, t.count(), cx);
                }
        };

    template <typename X, typename C, typename D>
        struct read<CBOR<X>, std::chrono::time_point<C, D>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::chrono::time_point<C, D>& t, II& i, II e, Cx& cx) {
                    D d{};
                    return read_value<Y>(d, i, e, cx) && (t = std::chrono::time_point<C, D>(d), true);
                }
        };

    template <typename X, typename C, typename D>
        struct write<CBOR<X>, std::chrono::time_point<C, D>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::chrono::time_point<C, D>& t, Cx& cx) {
                    return write_value<Y>(o, t.time_since_epoch(), cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_CHRONO_HXX_
