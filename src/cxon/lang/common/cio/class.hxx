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

#ifndef CXON_CIO_CLASS_HXX_
#define CXON_CIO_CLASS_HXX_

#include "cio.hxx"
#include "key.hxx"
#include "value.hxx"
#include <tuple>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace cls { // structured types reader/writer construction helpers

    template <typename F, typename ...>
        struct field;
    template <typename T, std::size_t N, typename F = val::sink<>>
        constexpr auto make_field(const char (&name)[N], F f) -> field<F>;
    template <typename T, std::size_t N, typename F, typename D>
        constexpr auto make_field(const char (&name)[N], F f, D isdf) -> field<F, D>;

    template <typename ...Fs>
        using fields = std::tuple<Fs...>;
    template <typename ...Fs> 
        constexpr fields<Fs...> make_fields(Fs&&... fs);

    template <typename X, typename T, typename ...Fs, typename II, typename Cx>
        inline bool read_fields(T& t, const fields<Fs...>& fs, II& i, II e, Cx& cx);
    template <typename X, typename T, typename ...Fs, typename O, typename Cx>
        inline bool write_fields(O& o, const T& t, const fields<Fs...>& fs, Cx& cx);

}}}

namespace cxon { namespace cio { namespace cls { // traits

    // bare-class support types
    struct cxon_bare_class_tag {};
    struct bare_class;
    template <typename T>
        struct is_bare_class;

    // trivial-key-class support types
    struct cxon_nontrivial_key_class_tag {};
    template <typename T>
        struct is_trivial_key_class;
    template <typename X, typename ...Fs>
        constexpr bool are_trivial_key_fields(Fs&&... fs);

    // ordered-keys-class support types
    struct cxon_ordered_keys_class_tag {};
    template <typename T>
        struct is_ordered_keys_class;
    template <typename ...Fs>
        constexpr bool are_fields_ordered(Fs&&... fs);
    template <typename T> struct ordered_keys_traits : T {};

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace cls {

    // bare-object support types

    struct bare_class {
        using cxon_bare_class_tag = cls::cxon_bare_class_tag;
    };

    namespace imp {
        template <typename T, typename E = void_t<>>
            struct is_bare_class_                                               : std::false_type   {};
        template <typename T>
            struct is_bare_class_<T, void_t<typename T::cxon_bare_class_tag>>   : std::true_type    {};
    }
    template <typename T>
        struct is_bare_class : imp::is_bare_class_<T> {};

    // trivial-key-class support types

    namespace imp {
        template <typename T, typename E = void_t<>>
            struct is_trivial_key_class_                                                        : std::true_type    {};
        template <typename T>
            struct is_trivial_key_class_<T, void_t<typename T::cxon_nontrivial_key_class_tag>>  : std::false_type   {};
    }
    template <typename T>
        struct is_trivial_key_class : imp::is_trivial_key_class_<T> {};

    namespace imp {
        template <typename X>
            constexpr bool is_trivial_key_(const char* s, std::size_t n, unsigned i = 0) {
                return i == n || (!chr::imp::should_escape_<X>(s[i]) && is_trivial_key_<X>(s, n, i + 1));
            }
        template <typename X>
            constexpr bool are_trivial_key_fields_() {
                return true;
            }
        template <typename X, typename F, typename ...Fs>
            constexpr bool are_trivial_key_fields_(F&& f, Fs&&... fs) {
                return is_trivial_key_<X>(f.name, f.nale) && are_trivial_key_fields_<X>(std::forward<Fs>(fs)...);
            }
    }
    template <typename X, typename ...Fs>
        constexpr bool are_trivial_key_fields(Fs&&... fs) {
            return imp::are_trivial_key_fields_<X>(std::forward<Fs>(fs)...);
        }

    // ordered-keys-class support types

    namespace imp {
        template <typename T, typename E = void_t<>>
            struct is_ordered_keys_class_                                                       : std::false_type   {};
        template <typename T>
            struct is_ordered_keys_class_<T, void_t<typename T::cxon_ordered_keys_class_tag>>   : std::true_type    {};
    }
    template <typename T>
        struct is_ordered_keys_class : imp::is_ordered_keys_class_<T> {};

    namespace imp {
        template <typename ...Fs>
                constexpr bool are_fields_ordered_(Fs&&...) {
                return  true;
            }
        template <typename F, typename S, typename ...Fs>
            constexpr bool are_fields_ordered_(F&& f, S&& s, Fs&&... fs) {
                return  std::char_traits<char>::compare(f.name, s.name, f.nale + 1) <= 0 &&
                        are_fields_ordered_(std::forward<S>(s), std::forward<Fs>(fs)...)
                ;
            }
    }
    template <typename ...Fs>
        constexpr bool are_fields_ordered(Fs&&... fs) {
            return imp::are_fields_ordered_(std::forward<Fs>(fs)...) ;
        }

}}}

#if __cplusplus < 202002L // use of function template name with no prior declaration in function call with explicit template arguments is a C++20 extension [-Wc++20-extensions]
    namespace cxon {
        template <typename X, typename T, typename II, typename Cx> // read_field in cxon namespace
            inline bool read_field(T& t, const char* name, std::size_t nale, II& i, II e, Cx& cx);
    }
#endif

namespace cxon { namespace cio { namespace cls {

