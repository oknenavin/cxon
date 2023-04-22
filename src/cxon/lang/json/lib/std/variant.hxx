// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_VARIANT_HXX_
#define CXON_JSON_LIB_STD_VARIANT_HXX_

#include "cxon/lang/common/cio/variant.hxx"

namespace cxon { namespace cio { // type traits
    template <typename ...T>    struct is_map<std::variant<T...>>       : var::has_type<is_map, T...> {};
    template <typename ...T>    struct is_list<std::variant<T...>>      : var::has_type<is_list, T...> {};
    template <typename ...T>    struct is_string<std::variant<T...>>    : var::has_type<is_string, T...> {};
    template <typename ...T>    struct is_number<std::variant<T...>>    : var::has_type<is_number, T...> {};
    template <typename ...T>    struct is_bool<std::variant<T...>>      : var::has_type<is_bool, T...> {};
    template <typename ...T>    struct is_null<std::variant<T...>>      : var::has_type<is_null, T...> {};
    template <>                 struct is_null<std::monostate>          : std::true_type {};
}}

namespace cxon {

    namespace json { namespace imp {

        template <typename X, typename V, typename II, typename Cx, typename N = std::make_index_sequence<std::variant_size<V>::value>>
            struct variant_;
        template <typename X, typename V, typename II, typename Cx, std::size_t ...N>
            struct variant_<X, V, II, Cx, std::index_sequence<N...>> {
                using rp_ = bool (*)(V&, II&, II, Cx&);
                static constexpr std::size_t S_ = std::index_sequence<N...>::size();

                template <std::size_t M>
                    static bool read_(V& t, II& i, II e, Cx& cx) {
                        auto u = V {std::in_place_index_t<M>()};
                        return cio::read_map_val<X>(std::get<M>(u), i, e, cx) && (t = std::move(u), true);
                    }
                static constexpr rp_ reader_[S_] = { &read_<N>... };

                static bool index_(std::size_t& n, II& i, II e, Cx& cx) {
                    II const o = i;
                    return  (cio::read_map_key<X>(n, i, e, cx) && n < S_) ||
                            (cio::rewind(i, o), cx/json::read_error::unexpected)
                    ;
                }
                static bool read(V& t, II& i, II e, Cx& cx) {
                    std::size_t n;
                        return index_(n, i, e, cx) && reader_[n](t, i, e, cx);
                }
            };

        template <typename X, typename V, typename II, typename Cx>
            inline auto variant_read_(V& t, II& i, II e, Cx& cx)
                -> enable_if_t< cio::is_key_context<X>::value ||  cio::var::has_ambiguous_types<V>::value, bool>
            {
                return  cio::consume<X>(X::map::beg, i, e, cx) &&
                            variant_<X, V, II, Cx>::read(t, i, e, cx) &&
                        cio::consume<X>(X::map::end, i, e, cx)
                ;
            }
        template <typename X, typename V, typename II, typename Cx>
            inline auto variant_read_(V& t, II& i, II e, Cx& cx)
                -> enable_if_t<!cio::is_key_context<X>::value && !cio::var::has_ambiguous_types<V>::value, bool>
            {
                return  cio::var::variant_read<X>(t, i, e, cx);
            }

        template <typename X, typename V, typename O, typename Cx>
            inline auto variant_write_(O& o, const V& t, Cx& cx)
                -> enable_if_t< cio::is_key_context<X>::value ||  cio::var::has_ambiguous_types<V>::value, bool>
            {
                return  cio::poke<X>(o, X::map::beg, cx) &&
                            cio::write_map_key<X>(o, t.index(), cx) &&
                            std::visit([&](auto&& v) { return cio::write_map_val<X>(o, v, cx); }, t) &&
                        cio::poke<X>(o, X::map::end, cx)
                ;
            }
        template <typename X, typename V, typename O, typename Cx>
            inline auto variant_write_(O& o, const V& t, Cx& cx)
                -> enable_if_t<!cio::is_key_context<X>::value && !cio::var::has_ambiguous_types<V>::value, bool>
            {
                return  std::visit([&](auto&& v) { return cio::write_map_val<X>(o, v, cx); }, t);
            }

    }}

    template <typename X>
        struct read<JSON<X>, std::monostate> {
            template <typename II, typename Cx>
                static bool value(std::monostate&, II& i, II e, Cx& cx) {
                    II const o = i;
                    return cio::consume<X>(i, e, cx) && (cio::consume<X>(X::id::nil, i, e) || (cio::rewind(i, o), cx/json::read_error::unexpected));
                }
        };
    template <typename X>
        struct write<JSON<X>, std::monostate> {
            template <typename O, typename Cx>
                static bool value(O& o, std::monostate, Cx& cx) {
                    return cio::poke<X>(o, X::id::nil, cx);
                }
        };

    template <typename X, typename ...T>
        struct read<JSON<X>, std::variant<T...>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::variant<T...>& t, II& i, II e, Cx& cx) {
                    return json::imp::variant_read_<Y>(t, i, e, cx);
                }
        };
    template <typename X, typename ...T>
        struct write<JSON<X>, std::variant<T...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::variant<T...>& t, Cx& cx) {
                    return json::imp::variant_write_<Y>(o, t, cx);
                }
        };

}

#endif // CXON_JSON_LIB_STD_VARIANT_HXX_
