// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_STRING_HXX_
#define CXON_JSON_LIB_STD_STRING_HXX_

#include <string>

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
                    return bits::basic_string_read<X>(t, i, e, cx);
                }
        };
    template <typename X, template <typename> class S, typename T, typename ...R>
        struct read<S<bits::UQKEY<X>>, std::basic_string<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
                    io::consume<S<X>>(i, e);
                    return (io::peek(i, e) == S<X>::string::beg && bits::basic_string_read<S<X>>(t, i, e, cx)) ||
                            bits::basic_string_read<S<bits::UQKEY<X>>>(t, i, e, cx)
                    ;
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::basic_string<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::basic_string<T, R...>& t, Cx& cx) {
                    return bits::pointer_write<X>(o, t.data(), t.size(), cx);
                }
        };
    template <typename X, template <typename> class S, typename T, typename ...R>
        struct write<S<bits::UQKEY<X>>, std::basic_string<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::basic_string<T, R...>& t, Cx& cx) {
                    return bits::uqkey_pointer_write<S<X>>(o, t.data(), t.size(), cx);
                }
        };

}   // cxon

#endif // CXON_JSON_LIB_STD_STRING_HXX_