    template <typename F, typename ...>
        struct field {
            using type  = F;
            char const* name;
            std::size_t nale;
            type        unit;
            template <typename T>
                constexpr bool isdf(const T&) const noexcept { return false; }
        };
    template <typename F, typename D>
        struct field<F, D> {
            using type  = F;
            char const* name;
            std::size_t nale;
            type        unit;
            D           isdf;
        };

    template <typename T, std::size_t N, typename F>
        constexpr auto make_field(const char (&name)[N], F f) -> field<F> {
            return { name, N - 1, f };
        }
    template <typename T, std::size_t N, typename F, typename D>
        constexpr auto make_field(const char (&name)[N], F f, D isdf) -> field<F, D> {
            return { name, N - 1, f, isdf };
        }

    template <typename ...Fs>
        constexpr fields<Fs...> make_fields(Fs&&... fs) {
            return std::make_tuple(std::forward<Fs>(fs)...);
        }

    // read

    namespace imp {

        template <typename T, typename F>
            constexpr auto field_value_(T& t, const F& f)
                -> std::enable_if_t< std::is_member_pointer<typename F::type>::value, decltype(t.*f.unit)&>
            { return t.*f.unit; }
        template <typename T, typename F>
            constexpr auto field_value_(T&, const F& f)
                -> std::enable_if_t<!std::is_member_pointer<typename F::type>::value, decltype(*f.unit)&>
            { return   *f.unit; }

        template <typename X, typename T, typename F, typename II, typename Cx>
            inline auto read_field_(T& t, const F& f, II& i, II e, Cx& cx)
                -> std::enable_if_t<!val::is_sink<typename F::type>::value, bool>
            {
                return  read_map_val<X>(field_value_(t, f), i, e, cx);
            }
        template <typename X, typename T, typename F, typename II, typename Cx>
            inline auto read_field_(T&, const F& f, II& i, II e, Cx& cx)
                -> std::enable_if_t< val::is_sink<typename F::type>::value, bool>
            {
                return  val::sink_read<X>(f.unit, i, e, cx);
            }

        namespace imp {
            namespace imp { // binary search

                template <typename X, int L, int R, typename EE = void_t<>>
                    struct read_ordered_ {
                        template <std::size_t Ix, typename T, typename ...Fs, typename II, typename Cx, typename Y = X>
                            static auto field_(T& t, const fields<Fs...>& fs, int (&st)[sizeof...(Fs)], II& i, II e, Cx& cx)
                                -> std::enable_if_t< Y::assume_unique_object_keys, bool>
                            {
                                return st[Ix] == 0 && read_field_<Y>(t, std::get<Ix>(fs), i, e, cx) && (st[Ix] = 1, true);
                            }
                        template <std::size_t Ix, typename T, typename ...Fs, typename II, typename Cx, typename Y = X>
                            static auto field_(T& t, const fields<Fs...>& fs, int (&)[sizeof...(Fs)], II& i, II e, Cx& cx)
                                -> std::enable_if_t<!Y::assume_unique_object_keys, bool>
                            {
                                return read_field_<Y>(t, std::get<Ix>(fs), i, e, cx);
                            }
                        template <typename T, typename ...Fs, typename II, typename Cx>
                            static bool field(T& t, const char* name, const fields<Fs...>& fs, int (&st)[sizeof...(Fs)], II& i, II e, Cx& cx) {
                                constexpr auto M = (L + R) / 2;
                                int const c = std::char_traits<char>::compare(std::get<M>(fs).name, name, std::get<M>(fs).nale + 1);
                                     if (c < 0) return read_ordered_<X, M + 1, R>::field(t, name, fs, st, i, e, cx);
                                else if (c > 0) return read_ordered_<X, L, M - 1>::field(t, name, fs, st, i, e, cx);
                                else            return field_<M>(t, fs, st, i, e, cx);
                            }
                    };
                template <typename X, int L, int R>
                    struct read_ordered_<X, L, R, void_t<std::enable_if_t<R < L>>> {
                        template <typename T, typename ...Fs, typename II, typename Cx, typename Y = X>
                            static constexpr bool field(T&, const char*, const fields<Fs...>&, int (&)[sizeof...(Fs)], II&, II, Cx&) noexcept {
                                return false;
                            }
                    };

            }
            namespace imp { // linear search

