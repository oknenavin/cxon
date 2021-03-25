// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_ENUM_HXX_
#define CXON_CBOR_ENUM_HXX_

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace enm {

    template <typename E>
        struct value {
            using type = typename std::underlying_type<E>::type;
            type const alias;
            E const value;
        };
    template <typename E>
        constexpr value<E> make_value(typename value<E>::type alias, E value);

    template <typename X, typename E, typename V, typename II, typename Cx>
        inline bool read_value(E& t, V f, V l, II& i, II e, Cx& cx);

    template <typename X, typename E, typename V, typename O, typename Cx>
        inline bool write_value(O& o, E t, V f, V l, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace enm {

    template <typename E>
        constexpr value<E> make_value(typename value<E>::type alias, E value) {
            return { alias, value };
        }

    template <typename X, typename E, typename V, typename II, typename Cx>
        inline bool read_value(E& t, V f, V l, II& i, II e, Cx& cx) {
            II const o = i;
            typename value<E>::type alias;
                if (!cxon::read_value<X>(alias, i, e, cx))
                    return false;
            for ( ; f != l && f->alias != alias; ++f)
                ;
            return  (f != l && (t = f->value, true)) ||
                    (bio::rewind(i, o), cx/cbor::read_error::unexpected)
            ;
        }

    template <typename X, typename E, typename V, typename O, typename Cx>
        inline bool write_value(O& o, E t, V f, V l, Cx& cx) {
            for ( ; f != l && t != f->value; ++f)
                ;
            return f != l ?
                cxon::write_value<X>(o, f->alias, cx) :
                cx/cbor::write_error::argument_invalid
            ;
        }

}}}

#define CXON_CBOR_ENM_VALUE(T, A, V)   cxon::cbor::enm::make_value((A), T::V)
#define CXON_CBOR_ENM_VALUE_NAME(A, V) CXON_CBOR_ENM_VALUE(T, (A), V)
#define CXON_CBOR_ENM_VALUE_ASIS(V)    CXON_CBOR_ENM_VALUE(T, V, V)

#define CXON_CBOR_ENM_READ(Type, ...)\
    namespace cxon {\
        template <typename X, typename II, typename Cx>\
            inline auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {\
                using T = Type;\
                static constexpr cbor::enm::value<Type> v[] = { __VA_ARGS__ };\
                return cbor::enm::read_value<X>(t, std::begin(v), std::end(v), i, e, cx);\
            }\
    }
#define CXON_CBOR_ENM_WRITE(Type, ...)\
    namespace cxon {\
        template <typename X, typename O, typename Cx>\
            inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, CBOR> {\
                using T = Type;\
                static constexpr cbor::enm::value<Type> v[] = { __VA_ARGS__ };\
                return cbor::enm::write_value<X>(o, t, std::begin(v), std::end(v), cx);\
            }\
    }
#define CXON_CBOR_ENM(Type, ...)\
    CXON_CBOR_ENM_READ(Type, __VA_ARGS__)\
    CXON_CBOR_ENM_WRITE(Type, __VA_ARGS__)

#endif // CXON_CBOR_ENUM_HXX_
