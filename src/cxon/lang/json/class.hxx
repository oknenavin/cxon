// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_CLASS_HXX_
#define CXON_JSON_CLASS_HXX_

#include "cxon/lang/common/cio/class.hxx"

namespace cxon { namespace json {
    namespace cls = cio::cls;
}}

namespace cxon { // cio::val::sink read

    template <typename X, typename T>
        struct read<JSON<X>, cio::val::sink<T>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(cio::val::sink<T>& s, II& i, II e, Cx& cx) {
                    return cio::val::sink_read<Y>(s, i, e, cx);
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, cio::val::sink<T>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const cio::val::sink<T>& s, Cx& cx) {
                    return cio::val::sink_write<Y>(o, s, cx);
                }
        };

}

#define CXON_JSON_CLS_FIELD(T, N, F)                cxon::json::cls::make_field<T>(N, &T::F)
#define CXON_JSON_CLS_FIELD_DFLT(T, N, F, ...)      cxon::json::cls::make_field<T>(N, &T::F, __VA_ARGS__)
#define CXON_JSON_CLS_FIELD_NAME(N, F)              CXON_JSON_CLS_FIELD(T, N, F)
#define CXON_JSON_CLS_FIELD_NAME_DFLT(N, F, ...)    CXON_JSON_CLS_FIELD_DFLT(T, N, F, __VA_ARGS__)
#define CXON_JSON_CLS_FIELD_ASIS(F)                 CXON_JSON_CLS_FIELD(T, #F, F)
#define CXON_JSON_CLS_FIELD_ASIS_DFLT(F, ...)       CXON_JSON_CLS_FIELD_DFLT(T, #F, F, __VA_ARGS__)
#define CXON_JSON_CLS_FIELD_SKIP(N)                 cxon::json::cls::make_field<T>(N, {})

#define CXON_JSON_CLS_READ(Type, ...)\
    namespace cxon {\
        template <typename X, typename II, typename Cx>\
            inline auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, JSON> {\
                using T = Type;\
                static auto const f = json::cls::make_fields(__VA_ARGS__);\
                return json::cls::read_fields<X>(t, f, i, e, cx);\
            }\
    }
#define CXON_JSON_CLS_WRITE(Type, ...)\
    namespace cxon {\
        template <typename X, typename O, typename Cx>\
            inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, JSON> {\
                using T = Type;\
                static auto const f = json::cls::make_fields(__VA_ARGS__);\
                return json::cls::write_fields<X>(o, t, f, cx);\
            }\
    }
#define CXON_JSON_CLS(Type, ...)\
    CXON_JSON_CLS_READ(Type, __VA_ARGS__)\
    CXON_JSON_CLS_WRITE(Type, __VA_ARGS__)

#define CXON_JSON_CLS_READ_MEMBER(Type, ...)\
    template <typename X, typename II, typename Cx>\
        static auto read_value(Type& t, II& i, II e, Cx& cx) -> cxon::enable_for_t<X, cxon::JSON> {\
            using T = Type;\
            static auto const f = cxon::json::cls::make_fields(__VA_ARGS__);\
            return cxon::json::cls::read_fields<X>(t, f, i, e, cx);\
        }
#define CXON_JSON_CLS_WRITE_MEMBER(Type, ...)\
    template <typename X, typename O, typename Cx>\
        static auto write_value(O& o, const Type& t, Cx& cx) -> cxon::enable_for_t<X, cxon::JSON> {\
            using T = Type;\
            static auto const f = cxon::json::cls::make_fields(__VA_ARGS__);\
            return cxon::json::cls::write_fields<X>(o, t, f, cx);\
        }
#define CXON_JSON_CLS_MEMBER(Type, ...)\
    CXON_JSON_CLS_READ_MEMBER(Type, __VA_ARGS__)\
    CXON_JSON_CLS_WRITE_MEMBER(Type, __VA_ARGS__)

#endif // CXON_JSON_CLASS_HXX_
