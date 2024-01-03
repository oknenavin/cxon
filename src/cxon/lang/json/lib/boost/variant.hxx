// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_BOOST_VARIANT_HXX_
#define CXON_JSON_LIB_BOOST_VARIANT_HXX_

#include "cxon/lang/common/cio/variant.hxx"

namespace cxon { namespace cio { // type traits
    template <typename ...T>    struct is_map<boost::variant<T...>>             : var::has_type<is_map, T...> {};
    template <typename ...T>    struct is_list<boost::variant<T...>>            : var::has_type<is_list, T...> {};
    template <typename ...T>    struct is_string<boost::variant<T...>>          : var::has_type<is_string, T...> {};
    template <typename ...T>    struct is_number<boost::variant<T...>>          : var::has_type<is_number, T...> {};
    template <typename ...T>    struct is_bool<boost::variant<T...>>            : var::has_type<is_bool, T...> {};
    template <typename ...T>    struct is_null<boost::variant<T...>>            : var::has_type<is_null, T...> {};
    template <typename T>       struct is_map<boost::recursive_wrapper<T>>      : is_map<T> {};
    template <typename T>       struct is_list<boost::recursive_wrapper<T>>     : is_list<T> {};
    template <typename T>       struct is_string<boost::recursive_wrapper<T>>   : is_string<T> {};
    template <typename T>       struct is_number<boost::recursive_wrapper<T>>   : is_number<T> {};
    template <typename T>       struct is_bool<boost::recursive_wrapper<T>>     : is_bool<T> {};
    template <typename T>       struct is_null<boost::recursive_wrapper<T>>     : is_null<T> {};
}}

namespace cxon {

    namespace imp { namespace json { namespace boost { // read

        template <typename X, typename II, typename Cx, typename ...T>
            struct read_ {
                using V = ::boost::variant<T...>;

                template <typename R>
                    static bool variant_(V& t, II& i, II e, Cx& cx) {
                        auto u = V {R{}};
                        return cio::read_map_val<X>(::boost::get<R>(u), i, e, cx) && (t = std::move(u), true);
                    }
                static bool variant_(V& t, std::size_t n, II& i, II e, Cx& cx) {
                    using rvp_ = bool (*)(V&, II&, II, Cx&);
                    static constexpr rvp_ rv_[sizeof...(T)] = { &variant_<T>... };
                    return rv_[n](t, i, e, cx);
                }

                static bool index_(std::size_t& n, II& i, II e, Cx& cx) {
                    II const o = i;
                    return  (cio::read_map_key<X>(n, i, e, cx) && n < sizeof...(T)) ||
                            (cio::rewind(i, o), cx/cxon::json::read_error::unexpected)
                    ;
                }
                static bool variant(V& t, II& i, II e, Cx& cx) {
                    std::size_t n;
                        return index_(n, i, e, cx) && variant_(t, n, i, e, cx);
                }
            };

        template <typename X, typename II, typename Cx, typename ...T>
            inline auto variant_read_(::boost::variant<T...>& t, II& i, II e, Cx& cx)
                -> enable_if_t< cio::is_key_context<X>::value ||  cio::var::is_ambiguous<T...>::value, bool>
            {
                return  cio::consume<X>(X::map::beg, i, e, cx) &&
                            read_<X, II, Cx, T...>::variant(t, i, e, cx) &&
                        cio::consume<X>(X::map::end, i, e, cx)
                ;
            }
        template <typename X, typename II, typename Cx, typename ...T>
            inline auto variant_read_(::boost::variant<T...>& t, II& i, II e, Cx& cx)
                -> enable_if_t<!cio::is_key_context<X>::value && !cio::var::is_ambiguous<T...>::value, bool>
            {
                return  cio::var::variant_read<X>(t, i, e, cx);
            }

    }}}

    namespace imp { namespace json { namespace boost { // write

        template <typename X, typename O, typename Cx, typename ...T>
            struct write_ {
                using V = ::boost::variant<T...>;

                template <typename R>
                    static bool variant_(O& o, const V& t, Cx& cx) {
                        return cio::write_map_val<X>(o, ::boost::get<R>(t), cx);
                    }

                static bool variant(O& o, const V& t, std::size_t n, Cx& cx) {
                    using wvp_ = bool (*)(O&, const V&, Cx&);
                    static constexpr wvp_ wv_[sizeof...(T)] = { &variant_<T>... };
                    return wv_[n](o, t, cx);
                }
                static bool variant(O& o, const V& t, Cx& cx) {
                    return cio::write_map_key<X>(o, t.which(), cx) && variant(o, t, t.which(), cx);
                }
            };

        template <typename X, typename O, typename Cx, typename ...T>
            inline auto variant_write_(O& o, const ::boost::variant<T...>& t, Cx& cx)
                -> enable_if_t< cio::is_key_context<X>::value ||  cio::var::is_ambiguous<T...>::value, bool>
            {
                return  cio::poke<X>(o, X::map::beg, cx) &&
                            write_<X, O, Cx, T...>::variant(o, t, cx) &&
                        cio::poke<X>(o, X::map::end, cx)
                ;
            }
        template <typename X, typename O, typename Cx, typename ...T>
            inline auto variant_write_(O& o, const ::boost::variant<T...>& t, Cx& cx)
                -> enable_if_t<!cio::is_key_context<X>::value && !cio::var::is_ambiguous<T...>::value, bool>
            {
                return  write_<X, O, Cx, T...>::variant(o, t, t.which(), cx);
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
                    return imp::json::boost::variant_read_<Y>(t, i, e, cx);
                }
        };
    template <typename X, typename ...T>
        struct write<JSON<X>, boost::variant<T...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const boost::variant<T...>& t, Cx& cx) {
                    return imp::json::boost::variant_write_<Y>(o, t, cx);
                }
        };

}

#endif // CXON_JSON_LIB_BOOST_VARIANT_HXX_
