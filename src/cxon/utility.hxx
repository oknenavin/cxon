// Copyright (c) 2017-2025 oknenavin.
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
#   if defined(__GNUC__) && __GNUC__ >= 7 && !defined(__clang__)
#       define CXON_FALLTHROUGH [[gnu::fallthrough]]
#   else
#       define CXON_FALLTHROUGH
#   endif
#else
#   define CXON_FALLTHROUGH [[fallthrough]]
#endif

#if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L
#   define CXON_CXX14_CONSTEXPR constexpr
#else
#   define CXON_CXX14_CONSTEXPR const
#endif

#if defined(__cpp_constexpr) && __cpp_constexpr >= 201703L
#   define CXON_CXX17_CONSTEXPR constexpr
#else
#   define CXON_CXX17_CONSTEXPR const
#endif

#ifdef __cpp_if_constexpr
#   define CXON_IF_CONSTEXPR(...) if constexpr (__VA_ARGS__)
#else 
#   define CXON_IF_CONSTEXPR(...) if (__VA_ARGS__)
#endif

#if defined(__GNUC__) || defined(__clang__)
#   define CXON_INLAY inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#   define CXON_INLAY __forceinline
#else
#   define CXON_INLAY inline
#endif

#if !defined(CXON_USE_SIMD) || CXON_USE_SIMD
#   if (defined(_M_IX86) && _M_IX86_FP == 2) || defined(_M_X64) || defined(__SSE2__)
#       define CXON_USE_SIMD_SSE2 1
#   else
#       define CXON_USE_SIMD_SSE2 0
#   endif
#   if defined(__ARM_NEON__ )
#       define CXON_USE_SIMD_NEON 1
#   else
#       define CXON_USE_SIMD_NEON 0
#   endif
#else
#   define CXON_USE_SIMD_SSE2 0
#   define CXON_USE_SIMD_NEON 0
#endif

// interface ///////////////////////////////////////////////////////////////////

namespace cxon {

    template <typename ...> // C++17
        using void_t = void;

    template <bool C, typename T = void> // C++14
        using enable_if_t = typename std::enable_if<C, T>::type;

    template <typename ...>
        constexpr bool unexpected();

    template <std::size_t N> struct option;

    // format traits

    template <typename T, template <typename> class U>
        struct has_traits                       : std::false_type {};
    template <template <typename> class U, typename V>
        struct has_traits<U<V>, U>              : std::true_type {};
    template <template <typename> class T, typename V, template <typename> class U>
        struct has_traits<T<V>, U>              : std::integral_constant<bool, has_traits<V, U>::value> {};

    template <typename T, template <typename> class U>
        struct bind_traits;
    template <template <typename> class T, typename V, template <typename> class U>
        struct bind_traits<T<V>, U>             { using type = T<U<V>>; };
    template <typename T, template <typename> class U>
        using bind_traits_t = typename std::conditional<!has_traits<T, U>::value, typename bind_traits<T, U>::type, T>::type;

    template <typename T, template <typename> class U>
        struct unbind_traits                    { using type = T; };
    template <template <typename> class U, typename V>
        struct unbind_traits<U<V>, U>           { using type = V; };
    template <template <typename> class T, typename V, template <typename> class U>
        struct unbind_traits<T<V>, U>           { using type = T<typename unbind_traits<V, U>::type>; };
    template <typename T, template <typename> class U>
        using unbind_traits_t = typename unbind_traits<T, U>::type;

    // replace U with B if bound
    template <typename T, template <typename> class U, template <typename> class B>
        using replace_traits_t = typename std::conditional<has_traits<T, U>::value, bind_traits_t<unbind_traits_t<T, U>, B>, T>::type;

    template <bool C, typename T, template <typename> class U>
        using conditional_bind_traits_t = typename std::conditional<C, bind_traits_t<T, U>, unbind_traits_t<T, U>>::type;

    // type sequence

    template <typename ...>
        struct type_sequence;

    template <template <typename> class Trait, typename ...T>
        struct type_sequence_conjunction;

    template <template <typename> class Trait, typename ...T>
        struct type_sequence_disjunction;

    // integer sequence

