// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_COMPOUND_HXX_
#define CXON_COMPOUND_HXX_

#include "cxon/lang/common/container.hxx"

namespace cxon { // pointer

    template <typename X, typename T>
        struct read<X, T*> {
            template <typename II, typename Cx>
                static bool value(T*& t, II& i, II e, Cx& cx) {
                    io::consume<X>(i, e);
                    if (io::peek(i, e) == *X::id::nil) { // TODO: not correct as T may start with *X::id::nil (e.g. 'nan'), but it's supposed to be used in structs anyway?
                        II const o = i;
                        return  (io::consume<X>(X::id::nil, i, e) || (io::rewind(i, o), cx|read_error::unexpected)) &&
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
                    return t ? write_value<X>(o, *t, cx) : io::poke<X>(o, X::id::nil, cx);
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
                    return chario::container::read<X, list<X>>(i, e, cx, [&] {
                        return (p != N || (io::rewind(i, o), cx|read_error::overflow)) &&
                                read_value<X>(t[p++], i, e, cx)
                        ;
                    });
                }
        };

    template <typename X, typename T, size_t N>
        struct write<X, T[N]> {
            template <typename O, typename Cx>
                static bool value(O& o, const T (&t)[N], Cx& cx) {
                    return chario::container::write<X, list<X>>(o, t, cx);
                }
        };

}   // cxon array

namespace cxon { // character pointer & array

#   define CXON_ARRAY(T)\
        template <typename X, size_t N>\
            struct read<X, T[N]> {\
                template <typename II, typename Cx>\
                    static bool value(T (&t)[N], II& i, II e, Cx& cx)   { return bits::array<X>::read(t, t + N, i, e, cx); }\
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
                    static bool value(T*& t, II& i, II e, Cx& cx)       { return bits::pointer_read<X>(t, i, e, cx); }\
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
                    static bool value(O& o, const T (&t)[N], Cx& cx)    { return bits::array_write<X>(o, t, t + N, cx); }\
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
                    static bool value(O& o, const T* t, Cx& cx)         { return bits::pointer<X>::write(o, t, cx); }\
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

#if 1 // cxon enumeration

#   define CXON_ENUM_VALUE(T, N, V)     enums::make_value(N, T::V)
#   define CXON_ENUM_VALUE_NAME(N, V)   CXON_ENUM_VALUE(T, N, V)
#   define CXON_ENUM_VALUE_ASIS(V)      CXON_ENUM_VALUE(T, #V, V)

#   define CXON_ENUM_READ(Type, ...)\
        namespace cxon {\
            template <typename X, typename II, typename Cx>\
                inline bool read_value(Type& t, II& i, II e, Cx& cx) {\
                    using T = Type;\
                    static constexpr enums::value<Type> v[] = { __VA_ARGS__ };\
                    return enums::read_value<X>(t, std::begin(v), std::end(v), i, e, cx);\
                }\
        }
#   define CXON_ENUM_WRITE(Type, ...)\
        namespace cxon {\
            template <typename X, typename O, typename Cx>\
                inline bool write_value(O& o, const Type& t, Cx& cx) {\
                    using T = Type;\
                    static constexpr enums::value<Type> v[] = { __VA_ARGS__ };\
                    return enums::write_value<X>(o, t, std::begin(v), std::end(v), cx);\
                }\
        }
#   define CXON_ENUM(Type, ...)\
        CXON_ENUM_READ(Type, __VA_ARGS__)\
        CXON_ENUM_WRITE(Type, __VA_ARGS__)

#endif // enumeration

#if 1 // cxon class

#   define CXON_STRUCT_FIELD(T, N, F)   cxon::structs::make_field(N, &T::F)
#   define CXON_STRUCT_FIELD_NAME(N, F) CXON_STRUCT_FIELD(T, N, F)
#   define CXON_STRUCT_FIELD_ASIS(F)    CXON_STRUCT_FIELD(T, #F, F)
#   define CXON_STRUCT_FIELD_SKIP(N)    cxon::structs::make_field(N)

#   define CXON_STRUCT_READ(Type, ...)\
        namespace cxon {\
            template <typename X, typename II, typename Cx>\
                inline bool read_value(Type& t, II& i, II e, Cx& cx) {\
                    using T = Type;\
                    static constexpr auto f = structs::make_fields(__VA_ARGS__);\
                    return structs::read_fields<X>(t, f, i, e, cx);\
                }\
        }
#   define CXON_STRUCT_WRITE(Type, ...)\
        namespace cxon {\
            template <typename X, typename O, typename Cx>\
                static bool write_value(O& o, const Type& t, Cx& cx) {\
                    using T = Type;\
                    static constexpr auto f = structs::make_fields(__VA_ARGS__);\
                    return structs::write_fields<X>(o, t, f, cx);\
                }\
        }
#   define CXON_STRUCT(Type, ...)\
        CXON_STRUCT_READ(Type, __VA_ARGS__)\
        CXON_STRUCT_WRITE(Type, __VA_ARGS__)

#   define CXON_STRUCT_READ_MEMBER(Type, ...)\
        template <typename X, typename II, typename Cx>\
            static bool read_value(Type& t, II& i, II e, Cx& cx) {\
                using T = Type;\
                static constexpr auto f = cxon::structs::make_fields(__VA_ARGS__);\
                return cxon::structs::read_fields<X>(t, f, i, e, cx);\
            }
#   define CXON_STRUCT_WRITE_MEMBER(Type, ...)\
        template <typename X, typename O, typename Cx>\
            static bool write_value(O& o, const Type& t, Cx& cx) {\
                using T = Type;\
                static constexpr auto f = cxon::structs::make_fields(__VA_ARGS__);\
                return cxon::structs::write_fields<X>(o, t, f, cx);\
            }
#   define CXON_STRUCT_MEMBER(Type, ...)\
        CXON_STRUCT_READ_MEMBER(Type, __VA_ARGS__)\
        CXON_STRUCT_WRITE_MEMBER(Type, __VA_ARGS__)

#endif // cxon class

#endif // CXON_COMPOUND_HXX_
