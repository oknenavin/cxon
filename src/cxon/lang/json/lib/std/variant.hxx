// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_VARIANT_HXX_
#define CXON_JSON_LIB_STD_VARIANT_HXX_

namespace cxon {

    namespace bits {

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
                    return chio::read_key<X>(n, i, e, cx) && (n < cnt_ || (chio::rewind(i, o), cx|chio::read_error::unexpected));
                }
                static bool read(V& t, II& i, II e, Cx& cx) {
                    size_t n;
                    return index(n, i, e, cx) && rdr_[n](t, i, e, cx);
                }
            };

    }

    template <typename X, typename II, typename Cx>
        inline bool read_value(std::monostate&, II& i, II e, Cx& cx) {
            II const o = i;
            return chio::consume<X>(X::id::nil, i, e) || (chio::rewind(i, o), cx|chio::read_error::unexpected);
        }

    template <typename X, typename ...T>
        struct read<X, std::variant<T...>> {
            template <typename II, typename Cx>
                static bool value(std::variant<T...>& t, II& i, II e, Cx& cx) {
                    return  chio::consume<X>(X::map::beg, i, e, cx) &&
                                bits::variant<X, std::variant<T...>, II, Cx>::read(t, i, e, cx) &&
                            chio::consume<X>(X::map::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, std::monostate, Cx& cx) {
            return chio::poke<X>(o, X::id::nil, cx);
        }

    template <typename X, typename ...T>
        struct write<X, std::variant<T...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::variant<T...>& t, Cx& cx) {
                    return  chio::poke(o, X::map::beg) &&
                                chio::write_key<X>(o, t.index(), cx) &&
                                std::visit([&](auto&& v) { return write_value<X>(o, v, cx); }, t) &&
                            chio::poke(o, X::map::end)
                    ;
                }
        };

}   // cxon

#endif // CXON_JSON_LIB_STD_VARIANT_HXX_
