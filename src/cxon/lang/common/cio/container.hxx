// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_CONTAINER_HXX_
#define CXON_CIO_CONTAINER_HXX_

#include "cio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace con { // container read/write helpers

    template <typename X, typename II, typename Cx, typename EA>
        inline bool read_list(II& i, II e, Cx& cx, EA element_add);

    template <typename X, typename II, typename Cx, typename EA>
        inline bool read_map(II& i, II e, Cx& cx, EA element_add);

    template <typename X, typename O, typename II, typename Cx, typename L>
        inline bool write_list(O& o, II b, II e, Cx& cx, L element_write);
    template <typename X, typename O, typename T, typename Cx, typename L>
        inline bool write_list(O& o, const T& t, Cx& cx, L element_write);
    template <typename X, typename O, typename II, typename Cx>
        inline bool write_list(O& o, II b, II e, Cx& cx);
    template <typename X, typename O, typename T, typename Cx>
        inline bool write_list(O& o, const T& t, Cx& cx);

    template <typename X, typename O, typename II, typename Cx, typename L>
        inline bool write_map(O& o, II b, II e, Cx& cx, L element_write);
    template <typename X, typename O, typename T, typename Cx, typename L>
        inline bool write_map(O& o, const T& t, Cx& cx, L element_write);
    template <typename X, typename O, typename II, typename Cx>
        inline bool write_map(O& o, II b, II e, Cx& cx);
    template <typename X, typename O, typename T, typename Cx>
        inline bool write_map(O& o, const T& t, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace con {

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

        template <typename X, typename Cr, typename II, typename Cx, typename EA>
            inline bool read(II& i, II e, Cx& cx, EA element_add) {
                if (!consume<X>(Cr::beg, i, e, cx)) return false;
                if ( consume<X>(Cr::end, i, e))     return true;
                return list_read<X, Cr>(i, e, element_add), !cx.ec && consume<X>(Cr::end, i, e, cx);
            }

        template <typename X, typename Cr, typename O, typename II, typename Cx, typename L>
            inline bool write(O& o, II b, II e, Cx& cx, L element_write) {
                if (!poke<X>(o, Cr::beg, cx)) return false;
                return list_write<X, Cr>(o, b, e, cx, element_write), !cx.ec && poke<X>(o, Cr::end, cx);
            }
        template <typename X, typename Cr, typename O, typename II, typename Cx>
            inline bool write(O& o, II b, II e, Cx& cx) {
                return write<X, Cr>(o, b, e, cx, [&](const decltype(*b)& e) {
                    return write_value<X>(o, e, cx);
                });
            }

    }

    template <typename X, typename II, typename Cx, typename EA>
        inline bool read_list(II& i, II e, Cx& cx, EA element_add) {
            return bits::read<X, typename X::list>(i, e, cx, element_add);
        }
    template <typename X, typename II, typename Cx, typename EA>
        inline bool read_map(II& i, II e, Cx& cx, EA element_add) {
            return bits::read<X, typename X::map>(i, e, cx, element_add);
        }

    template <typename X, typename O, typename II, typename Cx, typename L>
        inline bool write_list(O& o, II b, II e, Cx& cx, L element_write) {
            return bits::write<X, typename X::list>(o, b, e, cx, element_write);
        }
    template <typename X, typename O, typename T, typename Cx, typename L>
        inline bool write_list(O& o, const T& t, Cx& cx, L element_write) {
            return write_list<X>(o, std::begin(t), std::end(t), cx, element_write);
        }
    template <typename X, typename O, typename II, typename Cx>
        inline bool write_list(O& o, II b, II e, Cx& cx) {
            return bits::write<X, typename X::list>(o, b, e, cx);
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool write_list(O& o, const T& t, Cx& cx) {
            return write_list<X>(o, std::begin(t), std::end(t), cx);
        }

    template <typename X, typename O, typename II, typename Cx, typename L>
        inline bool write_map(O& o, II b, II e, Cx& cx, L element_write) {
            return bits::write<X, typename X::map>(o, b, e, cx, element_write);
        }
    template <typename X, typename O, typename T, typename Cx, typename L>
        inline bool write_map(O& o, const T& t, Cx& cx, L element_write) {
            return write_map<X>(o, std::begin(t), std::end(t), cx, element_write);
        }
    template <typename X, typename O, typename II, typename Cx>
        inline bool write_map(O& o, II b, II e, Cx& cx) {
            return bits::write<X, typename X::map>(o, b, e, cx);
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool write_map(O& o, const T& t, Cx& cx) {
            return write_map<X>(o, std::begin(t), std::end(t), cx);
        }

}}}

#endif // CXON_CIO_CONTAINER_HXX_
