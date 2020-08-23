// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_STRUCT_HXX_
#define CXON_JSON_STRUCT_HXX_

#include "cxon/lang/common/chio/struct.hxx"

namespace cxon { namespace json {
    namespace structs = chio::structs;
}}

namespace cxon { // structs::skip_type read

    template <typename X, typename II, typename Cx>
        inline auto read_value(json::structs::skip_type&, II& i, II e, Cx& cx) -> enable_for_t<X, JSON, bool> {
            return chio::value::read<X>(i, e, cx);
        }

}

#define CXON_JSON_STRUCT_FIELD(T, N, F)     cxon::json::structs::make_field(N, &T::F)
#define CXON_JSON_STRUCT_FIELD_NAME(N, F)   CXON_JSON_STRUCT_FIELD(T, N, F)
#define CXON_JSON_STRUCT_FIELD_ASIS(F)      CXON_JSON_STRUCT_FIELD(T, #F, F)
#define CXON_JSON_STRUCT_FIELD_SKIP(N)      cxon::json::structs::make_field(N)

#define CXON_JSON_STRUCT_READ(Type, ...)\
    namespace cxon {\
        template <typename X, typename II, typename Cx>\
            inline auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, JSON, bool> {\
                using T = Type;\
                static constexpr auto f = chio::structs::make_fields(__VA_ARGS__);\
                return chio::structs::read_fields<X>(t, f, i, e, cx);\
            }\
    }
#define CXON_JSON_STRUCT_WRITE(Type, ...)\
    namespace cxon {\
        template <typename X, typename O, typename Cx>\
            inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, JSON, bool> {\
                using T = Type;\
                static constexpr auto f = chio::structs::make_fields(__VA_ARGS__);\
                return chio::structs::write_fields<X>(o, t, f, cx);\
            }\
    }
#define CXON_STRUCT(Type, ...)\
    CXON_JSON_STRUCT_READ(Type, __VA_ARGS__)\
    CXON_JSON_STRUCT_WRITE(Type, __VA_ARGS__)

#define CXON_JSON_STRUCT_READ_MEMBER(Type, ...)\
    template <typename X, typename II, typename Cx>\
        static auto read_value(Type& t, II& i, II e, Cx& cx) -> cxon::enable_for_t<X, cxon::JSON, bool> {\
            using T = Type;\
            static constexpr auto f = cxon::chio::structs::make_fields(__VA_ARGS__);\
            return cxon::chio::structs::read_fields<X>(t, f, i, e, cx);\
        }
#define CXON_JSON_STRUCT_WRITE_MEMBER(Type, ...)\
    template <typename X, typename O, typename Cx>\
        static auto write_value(O& o, const Type& t, Cx& cx) -> cxon::enable_for_t<X, cxon::JSON, bool> {\
            using T = Type;\
            static constexpr auto f = cxon::chio::structs::make_fields(__VA_ARGS__);\
            return cxon::chio::structs::write_fields<X>(o, t, f, cx);\
        }
#define CXON_JSON_STRUCT_MEMBER(Type, ...)\
    CXON_JSON_STRUCT_READ_MEMBER(Type, __VA_ARGS__)\
    CXON_JSON_STRUCT_WRITE_MEMBER(Type, __VA_ARGS__)

#endif // CXON_JSON_STRUCT_HXX_
