// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_KEY_HXX_
#define CXON_CIO_KEY_HXX_

#include "cio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { // key read/write helpers: <key>:

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_map_key(T& t, II& i, II e, Cx& cx);

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_map_key(O& o, const T& t, Cx& cx);
    template <typename X, typename T, typename O, typename Cx>
        inline auto write_map_key(O& o, const T* t, std::size_t s, Cx& cx) -> enable_if_t<is_char<T>::value, bool>;

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_map_val(T& t, II& i, II e, Cx& cx);

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_map_val(O& o, const T& t, Cx& cx);

}}

namespace cxon { namespace cio { namespace key { // key read/write extension points: <key>

    template <typename X, typename T>
        struct read;

    template <typename X, typename T>
        struct write;

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx);

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx);

    template <typename T> struct simple_traits : T {};
    template <typename T>  using is_simple_key_context = has_traits<T, simple_traits>;

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio {

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_map_key(T& t, II& i, II e, Cx& cx) {
            return key::read<X, T>::key(t, i, e, cx) && consume<X>(X::map::div, i, e, cx);
        }

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_map_key(O& o, const T& t, Cx& cx) {
            return key::write<X, T>::key(o, t, cx) && poke<X>(o, X::map::div, cx);
        }

    template <typename X, typename T, typename O, typename Cx>
        inline auto write_map_key(O& o, const T* t, std::size_t s, Cx& cx)
            -> enable_if_t<is_char<T>::value, bool>
        {
            using Y = conditional_t<X::unquote_quoted_keys, quoted_key_context<X>, X>;
            using Z = conditional_t<key::is_simple_key_context<Y>::value, rebind_traits_t<Y, key::simple_traits, str::raw_traits>, Y>;
            return str::pointer_write<Z>(o, t, s, cx) && poke<Z>(o, Z::map::div, cx);
        }

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_map_val(T& t, II& i, II e, Cx& cx) {
            using Y = unbind_traits_t<X, key::quoted_traits>;
            return read_value<Y>(t, i, e, cx);
        }

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_map_val(O& o, const T& t, Cx& cx) {
            using Y = unbind_traits_t<X, key::quoted_traits>;
            return write_value<Y>(o, t, cx);
        }

}}

namespace cxon { namespace cio { namespace key {

    namespace imp {
        template <typename X, typename T>
            struct needs_quotes : bool_constant<!is_unquoted_key_context<X>::value && !is_string<T>::value && X::quote_unquoted_keys && !X::unquote_quoted_keys> {};
    }

    namespace imp {

        template <typename X, typename T, typename II, typename Cx>
            inline auto read_key_(T& t, II& i, II e, Cx& cx) -> enable_if_t< needs_quotes<X, T>::value, bool> {
                return  consume<X>(str::delim_be_read<X, II>, i, e, cx) &&
                            read_value<unquoted_key_context<X>>(t, i, e, cx) &&
                        consume<X>(str::delim_en_read<X, II>, i, e, cx)
                ;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline auto read_key_(T& t, II& i, II e, Cx& cx) -> enable_if_t<!needs_quotes<X, T>::value, bool> {
                using Y = conditional_t<X::unquote_quoted_keys, quoted_key_context<X>, X>;
                using Z = conditional_t<!is_unquoted_key_context<Y>::value && key::is_simple_key_context<Y>::value, rebind_traits_t<Y, key::simple_traits, str::raw_traits>, Y>;
                return  read_value<Z>(t, i, e, cx);
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
            inline auto write_key_(O& o, const T& t, Cx& cx) -> enable_if_t< needs_quotes<X, T>::value, bool> {
                return  poke<X>(o, str::delim_be_write<X, O>, cx) &&
                            write_value<unquoted_key_context<X>>(o, t, cx) &&
                        poke<X>(o, str::delim_en_write<X, O>, cx)
                ;
            }
        template <typename X, typename T, typename O, typename Cx>
            inline auto write_key_(O& o, const T& t, Cx& cx) -> enable_if_t<!needs_quotes<X, T>::value, bool> {
                using Y = conditional_t<X::unquote_quoted_keys, quoted_key_context<X>, X>;
                return  write_value<Y>(o, t, cx);
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
