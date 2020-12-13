// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_VARIANT_HXX_
#define CXON_CBOR_LIB_STD_VARIANT_HXX_

namespace cxon {

    template <typename X, typename II, typename Cx>
        inline auto read_value(std::monostate&, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {
            return CXON_ASSERT(0, "TODO"), false;
        }

    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, std::monostate, Cx& cx) -> enable_for_t<X, CBOR> {
            return CXON_ASSERT(0, "TODO"), false;
        }

    template <typename X, typename ...T>
        struct read<CBOR<X>, std::variant<T...>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::variant<T...>& t, II& i, II e, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

    template <typename X, typename ...T>
        struct write<CBOR<X>, std::variant<T...>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::variant<T...>& t, Cx& cx) {
                    return CXON_ASSERT(0, "TODO"), false;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_VARIANT_HXX_
