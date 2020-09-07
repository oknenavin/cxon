// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_TUPLE_HXX_
#define CXON_JSON_LIB_STD_TUPLE_HXX_

namespace cxon { namespace cio { namespace con { // container read/write helpers

    namespace bits {

        template <typename X, typename T, unsigned N, unsigned L>
            struct tuple_read {
                template <typename II, typename Cx>
                    static bool value(T& t, II& i, II e, Cx& cx) {
                        if (!read_value<X>(std::get<N>(t), i, e, cx)) return false;
                        if (N + 1 != L) if (!cio::consume<X>(X::list::sep, i, e, cx)) return false;
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
                        return  write_value<X>(o, std::get<N>(t), cx) && cio::poke<X>(o, X::list::sep, cx) &&
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

    }

    template <typename X, typename II, typename Cx, typename ...T>
        inline constexpr bool read_tuple(std::tuple<T...>& t, II& i, II e, Cx& cx) {
            return bits::tuple_read<X, std::tuple<T...>, 0, std::tuple_size<std::tuple<T...>>::value>::value(t, i, e, cx);
        }

    template <typename X, typename O, typename Cx, typename ...T>
        inline bool write_tuple(O& o, const std::tuple<T...>& t, Cx& cx) {
            return bits::tuple_write<X, std::tuple<T...>, 0, std::tuple_size<std::tuple<T...>>::value - 1>::value(o, t, cx);
        }

}}}

namespace cxon {

    template <typename X, typename H, typename ...T>
        struct read<JSON<X>, std::tuple<H, T...>> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(std::tuple<H, T...>& t, II& i, II e, Cx& cx) {
                    return  cio::consume<J>(J::list::beg, i, e, cx) &&
                                cio::con::read_tuple<J>(t, i, e, cx) &&
                            cio::consume<J>(J::list::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename ...T>
        struct write<JSON<X>, std::tuple<T...>> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const std::tuple<T...>& t, Cx& cx) {
                    return  cio::poke<J>(o, J::list::beg, cx) &&
                                cio::con::write_tuple<J>(o, t, cx) &&
                            cio::poke<J>(o, J::list::end, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_TUPLE_HXX_
