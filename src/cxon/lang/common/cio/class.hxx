// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_CLASS_HXX_
#define CXON_CIO_CLASS_HXX_

#include "cio.hxx"
#include "key.hxx"
#include "value.hxx"
#include <tuple>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace cls { // structured types reader/writer construction helpers

    template <typename S>
        struct default_false {
            constexpr bool operator ()(const S&) const noexcept { return false; }
        };

    template <typename F, typename D>
        struct field {
            using type = F;
            char const*const name;
            std::size_t nale;
            type const value;
            D dflt;
        };
    template <typename S, std::size_t N, typename F = val::sink<>>
        constexpr auto make_field(const char (&name)[N], F f) -> field<F, default_false<S>>;
    template <typename S, std::size_t N, typename F, typename D>
        constexpr auto make_field(const char (&name)[N], F f, D dflt) -> field<F, D>;

    template <typename X, typename S, typename F, typename II, typename Cx>
        inline bool read_field(S& s, const F& f, II& i, II e, Cx& cx);
    template <typename X, typename O, typename S, typename F, typename Cx>
        inline bool write_field(O& o, const S& s, const F& f, Cx& cx);

    template <typename ...F>
        using fields = std::tuple<F...>;
    template <typename ...F> 
        constexpr fields<F...> make_fields(F&&... f);

    template <typename X, typename S, typename ...F, typename II, typename Cx>
        inline bool read_fields(S& s, const fields<F...>& f, II& i, II e, Cx& cx);
    template <typename X, typename S, typename ...F, typename O, typename Cx>
        inline bool write_fields(O& o, const S& s, const fields<F...>& f, Cx& cx);

}}}

namespace cxon { namespace cio { namespace cls { // bare object support structured types

    struct bare_class_tag {};

    struct bare_class {
        using bare_class_tag = cls::bare_class_tag;
    };

