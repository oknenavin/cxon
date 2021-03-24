// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_ENUM_HXX_
#define CXON_CIO_ENUM_HXX_

#include "cio.hxx"
#include <cstring> // strcmp

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace enm { // enum reader/writer construction helpers

    template <typename E>
        struct value;
    template <typename E>
        constexpr value<E> make_value(const char* name, E value);

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
            E const value;
        };
    template <typename E>
        constexpr value<E> make_value(const char* name, E value) {
            return { name, value };
        }

    template <typename X, typename E, typename V, typename II, typename Cx>
        inline bool read_value(E& t, V f, V l, II& i, II e, Cx& cx) {
            consume<X>(i, e);
            II const o = i;
            char id[ids_len_max::constant<napa_type<Cx>>(64)];
                if (!cxon::read_value<X>(id, i, e, cx))
                    return false;
            for ( ; f != l && std::strcmp(f->name, id) != 0; ++f)
                ;
            return  (f != l && (t = f->value, true)) ||
                    (rewind(i, o), cx|read_error::unexpected);
        }

    template <typename X, typename E, typename V, typename O, typename Cx>
        inline bool write_value(O& o, E t, V f, V l, Cx& cx) {
            for ( ; f != l && t != f->value; ++f)
                ;
            return f != l ?
                poke<X>(o, X::string::beg, cx) && poke<X>(o, f->name, cx) && poke<X>(o, X::string::end, cx) :
                cx|write_error::argument_invalid
            ;
        }

}}}

#endif // CXON_CIO_ENUM_HXX_
