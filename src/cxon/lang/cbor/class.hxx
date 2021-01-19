// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_CLASS_HXX_
#define CXON_CBOR_CLASS_HXX_

#include <tuple>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cls {

    template <typename T = void>    struct sink;
    template <typename T>           struct is_sink;

    template <typename X, typename S>
        struct sink_reader;

    template <typename X, typename S, typename II, typename Cx>
        inline bool sink_read(S& s, II& i, II e, Cx& cx);

}}}

namespace cxon { namespace cbor { namespace cls {

    template <typename F>
        struct field;
    template <typename F = sink<>>
        constexpr field<F> make_field(const char* name, F f = {});

    template <typename X, typename S, typename F, typename II, typename Cx>
        inline bool read_field(S& s, F f, II& i, II e, Cx& cx);
    template <typename X, typename O, typename S, typename F, typename Cx>
        inline bool write_field(O& o, const S& s, F f, Cx& cx);

    template <typename ...F>
        using fields = std::tuple<F...>;
    template <typename ...F> 
        constexpr fields<F...> make_fields(F... f);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cls { // sink

    template <typename X, typename S>
        struct sink_reader<CBOR<X>, S> {
            template<typename II, typename Cx>
                static bool read(S& s, II& i, II e, Cx& cx) {
                    auto t = std::back_inserter(s);
                    auto const b = bio::peek(i, e);
                    switch (b & X::mjr) {
                        case X::pint: case X::nint: case X::tag: case X::svn: {
                            II const o = i;
                            switch (b & X::mnr) {
                                case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
                                case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F:
                                case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
                                    return  bio::get(t, i, e);
                                case 0x18: case 0x19: case 0x1A: case 0x1B:
                                    return  (bio::get(t, i, e, 1 + (1 << (b - 0x18)))) ||
                                            (bio::rewind(i, o), cx|cbor::read_error::unexpected)
                                    ;
                                case 0x1C: case 0x1D: case 0x1E: case 0x1F:
                                    return  (bio::rewind(i, o), cx|cbor::read_error::unexpected);
                                default:
                                    return  CXON_ASSERT(0, "unexpected"), false;
                            }
                        }
                        case X::bstr: case X::tstr:
                            return CXON_ASSERT(0, "TODO"), false;
                        case X::arr:
                            return CXON_ASSERT(0, "TODO"), false;
                        case X::map:
                            return CXON_ASSERT(0, "TODO"), false;
                    }
                    return false;
                }

            //private:
            //    template <typename N, typename II>
            //        static auto skip(N n, II& i, II e)
            //            -> enable_if_t<!is_random_access_iterator<II>::value, bool>
            //        {
            //            for ( ; n != 0 && i != e; --n, ++i)
            //                ;
            //            return n == 0;
            //        }
            //    template <typename N, typename II>
            //        static auto skip(N n, II& i, II e)
            //            -> enable_if_t< is_random_access_iterator<II>::value, bool>
            //        {
            //            using U = typename std::common_type<N, typename std::iterator_traits<II>::difference_type>::type;
            //            auto const m = std::min((U)n, (U)std::distance(i, e));
            //            return std::advance(i, m), m == n;
            //        }
        };

}}}

namespace cxon { namespace cbor { namespace cls {

    template <typename T>
        struct sink {
            using value_type = typename T::value_type;
            void push_back(value_type v) { sink_.push_back(v); }
            T sink_;
        };
    template <>
        struct sink<void> {
            using value_type = bio::byte;
            void push_back(value_type) {}
        };

    template <typename T>
        struct is_sink          : std::false_type {};
    template <typename T>
        struct is_sink<sink<T>> : std::true_type {};

    template <typename X, typename S, typename II, typename Cx>
        inline bool sink_read(S& s, II& i, II e, Cx& cx) {
            return sink_reader<X, S>::read(s, i, e, cx);
        }

}}}

namespace cxon { namespace cbor { namespace cls {

    // field

    template <typename F>
        struct field {
            using type = F;
            char const*const name;
            type value;
        };

    template <typename O>
        struct field<sink<O>> {
            using type = sink<O>;
            char const*const name;
            type value;
        };

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

        template <typename X, typename O, typename S, typename F, typename Cx>
            constexpr auto write_field_(O&, const S&, F, Cx&)
                -> enable_if_t< is_sink<typename F::type>::value, bool>
            {
                return true;
            }

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
        constexpr fields<F...> make_fields(F... f) {
            return { f... };
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
                            return cx && (bio::rewind(i, o), cx|cbor::read_error::unexpected);
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
                static constexpr auto f = cbor::cls::make_fields(__VA_ARGS__);\
                return cbor::cls::read_fields<X>(t, f, i, e, cx);\
            }\
    }
#define CXON_CBOR_CLS_WRITE(Type, ...)\
    namespace cxon {\
        template <typename X, typename O, typename Cx>\
            inline auto write_value(O& o, const Type& t, Cx& cx) -> enable_for_t<X, CBOR> {\
                using T = Type;\
                static constexpr auto f = cbor::cls::make_fields(__VA_ARGS__);\
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
            static constexpr auto f = cxon::cbor::cls::make_fields(__VA_ARGS__);\
            return cxon::cbor::cls::read_fields<X>(t, f, i, e, cx);\
        }
#define CXON_CBOR_CLS_WRITE_MEMBER(Type, ...)\
    template <typename X, typename O, typename Cx>\
        static auto write_value(O& o, const Type& t, Cx& cx) -> cxon::enable_for_t<X, cxon::CBOR> {\
            using T = Type;\
            static constexpr auto f = cxon::cbor::cls::make_fields(__VA_ARGS__);\
            return cxon::cbor::cls::write_fields<X>(o, t, f, cx);\
        }
#define CXON_CBOR_CLS_MEMBER(Type, ...)\
    CXON_CBOR_CLS_READ_MEMBER(Type, __VA_ARGS__)\
    CXON_CBOR_CLS_WRITE_MEMBER(Type, __VA_ARGS__)

#endif // CXON_CBOR_CLASS_HXX_