    namespace imp {
        template <typename T, typename E = void_t<>>
            struct is_bare_class_                                           : std::false_type {};
        template <typename T>
            struct is_bare_class_<T, void_t<typename T::bare_class_tag>>    : std::true_type {};
    }
    template <typename T>
        struct is_bare_class : imp::is_bare_class_<T> {};

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace cls {

    // field

    template <typename S, std::size_t N, typename F>
        constexpr auto make_field(const char (&name)[N], F f) -> field<F, default_false<S>> {
            return { name, N - 1, f, {} };
        }
    template <typename S, std::size_t N, typename F, typename D>
        constexpr auto make_field(const char (&name)[N], F f, D dflt) -> field<F, D> {
            return { name, N - 1, f, dflt };
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
                return  read_map_val<X>(field_value_(s, f), i, e, cx);
            }
        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field_(S&, const F& f, II& i, II e, Cx& cx)
                -> enable_if_t< val::is_sink<typename F::type>::value, bool>
            {
                return  val::sink_read<X>(f.value, i, e, cx);
            }

        template <typename X, typename O, typename S, typename F, typename Cx>
            inline auto write_field_(O& o, const S& s, const F& f, Cx& cx)
                -> enable_if_t<!val::is_sink<typename F::type>::value, bool>
            {
                return  write_map_key<X>(o, f.name, f.nale, cx) &&
                        write_map_val<X>(o, field_value_(s, f), cx)
                ;
            }
        template <typename X, typename O, typename S, typename F, typename Cx>
            constexpr auto write_field_(O&, const S&, const F&, Cx&) noexcept
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

    template <typename ...F> 
        constexpr fields<F...> make_fields(F&&... f) {
            return std::make_tuple(std::forward<F>(f)...);
        }

    // read

    namespace imp {

        template <typename X, std::size_t N, std::size_t L>
            struct read_ {
                template <typename S, typename F, typename II, typename Cx, typename Y = X>
                    static auto field(S& t, const char* name, const F& fs, int (&st)[L], II& i, II e, Cx& cx) -> enable_if_t< Y::assume_unique_object_keys, bool> {
                        return st[N] == 0 && std::char_traits<char>::compare(std::get<N>(fs).name, name, std::get<N>(fs).nale + 1) == 0 ?
                            (st[N] = 1, read_field<Y>(t, std::get<N>(fs), i, e, cx)) :
                            read_<Y, N + 1, L>::field(t, name, fs, st, i, e, cx)
                        ;
                    }
                template <typename S, typename F, typename II, typename Cx, typename Y = X>
                    static auto field(S& t, const char* name, const F& fs, int (&st)[L], II& i, II e, Cx& cx) -> enable_if_t<!Y::assume_unique_object_keys, bool> {
                        return std::char_traits<char>::compare(std::get<N>(fs).name, name, std::get<N>(fs).nale + 1) == 0 ?
                            read_field<Y>(t, std::get<N>(fs), i, e, cx) :
                            read_<Y, N + 1, L>::field(t, name, fs, st, i, e, cx)
                        ;
                    }
            };
        template <typename X, std::size_t N>
            struct read_<X, N, N> {
                template <typename S, typename F, typename II, typename Cx>
                    static constexpr bool field(S&, const char*, const F&, int (&)[N], II&, II, Cx&) noexcept {
                        return false;
                    }
            };
        template <typename X, std::size_t N, typename S, typename F, typename II, typename Cx>
            constexpr bool read_field_(S& s, const char* name, const F& fs, int (&st)[N], II& i, II e, Cx& cx) {
                return read_<X, 0, N>::field(s, name, fs, st, i, e, cx);
            }

        template <typename X, typename S, typename II, typename Cx>
            inline constexpr bool read_fields_(S&, const fields<>&, II&, II, Cx&) { return true; }

        template <typename X, typename S, typename ...F, typename II, typename Cx>
            inline auto read_fields_(S& s, const fields<F...>& fs, II& i, II e, Cx& cx)
                -> enable_if_t<!is_bare_class<S>::value, bool>
            {
                if (!consume<X>(X::map::beg, i, e, cx))
                    return false;
                int st[sizeof...(F)] = {0};
                for (char id[ids_len_max::constant<napa_type<Cx>>(64)]; ; ) {
                    if (!consume<X>(i, e, cx))
                        return false;
                    if (peek(i, e) == X::map::end)
                        return ++i, true;
                    II const o = i;
                        if (!read_map_key<X>(id, i, e, cx))
                            return rewind(i, o), false;
                        if (!read_field_<X>(s, id, fs, st, i, e, cx))
                            return cx && (rewind(i, o), cx/X::read_error::unexpected);
                        if (cnt::consume_sep<X, typename X::map>(i, e, cx))
                            continue;
                    return consume<X>(X::map::end, i, e, cx);
                }
                return true;
            }
        template <typename X, typename S, typename ...F, typename II, typename Cx>
            inline auto read_fields_(S& s, const fields<F...>& fs, II& i, II e, Cx& cx)
                -> enable_if_t< is_bare_class<S>::value, bool>
            {
                if (!consume<X>(i, e, cx)) return false;
                    if (i == e) return true;
                int st[sizeof...(F)] = {0};
                for (char id[ids_len_max::constant<napa_type<Cx>>(64)]; i != e && *i; ) {
                    II const o = i;
                        if (!read_map_key<X>(id, i, e, cx))
                            return false;
                        if (!read_field_<X>(s, id, fs, st, i, e, cx))
                            return cx && (rewind(i, o), cx/X::read_error::unexpected);
                        if (!consume<X>(i, e, cx))
                            return false;
                }
                return true;
            }

    }

    template <typename X, typename S, typename ...F, typename II, typename Cx>
        inline bool read_fields(S& s, const fields<F...>& fs, II& i, II e, Cx& cx) {
            return imp::read_fields_<X>(s, fs, i, e, cx);
        }

    // write

    namespace imp {

        template <typename X, typename S, typename O, typename Cx>
            inline auto write_sep_(O& o, Cx& cx) -> enable_if_t<!is_bare_class<S>::value, bool> {
                return poke<X>(o, X::map::sep, cx);
            }
        template <typename X, typename S, typename O, typename Cx>
            inline auto write_sep_(O& o, Cx& cx) -> enable_if_t< is_bare_class<S>::value, bool> {
                return poke<X>(o, '\n', cx);
            }

        template <typename X, std::size_t N, std::size_t L>
            struct write_next_ {
                template <typename S, typename F, typename O, typename Cx>
                    static bool field(O& o, const S& s, const F& fs, Cx& cx) {
                        using T = typename std::tuple_element<N, F>::type;
                        CXON_IF_CONSTEXPR (!val::is_sink<typename T::type>::value) {
                            if (!std::get<N>(fs).dflt(s))
                                return  write_sep_<X, S>(o, cx) && write_field<X>(o, s, std::get<N>(fs), cx) &&
                                        write_next_<X, N + 1, L>::field(o, s, fs, cx)
                                ;
                            return      write_next_<X, N + 1, L>::field(o, s, fs, cx);
                        }
                        return          write_next_<X, N + 1, L>::field(o, s, fs, cx);
                    }
            };
        template <typename X, std::size_t N>
            struct write_next_<X, N, N> {
                template <typename S, typename F, typename O, typename Cx>
                    static constexpr bool field(O&, const S&, const F&, Cx&) noexcept {
                        return true;
                    }
            };

        template <typename X, std::size_t N, std::size_t L>
            struct write_ {
                template <typename S, typename F, typename O, typename Cx>
                    static bool field(O& o, const S& s, const F& fs, Cx& cx) {
                        using T = typename std::tuple_element<N, F>::type;
                        CXON_IF_CONSTEXPR (!val::is_sink<typename T::type>::value) {
                            if (!std::get<N>(fs).dflt(s))
                                return  write_field<X>(o, s, std::get<N>(fs), cx) &&
                                        write_next_<X, N + 1, L>::field(o, s, fs, cx)
                                ;
                            return      write_<X, N + 1, L>::field(o, s, fs, cx);
                        }
                        return          write_<X, N + 1, L>::field(o, s, fs, cx);
                    }
            };
        template <typename X, std::size_t N>
            struct write_<X, N, N> {
                template <typename S, typename F, typename O, typename Cx>
                    static constexpr bool field(O&, const S&, const F&, Cx&) noexcept {
                        return true;
                    }
            };

        template <typename X, typename S, typename F, typename O, typename Cx>
            constexpr bool write_fields_bare_(O& o, const S& s, const F& fs, Cx& cx) {
                return write_<X, 0, std::tuple_size<F>::value>::field(o, s, fs, cx);
            }

        template <typename X, typename S, typename ...F, typename O, typename Cx>
            inline auto write_fields_(O& o, const S& s, const fields<F...>& fs, Cx& cx) -> enable_if_t<!is_bare_class<S>::value, bool> {
                return  poke<X>(o, X::map::beg, cx) &&
                            write_fields_bare_<X>(o, s, fs, cx) &&
                        poke<X>(o, X::map::end, cx)
                ;
            }
        template <typename X, typename S, typename ...F, typename O, typename Cx>
            inline auto write_fields_(O& o, const S& s, const fields<F...>& fs, Cx& cx) -> enable_if_t< is_bare_class<S>::value, bool> {
                return  write_fields_bare_<X>(o, s, fs, cx);
            }

    }

    template <typename X, typename S, typename ...F, typename O, typename Cx>
        inline bool write_fields(O& o, const S& s, const fields<F...>& fs, Cx& cx) {
            return imp::write_fields_<X>(o, s, fs, cx);
        }

}}}

#endif // CXON_CIO_CLASS_HXX_
