// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CXON_JSON_LIB_BOOST_VARIANT2_HXX_
#define CXON_JSON_LIB_BOOST_VARIANT2_HXX_

#include "cxon/lang/common/cio/variant.hxx"

namespace cxon { namespace cio { // type traits
    template <typename ...T>    struct is_map<boost::variant2::variant<T...>>       : var::has_type<is_map, T...> {};
    template <typename ...T>    struct is_list<boost::variant2::variant<T...>>      : var::has_type<is_list, T...> {};
    template <typename ...T>    struct is_string<boost::variant2::variant<T...>>    : var::has_type<is_string, T...> {};
    template <typename ...T>    struct is_number<boost::variant2::variant<T...>>    : var::has_type<is_number, T...> {};
    template <typename ...T>    struct is_bool<boost::variant2::variant<T...>>      : var::has_type<is_bool, T...> {};
    template <typename ...T>    struct is_null<boost::variant2::variant<T...>>      : var::has_type<is_null, T...> {};
    template <>                 struct is_null<boost::variant2::monostate>          : std::true_type {};
}}

namespace cxon {

    namespace imp { namespace json { namespace boost { namespace variant2 { // read

        template <typename X, typename V, typename II, typename Cx, typename N = make_index_sequence<::boost::variant2::variant_size<V>::value>>
            struct read_;
        template <typename X, typename V, typename II, typename Cx, std::size_t ...N>
            struct read_<X, V, II, Cx, index_sequence<N...>> {
                static constexpr std::size_t S_ = index_sequence<N...>::size();

                template <std::size_t M>
                    static bool variant_read_(V& t, II& i, II e, Cx& cx) {
                        auto u = V {::boost::variant2::in_place_index_t<M>()};
                        return cio::read_map_val<X>(::boost::variant2::get<M>(u), i, e, cx) && (t = std::move(u), true);
                    }
                static bool variant_read_(V& t, std::size_t n, II& i, II e, Cx& cx) {
                    using rvp_ = bool (*)(V&, II&, II, Cx&);
                    static constexpr rvp_ rv_[S_] = { &variant_read_<N>... };
                    return rv_[n](t, i, e, cx);
                }

                static bool index_(std::size_t& n, II& i, II e, Cx& cx) {
                    II const o = i;
                    return  (cio::read_map_key<X>(n, i, e, cx) && n < S_) ||
                            (cio::rewind(i, o), cx/cxon::json::read_error::unexpected)
                    ;
                }
                static bool variant(V& t, II& i, II e, Cx& cx) {
                    std::size_t n;
                        return index_(n, i, e, cx) && variant_read_(t, n, i, e, cx);
                }
            };

        template <typename X, typename V, typename II, typename Cx>
            inline auto variant_read_(V& t, II& i, II e, Cx& cx)
                -> enable_if_t< cio::is_key_context<X>::value ||  cio::var::has_ambiguous_types<V>::value, bool>
            {
                return  cio::consume<X>(X::map::beg, i, e, cx) &&
                            read_<X, V, II, Cx>::variant(t, i, e, cx) &&
                        cio::consume<X>(X::map::end, i, e, cx)
                ;
            }
        template <typename X, typename V, typename II, typename Cx>
            inline auto variant_read_(V& t, II& i, II e, Cx& cx)
                -> enable_if_t<!cio::is_key_context<X>::value && !cio::var::has_ambiguous_types<V>::value, bool>
            {
                return  cio::var::variant_read<X>(t, i, e, cx);
            }

    }}}}

    namespace imp { namespace json { namespace boost { namespace variant2 { // write

        template <typename X, typename O, typename V, typename Cx, typename N = make_index_sequence<::boost::variant2::variant_size<V>::value>>
            struct write_;
        template <typename X, typename O, typename V, typename Cx, std::size_t ...N>
            struct write_<X, O, V, Cx, index_sequence<N...>> {
                static constexpr std::size_t S_ = index_sequence<N...>::size();

                template <std::size_t M>
                    static bool variant_write_(O& o, const V& t, Cx& cx) {
                        return cio::write_map_val<X>(o, ::boost::variant2::get<M>(t), cx);
                    }

                static bool variant(O& o, const V& t, std::size_t n, Cx& cx) {
                    using wvp_ = bool (*)(O&, const V&, Cx&);
                    static constexpr wvp_ wv_[S_] = { &variant_write_<N>... };
                    return wv_[n](o, t, cx);
                }
                static bool variant(O& o, const V& t, Cx& cx) {
                    return cio::write_map_key<X>(o, t.index(), cx) && variant(o, t, t.index(), cx);
                }
            };

        template <typename X, typename V, typename O, typename Cx>
            inline auto variant_write_(O& o, const V& t, Cx& cx)
                -> enable_if_t< cio::is_key_context<X>::value ||  cio::var::has_ambiguous_types<V>::value, bool>
            {
                return  cio::poke<X>(o, X::map::beg, cx) &&
                            write_<X, O, V, Cx>::variant(o, t, cx) &&
                        cio::poke<X>(o, X::map::end, cx)
                ;
            }
        template <typename X, typename V, typename O, typename Cx>
            inline auto variant_write_(O& o, const V& t, Cx& cx)
                -> enable_if_t<!cio::is_key_context<X>::value && !cio::var::has_ambiguous_types<V>::value, bool>
            {
                return  write_<X, O, V, Cx>::variant(o, t, t.index(), cx);
            }

    }}}}

    template <typename X>
        struct read<JSON<X>, boost::variant2::monostate> {
            template <typename II, typename Cx>
                static bool value(boost::variant2::monostate&, II& i, II e, Cx& cx) {
                    II const o = i;
                    return cio::consume<X>(i, e, cx) && (cio::consume<X>(X::id::nil, i, e) || (cio::rewind(i, o), cx/json::read_error::unexpected));
                }
        };
    template <typename X>
        struct write<JSON<X>, boost::variant2::monostate> {
            template <typename O, typename Cx>
                static bool value(O& o, boost::variant2::monostate, Cx& cx) {
                    return cio::poke<X>(o, X::id::nil, cx);
                }
        };

    template <typename X, typename ...T>
        struct read<JSON<X>, boost::variant2::variant<T...>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(boost::variant2::variant<T...>& t, II& i, II e, Cx& cx) {
                    return imp::json::boost::variant2::variant_read_<Y>(t, i, e, cx);
                }
        };
    template <typename X, typename ...T>
        struct write<JSON<X>, boost::variant2::variant<T...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const boost::variant2::variant<T...>& t, Cx& cx) {
                    return imp::json::boost::variant2::variant_write_<Y>(o, t, cx);
                }
        };

}

#endif // CXON_JSON_LIB_BOOST_VARIANT2_HXX_
