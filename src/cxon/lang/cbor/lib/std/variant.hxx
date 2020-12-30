// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_VARIANT_HXX_
#define CXON_CBOR_LIB_STD_VARIANT_HXX_

#include "cxon/lang/cbor/compound.hxx"

namespace cxon {

    namespace cbor { namespace bits {

        template <typename X, typename V, size_t Ndx, typename II, typename Cx>
            static bool variant_read(V& t, II& i, II e, Cx& cx) {
                V v{std::in_place_index_t<Ndx>()};
                return read_value<X>(std::get<Ndx>(v), i, e, cx) && (t = std::move(v), true);
            }

        template <typename X, typename V, typename II, typename Cx, typename Ndx = std::make_index_sequence<std::variant_size<V>::value>>
            struct variant;
        template <typename X, typename V, typename II, typename Cx, size_t ...Ndx>
            struct variant<X, V, II, Cx, std::index_sequence<Ndx...>> {
                using reader_t = bool(*)(V&, II&, II, Cx&);
                static constexpr size_t cnt_ = std::index_sequence<Ndx...>::size();
                static constexpr reader_t rdr_[cnt_] = { &variant_read<X, V, Ndx, II, Cx>... };
                static bool index(size_t& n, II& i, II e, Cx& cx) {
                    II const o = i;
                    return read_value<X>(n, i, e, cx) && (n < cnt_ || (bio::rewind(i, o), cx|cbor::read_error::unexpected));
                }
                static bool read(V& t, II& i, II e, Cx& cx) {
                    size_t n;
                    return index(n, i, e, cx) && rdr_[n](t, i, e, cx);
                }
            };

    }}

    template <typename X, typename II, typename Cx>
        inline auto read_value(std::monostate&, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {
            return  (bio::peek(i, e) == X::nil && (bio::get(i, e), true)) ||
                    (cx|cbor::read_error::unexpected)
            ;
        }

    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, std::monostate, Cx& cx) -> enable_for_t<X, CBOR> {
            return bio::poke<X>(o, X::nil, cx);
        }

    template <typename X, typename ...T>
        struct read<CBOR<X>, std::variant<T...>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::variant<T...>& t, II& i, II e, Cx& cx) {
                    return  cbor::cnt::read_size_eq<J>(2, i, e, cx) &&
                            cbor::bits::variant<J, std::variant<T...>, II, Cx>::read(t, i, e, cx)
                    ;
                }
        };

    template <typename X, typename ...T>
        struct write<CBOR<X>, std::variant<T...>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::variant<T...>& t, Cx& cx) {
                    return cbor::cnt::write_size<J>(o, J::arr, 2, cx) &&
                                write_value<J>(o, t.index(), cx) &&
                                std::visit([&](auto&& v) { return write_value<J>(o, v, cx); }, t)
                    ;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_VARIANT_HXX_
