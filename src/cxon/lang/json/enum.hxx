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

#ifndef CXON_JSON_ENM_HXX_
#define CXON_JSON_ENM_HXX_

#include "cxon/lang/common/cio/enum.hxx"

namespace cxon { namespace json {
    namespace enm = cio::enm;
}}

#define CXON_JSON_ENM_VALUE(T, N, V)   cxon::json::enm::make_value(N, T::V)
#define CXON_JSON_ENM_VALUE_NAME(N, V) CXON_JSON_ENM_VALUE(CXON_T_, N, V)
#define CXON_JSON_ENM_VALUE_ASIS(V)    CXON_JSON_ENM_VALUE(CXON_T_, #V, V)

#define CXON_JSON_ENM_READ(Type, ...)\
    namespace cxon {\
        template <typename X, typename II, typename Cx>\
            inline auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, JSON> {\
                using CXON_T_ = Type;\
                static constexpr json::enm::value<Type> v[] = { __VA_ARGS__ };\
                return json::enm::read_value<X>(t, std::begin(v), std::end(v), i, e, cx);\
            }\
    }
#define CXON_JSON_ENM_WRITE(Type, ...)\
    namespace cxon {\
        template <typename X, typename O, typename Cx>\
            inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, JSON> {\
                using CXON_T_ = Type;\
                static constexpr json::enm::value<Type> v[] = { __VA_ARGS__ };\
                return json::enm::write_value<X>(o, t, std::begin(v), std::end(v), cx);\
            }\
    }
#define CXON_JSON_ENM(Type, ...)\
    CXON_JSON_ENM_READ(Type, __VA_ARGS__)\
    CXON_JSON_ENM_WRITE(Type, __VA_ARGS__)

#endif // CXON_JSON_ENM_HXX_