                template <typename X, std::size_t N, std::size_t L>
                    struct read_ {
                        template <typename T, typename Fs, typename II, typename Cx, typename Y = X>
                            static auto field(T& t, const char* name, const Fs& fs, int (&st)[L], II& i, II e, Cx& cx)
                                -> std::enable_if_t< Y::assume_unique_object_keys, bool>
                            {
                                return st[N] == 0 && std::char_traits<char>::compare(std::get<N>(fs).name, name, std::get<N>(fs).nale + 1) == 0 ?
                                    (st[N] = 1, read_field_<Y>(t, std::get<N>(fs), i, e, cx)) :
                                    read_<Y, N + 1, L>::field(t, name, fs, st, i, e, cx)
                                ;
                            }
                        template <typename T, typename Fs, typename II, typename Cx, typename Y = X>
                            static auto field(T& t, const char* name, const Fs& fs, int (&st)[L], II& i, II e, Cx& cx)
                                -> std::enable_if_t<!Y::assume_unique_object_keys, bool>
                            {
                                return std::char_traits<char>::compare(std::get<N>(fs).name, name, std::get<N>(fs).nale + 1) == 0 ?
                                    read_field_<Y>(t, std::get<N>(fs), i, e, cx) :
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

            }
            template <typename X, typename T, typename Fs, typename II, typename Cx, std::size_t N>
                inline bool read_field_(option<0>, T& t, const char* name, const Fs& fs, int (&st)[N], II& i, II e, Cx& cx) {
                    //CXON_IF_CONSTEXPR (has_traits<X, ordered_keys_traits>::value && N > 8)
                    //    return imp::read_ordered_<X, 0, N - 1>::field(t, name, fs, st, i, e, cx);
                    return imp::read_<X, 0, N>::field(t, name, fs, st, i, e, cx);
                }

            template <typename X, typename T, typename Fs, typename II, typename Cx, std::size_t N> // read_field in cxon namespace
                inline auto read_field_(option<1>, T&, const char* name, const Fs&, int (&)[N], II& i, II e, Cx& cx)
                    -> decltype(read_field<X>(name, 0, i, e, cx), bool())
                {   CXON_ASSERT(!X::assume_unique_object_keys, "not supported");
                    return read_field<X>(name, std::char_traits<char>::length(name), i, e, cx);
                }
            template <typename X, typename T, typename Fs, typename II, typename Cx, std::size_t N> // read_field static method of T
                inline auto read_field_(option<2>, T& t, const char* name, const Fs&, int (&)[N], II& i, II e, Cx& cx)
                    -> decltype(T::template read_field<X>(t, name, 0, i, e, cx), bool())
                {   CXON_ASSERT(!X::assume_unique_object_keys, "not supported");
                    return T::template read_field<X>(t, name, std::char_traits<char>::length(name), i, e, cx);
                }
            template <typename X, typename T, typename Fs, typename II, typename Cx, std::size_t N> // read_field method of T
                inline auto read_field_(option<3>, T& t, const char* name, const Fs&, int (&)[N], II& i, II e, Cx& cx)
                    -> decltype(t.template read_field<X>(name, 0, i, e, cx), bool())
                {   CXON_ASSERT(!X::assume_unique_object_keys, "not supported");
                    return t.template read_field<X>(name, std::char_traits<char>::length(name), i, e, cx);
                }
        }
        template <typename X, typename T, typename Fs, typename II, typename Cx, std::size_t N>
            inline bool read_field_(T& t, const char* name, const Fs& fs, int (&st)[N], II& i, II e, Cx& cx) {
                using Y = unbind_traits_t<X, cio::key::simple_traits>;
                return imp::read_field_<Y>(option<3>(), t, name, fs, st, i, e, cx);
            }

