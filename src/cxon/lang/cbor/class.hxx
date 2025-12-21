// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CXON_CBOR_CLASS_HXX_
#define CXON_CBOR_CLASS_HXX_

#include "common/sink.hxx"
#include <tuple>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cls {

    template <typename T>
        struct default_false {
            constexpr bool operator ()(const T&) const noexcept { return false; }
        };

    template <typename F, typename D>
        struct field {
            using type  = F;
            char const* name;
            std::size_t nale;
            type        value;
            D           dflt;
        };
    template <typename T, std::size_t N, typename F = sink<>>
        constexpr auto make_field(const char (&name)[N], F f) -> field<F, default_false<T>>;
    template <typename T, std::size_t N, typename F, typename D>
        constexpr auto make_field(const char (&name)[N], F f, D dflt) -> field<F, D>;

    template <typename X, typename T, typename F, typename II, typename Cx>
        inline bool read_field(T& t, const F& f, II& i, II e, Cx& cx);
    template <typename X, typename O, typename T, typename F, typename Cx>
        inline bool write_field(O& o, const T& t, const F& f, Cx& cx);

    template <typename ...Fs>
        using fields = std::tuple<Fs...>;
    template <typename ...Fs> 
        constexpr fields<Fs...> make_fields(Fs&&... fs);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cls {

    // field

    template <typename T, std::size_t N, typename F>
        constexpr auto make_field(const char (&name)[N], F f) -> field<F, default_false<T>> {
            return { name, N - 1, f, {} };
        }
    template <typename T, std::size_t N, typename F, typename D>
        constexpr auto make_field(const char (&name)[N], F f, D dflt) -> field<F, D> {
            return { name, N - 1, f, dflt };
        }

    namespace imp {

        template <typename T, typename F>
            constexpr auto field_value_(T& t, const F& f)
                -> enable_if_t< std::is_member_pointer<typename F::type>::value, decltype(t.*f.value)&>
            { return t.*f.value; }
        template <typename T, typename F>
            constexpr auto field_value_(T&, const F& f)
                -> enable_if_t<!std::is_member_pointer<typename F::type>::value, decltype(*f.value)&>
            { return   *f.value; }

    }

    namespace imp {

        template <typename X, typename T, typename F, typename II, typename Cx>
            inline auto read_field_(T& t, const F& f, II& i, II e, Cx& cx)
                -> enable_if_t<!is_sink<typename F::type>::value, bool>
            {
                return read_value<X>(field_value_(t, f), i, e, cx);
            }
        template <typename X, typename T, typename F, typename II, typename Cx>
            inline auto read_field_(T&, const F& f, II& i, II e, Cx& cx)
                -> enable_if_t< is_sink<typename F::type>::value, bool>
            {
                return sink_read<X>(f.value, i, e, cx);
            }

        template <typename X, typename O, typename T, typename F, typename Cx>
            inline auto write_field_(O& o, const T& t, const F& f, Cx& cx)
                -> enable_if_t<!is_sink<typename F::type>::value, bool>
            {
                return write_value<X>(o, f.name, cx) && write_value<X>(o, field_value_(t, f), cx);
            }
        template <typename X, typename O, typename T, typename F, typename Cx>
            constexpr auto write_field_(O&, const T&, const F&, Cx&) noexcept
                -> enable_if_t< is_sink<typename F::type>::value, bool>
            {
                return true;
            }

    }
    template <typename X, typename T, typename F, typename II, typename Cx>
        inline bool read_field(T& t, const F& f, II& i, II e, Cx& cx) {
            return imp::read_field_<X>(t, f, i, e, cx);
        }
    template <typename X, typename O, typename T, typename F, typename Cx>
        inline bool write_field(O& o, const T& t, const F& f, Cx& cx) {
            return imp::write_field_<X>(o, t, f, cx);
        }

    // fields

    template <typename ...Fs>
        constexpr fields<Fs...> make_fields(Fs&&... fs) {
            return std::make_tuple(std::forward<Fs>(fs)...);
        }

    namespace imp {

        template <typename X, std::size_t N, std::size_t L>
            struct read_ {
                template <typename T, typename Fs, typename II, typename Cx>
                    static bool field(T& t, const char* name, const Fs& fs, int (&st)[L], II& i, II e, Cx& cx) {
                        return st[N] == 0 && std::char_traits<char>::compare(std::get<N>(fs).name, name, std::get<N>(fs).nale + 1) == 0 ?
                            (st[N] = 1, read_field<X>(t, std::get<N>(fs), i, e, cx)) :
                            read_<X, N + 1, L>::field(t, name, fs, st, i, e, cx)
                        ;
                    }
            };
        template <typename X, std::size_t N>
            struct read_<X, N, N> {
                template <typename T, typename Fs, typename II, typename Cx>
                    static constexpr bool field(T&, const char*, const Fs&, int (&)[N], II&, II, Cx&) noexcept {
                        return false;
                    }
            };

        template <typename X, std::size_t N, typename T, typename Fs, typename II, typename Cx>
            constexpr bool read_field_(T& t, const char* name, const Fs& fs, int (&st)[N], II& i, II e, Cx& cx) {
                return read_<X, 0, N>::field(t, name, fs, st, i, e, cx);
            }

    }
    template <typename X, typename T, typename ...Fs, typename II, typename Cx>
        inline bool read_fields(T& t, const fields<Fs...>& fs, II& i, II e, Cx& cx) {
            std::size_t n;
            if (cbor::cnt::read_size_le<X>(n, sizeof...(Fs), i, e, cx)) {
                int st[sizeof...(Fs)] = {0};
                for (char id[bio::ids_len_max::constant<napa_type<Cx>>(64)]; n; --n) {
                    II const o = i;
                        if (!read_value<X>(id, i, e, cx))
                            return false;
                        if (!imp::read_field_<X>(t, id, fs, st, i, e, cx))
                            return cx && (bio::rewind(i, o), cx/cbor::read_error::unexpected);
                }
            }
            return n == 0;
        }

    namespace imp {

        template <std::size_t N, std::size_t L>
            struct field_ {
                template <typename T, typename Fs>
                    static unsigned state(const T& t, const Fs& fs, int (&st)[L]) {
                        using E = typename std::tuple_element<N, Fs>::type;
                            st[N] = !std::get<N>(fs).dflt(t) && !is_sink<typename E::type>::value;
                        return st[N] + field_<N + 1, L>::state(t, fs, st);
                    }
            };
        template <std::size_t N>
            struct field_<N, N> {
                template <typename T, typename Fs>
                    static constexpr unsigned state(const T&, const Fs&, int (&)[N]) noexcept {
                        return 0;
                    }
            };
        template <std::size_t N, typename T, typename Fs>
            constexpr unsigned field_state_(const T& t, const Fs& fs, int (&st)[N]) {
                return field_<0, N>::state(t, fs, st);
            }

        template <typename X, std::size_t N, std::size_t L>
            struct write_ {
                template <typename T, typename Fs, typename O, typename Cx>
                    static bool field(O& o, const T& t, const Fs& fs, int (&st)[L], Cx& cx) {
                        return  (std::get<N>(fs).dflt(t) || write_field<X>(o, t, std::get<N>(fs), cx)) &&
                                write_<X, N + 1, L>::field(o, t, fs, st, cx)
                        ;
                    }
            };
        template <typename X, std::size_t N>
            struct write_<X, N, N> {
                template <typename T, typename Fs, typename O, typename Cx>
                    static constexpr bool field(O&, const T&, const Fs&, int (&)[N], Cx&) noexcept {
                        return true;
                    }
            };
        template <typename X, typename T, typename Fs, std::size_t N, typename O, typename Cx>
            constexpr bool write_fields_(O& o, const T& t, const Fs& fs, int (&st)[N], Cx& cx) {
                return write_<X, 0, N>::field(o, t, fs, st, cx);
            }

    }
    template <typename X, typename T, typename ...Fs, typename O, typename Cx>
        inline bool write_fields(O& o, const T& t, const fields<Fs...>& fs, Cx& cx) {
            int st[sizeof...(Fs)] = {0};
                auto const ct = imp::field_state_<sizeof...(Fs)>(t, fs, st);
            return  cbor::cnt::write_size<X>(o, X::map, ct, cx) &&
                    imp::write_fields_<X>(o, t, fs, st, cx)
            ;
        }

}}}

