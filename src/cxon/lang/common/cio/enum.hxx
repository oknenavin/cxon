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

#ifndef CXON_CIO_ENUM_HXX_
#define CXON_CIO_ENUM_HXX_

#include "cio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { // type traits
    template <typename T> struct is_string<T, std::enable_if_t<std::is_enum<T>::value>> : std::true_type {};
}}

namespace cxon { namespace cio { namespace enm { // enum reader/writer construction helpers

    template <typename E>
        struct value;
    template <typename E, std::size_t N>
        constexpr value<E> make_value(const char (&name)[N], E value);

    template <typename X, typename E, typename V, typename II, typename Cx>
        inline bool read_value(E& t, V f, V l, II& i, II e, Cx& cx);

    template <typename X, typename E, typename V, typename O, typename Cx>
        inline bool write_value(O& o, E t, V f, V l, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace enm {

    template <typename E>
        struct value {
            char const*const name;
            std::size_t nale;
            E const value;
        };
    template <typename E, std::size_t N>
        constexpr value<E> make_value(const char (&name)[N], E value) {
            return { name, N - 1, value };
        }

    template <typename X, typename E, typename V, typename II, typename Cx>
        inline bool read_value(E& t, V f, V l, II& i, II e, Cx& cx) {
            if (!consume<X>(i, e, cx))
                return false;
            II const o = i;
            char id[ids_len_max::constant<napa_type<Cx>>(64)];
                if (!cxon::read_value<X>(id, i, e, cx))
                    return false;
            for ( ; f != l && std::char_traits<char>::compare(f->name, id, f->nale + 1) != 0; ++f)
                ;
            return  (f != l && (t = f->value, true)) ||
                    (rewind(i, o), cx/X::read_error::unexpected);
        }

    template <typename X, typename E, typename V, typename O, typename Cx>
        inline bool write_value(O& o, E t, V f, V l, Cx& cx) {
            for ( ; f != l && t != f->value; ++f)
                ;
            return f != l ?
                str::pointer_write<X>(o, f->name, f->nale, cx) :
                cx/X::write_error::argument_invalid
            ;
        }

}}}

#endif // CXON_CIO_ENUM_HXX_
