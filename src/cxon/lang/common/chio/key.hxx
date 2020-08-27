// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHIO_KEY_HXX_
#define CXON_CHIO_KEY_HXX_

#include "chio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace chio { // key read/write helpers

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx);

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx);

}}

namespace cxon { namespace chio { // key quoting

    template <typename T> struct is_quoted;
    template <typename S> struct UQKEY;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace chio {

    template <typename T> struct is_quoted : std::false_type {};
#   define CXON_QUOTED(T)\
        template <size_t N>         struct is_quoted<T[N]>                              : std::true_type  {};\
        template <size_t N>         struct is_quoted<const T[N]>                        : std::true_type  {};\
        template <>                 struct is_quoted<T*>                                : std::true_type  {};\
        template <>                 struct is_quoted<const T*>                          : std::true_type  {};
        CXON_QUOTED(char)
        CXON_QUOTED(char16_t)
        CXON_QUOTED(char32_t)
        CXON_QUOTED(wchar_t)
#   undef CXON_QUOTED

    template <typename S> struct UQKEY : S { using X = S; };

}}

namespace cxon { namespace chio {

    namespace bits {

        // read

        template <typename X> struct key_read;

        template <typename X, template <typename> class S>
            struct key_read<S<X>> {
                template <typename T, typename II, typename Cx, typename E = S<X>>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                    {
                        return read_value<S<UQKEY<X>>>(t, i, e, cx);
                    }
                template <typename T, typename II, typename Cx, typename E = S<X>>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value && !E::map::unquoted_keys, bool>
                    {
                        return read_value<E>(t, i, e, cx);
                    }
                template <typename T, typename II, typename Cx, typename E = S<X>>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                    {
                        return read_value<E>(t, i, e, cx);
                    }
                template <typename T, typename II, typename Cx, typename E = S<X>>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value && !E::map::unquoted_keys, bool>
                    {
                        return  chio::consume<E>(E::string::beg, i, e, cx) &&
                                    read_value<E>(t, i, e, cx) &&
                                chio::consume<E>(E::string::end, i, e, cx)
                        ;
                    }
            };

        // write

        template <typename X> struct key_write;

        template <typename X, template <typename> class S>
            struct key_write<S<X>> {
                template <typename T, typename O, typename Cx, typename E = S<X>>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                    {
                        return write_value<S<UQKEY<X>>>(o, t, cx);
                    }
                template <typename T, typename O, typename Cx, typename E = S<X>>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value && !E::map::unquoted_keys, bool>
                    {
                        return write_value<E>(o, t, cx);
                    }
                template <typename T, typename O, typename Cx, typename E = S<X>>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value &&  E::map::unquoted_keys, bool>
                    {
                        return write_value<E>(o, t, cx);
                    }
                template <typename T, typename O, typename Cx, typename E = S<X>>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value && !E::map::unquoted_keys, bool>
                    {
                        return  chio::poke<E>(o, E::string::beg, cx) &&
                                    write_value<E>(o, t, cx) &&
                                chio::poke<E>(o, E::string::end, cx)
                        ;
                    }
            };

    }

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx) {
            return bits::key_read<X>::value(t, i, e, cx) && consume<X>(X::map::div, i, e, cx);
        }

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx) {
            return bits::key_write<X>::value(o, t, cx) && poke<X>(o, X::map::div, cx);
        }

}}

#endif // CXON_CHIO_KEY_HXX_
