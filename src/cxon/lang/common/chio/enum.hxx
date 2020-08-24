// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHIO_ENUM_HXX_
#define CXON_CHIO_ENUM_HXX_

#include "chio.hxx"
#include <cstring> // strcmp

namespace cxon { namespace chio { namespace enm { // enum reader/writer construction helpers

    template <typename E>
        struct value;
    template <typename E>
        constexpr value<E> make_value(const char* name, E value);

    template <typename X, typename E, typename V, typename II, typename Cx>
        inline bool read_value(E& t, V vb, V ve, II& i, II e, Cx& cx);

    template <typename X, typename E, typename V, typename O, typename Cx>
        inline bool write_value(O& o, E t, V vb, V ve, Cx& cx);

}}}

// implementation /////////////////////////////////////////////////////////////

namespace cxon { namespace chio { namespace enm {

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
        inline bool read_value(E& t, V vb, V ve, II& i, II e, Cx& cx) {
            consume<X>(i, e);
            II const o = i;
                char id[ids_len_max::constant<napa_type<Cx>>(64)];
                    if (!cxon::read_value<X>(id, i, e, cx)) return false;
                for ( ; vb != ve; ++vb) if (std::strcmp(vb->name, id) == 0)
                    return t = vb->value, true;
            return rewind(i, o), cx|read_error::unexpected;
        }

    template <typename X, typename E, typename V, typename O, typename Cx>
        inline bool write_value(O& o, E t, V vb, V ve, Cx& cx) {
            for ( ; vb != ve; ++vb) if (t == vb->value)
                return poke<X>(o, X::string::beg, cx) && poke<X>(o, vb->name, cx) && poke<X>(o, X::string::end, cx);
            return cx|write_error::argument_invalid;
        }

}}}

#endif // CXON_CHIO_ENUM_HXX_
