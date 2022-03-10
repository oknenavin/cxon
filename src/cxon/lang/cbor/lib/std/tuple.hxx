// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_TUPLE_HXX_
#define CXON_CBOR_LIB_STD_TUPLE_HXX_

#include "cxon/lang/cbor/compound.hxx"

namespace cxon { namespace cbor { namespace cnt { // container read/write helpers

    namespace imp {

        template <typename X, typename T, unsigned N, unsigned L>
            struct tuple_read_ {
                template <typename II, typename Cx>
                    static bool value(T& t, II& i, II e, Cx& cx) {
                        return  read_value<X>(std::get<N>(t), i, e, cx) &&
                                tuple_read_<X, T, N + 1, L>::value(t, i, e, cx)
                        ;
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
                        return  write_value<X>(o, std::get<N>(t), cx) &&
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
            return imp::tuple_read_<X, std::tuple<T...>, 0, std::tuple_size<std::tuple<T...>>::value>::value(t, i, e, cx);
        }

    template <typename X, typename O, typename Cx, typename ...T>
        inline bool write_tuple(O& o, const std::tuple<T...>& t, Cx& cx) {
            return imp::tuple_write_<X, std::tuple<T...>, 0, std::tuple_size<std::tuple<T...>>::value - 1>::value(o, t, cx);
        }

}}}

namespace cxon {

    template <typename X>
        struct read<CBOR<X>, std::tuple<>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::tuple<>&, II& i, II e, Cx& cx) {
                    return  cbor::tag::read<Y>(i, e, cx) &&
                            cbor::cnt::read_size_eq<Y>(0, i, e, cx)
                    ;
                }
        };

    template <typename X, typename ...T>
        struct read<CBOR<X>, std::tuple<T...>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::tuple<T...>& t, II& i, II e, Cx& cx) {
                    return  cbor::tag::read<Y>(i, e, cx) &&
                            cbor::cnt::read_size_eq<Y>(std::tuple_size<std::tuple<T...>>::value, i, e, cx) &&
                            cbor::cnt::read_tuple<Y>(t, i, e, cx)
                    ;
                }
        };

    template <typename X>
        struct write<CBOR<X>, std::tuple<>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::tuple<>&, Cx& cx) {
                    return cbor::cnt::write_size<Y>(o, Y::arr, 0, cx);
                }
        };

    template <typename X, typename ...T>
        struct write<CBOR<X>, std::tuple<T...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::tuple<T...>& t, Cx& cx) {
                    return  cbor::cnt::write_size<Y>(o, Y::arr, std::tuple_size<std::tuple<T...>>::value, cx) &&
                            cbor::cnt::write_tuple<Y>(o, t, cx)
                    ;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_TUPLE_HXX_
