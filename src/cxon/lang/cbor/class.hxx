// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_CLASS_HXX_
#define CXON_CBOR_CLASS_HXX_

#include "common/sink.hxx"
#include <tuple>

// std::make_tuple is constexpr since C++14
#if __cplusplus < 201402L
#   define CXON_CBOR_CLS_CONSTEXPR
#else
#   define CXON_CBOR_CLS_CONSTEXPR constexpr
#endif

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cls {

    template <typename F>
        struct field {
            using type = F;
            char const*const name;
            type value;
        };
    template <typename F = sink<>>
        constexpr field<F> make_field(const char* name, F f = {});

    template <typename X, typename S, typename F, typename II, typename Cx>
        inline bool read_field(S& s, F f, II& i, II e, Cx& cx);
    template <typename X, typename O, typename S, typename F, typename Cx>
        inline bool write_field(O& o, const S& s, F f, Cx& cx);

    template <typename ...F>
        using fields = std::tuple<F...>;
    template <typename ...F> 
        inline CXON_CBOR_CLS_CONSTEXPR fields<F...> make_fields(F... f);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cls {

    // field

    template <typename F>
        constexpr field<F> make_field(const char* name, F f) {
            return { name, f };
        }

    namespace bits {

        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field_(S& s, F f, II& i, II e, Cx& cx)
                -> enable_if_t<!is_sink<typename F::type>::value &&  std::is_member_pointer<typename F::type>::value, bool>
            {
                return read_value<X>(s.*f.value, i, e, cx);
            }
        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field_(S&, F f, II& i, II e, Cx& cx)
                -> enable_if_t<!is_sink<typename F::type>::value && !std::is_member_pointer<typename F::type>::value, bool>
            {
                return read_value<X>(*f.value, i, e, cx);
            }

        template <typename X, typename O, typename S, typename F, typename Cx>
            inline auto write_field_(O& o, const S& s, F f, Cx& cx)
                -> enable_if_t<!is_sink<typename F::type>::value &&  std::is_member_pointer<typename F::type>::value, bool>
            {
                return write_value<X>(o, f.name, cx) && write_value<X>(o, s.*f.value, cx);
            }
        template <typename X, typename O, typename S, typename F, typename Cx>
            inline auto write_field_(O& o, const S&, F f, Cx& cx)
                -> enable_if_t<!is_sink<typename F::type>::value && !std::is_member_pointer<typename F::type>::value, bool>
            {
                return write_value<X>(o, f.name, cx) && write_value<X>(o, *f.value, cx);
            }

        template <typename X, typename S, typename F, typename II, typename Cx>
            inline auto read_field_(S&, F f, II& i, II e, Cx& cx)
                -> enable_if_t< is_sink<typename F::type>::value, bool>
            {
                return sink_read<X>(f.value, i, e, cx);
            }
        //template <typename X, typename O, typename S, typename F, typename Cx>
        //    constexpr auto write_field_(O&, const S&, F, Cx&)
        //        -> enable_if_t< is_sink<typename F::type>::value, bool>
        //    {
        //        return true;
        //    }

    }
    template <typename X, typename S, typename F, typename II, typename Cx>
        inline bool read_field(S& s, F f, II& i, II e, Cx& cx) {
            return bits::read_field_<X>(s, f, i, e, cx);
        }
    template <typename X, typename O, typename S, typename F, typename Cx>
        inline bool write_field(O& o, const S& s, F f, Cx& cx) {
            return bits::write_field_<X>(o, s, f, cx);
        }

    // fields

    template <typename ...F> 
        inline CXON_CBOR_CLS_CONSTEXPR fields<F...> make_fields(F... f) {
            return std::make_tuple(f...);
        }

    namespace bits {

        template <typename X, size_t N, size_t L>
            struct read_ {
                template <typename S, typename F, typename II, typename Cx>
                    static bool fields(S& s, const char* name, const F& f, II& i, II e, Cx& cx) {
                        return std::strcmp(std::get<N>(f).name, name) == 0 ?
                            read_field<X>(s, std::get<N>(f), i, e, cx) :
                            read_<X, N + 1, L>::fields(s, name, f, i, e, cx)
                        ;
                    }
            };
        template <typename X, size_t N>
            struct read_<X, N, N> {
                template <typename S, typename F, typename II, typename Cx>
                    static constexpr bool fields(S&, const char*, const F&, II&, II, Cx&) {
                        return false;
                    }
            };

        template <typename X, size_t N, size_t L>
            struct write_ {
                template <typename S, typename F, typename O, typename Cx>
                    static bool fields(O& o, const S& s, const F& f, Cx& cx) {
                        return  write_field<X>(o, s, std::get<N>(f), cx) &&
                                write_<X, N + 1, L>::fields(o, s, f, cx)
                        ;
                    }
            };
        template <typename X, size_t N>
            struct write_<X, N, N> {
                template <typename S, typename F, typename O, typename Cx>
                    static constexpr bool fields(O&, const S&, const F&, Cx&) {
                        return true;
                    }
            };

    }

    template <typename X, typename S, typename ...F, typename II, typename Cx>
        inline bool read_fields(S& s, const fields<F...>& f, II& i, II e, Cx& cx) {
            size_t n;
            if (cbor::cnt::read_size_le<X>(n, std::tuple_size<fields<F...>>::value, i, e, cx))
                for ( ; n; --n) {
                    char id[bio::ids_len_max::constant<napa_type<Cx>>(64)];
                    {   II const o = i;
                        if (!read_value<X>(id, i, e, cx))
                            return false;
                        if (!bits::read_<X, 0, std::tuple_size<fields<F...>>::value>::fields(s, id, f, i, e, cx))
                            return cx && (bio::rewind(i, o), cx/cbor::read_error::unexpected);
                    }
                }
            return n == 0;
        }

    template <typename X, typename S, typename ...F, typename O, typename Cx>
        inline bool write_fields(O& o, const S& s, const fields<F...>& f, Cx& cx) {
            return  cbor::cnt::write_size<X>(o, X::map, std::tuple_size<fields<F...>>::value, cx) &&
                    bits::write_<X, 0, std::tuple_size<fields<F...>>::value>::fields(o, s, f, cx)
            ;
        }

}}}

