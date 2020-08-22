// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHIO_CONTAINER_HXX_
#define CXON_CHIO_CONTAINER_HXX_

#include "chio.hxx"

namespace cxon { namespace chio { namespace container { // container read/write helpers

    namespace bits {

        template <typename X, typename Cr, typename II, typename EA>
            inline void list_read(II& i, II e, EA element_add) {
                // expects non-empty list
                while (element_add() && consume<X>(Cr::sep, i, e)) ;
            }

        template <typename X, typename Cr, typename O, typename II, typename Cx, typename L>
            inline void list_write(O& o, II b, II e, Cx& cx, L element_write) {
                if (b != e && element_write(*b)) {
                    while (++b != e && poke<X>(o, Cr::sep, cx) && element_write(*b)) ;
                }
            }

        template <typename A>
            struct adaptor : A {
                static const typename A::container_type& container(const A& a) noexcept { return ((adaptor&)a).c; }
            };

    }

    // read

    template <typename X, typename Cr, typename II, typename Cx, typename EA>
        inline bool read(II& i, II e, Cx& cx, EA element_add) {
            if (!consume<X>(Cr::beg, i, e, cx)) return false;
            if ( consume<X>(Cr::end, i, e))     return true;
            return bits::list_read<X, Cr>(i, e, element_add), !cx.ec && consume<X>(Cr::end, i, e, cx);
        }

    // write

    template <typename X, typename Cr, typename O, typename II, typename Cx, typename L>
        inline bool write(O& o, II b, II e, Cx& cx, L element_write) {
            if (!poke<X>(o, Cr::beg, cx)) return false;
            return bits::list_write<X, Cr>(o, b, e, cx, element_write), !cx.ec && poke<X>(o, Cr::end, cx);
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

}}}

#endif // CXON_CHIO_CONTAINER_HXX_
