// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_UTILITY_HXX_
#define CXON_UTILITY_HXX_

#include <type_traits>
#include <utility>
#include <iterator>
#include <algorithm>
#include <tuple>

#ifndef NDEBUG
#   if !defined(_MSC_VER)
#       include <cassert>
#       define CXON_ASSERT(e, m) assert(e)
#   else
#       include <crtdbg.h>
#       define CXON_ASSERT(e, m) _ASSERT_EXPR((e), m)
#   endif
#else
#   define CXON_ASSERT(e, m) ((void)(e))
#endif

#if __cplusplus < 201703L
#   if defined(__GNUC__)  && __GNUC__ >= 7 && !defined(__clang__)
#       define CXON_FALLTHROUGH [[gnu::fallthrough]]
#   else
#       define CXON_FALLTHROUGH
#   endif
#else
#   define CXON_FALLTHROUGH [[fallthrough]]
#endif

#ifdef __cpp_if_constexpr
#   define CXON_IF_CONSTEXPR(c) if constexpr (c)
#else 
#   define CXON_IF_CONSTEXPR(c) if (c)
#endif

// interface ///////////////////////////////////////////////////////////////////

namespace cxon {

    template <bool C, typename T = void> // C++14
        using enable_if_t = typename std::enable_if<C, T>::type;

    template <typename ...>
        constexpr bool unexpected();

    template <std::size_t N> struct option;

    // type traits

    template <typename T> struct is_bool;
    template <typename T> struct is_char;
    template <typename T> struct is_numeric;

    // container

    template <typename C>
        using has_emplace_back      = std::is_same<decltype(std::declval<C>().emplace_back()), typename C::reference>;
    template <typename C>
        using has_emplace_back_void = std::is_same<decltype(std::declval<C>().emplace_back()), void>;
    template <typename C>
        using has_back              = std::is_same<decltype(std::declval<C>().back()), typename C::reference>;
    template <typename C>
        using has_push_back         = std::is_same<decltype(std::declval<C>().push_back(std::declval<typename C::value_type>())), void>;

    template <typename C>
        using is_back_insertable    = has_push_back<C>;

    // iterators

    template <typename I>
        using   iterator_category_t = typename std::iterator_traits<I>::iterator_category;

    template <typename I>
        using   is_output_iterator = std::is_same<iterator_category_t<I>, std::output_iterator_tag>;
    template <typename, typename = void>
        struct  is_forward_iterator;
    template <typename I>
        using   is_random_access_iterator = std::is_same<iterator_category_t<I>, std::random_access_iterator_tag>;

}

namespace cxon { namespace napa { // named parameters

    template <typename ...Tg>
        using pack = std::tuple<Tg...>;

    template <typename ...Tg>
        inline constexpr pack<Tg...> make_pack(Tg&&... t);

    template <typename Tg, typename T, bool E = std::is_scalar<T>::value>
        struct parameter;

#   define CXON_PARAMETER(P, T) struct P : cxon::napa::parameter<P, T> {}

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon {

    template <typename ...>
        constexpr bool unexpected() { return false; }

    template <std::size_t N>    struct option : option<N - 1>   {};
    template<>                  struct option<0>                {};

    // type traits

    namespace imp {

        template <typename T>   struct is_bool_ : std::false_type {};
        template <>             struct is_bool_<bool> : std::true_type {};

        template <typename T>   struct is_char_ : std::false_type {};
        template <>             struct is_char_<char> : std::true_type {};
        template <>             struct is_char_<wchar_t> : std::true_type {};
#       if __cplusplus > 201703L /* C++20 */
            template <>             struct is_char_<char8_t> : std::true_type {};
#       endif
        template <>             struct is_char_<char16_t> : std::true_type {};
        template <>             struct is_char_<char32_t> : std::true_type {};
    }

    template <typename T>
        struct is_bool {
            static constexpr bool value = imp::is_bool_<typename std::remove_cv<T>::type>::value;
        };

    template <typename T>
        struct is_char {
            static constexpr bool value = imp::is_char_<typename std::remove_cv<T>::type>::value;
        };

    template <typename T>
        struct is_numeric {
            static constexpr bool value = std::is_arithmetic<T>::value && !is_char<T>::value && !is_bool<T>::value;
        };

    // iterators

    template <typename, typename/* = void*/>
        struct is_forward_iterator : std::false_type {};
    template <typename I>
        struct is_forward_iterator<I, enable_if_t<!std::is_same<iterator_category_t<I>, std::input_iterator_tag>::value>> : std::true_type {};

}

namespace cxon { namespace napa {

    namespace imp {

        template <typename Pk, std::size_t Ix>
            using tag = typename std::tuple_element<Ix, Pk>::type::tag;
        template <typename Tg, typename Pk, std::size_t Ix>
            using is_same_tag = std::is_same<Tg, tag<Pk, Ix>>;

        template <typename Tg>
            using tag_type = typename Tg::type;

        template <std::size_t Ix, typename T>
            using tuple_element_t = typename std::tuple_element<Ix, T>::type;

