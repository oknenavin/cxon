// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_STRING_VIEW_HXX_
#define CXON_CBOR_LIB_STD_STRING_VIEW_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::basic_string_view<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::basic_string_view<T, R...>&, II&, II, Cx&) {
                    static_assert(unexpected<std::basic_string_view<T, R...>>(), "std::string_view cannot be modified");
                    return false;
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::basic_string_view<T, R...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::basic_string_view<T, R...>& t, Cx& cx) {
                    return cbor::cnt::write_array<Y>(o, t, cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_STRING_VIEW_HXX_
