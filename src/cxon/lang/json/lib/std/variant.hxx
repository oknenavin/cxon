// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_VARIANT_HXX_
#define CXON_JSON_LIB_STD_VARIANT_HXX_

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
                        return read_value<X>(std::get<M>(u), i, e, cx) && (t = std::move(u), true);
                    }
                static constexpr rp_ read_[S_] = { &variant_read_<N>... };

                static bool index_(std::size_t& n, II& i, II e, Cx& cx) {
                    II const o = i;
                    return  (cio::read_key<X>(n, i, e, cx) && n < S_) ||
                            (cio::rewind(i, o), cx/json::read_error::unexpected)
                    ;
                }
                static bool read(V& t, II& i, II e, Cx& cx) {
                    std::size_t n;
                        return index_(n, i, e, cx) && read_[n](t, i, e, cx);
                }
            };

        template <typename X, typename V, typename II, typename Cx>
            inline bool variant_read_(V& t, II& i, II e, Cx& cx) {
                return variant_<X, V, II, Cx>::read(t, i, e, cx);
            }

    }}

    template <typename X>
        struct read<JSON<X>, std::monostate> {
            template <typename II, typename Cx>
                static bool value(std::monostate&, II& i, II e, Cx& cx) {
                    II const o = i;
                    return cio::consume<X>(X::id::nil, i, e) || (cio::rewind(i, o), cx/json::read_error::unexpected);
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
                    return  cio::consume<Y>(Y::map::beg, i, e, cx) &&
                                json::imp::variant_read_<Y>(t, i, e, cx) &&
                            cio::consume<Y>(Y::map::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename ...T>
        struct write<JSON<X>, std::variant<T...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::variant<T...>& t, Cx& cx) {
                    return  cio::poke<Y>(o, Y::map::beg, cx) &&
                                cio::write_key<Y>(o, t.index(), cx) &&
                                std::visit([&](auto&& v) { return write_value<Y>(o, v, cx); }, t) &&
                            cio::poke<Y>(o, Y::map::end, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_VARIANT_HXX_
