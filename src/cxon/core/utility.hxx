// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CORE_UTILITY_HXX_
#define CXON_CORE_UTILITY_HXX_

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

namespace cxon {

    template <bool C, typename T = void> // C++14
        using enable_if_t = typename std::enable_if<C, T>::type;

    template <typename ...>
        constexpr bool unexpected() { return false; }

    template <std::size_t N>
        struct option : option<N - 1>   {};
    template<>
        struct option<0>                {};

    // iterators

    template <typename I>
        using iterator_category_t = typename std::iterator_traits<I>::iterator_category;

    template <typename, typename = void>
        struct is_output_iterator : std::false_type {};
    template <typename I>
        struct is_output_iterator<I, enable_if_t<std::is_same<iterator_category_t<I>, std::output_iterator_tag>::value>> : std::true_type {};

    template <typename, typename = void>
        struct is_forward_iterator : std::false_type {};
    template <typename I>
        struct is_forward_iterator<I, enable_if_t<!std::is_same<iterator_category_t<I>, std::input_iterator_tag>::value>> : std::true_type {};

    template <typename, typename = void>
        struct is_back_insertable : std::false_type {};
    template <typename C>
        struct is_back_insertable<C, decltype(C().push_back(' '))> : std::true_type {};

    template <typename I>
        struct continuous {
            static auto range(const I& i) -> decltype(std::make_pair(std::begin(i), std::end(i))) {
                return std::make_pair(std::begin(i), std::end(i));
            }
        };

    template <typename FwIt>
        struct range_output_iterator {
            using iterator_category = std::output_iterator_tag;
            using value_type        = char;
            using difference_type   = void;
            using pointer           = void;
            using reference         = void;

            range_output_iterator& operator ++() noexcept { return *this; }
            range_output_iterator& operator *() noexcept  { return *this; }

            constexpr range_output_iterator(FwIt b, FwIt e)
            :   b_(b), e_(e)
            {
#               if __cplusplus >= 201402L
                    CXON_ASSERT(std::distance(b_, e_) >= 0, "unexpected range");
#               endif
            }

            range_output_iterator& operator =(char c) {
                CXON_ASSERT(*this, "unexpected state");
                *b_ = c, ++b_;
                return *this;
            }
            void append(const char* s) {
                CXON_ASSERT(*this, "unexpected state");
                for ( ; b_ != e_ && *s; ++s, ++b_) *b_ = *s;
            }
            void append(const char* s, size_t n) {
                CXON_ASSERT(*this, "unexpected state");
                n = std::min<size_t>(n, e_ - b_);
                std::copy_n(s, n, b_), std::advance(b_, n);
            }
            void append(size_t n, char c) {
                CXON_ASSERT(*this, "unexpected state");
                n = std::min<size_t>(n, e_ - b_);
                std::fill_n(b_, n, c), std::advance(b_, n);
            }

            operator bool() const noexcept { return b_ != e_; }
            operator FwIt() const noexcept { return b_; }

        private:
            FwIt        b_;
            FwIt const  e_;
        };

    template <typename FwIt>
        constexpr range_output_iterator<FwIt> make_output_iterator(FwIt b, FwIt e) {
            return range_output_iterator<FwIt>(b, e);
        }

}

namespace cxon { namespace prms {

    template <typename ...Tg>
        using pack = std::tuple<Tg...>;

    template <typename ...Tg>
        inline constexpr pack<Tg...> make_pack(Tg&&... t) {
            return std::make_tuple(std::forward<Tg>(t)...);
        }

    template <typename Pk, std::size_t Ix>
        using pack_tag_t = typename std::tuple_element<Ix, Pk>::type::tag;
    template <typename Tg, typename Pk, std::size_t Ix>
        using is_same_tag = std::is_same<Tg, pack_tag_t<Pk, Ix>>;

    template <typename Tg>
        using tag_type_t = typename Tg::tag_type;

    template <std::size_t Ix, typename T>
        using tuple_element_t = typename std::tuple_element<Ix, T>::type;

    template <typename Tg, typename Pk, std::size_t Sz = std::tuple_size<Pk>::value>
        struct pack_has_tag             { static constexpr bool value = !is_same_tag<Tg, Pk, Sz - 1>::value ? pack_has_tag<Tg, Pk, Sz - 1>::value : true; };
    template <typename Tg, typename Pk>
        struct pack_has_tag<Tg, Pk, 1>  { static constexpr bool value = is_same_tag<Tg, Pk, 0>::value; };
    template <typename Tg, typename Pk>
        struct pack_has_tag<Tg, Pk, 0>  { static constexpr bool value = false; };

    template <typename Tg, typename Pk, std::size_t Sz = std::tuple_size<Pk>::value>
        struct pack_ {
            static constexpr tag_type_t<Tg>& get(Pk& p) { return get_(p); }
            static constexpr tag_type_t<Tg>  get()      { return get_(); }

