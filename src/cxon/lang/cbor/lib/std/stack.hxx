// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_STACK_HXX_
#define CXON_CBOR_LIB_STD_STACK_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::stack<T, R...>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::stack<T, R...>& t, II& i, II e, Cx& cx) {
                    auto& c = cnt::container(t);
                    return cbor::cnt::read_array<Y>(c, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::stack<T, R...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::stack<T, R...>& t, Cx& cx) {
                    auto const& c = cnt::container(t);
                    return cbor::cnt::write_array<Y>(o, c, cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_STACK_HXX_
