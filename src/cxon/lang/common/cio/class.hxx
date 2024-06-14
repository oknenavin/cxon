// Copyright (c) 2017-2024 oknenavin.
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
    template <typename T, std::size_t N, typename F = val::sink<>>
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

    template <typename X, typename T, typename ...Fs, typename II, typename Cx>
        inline bool read_fields(T& t, const fields<Fs...>& fs, II& i, II e, Cx& cx);
    template <typename X, typename T, typename ...Fs, typename O, typename Cx>
        inline bool write_fields(O& o, const T& t, const fields<Fs...>& fs, Cx& cx);

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
                -> enable_if_t<!val::is_sink<typename F::type>::value, bool>
            {
                return  read_map_val<X>(field_value_(t, f), i, e, cx);
            }
        template <typename X, typename T, typename F, typename II, typename Cx>
            inline auto read_field_(T&, const F& f, II& i, II e, Cx& cx)
                -> enable_if_t< val::is_sink<typename F::type>::value, bool>
            {
                return  val::sink_read<X>(f.value, i, e, cx);
            }

        template <typename X, typename O, typename T, typename F, typename Cx>
            inline auto write_field_(O& o, const T& t, const F& f, Cx& cx)
                -> enable_if_t<!val::is_sink<typename F::type>::value, bool>
            {
                return  write_map_key<X>(o, f.name, f.nale, cx) &&
                        write_map_val<X>(o, field_value_(t, f), cx)
                ;
            }
        template <typename X, typename O, typename T, typename F, typename Cx>
            constexpr auto write_field_(O&, const T&, const F&, Cx&) noexcept
                -> enable_if_t< val::is_sink<typename F::type>::value, bool>
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
            return { std::forward<Fs>(fs)... };
        }

    // read

    namespace imp {

        template <typename X, std::size_t N, std::size_t L>
            struct read_ {
                template <typename T, typename Fs, typename II, typename Cx, typename Y = X>
                    static auto field(T& t, const char* name, const Fs& fs, int (&st)[L], II& i, II e, Cx& cx) -> enable_if_t< Y::assume_unique_object_keys, bool> {
                        return st[N] == 0 && std::char_traits<char>::compare(std::get<N>(fs).name, name, std::get<N>(fs).nale + 1) == 0 ?
                            (st[N] = 1, read_field<Y>(t, std::get<N>(fs), i, e, cx)) :
                            read_<Y, N + 1, L>::field(t, name, fs, st, i, e, cx)
                        ;
                    }
                template <typename T, typename Fs, typename II, typename Cx, typename Y = X>
                    static auto field(T& t, const char* name, const Fs& fs, int (&st)[L], II& i, II e, Cx& cx) -> enable_if_t<!Y::assume_unique_object_keys, bool> {
                        return std::char_traits<char>::compare(std::get<N>(fs).name, name, std::get<N>(fs).nale + 1) == 0 ?
                            read_field<Y>(t, std::get<N>(fs), i, e, cx) :
                            read_<Y, N + 1, L>::field(t, name, fs, st, i, e, cx)
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

        template <typename X, typename T, typename II, typename Cx>
            inline constexpr bool read_fields_(T&, const fields<>&, II& i, II e, Cx& cx) {
                return consume<X>(X::map::beg, i, e, cx) && consume<X>(X::map::end, i, e, cx);
            }

        template <typename X, typename T, typename ...Fs, typename II, typename Cx>
            inline auto read_fields_(T& t, const fields<Fs...>& fs, II& i, II e, Cx& cx)
                -> enable_if_t<!is_bare_class<T>::value, bool>
            {
                if (!consume<X>(X::map::beg, i, e, cx))
                    return false;
                int st[sizeof...(Fs)] = {0};
                for (char id[ids_len_max::constant<napa_type<Cx>>(64)]; ; ) {
                    if (!consume<X>(i, e, cx))
                        return false;
                    if (peek(i, e) == X::map::end)
                        return ++i, true;
                    II const o = i;
                        if (!read_map_key<X>(id, i, e, cx))
                            return rewind(i, o), false;
                        if (!read_field_<X>(t, id, fs, st, i, e, cx))
                            return cx && (rewind(i, o), cx/X::read_error::unexpected);
                        if (cnt::consume_sep<X, typename X::map>(i, e, cx))
                            continue;
                    return consume<X>(X::map::end, i, e, cx);
                }
                return true;
            }
        template <typename X, typename T, typename ...Fs, typename II, typename Cx>
            inline auto read_fields_(T& t, const fields<Fs...>& fs, II& i, II e, Cx& cx)
                -> enable_if_t< is_bare_class<T>::value, bool>
            {
                if (!consume<X>(i, e, cx)) return false;
                    if (i == e) return true;
                int st[sizeof...(Fs)] = {0};
                for (char id[ids_len_max::constant<napa_type<Cx>>(64)]; i != e && *i; ) {
                    II const o = i;
                        if (!read_map_key<X>(id, i, e, cx))
                            return false;
                        if (!read_field_<X>(t, id, fs, st, i, e, cx))
                            return cx && (rewind(i, o), cx/X::read_error::unexpected);
                        if (!consume<X>(i, e, cx))
                            return false;
                }
                return true;
            }

    }

    template <typename X, typename T, typename ...Fs, typename II, typename Cx>
        inline bool read_fields(T& t, const fields<Fs...>& fs, II& i, II e, Cx& cx) {
            return imp::read_fields_<X>(t, fs, i, e, cx);
        }

    // write

    namespace imp {

        template <typename X, typename T, typename O, typename Cx>
            inline auto write_sep_(O& o, Cx& cx) -> enable_if_t<!is_bare_class<T>::value, bool> {
                return poke<X>(o, X::map::sep, cx);
            }
        template <typename X, typename T, typename O, typename Cx>
            inline auto write_sep_(O& o, Cx& cx) -> enable_if_t< is_bare_class<T>::value, bool> {
                return poke<X>(o, '\n', cx);
            }

        template <typename X, std::size_t N, std::size_t L>
            struct write_next_ {
                template <typename T, typename Fs, typename O, typename Cx>
                    static bool field(O& o, const T& t, const Fs& fs, Cx& cx) {
                        using E = typename std::tuple_element<N, Fs>::type;
                        CXON_IF_CONSTEXPR (!val::is_sink<typename E::type>::value) {
                            if (!std::get<N>(fs).dflt(t))
                                return  write_sep_<X, T>(o, cx) && write_field<X>(o, t, std::get<N>(fs), cx) &&
                                        write_next_<X, N + 1, L>::field(o, t, fs, cx)
                                ;
                        }
                        return          write_next_<X, N + 1, L>::field(o, t, fs, cx);
                    }
            };
        template <typename X, std::size_t N>
            struct write_next_<X, N, N> {
                template <typename T, typename Fs, typename O, typename Cx>
                    static constexpr bool field(O&, const T&, const Fs&, Cx&) noexcept {
                        return true;
                    }
            };

        template <typename X, std::size_t N, std::size_t L>
            struct write_ {
                template <typename T, typename Fs, typename O, typename Cx>
                    static bool field(O& o, const T& t, const Fs& fs, Cx& cx) {
                        using E = typename std::tuple_element<N, Fs>::type;
                        CXON_IF_CONSTEXPR (!val::is_sink<typename E::type>::value) {
                            if (!std::get<N>(fs).dflt(t))
                                return  write_field<X>(o, t, std::get<N>(fs), cx) &&
                                        write_next_<X, N + 1, L>::field(o, t, fs, cx)
                                ;
                        }
                        return          write_<X, N + 1, L>::field(o, t, fs, cx);
                    }
            };
        template <typename X, std::size_t N>
            struct write_<X, N, N> {
                template <typename T, typename Fs, typename O, typename Cx>
                    static constexpr bool field(O&, const T&, const Fs&, Cx&) noexcept {
                        return true;
                    }
            };

        template <typename X, typename T, typename Fs, typename O, typename Cx>
            constexpr bool write_fields_bare_(O& o, const T& t, const Fs& fs, Cx& cx) {
                return write_<X, 0, std::tuple_size<Fs>::value>::field(o, t, fs, cx);
            }

        template <typename X, typename T, typename ...Fs, typename O, typename Cx>
            inline auto write_fields_(O& o, const T& t, const fields<Fs...>& fs, Cx& cx) -> enable_if_t<!is_bare_class<T>::value, bool> {
                return  poke<X>(o, X::map::beg, cx) &&
                            write_fields_bare_<X>(o, t, fs, cx) &&
                        poke<X>(o, X::map::end, cx)
                ;
            }
        template <typename X, typename T, typename ...Fs, typename O, typename Cx>
            inline auto write_fields_(O& o, const T& t, const fields<Fs...>& fs, Cx& cx) -> enable_if_t< is_bare_class<T>::value, bool> {
                return  write_fields_bare_<X>(o, t, fs, cx);
            }

    }

    template <typename X, typename T, typename ...Fs, typename O, typename Cx>
        inline bool write_fields(O& o, const T& t, const fields<Fs...>& fs, Cx& cx) {
            return imp::write_fields_<X>(o, t, fs, cx);
        }

}}}

#endif // CXON_CIO_CLASS_HXX_
