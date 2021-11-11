// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_CLASS_HXX_
#define CXON_CIO_CLASS_HXX_

#include "cio.hxx"
#include "value.hxx"
#include <cstring> // strcmp

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace cls { // structured types reader/writer construction helpers

    template <typename F, typename D>
        struct field {
            using type = F;
            char const*const name;
            type const value;
            D dflt;
        };
    template <typename S, typename F = val::sink<>>
        constexpr auto make_field(const char* name, F f) -> field<F, bool (*)(const S&)>;
    template <typename S, typename F, typename D>
        constexpr auto make_field(const char* name, F f) -> field<F, D>;

    template <typename X, typename S, typename F, typename II, typename Cx>
        inline bool read_field(S& s, const F& f, II& i, II e, Cx& cx);
    template <typename X, typename O, typename S, typename F, typename Cx>
        inline bool write_field(O& o, const S& s, const F& f, Cx& cx);

    template <typename ...>
        struct fields;
    template <typename ...F> 
        constexpr fields<F...> make_fields(F&&... f);

    template <typename X, typename S, typename ...F, typename II, typename Cx>
        inline bool read_fields(S& s, const fields<F...>& f, II& i, II e, Cx& cx);
    template <typename X, typename S, typename ...F, typename O, typename Cx>
        inline bool write_fields(O& o, const S& s, const fields<F...>& f, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace cls {

    // field

    template <typename S, typename F>
        constexpr auto make_field(const char* name, F f) -> field<F, bool (*)(const S&)> {
            return { name, f, [](const S&) { return false; } };
        }
    template <typename S, typename F, typename D>
        constexpr auto make_field(const char* name, F f, D dflt) -> field<F, D> {
            return { name, f, dflt };
        }

    namespace imp {

        template <typename S, typename F>
            constexpr auto field_value_(S& s, const F& f)
                -> enable_if_t< std::is_member_pointer<typename F::type>::value, decltype(s.*f.value)&>
            { return s.*f.value; }
        template <typename S, typename F>
            constexpr auto field_value_(S&, const F& f)
                -> enable_if_t<!std::is_member_pointer<typename F::type>::value, decltype(*f.value)&>
            { return   *f.value; }

    }

    namespace imp {

        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field_(S& s, const F& f, II& i, II e, Cx& cx)
                -> enable_if_t<!val::is_sink<typename F::type>::value, bool>
            {
                return read_value<X>(field_value_(s, f), i, e, cx);
            }
        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field_(S&, const F& f, II& i, II e, Cx& cx)
                -> enable_if_t< val::is_sink<typename F::type>::value, bool>
            {
                return val::sink_read<X>(f.value, i, e, cx);
            }

        template <typename X, typename O, typename S, typename F, typename Cx>
            inline auto write_field_(O& o, const S& s, const F& f, Cx& cx)
                -> enable_if_t<!val::is_sink<typename F::type>::value, bool>
            {
                return write_key<X>(o, f.name, cx) && write_value<X>(o, field_value_(s, f), cx);
            }
        template <typename X, typename O, typename S, typename F, typename Cx>
            constexpr auto write_field_(O&, const S&, const F&, Cx&)
                -> enable_if_t< val::is_sink<typename F::type>::value, bool>
            {
                return true;
            }

    }
    template <typename X, typename S, typename F, typename II, typename Cx>
        inline bool read_field(S& s, const F& f, II& i, II e, Cx& cx) {
            return imp::read_field_<X>(s, f, i, e, cx);
        }
    template <typename X, typename O, typename S, typename F, typename Cx>
        inline bool write_field(O& o, const S& s, const F& f, Cx& cx) {
            return imp::write_field_<X>(o, s, f, cx);
        }

    // fields

    template <>
        struct fields<> {};
    template <typename H, typename ...T>
        struct fields<H, T...> {
            H const field;
            fields<T...> const next;
            template <typename ...U>
                constexpr fields(H e, U&&... t) : field(e), next(std::forward<U>(t)...) {}
        };
    template <typename T>
        struct fields<T> {
            T const field;
            fields<> const next;
            constexpr fields(T e) : field(e), next() {}
        };

    template <typename ...F> 
        constexpr fields<F...> make_fields(F&&... f) { return { std::forward<F>(f)... }; }

    // read

    namespace imp {

        template <typename X, typename S, typename ...>
            struct read_ {
                template <typename II, typename Cx>
                    static constexpr bool fields(S&, const char*, const fields<>&, II&, II, Cx&) {
                        return false;
                    }
            };
        template <typename X, typename S, typename H, typename ...T>
            struct read_<X, S, H, T...> {
                template <typename II, typename Cx>
                    static bool fields(S& t, const char* name, const fields<H, T...>& f, II& i, II e, Cx& cx) {
                        return std::strcmp(f.field.name, name) == 0 ?
                            read_field<X>(t, f.field, i, e, cx) :
                            read_<X, S, T...>::fields(t, name, f.next, i, e, cx)
                        ;
                    }
            };

    }

    template <typename X, typename S, typename ...F, typename II, typename Cx>
        inline bool read_fields(S& s, const fields<F...>& f, II& i, II e, Cx& cx) {
            if (!consume<X>(X::map::beg, i, e, cx)) return false;
            if ( consume<X>(X::map::end, i, e))     return true;
            for (char id[ids_len_max::constant<napa_type<Cx>>(64)]; ; ) {
                consume<X>(i, e);
                II const o = i;
                    if (!read_key<X>(id, i, e, cx)) return false;
                    if (!imp::read_<X, S, F...>::fields(s, id, f, i, e, cx))
                        return cx && (rewind(i, o), cx/X::read_error::unexpected);
                    if (consume<X>(X::map::sep, i, e)) continue;
                return consume<X>(X::map::end, i, e, cx);
            }
            return true;
        }

    // write

    namespace imp {

        template <typename X, typename S, typename ...>
            struct write_next_ {
                template <typename O, typename Cx>
                    static constexpr bool fields(O&, const S&, const fields<>&, Cx&) {
                        return true;
                    }
            };
        template <typename X, typename S, typename H, typename ...T>
            struct write_next_<X, S, H, T...> {
                template <typename O, typename Cx>
                    static bool fields(O& o, const S& t, const fields<H, T...>& f, Cx& cx) {
                        return  (f.field.dflt(t) || (poke<X>(o, X::map::sep, cx) && write_field<X>(o, t, f.field, cx))) &&
                                write_next_<X, S, T...>::fields(o, t, f.next, cx)
                        ;
                    }
            };

        template <typename X, typename S, typename ...>
            struct write_ {
                template <typename O, typename Cx>
                    static constexpr bool fields(O&, const S&, const fields<>&, Cx&) {
                        return true;
                    }
            };
        template <typename X, typename S, typename H, typename ...T>
            struct write_<X, S, H, T...> {
                template <typename O, typename Cx>
                    static bool fields(O& o, const S& t, const fields<H, T...>& f, Cx& cx) {
                        bool const dflt = f.field.dflt(t);
                        return   (!dflt && write_field<X>(o, t, f.field, cx) && write_next_<X, S, T...>::fields(o, t, f.next, cx)) ||
                                 ( dflt && write_<X, S, T...>::fields(o, t, f.next, cx))
                        ;
                    }
            };

    }

    template <typename X, typename S, typename ...F, typename O, typename Cx>
        inline bool write_fields(O& o, const S& s, const fields<F...>& f, Cx& cx) {
            return  poke<X>(o, X::map::beg, cx) &&
                        imp::write_<X, S, F...>::fields(o, s, f, cx) &&
                    poke<X>(o, X::map::end, cx)
            ;
        }

}}}

#endif // CXON_CIO_CLASS_HXX_
