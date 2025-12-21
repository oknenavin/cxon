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

#ifndef CXON_CBOR_LIB_STD_MEMORY_HXX_
#define CXON_CBOR_LIB_STD_MEMORY_HXX_

#include "cxon/lang/common/allocator.hxx"

namespace cxon {

    template <typename X, typename T, typename D>
        struct read<CBOR<X>, std::unique_ptr<T, D>, enable_if_t<!std::is_array<T>::value>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::unique_ptr<T, D>& t, II& i, II e, Cx& cx) {
                        T* p;
                    return read_value<Y>(p, i, e, cx) && (t.reset(p), true);
                }
        };

    template <typename X, typename T, typename D>
        struct write<CBOR<X>, std::unique_ptr<T, D>, enable_if_t<!std::is_array<T>::value>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::unique_ptr<T, D>& t, Cx& cx) {
                    return write_value<Y>(o, t.get(), cx);
                }
        };

    template <typename X, typename T>
        struct read<CBOR<X>, std::shared_ptr<T>, enable_if_t<!std::is_array<T>::value>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::shared_ptr<T>& t, II& i, II e, Cx& cx) {
                    auto al = alc::make_context_allocator<T>(cx);
                        T* p;
#                   if __cplusplus < 201402L
                        return read_value<Y>(p, i, e, cx) && (t.reset(p, [al](T* p) mutable { al.release(p); }), true);
#                   else
                        return read_value<Y>(p, i, e, cx) && (t.reset(p, [al = std::move(al)](T* p) mutable { al.release(p); }), true);
#                   endif
                }
        };

    template <typename X, typename T>
        struct write<CBOR<X>, std::shared_ptr<T>, enable_if_t<!std::is_array<T>::value>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::shared_ptr<T>& t, Cx& cx) {
                    return write_value<Y>(o, t.get(), cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_MEMORY_HXX_
