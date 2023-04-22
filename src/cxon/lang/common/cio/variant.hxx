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
        struct is_ambiguous;

    template <typename T>
        struct has_ambiguous_types;

    template <typename X, typename V, typename II, typename Cx>
        inline bool variant_read(V& v, II& i, II e, Cx& cx);

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
        struct is_ambiguous                 : std::true_type {};
    template <typename T>
        struct is_ambiguous<T>              : std::false_type {};
    template <typename U, typename V>
        struct is_ambiguous<U, V>           : disjunction<negation<is_known<U>>, negation<is_known<V>>, are_same<U, V>> {};
    template <typename U, typename V, typename ...T>
        struct is_ambiguous<U, V, T...>     : disjunction<is_ambiguous<U, V>, is_ambiguous<U, T...>, is_ambiguous<V, T...>> {};

    template <typename T>
        struct has_ambiguous_types          : std::true_type {};
    template <template <typename ...> class P, typename ...T>
        struct has_ambiguous_types<P<T...>> : cxon::bool_constant<cio::var::is_ambiguous<T...>::value> {};

    namespace imp {
        template <typename X, template <typename, typename> class S, bool E, typename V, typename II, typename Cx>
            struct variant_ {
                static constexpr bool read(V&, II&, II, Cx& cx) {
                    return cx/json::read_error::unexpected;
                }
            };
        template <typename X, template <typename, typename> class S, template <typename ...> class V, typename II, typename Cx, typename ...T>
            struct variant_<X, S, true, V<T...>, II, Cx> {
                static bool read(V<T...>& v, II& i, II e, Cx& cx) {
                    get_type_t<S, T...> t;
                    return read_value<X>(t, i, e, cx) && (v = std::move(t), true);
                }
            };
        template <typename X, template <typename, typename> class S, template <typename ...> class V, typename II, typename Cx, typename ...T>
            inline bool variant_read_(V<T...>& v, II& i, II e, Cx& cx) {
                return variant_<X, S, has_type<S, T...>::value, V<T...>, II, Cx>::read(v, i, e, cx);
            }
    }
    template <typename X, typename V, typename II, typename Cx>
        inline bool variant_read(V& v, II& i, II e, Cx& cx) {
            if (!consume<X>(i, e, cx))
                return false;
            switch (peek(i, e)) {
                case X::map::beg:
                    return imp::variant_read_<X, is_map>(v, i, e, cx);
                case X::list::beg:
                    return imp::variant_read_<X, is_list>(v, i, e, cx);
                case X::string::del:
                    return imp::variant_read_<X, is_string>(v, i, e, cx);
                case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case 'I': case 'N':
                    return imp::variant_read_<X, is_number>(v, i, e, cx);
                case *X::id::pos: case *X::id::neg:
                    return imp::variant_read_<X, is_bool>(v, i, e, cx);
                case *X::id::nil:
                    return imp::variant_read_<X, is_null>(v, i, e, cx);
            }
            return cx/json::read_error::unexpected;
        }

}}}

#endif // CXON_CIO_VARIANT_HXX_
