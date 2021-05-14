// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_VARIANT_HXX_
#define CXON_CBOR_LIB_STD_VARIANT_HXX_

#include "cxon/lang/cbor/compound.hxx"

namespace cxon {

    namespace cbor { namespace imp {

        template <typename X, typename V, std::size_t Ndx, typename II, typename Cx>
            static bool variant_read_(V& t, II& i, II e, Cx& cx) {
                V v{std::in_place_index_t<Ndx>()};
                return read_value<X>(std::get<Ndx>(v), i, e, cx) && (t = std::move(v), true);
            }

        template <typename X, typename V, typename II, typename Cx, typename Ndx = std::make_index_sequence<std::variant_size<V>::value>>
            struct variant_;
        template <typename X, typename V, typename II, typename Cx, std::size_t ...Ndx>
            struct variant_<X, V, II, Cx, std::index_sequence<Ndx...>> {
                using reader_t = bool(*)(V&, II&, II, Cx&);
                static constexpr std::size_t cnt_ = std::index_sequence<Ndx...>::size();
                static constexpr reader_t rdr_[cnt_] = { &variant_read_<X, V, Ndx, II, Cx>... };
                static bool index(std::size_t& n, II& i, II e, Cx& cx) {
                    II const o = i;
                    return read_value<X>(n, i, e, cx) && (n < cnt_ || (bio::rewind(i, o), cx/cbor::read_error::unexpected));
                }
                static bool read(V& t, II& i, II e, Cx& cx) {
                    std::size_t n;
                    return index(n, i, e, cx) && rdr_[n](t, i, e, cx);
                }
            };

    }}

    template <typename X, typename II, typename Cx>
        inline auto read_value(std::monostate&, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {
            return cbor::tag::read<X>(i, e, cx) && (
                (bio::peek(i, e) == X::und && (bio::get(i, e), true)) ||
                (cx/cbor::read_error::unexpected)
            );
        }

    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, std::monostate, Cx& cx) -> enable_for_t<X, CBOR> {
            return bio::poke<X>(o, X::und, cx);
        }

    template <typename X, typename ...T>
        struct read<CBOR<X>, std::variant<T...>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::variant<T...>& t, II& i, II e, Cx& cx) {
                    return  cbor::tag::read<Y>(i, e, cx) &&
                            cbor::cnt::read_size_eq<Y>(2, i, e, cx) &&
                            cbor::imp::variant_<Y, std::variant<T...>, II, Cx>::read(t, i, e, cx)
                    ;
                }
        };

    template <typename X, typename ...T>
        struct write<CBOR<X>, std::variant<T...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::variant<T...>& t, Cx& cx) {
                    return  cbor::cnt::write_size<Y>(o, Y::arr, 2, cx) &&
                                write_value<Y>(o, t.index(), cx) &&
                                std::visit([&](auto&& v) { return write_value<Y>(o, v, cx); }, t)
                    ;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_VARIANT_HXX_
