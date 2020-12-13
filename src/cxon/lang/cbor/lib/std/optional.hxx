// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_OPTIONAL_HXX_
#define CXON_CBOR_LIB_STD_OPTIONAL_HXX_

namespace cxon {

    template <typename X, typename T>
        struct read<CBOR<X>, std::optional<T>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::optional<T>& t, II& i, II e, Cx& cx) {
                    return (bio::peek(i, e) == J::nil && (bio::get(i, e), true)) ||
                            read_value<J>(t.emplace(), i, e, cx)
                    ;
                }
        };

    template <typename X, typename T>
        struct write<CBOR<X>, std::optional<T>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::optional<T>& t, Cx& cx) {
                    return t.has_value() ?
                        write_value<J>(o, t.value(), cx) :
                        bio::poke<J>(o, J::nil, cx)
                    ;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_OPTIONAL_HXX_
