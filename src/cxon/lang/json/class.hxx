// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_CLASS_HXX_
#define CXON_JSON_CLASS_HXX_

#include "cxon/lang/common/chio/class.hxx"

namespace cxon { namespace json {
    namespace cls = chio::cls;
}}

namespace cxon { // chio::val::skip_t read

    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(chio::val::skip_t<T>& t, II& i, II e, Cx& cx) -> enable_for_t<X, JSON, bool> {
            return chio::val::skip<X>(t, i, e, cx);
        }

}

#define CXON_JSON_CLS_FIELD(T, N, F)     cxon::json::cls::make_field(N, &T::F)
#define CXON_JSON_CLS_FIELD_NAME(N, F)   CXON_JSON_CLS_FIELD(T, N, F)
#define CXON_JSON_CLS_FIELD_ASIS(F)      CXON_JSON_CLS_FIELD(T, #F, F)
#define CXON_JSON_CLS_FIELD_SKIP(N)      cxon::json::cls::make_field(N)

#define CXON_JSON_CLS_READ(Type, ...)\
    namespace cxon {\
        template <typename X, typename II, typename Cx>\
            inline auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, JSON, bool> {\
                using T = Type;\
                static constexpr auto f = json::cls::make_fields(__VA_ARGS__);\
                return json::cls::read_fields<X>(t, f, i, e, cx);\
            }\
    }
#define CXON_JSON_CLS_WRITE(Type, ...)\
    namespace cxon {\
        template <typename X, typename O, typename Cx>\
            inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, JSON, bool> {\
                using T = Type;\
                static constexpr auto f = json::cls::make_fields(__VA_ARGS__);\
                return json::cls::write_fields<X>(o, t, f, cx);\
            }\
    }
#define CXON_JSON_CLS(Type, ...)\
    CXON_JSON_CLS_READ(Type, __VA_ARGS__)\
    CXON_JSON_CLS_WRITE(Type, __VA_ARGS__)

#define CXON_JSON_CLS_READ_MEMBER(Type, ...)\
    template <typename X, typename II, typename Cx>\
        static auto read_value(Type& t, II& i, II e, Cx& cx) -> cxon::enable_for_t<X, cxon::JSON, bool> {\
            using T = Type;\
            static constexpr auto f = cxon::json::cls::make_fields(__VA_ARGS__);\
            return cxon::json::cls::read_fields<X>(t, f, i, e, cx);\
        }
#define CXON_JSON_CLS_WRITE_MEMBER(Type, ...)\
    template <typename X, typename O, typename Cx>\
        static auto write_value(O& o, const Type& t, Cx& cx) -> cxon::enable_for_t<X, cxon::JSON, bool> {\
            using T = Type;\
            static constexpr auto f = cxon::json::cls::make_fields(__VA_ARGS__);\
            return cxon::json::cls::write_fields<X>(o, t, f, cx);\
        }
#define CXON_JSON_CLS_MEMBER(Type, ...)\
    CXON_JSON_CLS_READ_MEMBER(Type, __VA_ARGS__)\
    CXON_JSON_CLS_WRITE_MEMBER(Type, __VA_ARGS__)

#endif // CXON_JSON_CLASS_HXX_
