// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_TUPLE_HXX_
#define CXON_JSON_LIB_STD_TUPLE_HXX_

namespace cxon { namespace cio { // type traits
    template <typename ...T> struct is_list<std::tuple<T...>> : std::true_type {};
}}

namespace cxon { namespace cio { namespace cnt { // container read/write helpers

    namespace imp {

        template <typename X, typename T, unsigned N, unsigned L>
            struct tuple_read_ {
                template <typename II, typename Cx>
                    static bool value(T& t, II& i, II e, Cx& cx) {
                        if (!read_value<X>(std::get<N>(t), i, e, cx)) return false;
                        if (N + 1 != L) if (!cio::consume<X>(cio::cnt::sep_read<X, typename X::list, II>, i, e, cx)) return false;
                        return tuple_read_<X, T, N + 1, L>::value(t, i, e, cx);
                    }
            };
        template <typename X, typename T, unsigned N>
            struct tuple_read_<X, T, N, N> {
                template <typename II, typename Cx>
                    static constexpr bool value(T&, II&, II, Cx&) { return true; }
            };

        template <typename X, typename T, unsigned N, unsigned L>
            struct tuple_write_ {
                template <typename O, typename Cx>
                    static bool value(O& o, const T& t, Cx& cx) {
                        return  write_value<X>(o, std::get<N>(t), cx) && cio::poke<X>(o, X::list::sep, cx) &&
                                tuple_write_<X, T, N + 1, L>::value(o, t, cx)
                        ;
                    }
            };
        template <typename X, typename T, unsigned N>
            struct tuple_write_<X, T, N, N> {
                template <typename O, typename Cx>
                    static bool value(O& o, const T& t, Cx& cx) {
                        return write_value<X>(o, std::get<N>(t), cx);
                    }
            };

    }

    template <typename X, typename II, typename Cx, typename ...T>
        inline bool read_tuple(std::tuple<T...>& t, II& i, II e, Cx& cx) {
            return imp::tuple_read_<X, std::tuple<T...>, 0, sizeof...(T)>::value(t, i, e, cx);
        }

    template <typename X, typename O, typename Cx, typename ...T>
        inline bool write_tuple(O& o, const std::tuple<T...>& t, Cx& cx) {
            return imp::tuple_write_<X, std::tuple<T...>, 0, sizeof...(T) - 1>::value(o, t, cx);
        }

}}}

namespace cxon {

    template <typename X>
        struct read<JSON<X>, std::tuple<>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::tuple<>&, II& i, II e, Cx& cx) {
                    return  cio::consume<Y>(Y::list::beg, i, e, cx) &&
                            cio::consume<Y>(Y::list::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename ...T>
        struct read<JSON<X>, std::tuple<T...>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::tuple<T...>& t, II& i, II e, Cx& cx) {
                    return  cio::consume<Y>(Y::list::beg, i, e, cx) &&
                                cio::cnt::read_tuple<Y>(t, i, e, cx) &&
                            cio::consume<Y>(Y::list::end, i, e, cx)
                    ;
                }
        };

    template <typename X>
        struct write<JSON<X>, std::tuple<>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::tuple<>&, Cx& cx) {
                    return  cio::poke<Y>(o, Y::list::beg, cx) &&
                            cio::poke<Y>(o, Y::list::end, cx)
                    ;
                }
        };

    template <typename X, typename ...T>
        struct write<JSON<X>, std::tuple<T...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::tuple<T...>& t, Cx& cx) {
                    return  cio::poke<Y>(o, Y::list::beg, cx) &&
                                cio::cnt::write_tuple<Y>(o, t, cx) &&
                            cio::poke<Y>(o, Y::list::end, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_TUPLE_HXX_
