// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_LIST_HXX_
#define CXON_CBOR_LIB_STD_LIST_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon {

    template <typename T, typename ...R>
        struct container_traits<std::list<T, R...>> {
            template <typename II>
                static bool append(std::list<T, R...>& c, II f, II l) {
                    return c.insert(c.end(), f, l), true;
                }
        };

}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::list<T, R...>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::list<T, R...>& t, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array<Y>(t, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::list<T, R...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::list<T, R...>& t, Cx& cx) {
                    return cbor::cnt::write_array<Y>(o, t, cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_LIST_HXX_