        template <typename Tg, typename Pk, std::size_t Sz = std::tuple_size<Pk>::value>
            struct has_tag              { static constexpr bool value = !is_same_tag<Tg, Pk, Sz - 1>::value ? has_tag<Tg, Pk, Sz - 1>::value : true; };
        template <typename Tg, typename Pk>
            struct has_tag<Tg, Pk, 1>   { static constexpr bool value = is_same_tag<Tg, Pk, 0>::value; };
        template <typename Tg, typename Pk>
            struct has_tag<Tg, Pk, 0>   { static constexpr bool value = false; };

        template <typename Tg, typename Pk, std::size_t Sz = std::tuple_size<Pk>::value>
            struct pack {
                static constexpr tag_type<Tg>& get(Pk& p)   { return get_(p); }
                static constexpr tag_type<Tg>  get()        { return get_(); }

                private:
                    template <typename X = Tg>
                        static constexpr auto get_(Pk& p) -> enable_if_t< is_same_tag<X, Pk, Sz - 1>::value, tag_type<X>&> {
                            return std::get<Sz - 1>(p).value;
                        }
                    template <typename X = Tg>
                        static constexpr auto get_(Pk& p) -> enable_if_t<!is_same_tag<X, Pk, Sz - 1>::value, tag_type<X>&> {
                            return pack<X, Pk, Sz - 1>::get(p);
                        }

                    template <typename X = Tg>
                        static constexpr auto get_() -> enable_if_t< is_same_tag<X, Pk, Sz - 1>::value, tag_type<X>> {
                            return tuple_element_t<Sz - 1, Pk>::value;
                        }
                    template <typename X = Tg>
                        static constexpr auto get_() -> enable_if_t<!is_same_tag<X, Pk, Sz - 1>::value, tag_type<X>> {
                            return pack<X, Pk, Sz - 1>::get();
                        }
            };
        template <typename Tg, typename Pk>
            struct pack<Tg, Pk, 1> {
                static_assert(is_same_tag<Tg, Pk, 0>::value, "no such tag");
                static constexpr tag_type<Tg>& get(Pk& p)   { return std::get<0>(p).value; }
                static constexpr tag_type<Tg>  get()        { return tuple_element_t<0, Pk>::value; }
            };
        template <typename Tg, typename Pk>
            struct pack<Tg, Pk, 0> {
                static_assert(unexpected<Tg>(), "no such tag");
                static void get(Pk&)    {}
                static void get()       {}
            };
      
        template <typename Tg, typename Pk>
            inline constexpr auto get(Pk& p) -> tag_type<Tg>& {
                return pack<Tg, Pk>::get(p);
            }
        template <typename Tg, typename Pk>
            inline constexpr auto get(const Pk& p) -> const tag_type<Tg>& {
                return pack<Tg, Pk>::get(const_cast<Pk&>(p));
            }

        template <typename Tg, typename Pk>
            inline constexpr auto get() -> tag_type<Tg> {
                return pack<Tg, Pk>::get();
            }


        template <typename Tg, typename T, T v>
            struct cst {
                using tag   = Tg;
                using type  = T;

                static constexpr type value = v;
            };

        template <typename Tg, typename T>
            struct var {
                using tag   = Tg;
                using type  = T;

                type value;

                var()               : value() {}
                var(type&& v)       : value(std::move(v)) {}
                var(const type& v)  : value(v) {}
            };

    }

    template <typename ...Tg>
        inline constexpr pack<Tg...> make_pack(Tg&&... t) {
            return std::make_tuple(std::forward<Tg>(t)...);
        }

    template <typename Tg, typename T, bool E/* = std::is_scalar<T>::value*/>
        struct parameter {
            using tag   = Tg;
            using type  = T;

            template <typename Pk>
                using in = imp::has_tag<Tg, Pk>;

            static constexpr imp::var<Tg, T> set(T&& v)         { return imp::var<Tg, T>(std::forward<T>(v)); }
            static constexpr imp::var<Tg, T> set(const T& v)    { return imp::var<Tg, T>(v); }

            template <typename Pk>
                static constexpr auto constant(T)
                    -> enable_if_t< imp::has_tag<Tg, Pk>::value, T>
                { return imp::get<Tg, Pk>(); }
            template <typename Pk>
                static constexpr auto constant(T dflt)
                    -> enable_if_t<!imp::has_tag<Tg, Pk>::value, T>
                { return dflt; }

            template <typename Pk>
                static constexpr T& reference(Pk& p)    { return imp::get<Tg>(p); }
            template <typename Pk>
                static constexpr T value(const Pk& p)   { return imp::get<Tg>(p); }

            template <typename Pk>
                static constexpr auto value(const Pk& p, T)
                    -> enable_if_t< imp::has_tag<Tg, Pk>::value, T>
                { return imp::get<Tg>(p); }
            template <typename Pk>
                static constexpr auto value(const Pk&, T dflt)
                    -> enable_if_t<!imp::has_tag<Tg, Pk>::value, T>
                { return dflt; }
        };
    template <typename Tg, typename T>
        struct parameter<Tg, T, true> : parameter<Tg, T, false> {
            using tag   = Tg;
            using type  = T;

            using parameter<Tg, T, false>::set;

            template <T c>
                static constexpr imp::cst<Tg, T, c> set() { return {}; }
        };

}}

#endif // CXON_UTILITY_HXX_
