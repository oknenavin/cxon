// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_OPTIONAL_HXX_
#define CXON_JSON_LIB_STD_OPTIONAL_HXX_

namespace cxon {

    template <typename X, typename T>
        struct read<JSON<X>, std::optional<T>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::optional<T>& t, II& i, II e, Cx& cx) {
                    if (cio::peek(i, e) == *J::id::nil) { // TODO: not correct as T may start with *X::id::nil (e.g. 'nan')
                        II const o = i;
                        return cio::consume<J>(J::id::nil, i, e) || (cio::rewind(i, o), cx|json::read_error::unexpected);
                    }
                    return read_value<J>(t.emplace(), i, e, cx);
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, std::optional<T>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::optional<T>& t, Cx& cx) {
                    return t.has_value() ?
                        write_value<J>(o, t.value(), cx) :
                        cio::poke<J>(o, J::id::nil, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_OPTIONAL_HXX_
