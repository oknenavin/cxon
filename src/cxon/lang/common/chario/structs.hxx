// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHIO_STRUCTS_HXX_
#define CXON_CHIO_STRUCTS_HXX_

#include "chio.hxx"
#include "unquoted-value.hxx"

namespace cxon { namespace structs { // structured types reader/writer construction helpers

    struct skip_type {};

    template <typename D>
        struct field;
    template <typename D = skip_type>
        constexpr field<D> make_field(const char* name, D d = {}) { return { name, d }; }

    template <typename ...>
        struct fields {};
    template <typename ...T> 
        constexpr fields<T...> make_fields(T... t) { return { t... }; }

    // field

    template <typename D>
        struct field {
            using type = D;
            char const*const name;
            D mptr;
        };
        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field(S& s, F f, II& i, II e, Cx& cx)
                -> enable_if_t<!std::is_same<typename F::type, skip_type>::value &&  std::is_member_pointer<typename F::type>::value, bool>
            {
                return read_value<X>(s.*f.mptr, i, e, cx);
            }
        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field(S&, F f, II& i, II e, Cx& cx)
                -> enable_if_t<!std::is_same<typename F::type, skip_type>::value && !std::is_member_pointer<typename F::type>::value, bool>
            {
                return read_value<X>(*f.mptr, i, e, cx);
            }
        template <typename X, typename O, typename S, typename F, typename Cx>
            inline auto write_field(O& o, const S& s, F f, Cx& cx)
                -> enable_if_t<!std::is_same<typename F::type, skip_type>::value &&  std::is_member_pointer<typename F::type>::value, bool>
            {
                return chio::write_key<X>(o, f.name, cx) && write_value<X>(o, s.*f.mptr, cx);
            }
        template <typename X, typename O, typename S, typename F, typename Cx>
            inline auto write_field(O& o, const S&, F f, Cx& cx)
                -> enable_if_t<!std::is_same<typename F::type, skip_type>::value && !std::is_member_pointer<typename F::type>::value, bool>
            {
                return chio::write_key<X>(o, f.name, cx) && write_value<X>(o, *f.mptr, cx);
            }
    template <>
        struct field<skip_type> {
            using type = skip_type;
            char const*const name;
            skip_type const _;
        };
        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field(S&, F, II& i, II e, Cx& cx)
                -> enable_if_t<std::is_same<typename F::type, skip_type>::value, bool>
            {
                return unquoted::read_value<X>(i, e, cx);
            }
        template <typename X, typename O, typename S, typename F, typename Cx>
            constexpr auto write_field(O&, const S&, F, Cx&)
                -> enable_if_t<std::is_same<typename F::type, skip_type>::value, bool>
            {
                return true;
            }

    // fields

    template <typename H, typename ...T>
        struct fields<H, T...> {
            H const field;
            fields<T...> const next;
            template <typename ...U>
                constexpr fields(H e, U ...t) : field(e), next(t...) {}
        };
    template <typename T>
        struct fields<T> {
            T const field;
            fields<> const next;
            constexpr fields(T e) : field(e), next() {}
        };

    // read

    namespace bits {

        template <typename X, typename S, typename ...>
            struct read {
                template <typename II, typename Cx>
                    static constexpr bool fields(S&, const char*, const fields<>&, II&, II, Cx&) {
                        return false;
                    }
            };
        template <typename X, typename S, typename H, typename ...T>
            struct read<X, S, H, T...> {
                template <typename II, typename Cx>
                    static bool fields(S& t, const char* name, const fields<H, T...>& f, II& i, II e, Cx& cx) {
                        return std::strcmp(f.field.name, name) == 0 ?
                            read_field<X>(t, f.field, i, e, cx) :
                            read<X, S, T...>::fields(t, name, f.next, i, e, cx)
                        ;
                    }
            };

    }

    template <typename X, typename S, typename ...F, typename II, typename Cx>
        inline bool read_fields(S& s, const fields<F...>& f, II& i, II e, Cx& cx) {
            if (!chio::consume<X>(X::map::beg, i, e, cx)) return false;
            if ( chio::consume<X>(X::map::end, i, e)) return true;
            for (char id[ids_len_max::constant<prms_type<Cx>>(64)]; ; ) {
                chio::consume<X>(i, e);
                II const o = i;
                    if (!chio::read_key<X>(id, i, e, cx)) return false;
                    if (!bits::read<X, S, F...>::fields(s, id, f, i, e, cx))
                        return cx && (chio::rewind(i, o), cx|read_error::unexpected);
                    if (chio::consume<X>(X::map::sep, i, e)) continue;
                return chio::consume<X>(X::map::end, i, e, cx);
            }
            return true;
        }

    // write

    namespace bits {

        template <typename X, typename S, typename H, typename ...T>
            struct write {
                template <typename O, typename Cx>
                    static bool fields(O& o, const S& t, const fields<H, T...>& f, Cx& cx) {
                        return  write_field<X>(o, t, f.field, cx) && chio::poke<X>(o, X::map::sep, cx) &&
                                write<X, S, T...>::fields(o, t, f.next, cx)
                        ;
                    }
            };
        template <typename X, typename S, typename F>
            struct write<X, S, F> {
                template <typename O, typename Cx>
                    static bool fields(O& o, const S& t, const fields<F>& f, Cx& cx) {
                        return write_field<X>(o, t, f.field, cx);
                    }
            };

    }

    template <typename X, typename S, typename ...F, typename O, typename Cx>
        inline bool write_fields(O& o, const S& s, const fields<F...>& f, Cx& cx) {
            return  chio::poke<X>(o, X::map::beg, cx) &&
                        bits::write<X, S, F...>::fields(o, s, f, cx) &&
                    chio::poke<X>(o, X::map::end, cx)
            ;
        }

}}  // cxon::structs structured types reader/writer construction helpers

namespace cxon { // structs::skip_type read

    template <typename X>
        struct read<X, structs::skip_type> {
            template <typename InIt, typename Cx>
                static bool value(structs::skip_type&, InIt& i, InIt e, Cx& cx) {
                    return unquoted::read_value<X>(i, e, cx);
                }
        };

}   // cxon structs::skip_type read

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

#endif // CXON_CHIO_STRUCTS_HXX_
