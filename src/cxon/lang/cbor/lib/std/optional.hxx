// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_OPTIONAL_HXX_
#define CXON_CBOR_LIB_STD_OPTIONAL_HXX_

namespace cxon {

    template <typename X, typename T>
        struct read<CBOR<X>, std::optional<T>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::optional<T>& t, II& i, II e, Cx& cx) {
                    return cbor::tag::read<Y>(i, e, cx) && (
                        (bio::peek(i, e) == Y::und && (bio::get(i, e), true)) ||
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
