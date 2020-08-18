// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_COMPOUND_HXX_
#define CXON_COMPOUND_HXX_

#include "cxon/lang/common/chario/container.hxx"

namespace cxon { // pointer

    template <typename X, typename T>
        struct read<X, T*> {
            template <typename II, typename Cx>
                static bool value(T*& t, II& i, II e, Cx& cx) {
                    chio::consume<X>(i, e);
                    if (chio::peek(i, e) == *X::id::nil) { // TODO: not correct as T may start with *X::id::nil (e.g. 'nan'), but it's supposed to be used in structs anyway?
                        II const o = i;
                        return  (chio::consume<X>(X::id::nil, i, e) || (chio::rewind(i, o), cx|read_error::unexpected)) &&
                                (t = nullptr, true)
                        ;
                    }
                    auto ax = allocator::value(cx.ps, std::allocator<T>());
                    typename std::allocator_traits<decltype(ax)>::template rebind_alloc<T> at;
                        using al = std::allocator_traits<decltype(at)>;
                    T *const n = al::allocate(at, 1); al::construct(at, n);
                        if (!read_value<X>(*n, i, e, cx))
                            return al::destroy(at, n), al::deallocate(at, n, 1), false;
                    return t = n, true;
                }
        };

    template <typename X, typename T>
        struct write<X, T*> {
            template <typename O, typename Cx>
                static bool value(O& o, const T* t, Cx& cx) {
                    return t ? write_value<X>(o, *t, cx) : chio::poke<X>(o, X::id::nil, cx);
                }
        };

}   // cxon pointer

namespace cxon { // array

    template <typename X, typename T, size_t N>
        struct read<X, T[N]> {
            template <typename II, typename Cx>
                static bool value(T (&t)[N], II& i, II e, Cx& cx) {
                    II const o = i;
                        size_t p = 0;
                    return chio::container::read<X, list<X>>(i, e, cx, [&] {
                        return (p != N || (chio::rewind(i, o), cx|read_error::overflow)) &&
                                read_value<X>(t[p++], i, e, cx)
                        ;
                    });
                }
        };

    template <typename X, typename T, size_t N>
        struct write<X, T[N]> {
            template <typename O, typename Cx>
                static bool value(O& o, const T (&t)[N], Cx& cx) {
                    return chio::container::write<X, list<X>>(o, t, cx);
                }
        };

}   // cxon array

namespace cxon { // character pointer & array

#   define CXON_ARRAY(T)\
        template <typename X, size_t N>\
            struct read<X, T[N]> {\
                template <typename II, typename Cx>\
                    static bool value(T (&t)[N], II& i, II e, Cx& cx)   { return strs::array<X>::read(t, t + N, i, e, cx); }\
            };
        CXON_ARRAY(char)
        CXON_ARRAY(char16_t)
        CXON_ARRAY(char32_t)
        CXON_ARRAY(wchar_t)
#   undef CXON_ARRAY

#   define CXON_POINTER(T)\
        template <typename X>\
            struct read<X, const T*> {\
                template <typename II, typename Cx>\
                    static bool value(const T*& t, II& i, II e, Cx& cx) { return read_value<X>((T*&)t, i, e, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

#   define CXON_POINTER(T)\
        template <typename X>\
            struct read<X, T*> {\
                template <typename II, typename Cx>\
                    static bool value(T*& t, II& i, II e, Cx& cx)       { return strs::pointer_read<X>(t, i, e, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

#   define CXON_ARRAY(T)\
        template <typename X, size_t N>\
            struct write<X, T[N]> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const T (&t)[N], Cx& cx)    { return strs::array_write<X>(o, t, t + N, cx); }\
            };
        CXON_ARRAY(char)
        CXON_ARRAY(char16_t)
        CXON_ARRAY(char32_t)
        CXON_ARRAY(wchar_t)
#   undef CXON_ARRAY

#   define CXON_POINTER(T)\
        template <typename X>\
            struct write<X, const T*> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, const T* t, Cx& cx)         { return strs::pointer<X>::write(o, t, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

#   define CXON_POINTER(T)\
        template <typename X>\
            struct write<X, T*> {\
                template <typename O, typename Cx>\
                    static bool value(O& o, T* t, Cx& cx)               { return write_value<X, const T*>(o, t, cx); }\
            };
        CXON_POINTER(char)
        CXON_POINTER(char16_t)
        CXON_POINTER(char32_t)
        CXON_POINTER(wchar_t)
#   undef CXON_POINTER

}   // cxon character pointer & array

#endif // CXON_COMPOUND_HXX_
