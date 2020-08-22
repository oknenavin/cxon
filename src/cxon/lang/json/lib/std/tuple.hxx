// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_TUPLE_HXX_
#define CXON_JSON_LIB_STD_TUPLE_HXX_

namespace cxon { namespace json { namespace bits { // read/write

    template <typename X, typename T, unsigned N, unsigned L>
        struct tuple_read {
            template <typename II, typename Cx>
                static bool value(T& t, II& i, II e, Cx& cx) {
                    if (!read_value<X>(std::get<N>(t), i, e, cx)) return false;
                    if (N + 1 != L) if (!chio::consume<X>(X::list::sep, i, e, cx)) return false;
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
                    return  write_value<X>(o, std::get<N>(t), cx) && chio::poke<X>(o, X::list::sep, cx) &&
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

}}}

namespace cxon {

    template <typename X, typename H, typename ...T>
        struct read<JSON<X>, std::tuple<H, T...>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::tuple<H, T...>& t, II& i, II e, Cx& cx) {
                    using U = std::tuple<H, T...>;
                    return  chio::consume<J>(J::list::beg, i, e, cx) &&
                                json::bits::tuple_read<J, U, 0, std::tuple_size<U>::value>::value(t, i, e, cx) &&
                            chio::consume<J>(J::list::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename ...T>
        struct write<JSON<X>, std::tuple<T...>> {
            using U = std::tuple<T...>;
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const U& t, Cx& cx) {
                    return  chio::poke<J>(o, J::list::beg, cx) &&
                                json::bits::tuple_write<J, U, 0, std::tuple_size<U>::value - 1>::value(o, t, cx) &&
                            chio::poke<J>(o, J::list::end, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_TUPLE_HXX_
