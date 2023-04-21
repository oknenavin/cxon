// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_CLASS_HXX_
#define CXON_CBOR_CLASS_HXX_

#include "common/sink.hxx"
#include <tuple>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cls {

    template <typename S>
        struct default_false {
            constexpr bool operator ()(const S&) const noexcept { return false; }
        };

    template <typename F, typename D>
        struct field {
            using type = F;
            char const*const name;
            std::size_t nale;
            type value;
            D dflt;
        };
    template <typename S, std::size_t N, typename F = sink<>>
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

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cls {

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
                -> enable_if_t<!is_sink<typename F::type>::value, bool>
            {
                return read_value<X>(field_value_(s, f), i, e, cx);
            }
        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field_(S&, const F& f, II& i, II e, Cx& cx)
                -> enable_if_t< is_sink<typename F::type>::value, bool>
            {
                return sink_read<X>(f.value, i, e, cx);
            }

        template <typename X, typename O, typename S, typename F, typename Cx>
            inline auto write_field_(O& o, const S& s, const F& f, Cx& cx)
                -> enable_if_t<!is_sink<typename F::type>::value, bool>
            {
                return write_value<X>(o, f.name, cx) && write_value<X>(o, field_value_(s, f), cx);
            }
        template <typename X, typename O, typename S, typename F, typename Cx>
            constexpr auto write_field_(O&, const S&, const F&, Cx&) noexcept
                -> enable_if_t< is_sink<typename F::type>::value, bool>
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

    namespace imp {

        template <typename X, std::size_t N, std::size_t L>
            struct read_ {
                template <typename S, typename F, typename II, typename Cx>
                    static bool field(S& s, const char* name, const F& fs, int (&st)[L], II& i, II e, Cx& cx) {
                        return st[N] == 0 && std::char_traits<char>::compare(std::get<N>(fs).name, name, std::get<N>(fs).nale + 1) == 0 ?
                            (st[N] = 1,read_field<X>(s, std::get<N>(fs), i, e, cx)) :
                            read_<X, N + 1, L>::field(s, name, fs, st, i, e, cx)
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
            std::size_t n;
            if (cbor::cnt::read_size_le<X>(n, sizeof...(F), i, e, cx)) {
                int st[sizeof...(F)] = {0};
                for (char id[bio::ids_len_max::constant<napa_type<Cx>>(64)]; n; --n) {
                    II const o = i;
                        if (!read_value<X>(id, i, e, cx))
                            return false;
                        if (!imp::read_field_<X>(s, id, fs, st, i, e, cx))
                            return cx && (bio::rewind(i, o), cx/cbor::read_error::unexpected);
                }
            }
            return n == 0;
        }

    namespace imp {

        template <std::size_t N, std::size_t L>
            struct field_ {
                template <typename S, typename F>
                    static unsigned state(const S& s, const F& fs, int (&st)[L]) {
                        using T = typename std::tuple_element<N, F>::type;
                            st[N] = !std::get<N>(fs).dflt(s) && !is_sink<typename T::type>::value;
                        return st[N] + field_<N + 1, L>::state(s, fs, st);
                    }
            };
        template <std::size_t N>
            struct field_<N, N> {
                template <typename S, typename F>
                    static constexpr unsigned state(const S&, const F&, int (&)[N]) noexcept {
                        return 0;
                    }
            };
        template <std::size_t N, typename S, typename F>
            constexpr unsigned field_state_(const S& s, const F& fs, int (&st)[N]) {
                return field_<0, N>::state(s, fs, st);
            }

        template <typename X, std::size_t N, std::size_t L>
            struct write_ {
                template <typename S, typename F, typename O, typename Cx>
                    static bool field(O& o, const S& s, const F& fs, int (&st)[L], Cx& cx) {
                        return  (std::get<N>(fs).dflt(s) || write_field<X>(o, s, std::get<N>(fs), cx)) &&
                                write_<X, N + 1, L>::field(o, s, fs, st, cx)
                        ;
                    }
            };
        template <typename X, std::size_t N>
            struct write_<X, N, N> {
                template <typename S, typename F, typename O, typename Cx>
                    static constexpr bool field(O&, const S&, const F&, int (&)[N], Cx&) noexcept {
                        return true;
                    }
            };
        template <typename X, typename S, typename F, std::size_t N, typename O, typename Cx>
            constexpr bool write_fields_(O& o, const S& s, const F& fs, int (&st)[N], Cx& cx) {
                return write_<X, 0, N>::field(o, s, fs, st, cx);
            }

    }
    template <typename X, typename S, typename ...F, typename O, typename Cx>
        inline bool write_fields(O& o, const S& s, const fields<F...>& fs, Cx& cx) {
            int st[sizeof...(F)] = {0};
                auto const ct = imp::field_state_<sizeof...(F)>(s, fs, st);
            return  cbor::cnt::write_size<X>(o, X::map, ct, cx) &&
                    imp::write_fields_<X>(o, s, fs, st, cx)
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

#if defined(__cpp_constexpr) && __cpp_constexpr >= 201603L
#   define CXON_CBOR_CLS_READ(Type, ...)\
        namespace cxon {\
            template <typename X, typename II, typename Cx>\
                inline auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {\
                    using CXON_T_ = Type;\
                    static constexpr auto f = cbor::cls::make_fields(__VA_ARGS__);\
                    return  cbor::tag::read<X>(i, e, cx) &&\
                            cbor::cls::read_fields<X>(t, f, i, e, cx)\
                    ;\
                }\
        }
#   define CXON_CBOR_CLS_WRITE(Type, ...)\
        namespace cxon {\
            template <typename X, typename O, typename Cx>\
                inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, CBOR> {\
                    using CXON_T_ = Type;\
                    static constexpr auto f = cbor::cls::make_fields(__VA_ARGS__);\
                    return cbor::cls::write_fields<X>(o, t, f, cx);\
                }\
        }
#else
#   define CXON_CBOR_CLS_READ(Type, ...)\
        namespace cxon {\
            template <typename X, typename II, typename Cx>\
                inline auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {\
                    using CXON_T_ = Type;\
                    static auto const f = cbor::cls::make_fields(__VA_ARGS__);\
                    return  cbor::tag::read<X>(i, e, cx) &&\
                            cbor::cls::read_fields<X>(t, f, i, e, cx)\
                    ;\
                }\
        }
#   define CXON_CBOR_CLS_WRITE(Type, ...)\
        namespace cxon {\
            template <typename X, typename O, typename Cx>\
                inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, CBOR> {\
                    using CXON_T_ = Type;\
                    static auto const f = cbor::cls::make_fields(__VA_ARGS__);\
                    return cbor::cls::write_fields<X>(o, t, f, cx);\
                }\
        }
#endif
#define CXON_CBOR_CLS(Type, ...)\
    CXON_CBOR_CLS_READ(Type, __VA_ARGS__)\
    CXON_CBOR_CLS_WRITE(Type, __VA_ARGS__)

#if defined(__cpp_constexpr) && __cpp_constexpr >= 201603L
#   define CXON_CBOR_CLS_READ_MEMBER(Type, ...)\
        template <typename X, typename II, typename Cx>\
            static auto read_value(Type& t, II& i, II e, Cx& cx) -> cxon::enable_for_t<X, cxon::CBOR> {\
                using CXON_T_ = Type;\
                static constexpr auto f = cxon::cbor::cls::make_fields(__VA_ARGS__);\
                return  cxon::cbor::tag::read<X>(i, e, cx) &&\
                        cxon::cbor::cls::read_fields<X>(t, f, i, e, cx)\
                ;\
            }
#   define CXON_CBOR_CLS_WRITE_MEMBER(Type, ...)\
        template <typename X, typename O, typename Cx>\
            static auto write_value(O& o, const Type& t, Cx& cx) -> cxon::enable_for_t<X, cxon::CBOR> {\
                using CXON_T_ = Type;\
                static constexpr auto f = cxon::cbor::cls::make_fields(__VA_ARGS__);\
                return cxon::cbor::cls::write_fields<X>(o, t, f, cx);\
            }
#else
#   define CXON_CBOR_CLS_READ_MEMBER(Type, ...)\
        template <typename X, typename II, typename Cx>\
            static auto read_value(Type& t, II& i, II e, Cx& cx) -> cxon::enable_for_t<X, cxon::CBOR> {\
                using CXON_T_ = Type;\
                static auto const f = cxon::cbor::cls::make_fields(__VA_ARGS__);\
                return  cxon::cbor::tag::read<X>(i, e, cx) &&\
                        cxon::cbor::cls::read_fields<X>(t, f, i, e, cx)\
                ;\
            }
#   define CXON_CBOR_CLS_WRITE_MEMBER(Type, ...)\
        template <typename X, typename O, typename Cx>\
            static auto write_value(O& o, const Type& t, Cx& cx) -> cxon::enable_for_t<X, cxon::CBOR> {\
                using CXON_T_ = Type;\
                static auto const f = cxon::cbor::cls::make_fields(__VA_ARGS__);\
                return cxon::cbor::cls::write_fields<X>(o, t, f, cx);\
            }
#endif
#define CXON_CBOR_CLS_MEMBER(Type, ...)\
    CXON_CBOR_CLS_READ_MEMBER(Type, __VA_ARGS__)\
    CXON_CBOR_CLS_WRITE_MEMBER(Type, __VA_ARGS__)

#endif // CXON_CBOR_CLASS_HXX_
