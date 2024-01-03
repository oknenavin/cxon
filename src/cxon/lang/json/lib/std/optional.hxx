// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_OPTIONAL_HXX_
#define CXON_JSON_LIB_STD_OPTIONAL_HXX_

namespace cxon { namespace cio { // type traits
    template <typename T> struct is_map<std::optional<T>>       : is_map<T> {};
    template <typename T> struct is_list<std::optional<T>>      : is_list<T> {};
    template <typename T> struct is_string<std::optional<T>>    : is_string<T> {};
    template <typename T> struct is_number<std::optional<T>>    : is_number<T> {};
    template <typename T> struct is_bool<std::optional<T>>      : is_bool<T> {};
    template <typename T> struct is_null<std::optional<T>>      : std::true_type {};
}}

namespace cxon {

    template <typename X, typename T>
        struct read<JSON<X>, std::optional<T>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::optional<T>& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<Y>(i, e, cx))
                        return false;
                    if (cio::peek(i, e) == *Y::id::nil) {
                        II const o = i;
                        return  (cio::consume<Y>(Y::id::nil, i, e) && (t = std::nullopt, true)) ||
                                (cio::rewind(i, o), cx/json::read_error::unexpected);
                    }
                    return read_value<Y>(t.emplace(), i, e, cx);
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, std::optional<T>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::optional<T>& t, Cx& cx) {
                    return t.has_value() ?
                        write_value<Y>(o, t.value(), cx) :
                        cio::poke<Y>(o, Y::id::nil, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_OPTIONAL_HXX_