    template <typename T, T ...>
        struct scalar_sequence;
    namespace imp {
        template <typename T, std::size_t H, T ...Ts>
            struct make_scalar_sequence_ : make_scalar_sequence_<T, H - 1, H - 1, Ts...> {};
        template <typename T, T ...Ts>
            struct make_scalar_sequence_<T, 0, Ts...> : scalar_sequence<T, Ts...> {
                using type = scalar_sequence<T, Ts...>;
            };
    }
    template <typename T, T N>
        using make_scalar_sequence = typename imp::make_scalar_sequence_<T, N>::type;

    template <typename S, typename S::value_type>
        struct scalar_sequence_contains;

#   if __cplusplus < 201402L
        template <typename T, T ...Ts>  // C++14
            using integer_sequence      = scalar_sequence<T, Ts...>;
        template <typename T, T N>
            using make_integer_sequence = make_scalar_sequence<T, N>;
        template <std::size_t ...N>     // C++14
            using index_sequence        = integer_sequence<std::size_t, N...>;
        template <std::size_t N>
            using make_index_sequence   = make_integer_sequence<std::size_t, N>;
#   else
        template <typename T, T ...Ts>  // C++14
            using integer_sequence      = std::integer_sequence<T, Ts...>;
        template <typename T, T N>
            using make_integer_sequence = std::integer_sequence<T, N>;
        template <std::size_t ...N>     // C++14
            using index_sequence        = std::integer_sequence<std::size_t, N...>;
        template <std::size_t N>
            using make_index_sequence   = std::make_integer_sequence<std::size_t, N>;
#   endif

    // type traits

    template <typename T> struct is_bool;
    template <typename T> struct is_char;
    template <typename T> struct is_numeric;

    template <typename T> using remove_const_t      = typename std::remove_const<T>::type;      // C++14
    template <typename T> using remove_volatile_t   = typename std::remove_volatile<T>::type;   // C++14
    template <typename T> using remove_cv_t         = typename std::remove_cv<T>::type;         // C++14
    template <typename T> using remove_reference_t  = typename std::remove_reference<T>::type;  // C++14
    template <typename T> using decay_t             = typename std::decay<T>::type;             // C++14

    template <typename T>       // C++20
        struct remove_cvref     : std::remove_cv<remove_reference_t<T>> {};
    template <typename T>       // C++20
        using  remove_cvref_t   = typename remove_cvref<T>::type;

    template <bool B, typename T, typename F> // C++14
        using conditional_t     = typename std::conditional<B, T, F>::type;

    template <bool B>       // C++17
        using bool_constant = std::integral_constant<bool, B>;

    template <typename T>   // C++20
        struct type_identity { using type = T; };

    template <typename ...> // C++17
        struct conjunction;
    template <typename ...> // C++17
        struct disjunction;
    template <typename B>   // C++17
        struct negation : bool_constant<!B::value> {};

    template <typename ...T>
        struct aligned_union; // std::aligned_union deprecated in c++23

    // container

    template <typename T>
        using is_back_insertable = std::is_same<decltype(std::declval<T>().push_back(std::declval<typename T::value_type&&>())), void>;

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

    //template <typename ...Tg>
    //    constexpr pack<...> make_pack(Tg&&... t);

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

    // type sequence

    template <template <typename> class Trait, typename ...T>
        struct type_sequence_conjunction :
            std::true_type {};
    template <template <typename> class Trait, typename T, typename ...Ts>
        struct type_sequence_conjunction<Trait, type_sequence<T, Ts...>> :
            bool_constant<Trait<T>::value && type_sequence_conjunction<Trait, type_sequence<Ts...>>::value> {};

    template <template <typename> class Trait, typename ...T>
        struct type_sequence_disjunction :
            std::true_type {};
    template <template <typename> class Trait, typename T, typename ...Ts>
        struct type_sequence_disjunction<Trait, type_sequence<T, Ts...>> :
            bool_constant<Trait<T>::value || type_sequence_disjunction<Trait, type_sequence<Ts...>>::value> {};

    namespace imp {

        template <typename H, typename ...T>
            class max_sizeof_ {
                static constexpr auto f_ = sizeof(H);
                static constexpr auto t_ = max_sizeof_<T...>::value;
                public: static constexpr auto value = f_ < t_ ? t_ : f_;
            };
        template <typename T>
            class max_sizeof_<T> {
                public: static constexpr auto value = sizeof(T);
            };

