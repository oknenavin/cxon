// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CORE_UTILITY_HXX_
#define CXON_CORE_UTILITY_HXX_

#include <tuple>

namespace cxon {

    template <bool C, typename T = void> // C++14
        using enable_if_t = typename std::enable_if<C, T>::type;

    template <typename ...>
        constexpr bool unexpected() { return false; }

    template <std::size_t N>
        struct option : option<N - 1>   {};
    template<>
        struct option<0>                {};

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
