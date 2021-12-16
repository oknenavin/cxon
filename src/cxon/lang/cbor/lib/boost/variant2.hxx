// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_BOOST_VARIANT2_HXX_
#define CXON_CBOR_LIB_BOOST_VARIANT2_HXX_

#include "cxon/lang/cbor/compound.hxx"

namespace cxon {

    namespace cbor { namespace imp { namespace boost { namespace variant2 {

        template <typename X, typename V, typename II, typename Cx, typename N = make_index_sequence<::boost::variant2::variant_size<V>::value>>
            struct read_;
        template <typename X, typename V, typename II, typename Cx, std::size_t ...N>
            struct read_<X, V, II, Cx, index_sequence<N...>> {
                static constexpr std::size_t S_ = index_sequence<N...>::size();

                template <std::size_t M>
                    static bool variant_read_(V& t, II& i, II e, Cx& cx) {
                        auto u = V {::boost::variant2::in_place_index_t<M>()};
                        return read_value<X>(::boost::variant2::get<M>(u), i, e, cx) && (t = std::move(u), true);
                    }
                static bool variant_read_(V& t, std::size_t n, II& i, II e, Cx& cx) {
                    using rvp_ = bool (*)(V&, II&, II, Cx&);
                    static constexpr rvp_ rv_[S_] = { &variant_read_<N>... };
                    return rv_[n](t, i, e, cx);
                }

                static bool index_(std::size_t& n, II& i, II e, Cx& cx) {
                    II const o = i;
                    return  (read_value<X>(n, i, e, cx) && n < S_) ||
                            (bio::rewind(i, o), cx/cbor::read_error::unexpected)
                    ;
                }
                static bool variant(V& t, II& i, II e, Cx& cx) {
                    std::size_t n;
                        return index_(n, i, e, cx) && variant_read_(t, n, i, e, cx);
                }
            };

        template <typename X, typename V, typename II, typename Cx>
            inline bool variant_read_(V& t, II& i, II e, Cx& cx) {
                return read_<X, V, II, Cx>::variant(t, i, e, cx);
            }

        template <typename X, typename O, typename V, typename Cx, typename N = make_index_sequence<::boost::variant2::variant_size<V>::value>>
            struct write_;
        template <typename X, typename O, typename V, typename Cx, std::size_t ...N>
            struct write_<X, O, V, Cx, index_sequence<N...>> {
                static constexpr std::size_t S_ = index_sequence<N...>::size();

                template <std::size_t M>
                    static bool variant_write_(O& o, const V& t, Cx& cx) {
                        return write_value<X>(o, ::boost::variant2::get<M>(t), cx);
                    }
                static bool variant_write_(O& o, const V& t, std::size_t n, Cx& cx) {
                    using wvp_ = bool (*)(O&, const V&, Cx&);
                    static constexpr wvp_ wv_[S_] = { &variant_write_<N>... };
                    return wv_[n](o, t, cx);
                }

                static bool variant(O& o, const V& t, Cx& cx) {
                    return write_value<X>(o, t.index(), cx) && variant_write_(o, t, t.index(), cx);
                }
            };

        template <typename X, typename O, typename V, typename Cx>
            inline bool variant_write_(O& o, const V& t, Cx& cx) {
                return write_<X, O, V, Cx>::variant(o, t, cx);
            }

    }}}}

    template <typename X>
        struct read<CBOR<X>, boost::variant2::monostate> {
            template <typename II, typename Cx>
                static bool value(boost::variant2::monostate&, II& i, II e, Cx& cx) {
                    return cbor::tag::read<X>(i, e, cx) && (
                        (bio::peek(i, e) == X::und && (bio::get(i, e), true)) ||
                        (cx/cbor::read_error::unexpected)
                    );
                }
        };

    template <typename X>
        struct write<CBOR<X>, boost::variant2::monostate> {
            template <typename O, typename Cx>
                static bool value(O& o, boost::variant2::monostate, Cx& cx) {
                    return bio::poke<X>(o, X::und, cx);
                }
        };

    template <typename X, typename ...T>
        struct read<CBOR<X>, boost::variant2::variant<T...>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(boost::variant2::variant<T...>& t, II& i, II e, Cx& cx) {
                    return  cbor::tag::read<Y>(i, e, cx) &&
                            cbor::cnt::read_size_eq<Y>(2, i, e, cx) &&
                            cbor::imp::boost::variant2::variant_read_<Y>(t, i, e, cx)
                    ;
                }
        };

    template <typename X, typename ...T>
        struct write<CBOR<X>, boost::variant2::variant<T...>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const boost::variant2::variant<T...>& t, Cx& cx) {
                    return  cbor::cnt::write_size<Y>(o, Y::arr, 2, cx) &&
                            cbor::imp::boost::variant2::variant_write_<Y>(o, t, cx)
                    ;
                }
        };

}

#endif // CXON_CBOR_LIB_BOOST_VARIANT2_HXX_