        template <typename H, typename ...T>
            class max_alignof_ {
                static constexpr auto f_ = alignof(H);
                static constexpr auto t_ = max_alignof_<T...>::value;
                public: static constexpr auto value = f_ < t_ ? t_ : f_;
            };
        template <typename T>
            class max_alignof_<T> {
                public: static constexpr auto value = alignof(T);
            };

    }
    template <typename ...T>
        struct aligned_union {
            static constexpr auto alignment_value = imp::max_alignof_<T...>::value;
            class alignas(alignment_value) type {
                unsigned char s_[imp::max_sizeof_<T...>::value];
            };
        };

    // integer sequence

    template <typename T, T ...Ts>
        struct scalar_sequence {
            using value_type = T;
            static constexpr std::size_t size() noexcept { return sizeof...(Ts); }
        };

    namespace imp {
        template <typename S, typename T, T V, T ...Ts>
            struct scalar_sequence_contains_                        : std::false_type {};
        template <template <typename U, U ...> class S, typename T, T V, T H, T ...Ts>
            struct scalar_sequence_contains_<S<T, H, Ts...>, T, V>  :
                bool_constant<H == V || scalar_sequence_contains_<S<T, Ts...>, T, V>::value> {};
    }
    template <typename S, typename S::value_type V>
        struct scalar_sequence_contains : bool_constant<imp::scalar_sequence_contains_<S, typename S::value_type, V>::value> {};

    // type traits

    namespace imp {
        template <typename T>   struct is_bool_             : std::false_type {};
        template <>             struct is_bool_<bool>       : std::true_type {};

        template <typename T>   struct is_char_             : std::false_type {};
        template <>             struct is_char_<char>       : std::true_type {};
        template <>             struct is_char_<wchar_t>    : std::true_type {};
#       if defined(__cpp_char8_t)
            template <>         struct is_char_<char8_t>    : std::true_type {};
#       endif
        template <>             struct is_char_<char16_t>   : std::true_type {};
        template <>             struct is_char_<char32_t>   : std::true_type {};
    }

    template <typename T>
        struct is_bool      : bool_constant<imp::is_bool_<remove_cv_t<T>>::value> {};
    template <typename T>
        struct is_char      : bool_constant<imp::is_char_<remove_cv_t<T>>::value> {};
    template <typename T>
        struct is_numeric   : bool_constant<std::is_arithmetic<T>::value && !is_char<T>::value && !is_bool<T>::value> {};

    template <typename ...>
        struct conjunction                  : std::true_type {};
    template <typename B, typename ...Bs>
        struct conjunction<B, Bs...>        : bool_constant<B::value && conjunction<Bs...>::value> {};

    template <typename ...>
        struct disjunction                  : std::false_type {};
    template <typename B, typename ...Bs>
        struct disjunction<B, Bs...>        : bool_constant<B::value || disjunction<Bs...>::value> {};

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
            struct has_tag              : bool_constant<!is_same_tag<Tg, Pk, Sz - 1>::value ? has_tag<Tg, Pk, Sz - 1>::value : true> {};
        template <typename Tg, typename Pk>
            struct has_tag<Tg, Pk, 1>   : bool_constant<is_same_tag<Tg, Pk, 0>::value> {};
        template <typename Tg, typename Pk>
            struct has_tag<Tg, Pk, 0>   : std::false_type {};

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
            constexpr auto get(Pk& p) -> tag_type<Tg>& {
                return pack<Tg, Pk>::get(p);
            }
        template <typename Tg, typename Pk>
            constexpr auto get(const Pk& p) -> const tag_type<Tg>& {
                return pack<Tg, Pk>::get(const_cast<Pk&>(p));
            }

        template <typename Tg, typename Pk>
            constexpr auto get() -> tag_type<Tg> {
                return pack<Tg, Pk>::get();
            }


        template <typename Tg, typename T, T v>
            struct cst {
                using tag   = Tg;
                using type  = T;

                static constexpr type value = v;

