// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_COMPOUND_HXX_
#define CXON_JSON_COMPOUND_HXX_

#include "cxon/lang/common/cio/string.hxx"
#include "cxon/lang/common/cio/container.hxx"
#include "cxon/lang/common/allocator.hxx"

namespace cxon { namespace cio { // type traits
    template <typename T, std::size_t N>    struct is_string<T[N]>  : is_char<T> {};
    template <typename T>                   struct is_string<T*>    : is_char<T> {};
    template <typename T, std::size_t N>    struct is_list<T[N]>    : std::true_type {};
}}

namespace cxon { // pointer

    template <typename X, typename T>
        struct read<JSON<X>, T*> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(T*& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<Y>(i, e, cx))
                        return false;
                    if (cio::peek(i, e) == *Y::id::nil) { // TODO: needs buffering
                        II const o = i;
                        return  (cio::consume<Y>(Y::id::nil, i, e) && (t = nullptr, true)) ||
                                (cio::rewind(i, o), cx/json::read_error::unexpected)
                        ;
                    }
                    auto al = alc::make_context_allocator<T>(cx);
                    T *const n = al.create();
                        if (!read_value<Y>(*n, i, e, cx))
                            return al.release(n), false;
                    return t = n, true;
                }
        };
    template <typename X, typename T>
        struct read<JSON<X>, const T*> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(const T*& t, II& i, II e, Cx& cx) {
                    return read<Y, T*>::value((T*&)t, i, e, cx);
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, T*> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const T* t, Cx& cx) {
                    return t ? write_value<Y>(o, *t, cx) : cio::poke<Y>(o, Y::id::nil, cx);
                }
        };

}

namespace cxon { // array

    template <typename X, typename T, std::size_t N>
        struct read<JSON<X>, T[N]> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(T (&t)[N], II& i, II e, Cx& cx) {
                    II const o = i;
                        std::size_t p = 0;
                    return cio::cnt::read_list<Y>(i, e, cx, [&] {
                        return (p != N || (cio::rewind(i, o), cx/json::read_error::overflow)) &&
                                read_value<Y>(t[p++], i, e, cx)
                        ;
                    });
                }
        };

    template <typename X, typename T, std::size_t N>
        struct write<JSON<X>, T[N]> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const T (&t)[N], Cx& cx) {
                    return cio::cnt::write_list<Y>(o, t, cx);
                }
        };

}

namespace cxon { // character pointer & array

#   define CXON_ARRAY(T)\
        template <typename X, std::size_t N>\
            struct read<JSON<X>, T[N]> {\
                template <typename II, typename Cx>\
                    static bool value(T (&t)[N], II& i, II e, Cx& cx)   { return cio::str::array_read<JSON<X>>(t, t + N, i, e, cx); }\
            };
        CXON_ARRAY(char)
        CXON_ARRAY(wchar_t)
#       if defined(__cpp_char8_t)
            CXON_ARRAY(char8_t)
#       endif
        CXON_ARRAY(char16_t)
        CXON_ARRAY(char32_t)
#   undef CXON_ARRAY

#   define CXON_POINTER(T)\
        template <typename X>\
            struct read<JSON<X>, const T*> {\
                template <typename II, typename Cx>\
                    static bool value(const T*& t, II& i, II e, Cx& cx) { return read_value<JSON<X>>((T*&)t, i, e, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(wchar_t)
#       if defined(__cpp_char8_t)
            CXON_POINTER(char8_t)
#       endif
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
#   undef CXON_POINTER

#   define CXON_POINTER(T)\
        template <typename X>\
            struct read<JSON<X>, T*> {\
                template <typename II, typename Cx>\
                    static bool value(T*& t, II& i, II e, Cx& cx)       { return cio::str::pointer_read<JSON<X>>(t, i, e, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(wchar_t)
#       if defined(__cpp_char8_t)
            CXON_POINTER(char8_t)
#       endif
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
#   undef CXON_POINTER

#   define CXON_ARRAY(T)\
        template <typename X, std::size_t N>\
            struct write<JSON<X>, T[N]> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const T (&t)[N], Cx& cx)    { return cio::str::array_write<JSON<X>>(o, t, t + N, cx); }\
            };
        CXON_ARRAY(char)
        CXON_ARRAY(wchar_t)
#       if defined(__cpp_char8_t)
            CXON_ARRAY(char8_t)
#       endif
        CXON_ARRAY(char16_t)
        CXON_ARRAY(char32_t)
#   undef CXON_ARRAY

#   define CXON_POINTER(T)\
        template <typename X>\
            struct write<JSON<X>, const T*> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const T* t, Cx& cx)         { return cio::str::pointer_write<JSON<X>>(o, t, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(wchar_t)
#       if defined(__cpp_char8_t)
            CXON_POINTER(char8_t)
#       endif
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
#   undef CXON_POINTER

#   define CXON_POINTER(T)\
        template <typename X>\
            struct write<JSON<X>, T*> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, T* t, Cx& cx)               { return write_value<JSON<X>, const T*>(o, t, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(wchar_t)
#       if defined(__cpp_char8_t)
            CXON_POINTER(char8_t)
#       endif
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
#   undef CXON_POINTER

}

#endif // CXON_JSON_COMPOUND_HXX_