#define CXON_CBOR_CLS_FIELD(T, N, F)                cxon::cbor::cls::make_field<T>(N, &T::F)
#if defined(__cpp_constexpr) && __cpp_constexpr >= 201603L
#   define CXON_CBOR_CLS_FIELD_DFLT(T, N, F, ...)   cxon::cbor::cls::make_field<T>(N, &T::F, [](const T& self) constexpr { return __VA_ARGS__; })
#else
#   define CXON_CBOR_CLS_FIELD_DFLT(T, N, F, ...)   cxon::cbor::cls::make_field<T>(N, &T::F, [](const T& self)           { return __VA_ARGS__; })
#endif
#define CXON_CBOR_CLS_FIELD_NAME(N, F)              CXON_CBOR_CLS_FIELD(CXON_T_, N, F)
#define CXON_CBOR_CLS_FIELD_NAME_DFLT(N, F, ...)    CXON_CBOR_CLS_FIELD_DFLT(CXON_T_, N, F, __VA_ARGS__)
#define CXON_CBOR_CLS_FIELD_ASIS(F)                 CXON_CBOR_CLS_FIELD(CXON_T_, #F, F)
#define CXON_CBOR_CLS_FIELD_ASIS_DFLT(F, ...)       CXON_CBOR_CLS_FIELD_DFLT(CXON_T_, #F, F, __VA_ARGS__)
#define CXON_CBOR_CLS_FIELD_SKIP(N)                 cxon::cbor::cls::make_field<CXON_T_>(N, {})

