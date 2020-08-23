// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_STRING_VIEW_HXX_
#define CXON_JSON_LIB_STD_STRING_VIEW_HXX_

namespace cxon { namespace chio {

#   define CXON_QUOTED(T)\
        template <typename ...R> struct is_quoted<std::basic_string_view<T, R...>> : std::true_type  {};
        CXON_QUOTED(char)
        CXON_QUOTED(char16_t)
        CXON_QUOTED(char32_t)
        CXON_QUOTED(wchar_t)
#   undef CXON_QUOTED

}}

namespace cxon {

    template <typename T, typename ...R>
        struct continuous<std::basic_string_view<T, R...>> {
            static auto range(const std::basic_string_view<T, R...>& i) -> decltype(std::make_pair(&i[0], &i[0] + i.size())) {
                return std::make_pair(&i[0], &i[0] + i.size());
            }
        };

    template <typename X, typename T, typename ...R>
        struct read<JSON<X>, std::basic_string_view<T, R...>> {
            template <typename II, typename Cx>
                static bool value(std::basic_string_view<T, R...>&, II&, II, Cx&) {
                    static_assert(unexpected<std::basic_string_view<T, R...>>(), "std::string_view cannot be modified");
                    return false;
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<JSON<X>, std::basic_string_view<T, R...>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::basic_string_view<T, R...>& t, Cx& cx) {
                    return chio::strs::pointer_write<J>(o, t.data(), t.size(), cx);
                }
        };
    template <typename X, typename T, typename ...R>
        struct write<JSON<chio::UQKEY<X>>, std::basic_string_view<T, R...>> {
            template <typename O, typename Cx, typename J = JSON<chio::UQKEY<X>>>
                static bool value(O& o, const std::basic_string_view<T, R...>& t, Cx& cx) {
                    return chio::strs::uqkey_pointer_write<J>(o, t.data(), t.size(), cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_STRING_VIEW_HXX_