            private:
                template <typename X = Tg>
                    static constexpr auto get_(Pk& p) -> enable_if_t< is_same_tag<X, Pk, Sz - 1>::value, tag_type_t<X>&> {
                        return std::get<Sz - 1>(p).value;
                    }
                template <typename X = Tg>
                    static constexpr auto get_(Pk& p) -> enable_if_t<!is_same_tag<X, Pk, Sz - 1>::value, tag_type_t<X>&> {
                        return pack_<X, Pk, Sz - 1>::get(p);
                    }
                template <typename X = Tg>
                    static constexpr auto get_() -> enable_if_t< is_same_tag<X, Pk, Sz - 1>::value, tag_type_t<X>> {
                        return tuple_element_t<Sz - 1, Pk>::value;
                    }
                template <typename X = Tg>
                    static constexpr auto get_() -> enable_if_t<!is_same_tag<X, Pk, Sz - 1>::value, tag_type_t<X>> {
                        return pack_<X, Pk, Sz - 1>::get();
                    }
        };
    template <typename Tg, typename Pk>
        struct pack_<Tg, Pk, 1> {
            static_assert(is_same_tag<Tg, Pk, 0>::value, "no such tag");

            static constexpr tag_type_t<Tg>& get(Pk& p) { return std::get<0>(p).value; }
            static constexpr tag_type_t<Tg>  get()      { return tuple_element_t<0, Pk>::value; }
        };
    template <typename Tg, typename Pk>
        struct pack_<Tg, Pk, 0> {
            static_assert(unexpected<Tg>(), "no such tag");

            static void get(Pk&)    {}
            static void get()       {}
        };
      
    template <typename Tg, typename Pk>
        inline constexpr auto pack_get(Pk& p) -> tag_type_t<Tg>& {
            return pack_<Tg, Pk>::get(p);
        }
    template <typename Tg, typename Pk>
        inline constexpr auto pack_get(const Pk& p) -> const tag_type_t<Tg>& {
            return pack_<Tg, Pk>::get(const_cast<Pk&>(p));
        }

    template <typename Tg, typename Pk>
        inline constexpr auto pack_get() -> tag_type_t<Tg> {
            return pack_<Tg, Pk>::get();
        }


    template <typename Tg, typename Tp, Tp v>
        struct cst {
            using tag  = Tg;
            using tag_type  = Tp;

            static constexpr tag_type value = v;
        };

    template <typename Tg, typename Tp>
        struct var {
            using tag  = Tg;
            using tag_type  = Tp;

            tag_type value;

            var()                   : value() {}
            var(tag_type&& v)       : value(std::move(v)) {}
            var(const tag_type& v)  : value(v) {}
        };

    template <typename Tg, typename Tp, bool E = std::is_scalar<Tp>::value>
        struct parameter {
            using tag = Tg;
            using tag_type = Tp;

            template <typename Pk>
                using in = pack_has_tag<Tg, Pk>;

            static constexpr var<Tg, Tp> set(Tp&& v)        { return var<Tg, Tp>(std::forward<Tp>(v)); }
            static constexpr var<Tg, Tp> set(const Tp& v)   { return var<Tg, Tp>(v); }

            template <typename Pk>
                static constexpr auto constant(Tp)
                    -> enable_if_t< pack_has_tag<Tg, Pk>::value, Tp>
                { return pack_get<Tg, Pk>(); }
            template <typename Pk>
                static constexpr auto constant(Tp dflt)
                    -> enable_if_t<!pack_has_tag<Tg, Pk>::value, Tp>
                { return dflt; }

            template <typename Pk>
                static constexpr Tp& reference(Pk& p)   { return pack_get<Tg>(p); }
            template <typename Pk>
                static constexpr Tp value(const Pk& p)  { return pack_get<Tg>(p); }

            template <typename Pk>
                static constexpr auto value(const Pk& p, Tp)
                    -> enable_if_t< pack_has_tag<Tg, Pk>::value, Tp>
                { return pack_get<Tg>(p); }
            template <typename Pk>
                static constexpr auto value(const Pk&, Tp dflt)
                    -> enable_if_t<!pack_has_tag<Tg, Pk>::value, Tp>
                { return dflt; }
        };
    template <typename Tg, typename Tp>
        struct parameter<Tg, Tp, true> : parameter<Tg, Tp, false> {
            using tag = Tg;
            using tag_type = Tp;

            using parameter<Tg, Tp, false>::set;

            template <Tp c>
                static constexpr cst<Tg, Tp, c> set() { return {}; }
        };

#   define CXON_PARAMETER(P, T) struct P : cxon::prms::parameter<P, T> {}

}}

#endif // CXON_CORE_UTILITY_HXX_
