// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_VARIANT_HXX_
#define CXON_VARIANT_HXX_

#if __cplusplus >= 201703L
#   if defined(__has_include) && __has_include(<variant>)
#       include <variant>
#       define CXON_HAS_VARIANT
#   endif
#endif

#ifdef CXON_HAS_VARIANT

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
                    return read_key<X>(n, i, e, cx) && (n < cnt_ || (bits::rewind(i, o), cx|read_error::unexpected));
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
            return io::consume<X>(X::id::nil, i, e) || (bits::rewind(i, o), cx|read_error::unexpected);
        }

    template <typename X, typename ...T>
        struct read<X, std::variant<T...>> {
            template <typename II, typename Cx>
                static bool value(std::variant<T...>& t, II& i, II e, Cx& cx) {
                    return  io::consume<X>(X::map::beg, i, e, cx) &&
                                bits::variant<X, std::variant<T...>, II, Cx>::read(t, i, e, cx) &&
                            io::consume<X>(X::map::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename O, typename Cx>
        inline bool write_value(O& o, std::monostate, Cx& cx) {
            return io::poke<X>(o, X::id::nil, cx);
        }

    template <typename X, typename ...T>
        struct write<X, std::variant<T...>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::variant<T...>& t, Cx& cx) {
                    return  io::poke(o, X::map::beg) &&
                                write_key<X>(o, t.index(), cx) &&
                                std::visit([&](auto&& v) { return write_value<X>(o, v, cx); }, t) &&
                            io::poke(o, X::map::end)
                    ;
                }
        };

}   // cxon

#endif // CXON_HAS_VARIANT

#endif // CXON_VARIANT_HXX_
