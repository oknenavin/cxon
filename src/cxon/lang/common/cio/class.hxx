// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_CLASS_HXX_
#define CXON_CIO_CLASS_HXX_

#include "cio.hxx"
#include "value.hxx"
#include <tuple>
#include <cstring> // strncmp

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
            std::size_t nlen;
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
            { return s.*f.value; } // lgtm [cpp/missing-return]
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
                return  str::pointer_write<X>(o, f.name, f.nlen, cx) &&
                            cio::poke<X>(o, X::map::div, cx) &&
                        write_value<X>(o, field_value_(s, f), cx)
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
                template <typename S, typename F, typename II, typename Cx>
                    static bool field(S& t, const char* name, const F& fs, int (&st)[L], II& i, II e, Cx& cx) {
                        return st[N] == 0 && std::strncmp(std::get<N>(fs).name, name, std::get<N>(fs).nlen) == 0 ?
                            (st[N] = 1, read_field<X>(t, std::get<N>(fs), i, e, cx)) :
                            read_<X, N + 1, L>::field(t, name, fs, st, i, e, cx)
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

    }

    template <typename X, typename S, typename ...F, typename II, typename Cx>
        inline bool read_fields(S& s, const fields<F...>& fs, II& i, II e, Cx& cx) {
            if (!consume<X>(X::map::beg, i, e, cx)) return false;
                if ( consume<X>(X::map::end, i, e)) return true;
            int st[std::tuple_size<fields<F...>>::value] = {0};
            for (char id[ids_len_max::constant<napa_type<Cx>>(64)]; ; ) {
                consume<X>(i, e);
                II const o = i;
                    if (!read_key<X>(id, i, e, cx))
                        return false;
                    if (!imp::read_field_<X>(s, id, fs, st, i, e, cx))
                        return cx && (rewind(i, o), cx/X::read_error::unexpected);
                    if (consume<X>(X::map::sep, i, e))
                        continue;
                return consume<X>(X::map::end, i, e, cx);
            }
            return true;
        }

    // write

    namespace imp {

        template <typename X, std::size_t N, std::size_t L>
            struct write_next_ {
                template <typename S, typename F, typename O, typename Cx>
                    static bool field(O& o, const S& s, const F& fs, Cx& cx) {
                        using T = typename std::tuple_element<N, F>::type;
                        bool const skip = std::get<N>(fs).dflt(s) || val::is_sink<typename T::type>::value;
                        return  (skip || (poke<X>(o, X::map::sep, cx) && write_field<X>(o, s, std::get<N>(fs), cx))) &&
                                write_next_<X, N + 1, L>::field(o, s, fs, cx)
                        ;
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
                        bool const skip = std::get<N>(fs).dflt(s) || val::is_sink<typename T::type>::value;
                        return   (!skip && write_field<X>(o, s, std::get<N>(fs), cx) && write_next_<X, N + 1, L>::field(o, s, fs, cx)) ||
                                 ( skip && write_<X, N + 1, L>::field(o, s, fs, cx))
                        ;
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
            constexpr bool write_fields_(O& o, const S& s, const F& fs, Cx& cx) {
                return write_<X, 0, std::tuple_size<F>::value>::field(o, s, fs, cx);
            }

    }

    template <typename X, typename S, typename ...F, typename O, typename Cx>
        inline bool write_fields(O& o, const S& s, const fields<F...>& fs, Cx& cx) {
            return  poke<X>(o, X::map::beg, cx) &&
                        imp::write_fields_<X>(o, s, fs, cx) &&
                    poke<X>(o, X::map::end, cx)
            ;
        }

}}}

#endif // CXON_CIO_CLASS_HXX_
