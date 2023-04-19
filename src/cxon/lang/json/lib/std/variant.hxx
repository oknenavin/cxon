// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_VARIANT_HXX_
#define CXON_JSON_LIB_STD_VARIANT_HXX_

#include "cxon/lang/common/cio/variant.hxx"

namespace cxon {

    namespace json { namespace imp {

        template <typename X, typename V, typename II, typename Cx, typename N = std::make_index_sequence<std::variant_size<V>::value>>
            struct variant_;
        template <typename X, typename V, typename II, typename Cx, std::size_t ...N>
            struct variant_<X, V, II, Cx, std::index_sequence<N...>> {
                using rp_ = bool (*)(V&, II&, II, Cx&);
                static constexpr std::size_t S_ = std::index_sequence<N...>::size();

                template <std::size_t M>
                    static bool variant_read_(V& t, II& i, II e, Cx& cx) {
                        auto u = V {std::in_place_index_t<M>()};
                        return cio::read_map_val<X>(std::get<M>(u), i, e, cx) && (t = std::move(u), true);
                    }
                static constexpr rp_ read_[S_] = { &variant_read_<N>... };

                static bool index_(std::size_t& n, II& i, II e, Cx& cx) {
                    II const o = i;
                    return  (cio::read_map_key<X>(n, i, e, cx) && n < S_) ||
                            (cio::rewind(i, o), cx/json::read_error::unexpected)
                    ;
                }
                static bool read(V& t, II& i, II e, Cx& cx) {
                    std::size_t n;
                        return index_(n, i, e, cx) && read_[n](t, i, e, cx);
                }
            };

        namespace var = cio::var;

        template <typename X, typename II, typename Cx, typename ...T>
            inline auto variant_read_(std::variant<T...>& t, II& i, II e, Cx& cx)
                -> enable_if_t< cio::is_key_context<X>::value || !var::is_disambiguous<T...>::value, bool>
            {
                return  cio::consume<X>(X::map::beg, i, e, cx) &&
                            variant_<X, std::variant<T...>, II, Cx>::read(t, i, e, cx) &&
                        cio::consume<X>(X::map::end, i, e, cx)
                ;
            }
        template <typename X, typename II, typename Cx, typename ...T>
            inline auto variant_read_(std::variant<T...>& t, II& i, II e, Cx& cx)
                -> enable_if_t<!cio::is_key_context<X>::value &&  var::is_disambiguous<T...>::value, bool>
            {
                if (!cio::consume<X>(i, e, cx))
                    return false;
                switch (cio::peek(i, e)) {
                    case X::map::beg:
                        CXON_IF_CONSTEXPR (var::has_type<cio::is_map, T...>::value) {
                            var::get_type_t<cio::is_map, T...> u;
                            return read_value<X>(u, i, e, cx) && (t = std::move(u), true);
                        }   break;
                    case X::list::beg:
                        CXON_IF_CONSTEXPR (var::has_type<cio::is_list, T...>::value) {
                            var::get_type_t<cio::is_list, T...> u;
                            return read_value<X>(u, i, e, cx) && (t = std::move(u), true);
                        }   break;
                    case X::string::del:
                        CXON_IF_CONSTEXPR (var::has_type<cio::is_string, T...>::value) {
                            var::get_type_t<cio::is_string, T...> u;
                            return read_value<X>(u, i, e, cx) && (t = std::move(u), true);
                        }   break;
                    case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case 'I': case 'N':
                        CXON_IF_CONSTEXPR (var::has_type<cio::is_number, T...>::value) {
                            var::get_type_t<cio::is_number, T...> u;
                            return read_value<X>(u, i, e, cx) && (t = std::move(u), true);
                        }   break;
                    case *X::id::pos: case *X::id::neg:
                        CXON_IF_CONSTEXPR (var::has_type<cio::is_bool, T...>::value) {
                            var::get_type_t<cio::is_bool, T...> u;
                            return read_value<X>(u, i, e, cx) && (t = std::move(u), true);
                        }   break;
                    case *X::id::nil:
                        CXON_IF_CONSTEXPR (var::has_type<cio::is_null, T...>::value) {
                            var::get_type_t<cio::is_null, T...> u;
                            return read_value<X>(u, i, e, cx) && (t = std::move(u), true);
                        }   break;
                }
                return cx/json::read_error::unexpected;
            }

        template <typename X, typename O, typename Cx, typename ...T>
            inline auto variant_write_(O& o, const std::variant<T...>& t, Cx& cx)
                -> enable_if_t< cio::is_key_context<X>::value || !var::is_disambiguous<T...>::value, bool>
            {
                return  cio::poke<X>(o, X::map::beg, cx) &&
                            cio::write_map_key<X>(o, t.index(), cx) &&
                            std::visit([&](auto&& v) { return cio::write_map_val<X>(o, v, cx); }, t) &&
                        cio::poke<X>(o, X::map::end, cx)
                ;
            }
        template <typename X, typename O, typename Cx, typename ...T>
            inline auto variant_write_(O& o, const std::variant<T...>& t, Cx& cx)
                -> enable_if_t<!cio::is_key_context<X>::value &&  var::is_disambiguous<T...>::value, bool>
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
