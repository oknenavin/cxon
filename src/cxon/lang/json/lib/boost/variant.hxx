// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_BOOST_VARIANT_HXX_
#define CXON_JSON_LIB_BOOST_VARIANT_HXX_

namespace cxon {

    namespace json { namespace imp { namespace boost {

        template <typename X, typename II, typename Cx, typename ...T>
            struct read_ {
                using V = ::boost::variant<T...>;

                template <typename R>
                    static bool variant_read_(V& t, II& i, II e, Cx& cx) {
                        auto u = V {R{}};
                        return cio::read_map_val<X>(::boost::get<R>(u), i, e, cx) && (t = std::move(u), true);
                    }
                static bool variant_read_(V& t, std::size_t n, II& i, II e, Cx& cx) {
                    using rvp_ = bool (*)(V&, II&, II, Cx&);
                    static constexpr rvp_ rv_[sizeof...(T)] = { &variant_read_<T>... };
                    return rv_[n](t, i, e, cx);
                }

                static bool index_(std::size_t& n, II& i, II e, Cx& cx) {
                    II const o = i;
                    return  (cio::read_map_key<X>(n, i, e, cx) && n < sizeof...(T)) ||
                            (cio::rewind(i, o), cx/json::read_error::unexpected)
                    ;
                }
                static bool variant(V& t, II& i, II e, Cx& cx) {
                    std::size_t n;
                        return index_(n, i, e, cx) && variant_read_(t, n, i, e, cx);
                }
            };

        template <typename X, typename II, typename Cx, typename ...T>
            inline bool variant_read_(::boost::variant<T...>& t, II& i, II e, Cx& cx) {
                return read_<X, II, Cx, T...>::variant(t, i, e, cx);
            }

        template <typename X, typename O, typename Cx, typename ...T>
            struct write_ {
                using V = ::boost::variant<T...>;

                template <typename R>
                    static bool variant_write_(O& o, const V& t, Cx& cx) {
                        return cio::write_map_val<X>(o, ::boost::get<R>(t), cx);
                    }
                static bool variant_write_(O& o, const V& t, std::size_t n, Cx& cx) {
                    using wvp_ = bool (*)(O&, const V&, Cx&);
                    static constexpr wvp_ wv_[sizeof...(T)] = { &variant_write_<T>... };
                    return wv_[n](o, t, cx);
                }

                static bool variant(O& o, const V& t, Cx& cx) {
                    return cio::write_map_key<X>(o, t.which(), cx) && variant_write_(o, t, t.which(), cx);
                }
            };

        template <typename X, typename O, typename Cx, typename ...T>
            inline bool variant_write_(O& o, const ::boost::variant<T...>& t, Cx& cx) {
                return write_<X, O, Cx, T...>::variant(o, t, cx);
            }

    }}}

    template <typename X, typename T>
        struct read<JSON<X>, boost::recursive_wrapper<T>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(boost::recursive_wrapper<T>& t, II& i, II e, Cx& cx) {
                    return read_value<Y>(t.get(), i, e, cx);
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, boost::recursive_wrapper<T>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const boost::recursive_wrapper<T>& t, Cx& cx) {
                    return write_value<Y>(o, t.get(), cx);
                }
        };

    template <typename X, typename ...T>
        struct read<JSON<X>, boost::variant<T...>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(boost::variant<T...>& t, II& i, II e, Cx& cx) {
                    return  cio::consume<Y>(Y::map::beg, i, e, cx) &&
                                json::imp::boost::variant_read_<Y>(t, i, e, cx) &&
                            cio::consume<Y>(Y::map::end, i, e, cx)
                    ;
                }
        };

    template <typename X, typename ...T>
        struct write<JSON<X>, boost::variant<T...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const boost::variant<T...>& t, Cx& cx) {
                    return  cio::poke<Y>(o, Y::map::beg, cx) &&
                                json::imp::boost::variant_write_<Y>(o, t, cx) &&
                            cio::poke<Y>(o, Y::map::end, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_BOOST_VARIANT_HXX_
