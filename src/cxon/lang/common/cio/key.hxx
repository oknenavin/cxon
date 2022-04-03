// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_KEY_HXX_
#define CXON_CIO_KEY_HXX_

#include "cio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { // key read/write helpers

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx); // <key>:

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx); // <key>:

}}

namespace cxon { namespace cio { namespace key { // key read/write extension points

    template <typename T>
        struct is_quoted;

    template <typename X, typename T>
        struct read;

    template <typename X, typename T>
        struct write;

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx); // <key>

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx); // <key>

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio {

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx) {
            return key::read<X, T>::key(t, i, e, cx) && consume<X>(X::map::div, i, e, cx);
        }

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx) {
            return key::write<X, T>::key(o, t, cx) && poke<X>(o, X::map::div, cx);
        }

}}

namespace cxon { namespace cio { namespace key {

    template <typename T> struct is_quoted : std::false_type {};
#   define CXON_QUOTED(T)\
        template <>                 struct is_quoted<T>             : std::true_type  {};\
        template <std::size_t N>    struct is_quoted<T[N]>          : std::true_type  {};\
        template <std::size_t N>    struct is_quoted<const T[N]>    : std::true_type  {};\
        template <>                 struct is_quoted<T*>            : std::true_type  {};\
        template <>                 struct is_quoted<const T*>      : std::true_type  {};
        CXON_QUOTED(char)
        CXON_QUOTED(wchar_t)
#       if __cplusplus > 201703L /* C++20 */
            CXON_QUOTED(char8_t)
#       endif
        CXON_QUOTED(char16_t)
        CXON_QUOTED(char32_t)
#   undef CXON_QUOTED

}}}

namespace cxon { namespace cio { namespace key {

    template <typename T>
        struct traits : T {
            struct string : T::string {
                template <typename II>
                    static bool del_read(II& i, II e) {
                        return i != e && *i == '\\' && ++i != e && *i == string::del && (++i, true);
                    }
                template <typename O>
                    static bool del_write(O& o) {
                        return poke(o, '\\') && poke(o, string::del);
                    }
            };
        };
    template <typename X>
        using key_context = typename std::conditional<is_key_context<X>::value, X, rebind_traits_t<X, traits>>::type;

}}}

namespace cxon { namespace cio { namespace key {

    namespace imp {

        template <typename X, typename T, typename II, typename Cx>
            inline auto read_key_(T& t, II& i, II e, Cx& cx) -> enable_if_t<!is_quoted<T>::value && !X::unquoted_keys, bool> {
                return  consume<X>(X::string::template del_read<II>, i, e, cx) &&
                            read_value<key_context<X>>(t, i, e, cx) &&
                        consume<X>(X::string::template del_read<II>, i, e, cx)
                ;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline auto read_key_(T& t, II& i, II e, Cx& cx) -> enable_if_t< is_quoted<T>::value ||  X::unquoted_keys, bool> {
                return  read_value<X>(t, i, e, cx);
            }

    }
    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx) {
            return imp::read_key_<X>(t, i, e, cx);
        }

    template <typename X, typename T>
        struct read {
            template <typename II, typename Cx>
                static bool key(T& t, II& i, II e, Cx& cx) {
                    return key::read_key<X>(t, i, e, cx);
                }
        };

    namespace imp {

        template <typename X, typename T, typename O, typename Cx>
            inline auto write_key_(O& o, const T& t, Cx& cx) -> enable_if_t<!is_quoted<T>::value && !X::unquoted_keys, bool> {
                return  poke<X>(o, X::string::template del_write<O>, cx) &&
                            write_value<key_context<X>>(o, t, cx) &&
                        poke<X>(o, X::string::template del_write<O>, cx)
                ;
            }
        template <typename X, typename T, typename O, typename Cx>
            inline auto write_key_(O& o, const T& t, Cx& cx) -> enable_if_t< is_quoted<T>::value ||  X::unquoted_keys, bool> {
                return  write_value<X>(o, t, cx);
            }

    }
    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx) {
            return imp::write_key_<X>(o, t, cx);
        }

    template <typename X, typename T>
        struct write {
            template <typename O, typename Cx>
                static bool key(O& o, const T& t, Cx& cx) {
                    return key::write_key<X>(o, t, cx);
                }
        };

}}}

#endif // CXON_CIO_KEY_HXX_