                friend constexpr bool operator ==(const cst& c1, const cst& c2) { return c1.value == c2.value; }
                friend constexpr bool operator  <(const cst& c1, const cst& c2) { return c1.value  < c2.value; }
            };

        template <typename Tg, typename T>
            struct var {
                using tag   = Tg;
                using type  = T;

                type value;

                var()               : value() {}
                var(type&& v)       : value(std::move(v)) {}
                var(const type& v)  : value(v) {}

                friend constexpr bool operator ==(const var& v1, const var& v2) { return v1.value == v2.value; }
                friend constexpr bool operator  <(const var& v1, const var& v2) { return v1.value  < v2.value; }
            };

    }
    
    namespace imp {

            template <typename Tg>
                struct head_ {
                    static constexpr std::tuple<Tg> pack(Tg&& t)        { return std::make_tuple(std::move(t)); }
                    static constexpr std::tuple<Tg> pack(const Tg& t)   { return std::make_tuple(t); }
                };
            template <typename ...Tg>
                struct head_<std::tuple<Tg...>> {
                    static constexpr std::tuple<Tg...>          pack(std::tuple<Tg...>&& t)         { return std::move(t); }
#                   if defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ > 5)) && !defined(__clang__)
                        static constexpr std::tuple<Tg...>      pack(const std::tuple<Tg...>& t)    { return t; }
#                   else // type_traits:2204:7: error: static assertion failed: declval() must not be used!
                        static /*constexpr */std::tuple<Tg...>  pack(const std::tuple<Tg...>& t)    { return t; }
#                   endif
                };
        template <typename T>
            constexpr auto head_pack_(T&& t)
                -> decltype(head_<decay_t<T>>::pack(std::forward<T>(t)))
            {
                return      head_<decay_t<T>>::pack(std::forward<T>(t));
            }

        template <typename ...>
            struct make_ {
                static constexpr auto pack() -> std::tuple<> {
                    return {};
                }
            };
        template <typename H, typename ...T>
            struct make_<H, T...> {
                static constexpr auto pack(H&& h, T&&... t)
                    -> decltype(std::tuple_cat(head_pack_(std::forward<H>(h)), make_<T...>::pack(std::forward<T>(t)...)))
                {
                    return      std::tuple_cat(head_pack_(std::forward<H>(h)), make_<T...>::pack(std::forward<T>(t)...));
                }
            };
        template <typename T>
            struct make_<T> {
                static constexpr auto pack(T&& t)
                    -> decltype(head_pack_(std::forward<T>(t)))
                {
                    return      head_pack_(std::forward<T>(t));
                }
            };

    }
    template <typename ...Tg>
        constexpr auto make_pack(Tg&&... t) -> decltype(imp::make_<Tg...>::pack(std::forward<Tg>(t)...)) {
            return imp::make_<Tg...>::pack(std::forward<Tg>(t)...);
        }

    template <typename Tg, typename T, bool E/* = std::is_scalar<T>::value*/>
        struct parameter {
            using tag   = Tg;
            using type  = T;

            template <typename Pk>
                using in = imp::has_tag<Tg, Pk>;

            static constexpr imp::var<Tg, T> set(T&& v)         { return imp::var<Tg, T>(std::move(v)); }
            static constexpr imp::var<Tg, T> set(const T& v)    { return imp::var<Tg, T>(v); }

            template <typename Pk>
                static constexpr auto constant(T)       -> enable_if_t< imp::has_tag<Tg, Pk>::value, T>
                { return imp::get<Tg, Pk>(); }
            template <typename Pk>
                static constexpr auto constant(T dflt)  -> enable_if_t<!imp::has_tag<Tg, Pk>::value, T>
                { return dflt; }

            template <typename Pk>
                static constexpr T& reference(Pk& p)    { return imp::get<Tg>(p); }
            template <typename Pk>
                static constexpr T value(const Pk& p)   { return imp::get<Tg>(p); }

            template <typename Pk>
                static constexpr auto value(const Pk& p, T)     -> enable_if_t< imp::has_tag<Tg, Pk>::value, T>
                { return imp::get<Tg>(p); }
            template <typename Pk>
                static constexpr auto value(const Pk&, T dflt)  -> enable_if_t<!imp::has_tag<Tg, Pk>::value, T>
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
