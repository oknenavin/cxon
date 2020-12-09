// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_STRING_HXX_
#define CXON_JSON_LIB_STD_STRING_HXX_

#include "bits/string.hxx"

namespace cxon { namespace cio {

#   define CXON_QUOTED(T)\
        template <typename ...R> struct is_quoted<std::basic_string<T, R...>> : std::true_type  {};
        CXON_QUOTED(char)
        CXON_QUOTED(wchar_t)
#       if __cplusplus > 201703L /* C++20 */
            CXON_QUOTED(char8_t)
#       endif
        CXON_QUOTED(char16_t)
        CXON_QUOTED(char32_t)
#   undef CXON_QUOTED

}}

namespace cxon {

    template <typename T, typename ...R>
        struct continuous<std::basic_string<T, R...>> {
            static auto range(const std::basic_string<T, R...>& i) -> decltype(std::make_pair(&i[0], &i[0] + i.size())) {
                return std::make_pair(&i[0], &i[0] + i.size());
            }
        };

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::basic_string<T, R...>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
                    return json::bits::basic_string_read<J>(t, i, e, cx);
                }
        };
    template <typename X, typename T, typename ...R>
        struct read<JSON<cio::UQKEY<X>>, std::basic_string<T, R...>> {
            template <typename II, typename Cx, typename J = JSON<cio::UQKEY<X>>>
                static bool value(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
                    cio::consume<J>(i, e);
                    return (cio::peek(i, e) == J::string::beg && json::bits::basic_string_read<JSON<X>>(t, i, e, cx)) ||
                            json::bits::basic_string_read<J>(t, i, e, cx)
                    ;
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::basic_string<T, R...>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::basic_string<T, R...>& t, Cx& cx) {
                    return json::bits::pointer_write<J>(o, t.data(), t.size(), cx);
                }
        };
    template <typename X, typename T, typename ...R>
        struct write<JSON<cio::UQKEY<X>>, std::basic_string<T, R...>> {
            template <typename O, typename Cx, typename J = JSON<cio::UQKEY<X>>>
                static bool value(O& o, const std::basic_string<T, R...>& t, Cx& cx) {
                    return json::bits::uqkey_pointer_write<J>(o, t.data(), t.size(), cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_STRING_HXX_