#define CXON_CBOR_CLS_FIELD(T, N, F)     cxon::cbor::cls::make_field(N, &T::F)
#define CXON_CBOR_CLS_FIELD_NAME(N, F)   CXON_CBOR_CLS_FIELD(T, N, F)
#define CXON_CBOR_CLS_FIELD_ASIS(F)      CXON_CBOR_CLS_FIELD(T, #F, F)
#define CXON_CBOR_CLS_FIELD_SKIP(N)      cxon::cbor::cls::make_field(N)

#define CXON_CBOR_CLS_READ(Type, ...)\
    namespace cxon {\
        template <typename X, typename II, typename Cx>\
            inline auto read_value(Type& t, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {\
                using T = Type;\
                static CXON_CBOR_CLS_CONSTEXPR auto f = cbor::cls::make_fields(__VA_ARGS__);\
                return  cbor::tag::read<X>(i, e, cx) &&\
                        cbor::cls::read_fields<X>(t, f, i, e, cx)\
                ;\
            }\
    }
#define CXON_CBOR_CLS_WRITE(Type, ...)\
    namespace cxon {\
        template <typename X, typename O, typename Cx>\
            inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, CBOR> {\
                using T = Type;\
                static CXON_CBOR_CLS_CONSTEXPR auto f = cbor::cls::make_fields(__VA_ARGS__);\
                return cbor::cls::write_fields<X>(o, t, f, cx);\
            }\
    }
#define CXON_CBOR_CLS(Type, ...)\
    CXON_CBOR_CLS_READ(Type, __VA_ARGS__)\
    CXON_CBOR_CLS_WRITE(Type, __VA_ARGS__)

#define CXON_CBOR_CLS_READ_MEMBER(Type, ...)\
    template <typename X, typename II, typename Cx>\
        static auto read_value(Type& t, II& i, II e, Cx& cx) -> cxon::enable_for_t<X, cxon::CBOR> {\
            using T = Type;\
            static CXON_CBOR_CLS_CONSTEXPR auto f = cxon::cbor::cls::make_fields(__VA_ARGS__);\
            return  cxon::cbor::tag::read<X>(i, e, cx) &&\
                    cxon::cbor::cls::read_fields<X>(t, f, i, e, cx)\
            ;\
        }
#define CXON_CBOR_CLS_WRITE_MEMBER(Type, ...)\
    template <typename X, typename O, typename Cx>\
        static auto write_value(O& o, const Type& t, Cx& cx) -> cxon::enable_for_t<X, cxon::CBOR> {\
            using T = Type;\
            static CXON_CBOR_CLS_CONSTEXPR auto f = cxon::cbor::cls::make_fields(__VA_ARGS__);\
            return cxon::cbor::cls::write_fields<X>(o, t, f, cx);\
        }
#define CXON_CBOR_CLS_MEMBER(Type, ...)\
    CXON_CBOR_CLS_READ_MEMBER(Type, __VA_ARGS__)\
    CXON_CBOR_CLS_WRITE_MEMBER(Type, __VA_ARGS__)

#endif // CXON_CBOR_CLASS_HXX_
