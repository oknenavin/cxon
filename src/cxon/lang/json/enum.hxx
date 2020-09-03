// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_ENM_HXX_
#define CXON_JSON_ENM_HXX_

#include "cxon/lang/common/chio/enum.hxx"

namespace cxon { namespace json {
    namespace enm = chio::enm;
}}

#define CXON_JSON_ENM_VALUE(T, N, V)   cxon::json::enm::make_value(N, T::V)
#define CXON_JSON_ENM_VALUE_NAME(N, V) CXON_JSON_ENM_VALUE(T, N, V)
#define CXON_JSON_ENM_VALUE_ASIS(V)    CXON_JSON_ENM_VALUE(T, #V, V)

#define CXON_JSON_ENM_READ(Type, ...)\
    namespace cxon {\
        template <typename X, typename II, typename Cx>\
            inline auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, JSON> {\
                using T = Type;\
                static constexpr json::enm::value<Type> v[] = { __VA_ARGS__ };\
                return json::enm::read_value<X>(t, std::begin(v), std::end(v), i, e, cx);\
            }\
    }
#define CXON_JSON_ENM_WRITE(Type, ...)\
    namespace cxon {\
        template <typename X, typename O, typename Cx>\
            inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, JSON> {\
                using T = Type;\
                static constexpr json::enm::value<Type> v[] = { __VA_ARGS__ };\
                return json::enm::write_value<X>(o, t, std::begin(v), std::end(v), cx);\
            }\
    }
#define CXON_JSON_ENM(Type, ...)\
    CXON_JSON_ENM_READ(Type, __VA_ARGS__)\
    CXON_JSON_ENM_WRITE(Type, __VA_ARGS__)

#endif // CXON_JSON_ENM_HXX_
