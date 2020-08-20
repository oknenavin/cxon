// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_STRING_HXX_
#define CXON_JSON_LIB_STD_STRING_HXX_

#include "bits/string.hxx"

namespace cxon {

    template <typename T, typename ...R>
        struct continuous<std::basic_string<T, R...>> {
            static auto range(const std::basic_string<T, R...>& i) -> decltype(std::make_pair(&i[0], &i[0] + i.size())) {
                return std::make_pair(&i[0], &i[0] + i.size());
            }
        };

    template <typename X, typename T, typename ...R>
        struct read<X, std::basic_string<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
                    return json::bits::basic_string_read<X>(t, i, e, cx);
                }
        };
    template <typename X, template <typename> class S, typename T, typename ...R>
        struct read<S<chio::strs::UQKEY<X>>, std::basic_string<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
                    chio::consume<S<X>>(i, e);
                    return (chio::peek(i, e) == S<X>::string::beg && json::bits::basic_string_read<S<X>>(t, i, e, cx)) ||
                            json::bits::basic_string_read<S<chio::strs::UQKEY<X>>>(t, i, e, cx)
                    ;
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::basic_string<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::basic_string<T, R...>& t, Cx& cx) {
                    return json::bits::pointer_write<X>(o, t.data(), t.size(), cx);
                }
        };
    template <typename X, template <typename> class S, typename T, typename ...R>
        struct write<S<chio::strs::UQKEY<X>>, std::basic_string<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::basic_string<T, R...>& t, Cx& cx) {
                    return json::bits::uqkey_pointer_write<S<X>>(o, t.data(), t.size(), cx);
                }
        };

}   // cxon

#endif // CXON_JSON_LIB_STD_STRING_HXX_
