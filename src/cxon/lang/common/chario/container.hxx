// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHARIO_CONTAINER_HXX_
#define CXON_CHARIO_CONTAINER_HXX_

#include "chario.hxx"
#include "bits/container.hxx"

namespace cxon { namespace chario { namespace container { // container read/write helpers

    namespace io = cxon::chario;

    // read

    template <typename X, typename Cr, typename II, typename Cx, typename EA>
        inline bool read(II& i, II e, Cx& cx, EA element_add) {
            if (!io::consume<X>(Cr::beg, i, e, cx)) return false;
            if ( io::consume<X>(Cr::end, i, e))     return true;
            return bits::list_read<X, Cr>(i, e, element_add), !cx.ec && io::consume<X>(Cr::end, i, e, cx);
        }

    // write

    template <typename X, typename Cr, typename O, typename II, typename Cx, typename L>
        inline bool write(O& o, II b, II e, Cx& cx, L element_write) {
            if (!io::poke<X>(o, Cr::beg, cx)) return false;
            return bits::list_write<X, Cr>(o, b, e, cx, element_write), !cx.ec && io::poke<X>(o, Cr::end, cx);
        }
    template <typename X, typename Cr, typename O, typename T, typename Cx, typename L>
        inline bool write(O& o, const T& t, Cx& cx, L element_write) {
            return write<X, Cr>(o, std::begin(t), std::end(t), cx, element_write);
        }
    template <typename X, typename Cr, typename O, typename II, typename Cx>
        inline bool write(O& o, II b, II e, Cx& cx) {
            return write<X, Cr>(o, b, e, cx, [&](const decltype(*b)& e) {
                return write_value<X>(o, e, cx);
            });
        }
    template <typename X, typename Cr, typename O, typename T, typename Cx>
        inline bool write(O& o, const T& t, Cx& cx) {
            return write<X, Cr>(o, std::begin(t), std::end(t), cx);
        }

}}} //cxon::chario::container container read/write helpers

#endif // CXON_CHARIO_CONTAINER_HXX_