        template <typename X, typename T, typename ...Fs, typename II, typename Cx>
            inline auto read_fields_(T& t, const fields<Fs...>& fs, II& i, II e, Cx& cx)
                -> std::enable_if_t<!is_bare_class<T>::value, bool>
            {
                if (!consume<X>(X::map::beg, i, e, cx))
                    return false;
                int st[sizeof...(Fs)];
                    CXON_IF_CONSTEXPR (X::assume_unique_object_keys)
                        std::fill_n(st, sizeof...(Fs), 0);
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
                -> std::enable_if_t< is_bare_class<T>::value, bool>
            {
                if (!consume<X>(i, e, cx)) return false;
                    if (i == e) return true;
                int st[sizeof...(Fs)];
                    CXON_IF_CONSTEXPR (X::assume_unique_object_keys)
                        std::fill_n(st, sizeof...(Fs), 0);
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
        template <typename X, typename T, typename II, typename Cx>
            inline constexpr bool read_fields_(T&, const fields<>&, II& i, II e, Cx& cx) {
                return consume<X>(X::map::beg, i, e, cx) && consume<X>(X::map::end, i, e, cx);
            }

    }

    template <typename X, typename T, typename ...Fs, typename II, typename Cx>
        inline bool read_fields(T& t, const fields<Fs...>& fs, II& i, II e, Cx& cx) {
            return imp::read_fields_<X>(t, fs, i, e, cx);
        }

    // write

    namespace imp {

        template <typename X, typename O, typename T, typename F, typename Cx>
            inline auto write_field_(O& o, const T& t, const F& f, Cx& cx)
                -> std::enable_if_t<!val::is_sink<typename F::type>::value, bool>
            {
                using Y = unbind_traits_t<X, cio::key::simple_traits>;
                return  write_map_key<X>(o, f.name, f.nale, cx) &&
                        write_map_val<Y>(o, field_value_(t, f), cx)
                ;
            }
        template <typename X, typename O, typename T, typename F, typename Cx>
            constexpr auto write_field_(O&, const T&, const F&, Cx&) noexcept
                -> std::enable_if_t< val::is_sink<typename F::type>::value, bool>
            {
                return true;
            }

        namespace imp {

            template <typename X, typename T, typename O, typename Cx>
                inline auto write_sep_(O& o, Cx& cx) -> std::enable_if_t<!is_bare_class<T>::value, bool> {
                    return poke<X>(o, X::map::sep, cx);
                }
            template <typename X, typename T, typename O, typename Cx>
                inline auto write_sep_(O& o, Cx& cx) -> std::enable_if_t< is_bare_class<T>::value, bool> {
                    return poke<X>(o, '\n', cx);
                }

            template <typename X, std::size_t N, std::size_t L>
                struct write_next_ {
                    template <typename T, typename Fs, typename O, typename Cx>
                        static bool field(O& o, const T& t, const Fs& fs, Cx& cx) {
                            using E = typename std::tuple_element<N, Fs>::type;
                            CXON_IF_CONSTEXPR (!val::is_sink<typename E::type>::value)
                                if (!std::get<N>(fs).isdf(t))
                                    return  write_sep_<X, T>(o, cx) && write_field_<X>(o, t, std::get<N>(fs), cx) &&
                                            write_next_<X, N + 1, L>::field(o, t, fs, cx)
                                    ;
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
                            CXON_IF_CONSTEXPR (!val::is_sink<typename E::type>::value)
                                if (!std::get<N>(fs).isdf(t))
                                    return  write_field_<X>(o, t, std::get<N>(fs), cx) &&
                                            write_next_<X, N + 1, L>::field(o, t, fs, cx)
                                    ;
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

        }
        template <typename X, typename T, typename Fs, typename O, typename Cx>
            constexpr bool write_fields_bare_(O& o, const T& t, const Fs& fs, Cx& cx) {
                return imp::write_<X, 0, std::tuple_size<Fs>::value>::field(o, t, fs, cx);
            }

        template <typename X, typename T, typename ...Fs, typename O, typename Cx>
            inline auto write_fields_(O& o, const T& t, const fields<Fs...>& fs, Cx& cx)
                -> std::enable_if_t<!is_bare_class<T>::value, bool>
            {
                return  poke<X>(o, X::map::beg, cx) &&
                            write_fields_bare_<X>(o, t, fs, cx) &&
                        poke<X>(o, X::map::end, cx)
                ;
            }
        template <typename X, typename T, typename ...Fs, typename O, typename Cx>
            inline auto write_fields_(O& o, const T& t, const fields<Fs...>& fs, Cx& cx)
                -> std::enable_if_t< is_bare_class<T>::value, bool>
            {
                return  write_fields_bare_<X>(o, t, fs, cx);
            }

    }

    template <typename X, typename T, typename ...Fs, typename O, typename Cx>
        inline bool write_fields(O& o, const T& t, const fields<Fs...>& fs, Cx& cx) {
            return imp::write_fields_<X>(o, t, fs, cx);
        }

}}}

#endif // CXON_CIO_CLASS_HXX_
