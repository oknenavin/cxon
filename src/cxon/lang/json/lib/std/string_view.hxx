// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_STRING_VIEW_HXX_
#define CXON_JSON_LIB_STD_STRING_VIEW_HXX_

#if __cplusplus >= 201703L
#   if defined(__has_include) && __has_include(<string_view>)
#       include <string_view>
#       define CXON_HAS_JSON_LIB_STD_STRING_VIEW
#   endif
#endif

#ifdef CXON_HAS_JSON_LIB_STD_STRING_VIEW

namespace cxon { namespace bits {
    template <typename ...A> struct is_quoted< std::basic_string_view<A...> > : std::true_type {};
}}

namespace cxon {

    template <typename T, typename ...R>
        struct continuous<std::basic_string_view<T, R...>> {
            static auto range(const std::basic_string_view<T, R...>& i) -> decltype(std::make_pair(&i[0], &i[0] + i.size())) {
                return std::make_pair(&i[0], &i[0] + i.size());
            }
        };

    template <typename X, typename T, typename ...R>
        struct read<X, std::basic_string_view<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::basic_string_view<T, R...>&, II&, II, Cx&) {
                    static_assert(unexpected<std::basic_string_view<T, R...>>(), "std::string_view cannot be modified");
                    return false;
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<X, std::basic_string_view<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::basic_string_view<T, R...>& t, Cx& cx) {
                    return bits::pointer_write<X>(o, t.data(), t.size(), cx);
                }
        };
    template <typename X, template <typename> class S, typename T, typename ...R>
        struct write<S<bits::UQKEY<X>>, std::basic_string_view<T, R...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::basic_string_view<T, R...>& t, Cx& cx) {
                    return bits::uqkey_pointer_write<S<X>>(o, t.data(), t.size(), cx);
                }
        };

}   // cxon

#endif // CXON_HAS_JSON_LIB_STD_STRING_VIEW

#endif // CXON_JSON_LIB_STD_STRING_VIEW_HXX_
