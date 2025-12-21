// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

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
#define CXON_CBOR_ENM_VALUE_NAME(A, V) CXON_CBOR_ENM_VALUE(CXON_T_, (A), V)
#define CXON_CBOR_ENM_VALUE_ASIS(V)    CXON_CBOR_ENM_VALUE(CXON_T_, V, V)

#define CXON_CBOR_ENM_READ(Type, ...)\
    namespace cxon {\
        template <typename X, typename II, typename Cx>\
            inline auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {\
                using CXON_T_ = Type;\
                static constexpr cbor::enm::value<Type> v[] = { __VA_ARGS__ };\
                return cbor::enm::read_value<X>(t, std::begin(v), std::end(v), i, e, cx);\
            }\
    }
#define CXON_CBOR_ENM_WRITE(Type, ...)\
    namespace cxon {\
        template <typename X, typename O, typename Cx>\
            inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, CBOR> {\
                using CXON_T_ = Type;\
                static constexpr cbor::enm::value<Type> v[] = { __VA_ARGS__ };\
                return cbor::enm::write_value<X>(o, t, std::begin(v), std::end(v), cx);\
            }\
    }
#define CXON_CBOR_ENM(Type, ...)\
    CXON_CBOR_ENM_READ(Type, __VA_ARGS__)\
    CXON_CBOR_ENM_WRITE(Type, __VA_ARGS__)

#endif // CXON_CBOR_ENUM_HXX_
