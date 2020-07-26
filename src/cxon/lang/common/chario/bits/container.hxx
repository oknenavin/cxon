// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_BITS_CHARIO_CONTAINER_HXX_
#define CXON_BITS_CHARIO_CONTAINER_HXX_

namespace cxon { namespace chario { namespace container { namespace bits { // container read/write helpers

    template <typename X, typename Cr, typename II, typename EA>
        inline void list_read(II& i, II e, EA element_add) {
            // expects non-empty list
            while (element_add() && io::consume<X>(Cr::sep, i, e)) ;
        }

    template <typename X, typename Cr, typename O, typename II, typename Cx, typename L>
        inline void list_write(O& o, II b, II e, Cx& cx, L element_write) {
            if (b != e && element_write(*b)) {
                while (++b != e && io::poke<X>(o, Cr::sep, cx) && element_write(*b)) ;
            }
        }

}}}}//cxon::chario::container container read/write helpers

#endif // CXON_BITS_CHARIO_CONTAINER_HXX_
