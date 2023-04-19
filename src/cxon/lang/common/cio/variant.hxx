// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_VARIANT_HXX_
#define CXON_CIO_VARIANT_HXX_

#include "cio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace var { // variant type traits

    template <template <typename ...> class P, typename ...T>
        struct get_type;
    template <template <typename ...> class P, typename ...T>
        using  get_type_t = typename get_type<P, T...>::type;

    template <template <typename ...> class P, typename ...T>
        using  has_type = negation<std::is_same<get_type_t<P, T...>, void>>;

    template <typename ...T>
        struct is_disambiguous;

    template <typename T>
        struct has_disambiguous_pack;

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace var {

    template <template <typename ...> class P, typename ...T>
        struct get_type             : type_identity<void> {};
    template <template <typename ...> class P, typename H, typename ...T>
        struct get_type<P, H, T...> {
            using type = conditional_t<P<H>::value, H, typename get_type<P, T...>::type>;
        };

    namespace imp {
        template <typename U, typename V>
            using are_maps_     = conjunction<cio::is_map<U>, cio::is_map<V>>;
        template <typename U, typename V>
            using are_lists_    = conjunction<cio::is_list<U>, cio::is_list<V>>;
        template <typename U, typename V>
            using are_strings_  = conjunction<cio::is_string<U>, cio::is_string<V>>;
        template <typename U, typename V>
            using are_numbers_  = conjunction<cio::is_number<U>, cio::is_number<V>>;
        template <typename U, typename V>
            using are_bools_    = conjunction<cio::is_bool<U>, cio::is_bool<V>>;
        template <typename U, typename V>
            using are_nulls_    = conjunction<cio::is_null<U>, cio::is_null<V>>;
    }
    template <typename U, typename V>
        struct are_same : disjunction<
            imp::are_maps_<U, V>, imp::are_lists_<U, V>, imp::are_strings_<U, V>, imp::are_numbers_<U, V>, imp::are_bools_<U, V>, imp::are_nulls_<U, V>
        > {};

    template <typename T>
        using is_known = disjunction<
            cio::is_map<T>, cio::is_list<T>, cio::is_string<T>, cio::is_number<T>, cio::is_bool<T>, cio::is_null<T>
        >;

    template <typename ...>
        struct is_disambiguous                  : std::false_type {};
    template <typename T>
        struct is_disambiguous<T>               : std::true_type {};
    template <typename U, typename V>
        struct is_disambiguous<U, V>            : conjunction<is_known<U>, is_known<V>, negation<are_same<U, V>>> {};
    template <typename U, typename V, typename ...T>
        struct is_disambiguous<U, V, T...>      : conjunction<is_disambiguous<U, V>, is_disambiguous<U, T...>, is_disambiguous<V, T...>> {};

    template <typename T>
        struct has_disambiguous_pack            : std::false_type {};
    template <template <typename ...> class P, typename ...T>
        struct has_disambiguous_pack<P<T...>>   : cxon::bool_constant<cio::var::is_disambiguous<T...>::value> {};

}}}

#endif // CXON_CIO_VARIANT_HXX_