#define CXON_CBOR_CLS_READ(Type, ...)\
    namespace cxon {\
        template <typename X, typename II, typename Cx>\
            inline auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {\
                using CXON_T_ = Type;\
                static CXON_CXX17_CONSTEXPR auto f = cbor::cls::make_fields(__VA_ARGS__);\
                return  cbor::tag::read<X>(i, e, cx) &&\
                        cbor::cls::read_fields<X>(t, f, i, e, cx)\
                ;\
            }\
    }
#define CXON_CBOR_CLS_WRITE(Type, ...)\
    namespace cxon {\
        template <typename X, typename O, typename Cx>\
            inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, CBOR> {\
                using CXON_T_ = Type;\
                static CXON_CXX17_CONSTEXPR auto f = cbor::cls::make_fields(__VA_ARGS__);\
                return cbor::cls::write_fields<X>(o, t, f, cx);\
            }\
    }
#define CXON_CBOR_CLS(Type, ...)\
    CXON_CBOR_CLS_READ(Type, __VA_ARGS__)\
    CXON_CBOR_CLS_WRITE(Type, __VA_ARGS__)

#define CXON_CBOR_CLS_READ_MEMBER(Type, ...)\
    template <typename X, typename II, typename Cx>\
        static auto read_value(Type& t, II& i, II e, Cx& cx) -> cxon::enable_for_t<X, cxon::CBOR> {\
            using CXON_T_ = Type;\
            static CXON_CXX17_CONSTEXPR auto f = cxon::cbor::cls::make_fields(__VA_ARGS__);\
            return  cxon::cbor::tag::read<X>(i, e, cx) &&\
                    cxon::cbor::cls::read_fields<X>(t, f, i, e, cx)\
            ;\
        }
#define CXON_CBOR_CLS_WRITE_MEMBER(Type, ...)\
    template <typename X, typename O, typename Cx>\
        static auto write_value(O& o, const Type& t, Cx& cx) -> cxon::enable_for_t<X, cxon::CBOR> {\
            using CXON_T_ = Type;\
            static CXON_CXX17_CONSTEXPR auto f = cxon::cbor::cls::make_fields(__VA_ARGS__);\
            return cxon::cbor::cls::write_fields<X>(o, t, f, cx);\
        }
#define CXON_CBOR_CLS_MEMBER(Type, ...)\
    CXON_CBOR_CLS_READ_MEMBER(Type, __VA_ARGS__)\
    CXON_CBOR_CLS_WRITE_MEMBER(Type, __VA_ARGS__)

#endif // CXON_CBOR_CLASS_HXX_
