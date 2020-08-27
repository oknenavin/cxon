// Copyright (c) 2017-2020 oknenavin.
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

// interface ///////////////////////////////////////////////////////////////////

namespace cxon {

    template <bool C, typename T = void> // C++14
        using enable_if_t = typename std::enable_if<C, T>::type;

    template <typename ...>
        constexpr bool unexpected();

    template <std::size_t N> struct option;

    // type traits

    template <typename T> struct is_char;

    // iterators

    template <typename I>
        using iterator_category_t = typename std::iterator_traits<I>::iterator_category;

    template <typename, typename = void>
        struct is_output_iterator;

    template <typename, typename = void>
        struct is_forward_iterator;

    template <typename, typename = void>
        struct is_back_insertable;

    template <typename I>
        struct continuous;

    template <typename FwIt>
        struct range_output_iterator;

    template <typename FwIt>
        constexpr range_output_iterator<FwIt> make_output_iterator(FwIt b, FwIt e);

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

    template <typename T>   struct is_char : std::false_type {};
    template <>             struct is_char<char> : std::true_type {};
    template <>             struct is_char<wchar_t> : std::true_type {};
#   if __cplusplus >= 202002L
        template <>             struct is_char<char8_t> : std::true_type {};
#   endif
    template <>             struct is_char<char16_t> : std::true_type {};
    template <>             struct is_char<char32_t> : std::true_type {};

    // iterators

    template <typename I>
        using iterator_category_t = typename std::iterator_traits<I>::iterator_category;

    template <typename, typename/* = void*/>
        struct is_output_iterator : std::false_type {};
    template <typename I>
        struct is_output_iterator<I, enable_if_t<std::is_same<iterator_category_t<I>, std::output_iterator_tag>::value>> : std::true_type {};

    template <typename, typename/* = void*/>
        struct is_forward_iterator : std::false_type {};
    template <typename I>
        struct is_forward_iterator<I, enable_if_t<!std::is_same<iterator_category_t<I>, std::input_iterator_tag>::value>> : std::true_type {};

    template <typename, typename/* = void*/>
        struct is_back_insertable : std::false_type {};
    template <typename C>
        struct is_back_insertable<C, decltype(C().push_back(' '))> : std::true_type {};

    template <typename I>
        struct continuous {
            static constexpr auto range(const I& i) -> decltype(std::make_pair(std::begin(i), std::end(i))) {
                return std::make_pair(std::begin(i), std::end(i));
            }
        };

    template <typename FwIt>
        struct range_output_iterator {
            static_assert(is_forward_iterator<FwIt>::value, "expecting forward iterator");

            using iterator_category = std::output_iterator_tag;
            using value_type        = typename std::iterator_traits<FwIt>::value_type;
            using difference_type   = void;
            using pointer           = void;
            using reference         = void;

            range_output_iterator& operator ++() noexcept { return *this; }
            range_output_iterator& operator  *() noexcept { return *this; }

            range_output_iterator(FwIt b, FwIt e)
            :   b_(b), e_(e)
            { CXON_ASSERT(std::distance(b_, e_) >= 0, "unexpected range"); }

            range_output_iterator& operator =(const value_type& c) {
                CXON_ASSERT(*this, "unexpected state");
                return b_ != e_ ? (*b_ = c, ++b_, *this) : (g_ = false, *this);
            }

            template <typename T = value_type>
                auto append(const T* s) -> enable_if_t<is_char<T>::value, range_output_iterator&> {
                    CXON_ASSERT(*this, "unexpected state");
                    for ( ; b_ != e_ && *s; ++s, ++b_) *b_ = *s;
                    return (g_ = *s == (T)0), *this;
                }
            auto append(const value_type* s, size_t n) -> range_output_iterator& {
                CXON_ASSERT(*this, "unexpected state");
                g_ = n <= (size_t)std::distance(b_, e_);
                n = std::min<size_t>(n, std::distance(b_, e_));
                std::copy_n(s, n, b_), std::advance(b_, n);
                return *this;
            }
            auto append(size_t n, const value_type& c) -> range_output_iterator& {
                CXON_ASSERT(*this, "unexpected state");
                g_ = n <= (size_t)std::distance(b_, e_);
                n = std::min<size_t>(n, std::distance(b_, e_));
                std::fill_n(b_, n, c), std::advance(b_, n);
                return *this;
            }

            operator bool() const noexcept { return g_; }
            operator FwIt() const noexcept { return b_; }

            private:
                FwIt        b_;
                FwIt const  e_;
                bool        g_ = true;
        };

    template <typename FwIt>
        constexpr range_output_iterator<FwIt> make_output_iterator(FwIt b, FwIt e) {
            return { b, e };
        }

}

namespace cxon { namespace napa {

    namespace bits {

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
                using in = bits::has_tag<Tg, Pk>;

            static constexpr bits::var<Tg, T> set(T&& v)        { return bits::var<Tg, T>(std::forward<T>(v)); }
            static constexpr bits::var<Tg, T> set(const T& v)   { return bits::var<Tg, T>(v); }

            template <typename Pk>
                static constexpr auto constant(T)
                    -> enable_if_t< bits::has_tag<Tg, Pk>::value, T>
                { return bits::get<Tg, Pk>(); }
            template <typename Pk>
                static constexpr auto constant(T dflt)
                    -> enable_if_t<!bits::has_tag<Tg, Pk>::value, T>
                { return dflt; }

            template <typename Pk>
                static constexpr T& reference(Pk& p)    { return bits::get<Tg>(p); }
            template <typename Pk>
                static constexpr T value(const Pk& p)   { return bits::get<Tg>(p); }

            template <typename Pk>
                static constexpr auto value(const Pk& p, T)
                    -> enable_if_t< bits::has_tag<Tg, Pk>::value, T>
                { return bits::get<Tg>(p); }
            template <typename Pk>
                static constexpr auto value(const Pk&, T dflt)
                    -> enable_if_t<!bits::has_tag<Tg, Pk>::value, T>
                { return dflt; }
        };
    template <typename Tg, typename T>
        struct parameter<Tg, T, true> : parameter<Tg, T, false> {
            using tag   = Tg;
            using type  = T;

            using parameter<Tg, T, false>::set;

            template <T c>
                static constexpr bits::cst<Tg, T, c> set() { return {}; }
        };

}}

#endif // CXON_UTILITY_HXX_
