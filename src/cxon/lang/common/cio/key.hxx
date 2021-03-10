// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_KEY_HXX_
#define CXON_CIO_KEY_HXX_

#include "cio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { // key read/write helpers

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx);

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx);

}}

namespace cxon { namespace cio { // key quoting

    template <typename T> struct is_quoted;

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio {

    template <typename T> struct is_quoted : std::false_type {};
#   define CXON_QUOTED(T)\
        template <size_t N>         struct is_quoted<T[N]>                              : std::true_type  {};\
        template <size_t N>         struct is_quoted<const T[N]>                        : std::true_type  {};\
        template <>                 struct is_quoted<T*>                                : std::true_type  {};\
        template <>                 struct is_quoted<const T*>                          : std::true_type  {};
        CXON_QUOTED(char)
        CXON_QUOTED(wchar_t)
#       if __cplusplus > 201703L /* C++20 */
            CXON_QUOTED(char8_t)
#       endif
        CXON_QUOTED(char16_t)
        CXON_QUOTED(char32_t)
#   undef CXON_QUOTED

}}

namespace cxon { namespace cio {

    namespace bits {

        // read

        template <typename X>
            struct key_read {
                template <typename T, typename II, typename Cx>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value, bool>
                    {
                        return read_value<X>(t, i, e, cx);
                    }
                template <typename T, typename II, typename Cx>
                    static auto value(T& t, II& i, II e, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value, bool>
                    {
                        return  cio::consume<X>(X::string::beg, i, e, cx) &&
                                    read_value<X>(t, i, e, cx) &&
                                cio::consume<X>(X::string::end, i, e, cx)
                        ;
                    }
            };

        // write

        template <typename X>
            struct key_write {
                template <typename T, typename O, typename Cx>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t< is_quoted<T>::value, bool>
                    {
                        return write_value<X>(o, t, cx);
                    }
                template <typename T, typename O, typename Cx>
                    static auto value(O& o, const T& t, Cx& cx)
                        -> enable_if_t<!is_quoted<T>::value, bool>
                    {
                        return  cio::poke<X>(o, X::string::beg, cx) &&
                                    write_value<X>(o, t, cx) &&
                                cio::poke<X>(o, X::string::end, cx)
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

#endif // CXON_CIO_KEY_HXX_
