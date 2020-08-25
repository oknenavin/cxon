// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_COMPOUND_HXX_
#define CXON_JSON_COMPOUND_HXX_

#include "cxon/lang/common/chio/string.hxx"
#include "cxon/lang/common/chio/container.hxx"

namespace cxon { // pointer

    template <typename X, typename T>
        struct read<JSON<X>, T*> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(T*& t, II& i, II e, Cx& cx) {
                    chio::consume<J>(i, e);
                    if (chio::peek(i, e) == *J::id::nil) { // TODO: not correct as T may start with *X::id::nil (e.g. 'nan'), but it's supposed to be used in structs anyway?
                        II const o = i;
                        return  (chio::consume<J>(J::id::nil, i, e) || (chio::rewind(i, o), cx|chio::read_error::unexpected)) &&
                                (t = nullptr, true)
                        ;
                    }
                    auto ax = chio::allocator::value(cx.ps, std::allocator<T>());
                    typename std::allocator_traits<decltype(ax)>::template rebind_alloc<T> at;
                        using al = std::allocator_traits<decltype(at)>;
                    T *const n = al::allocate(at, 1); al::construct(at, n);
                        if (!read_value<J>(*n, i, e, cx))
                            return al::destroy(at, n), al::deallocate(at, n, 1), false;
                    return t = n, true;
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, T*> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const T* t, Cx& cx) {
                    return t ? write_value<J>(o, *t, cx) : chio::poke<J>(o, X::id::nil, cx);
                }
        };

}

namespace cxon { // array

    template <typename X, typename T, size_t N>
        struct read<JSON<X>, T[N]> {
            template <typename II, typename Cx, typename J = JSON<X>>
                static bool value(T (&t)[N], II& i, II e, Cx& cx) {
                    II const o = i;
                        size_t p = 0;
                    return chio::con::read_list<J>(i, e, cx, [&] {
                        return (p != N || (chio::rewind(i, o), cx|chio::read_error::overflow)) &&
                                read_value<J>(t[p++], i, e, cx)
                        ;
                    });
                }
        };

    template <typename X, typename T, size_t N>
        struct write<JSON<X>, T[N]> {
            template <typename O, typename Cx, typename J = JSON<X>>
                static bool value(O& o, const T (&t)[N], Cx& cx) {
                    return chio::con::write_list<J>(o, t, cx);
                }
        };

}

namespace cxon { // character pointer & array

#   define CXON_ARRAY(T)\
        template <typename X, size_t N>\
            struct read<JSON<X>, T[N]> {\
                template <typename II, typename Cx>\
                    static bool value(T (&t)[N], II& i, II e, Cx& cx)   { return chio::str::array<JSON<X>>::read(t, t + N, i, e, cx); }\
            };
        CXON_ARRAY(char)
        CXON_ARRAY(char16_t)
        CXON_ARRAY(char32_t)
        CXON_ARRAY(wchar_t)
#   undef CXON_ARRAY

#   define CXON_POINTER(T)\
        template <typename X>\
            struct read<JSON<X>, const T*> {\
                template <typename II, typename Cx>\
                    static bool value(const T*& t, II& i, II e, Cx& cx) { return read_value<JSON<X>>((T*&)t, i, e, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

#   define CXON_POINTER(T)\
        template <typename X>\
            struct read<JSON<X>, T*> {\
                template <typename II, typename Cx>\
                    static bool value(T*& t, II& i, II e, Cx& cx)       { return chio::str::pointer_read<JSON<X>>(t, i, e, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

#   define CXON_ARRAY(T)\
        template <typename X, size_t N>\
            struct write<JSON<X>, T[N]> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const T (&t)[N], Cx& cx)    { return chio::str::array_write<JSON<X>>(o, t, t + N, cx); }\
            };
        CXON_ARRAY(char)
        CXON_ARRAY(char16_t)
        CXON_ARRAY(char32_t)
        CXON_ARRAY(wchar_t)
#   undef CXON_ARRAY

#   define CXON_POINTER(T)\
        template <typename X>\
            struct write<JSON<X>, const T*> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const T* t, Cx& cx)         { return chio::str::pointer<JSON<X>>::write(o, t, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

#   define CXON_POINTER(T)\
        template <typename X>\
            struct write<JSON<X>, T*> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, T* t, Cx& cx)               { return write_value<JSON<X>, const T*>(o, t, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

}

#endif // CXON_JSON_COMPOUND_HXX_
