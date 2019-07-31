// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_TUPLE_HXX_
#define CXON_TUPLE_HXX_

#include <tuple>

namespace cxon { namespace bits { // read/write

    template <typename X, typename T, unsigned N, unsigned L>
        struct tuple_read {
            template <typename II, typename Cx>
                static bool value(T& t, II& i, II e, Cx& cx) {
                    if (!read_value<X>(std::get<N>(t), i, e, cx)) return false;
                    if (N + 1 != L) if (!io::consume<X>(X::list::sep, i, e, cx)) return false;
                    return tuple_read<X, T, N + 1, L>::value(t, i, e, cx);
                }
        };

    template <typename X, typename T, unsigned N>
        struct tuple_read<X, T, N, N> {
            template <typename II, typename Cx>
                static constexpr bool value(T&, II&, II, Cx&) { return true; }
        };

    template <typename X, typename T, unsigned N, unsigned L>
        struct tuple_write {
            template <typename O, typename Cx>
                static bool value(O& o, const T& t, Cx& cx) {
                    return  write_value<X>(o, std::get<N>(t), cx) && io::poke<X>(o, X::list::sep, cx) &&
                            tuple_write<X, T, N + 1, L>::value(o, t, cx)
                    ;
                }
        };

    template <typename X, typename T, unsigned N>
        struct tuple_write<X, T, N, N> {
            template <typename O, typename Cx>
                static bool value(O& o, const T& t, Cx& cx) {
                    return write_value<X>(o, std::get<N>(t), cx);
                }
        };

}}  // cxon::bits read/write

namespace cxon {

    template <typename X, typename H, typename ...T>
        struct read<X, std::tuple<H, T...>> {
            template <typename II, typename Cx>
                static bool value(std::tuple<H, T...>& t, II& i, II e, Cx& cx) {
                    using U = std::tuple<H, T...>;
                    return  io::consume<X>(X::list::beg, i, e, cx) &&
                            bits::tuple_read<X, U, 0, std::tuple_size<U>::value>::value(t, i, e, cx) &&
                            io::consume<X>(X::list::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename ...T>
        struct write<X, std::tuple<T...>> {
            using U = std::tuple<T...>;
            template <typename O, typename Cx>
                static bool value(O& o, const U& t, Cx& cx) {
                    return  io::poke<X>(o, X::list::beg, cx) &&
                                bits::tuple_write<X, U, 0, std::tuple_size<U>::value - 1>::value(o, t, cx) &&
                            io::poke<X>(o, X::list::end, cx)
                    ;
                }
        };

}   // cxon

#endif // CXON_TUPLE_HXX_
