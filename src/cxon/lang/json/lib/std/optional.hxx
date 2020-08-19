// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_OPTIONAL_HXX_
#define CXON_JSON_LIB_STD_OPTIONAL_HXX_

namespace cxon {

    template <typename X, typename T>
        struct read<X, std::optional<T>> {
            template <typename II, typename Cx>
                static bool value(std::optional<T>& t, II& i, II e, Cx& cx) {
                    if (chio::peek(i, e) == *X::id::nil) { // TODO: not correct as T may start with *X::id::nil (e.g. 'nan')
                        II const o = i;
                        return chio::consume<X>(X::id::nil, i, e) || (chio::rewind(i, o), cx|chio::read_error::unexpected);
                    }
                    return read_value<X>(t.emplace(), i, e, cx);
                }
        };

    template <typename X, typename T>
        struct write<X, std::optional<T>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::optional<T>& t, Cx& cx) {
                    return t.has_value() ?
                        write_value<X>(o, t.value(), cx) :
                        chio::poke<X>(o, X::id::nil, cx)
                    ;
                }
        };

}   // cxon

#endif // CXON_JSON_LIB_STD_OPTIONAL_HXX_
