// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_CONTAINER_HXX_
#define CXON_CIO_CONTAINER_HXX_

#include "cio.hxx"
#include "cxon/lang/common/container.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace cnt { // container read/write helpers
    
    template <typename X, typename C, typename II, typename Cx>
        inline bool read_list(C& c, II& i, II e, Cx& cx);
    template <typename X, typename II, typename Cx, typename EA>
        inline bool read_list(II& i, II e, Cx& cx, EA element_add);

    template <typename X, typename C, typename II, typename Cx>
        inline bool read_map(C& c, II& i, II e, Cx& cx);
    template <typename X, typename II, typename Cx, typename EA>
        inline bool read_map(II& i, II e, Cx& cx, EA element_add);

    template <typename X, typename C, typename O, typename II, typename Cx, typename L>
        inline bool write_list(O& o, II b, II e, Cx& cx, L element_write);
    template <typename X, typename O, typename C, typename Cx, typename L>
        inline bool write_list(O& o, const C& c, Cx& cx, L element_write);
    template <typename X, typename C, typename O, typename II, typename Cx>
        inline bool write_list(O& o, II b, II e, Cx& cx);
    template <typename X, typename O, typename C, typename Cx>
        inline bool write_list(O& o, const C& c, Cx& cx);

    template <typename X, typename C, typename O, typename II, typename Cx, typename L>
        inline bool write_map(O& o, II b, II e, Cx& cx, L element_write);
    template <typename X, typename O, typename C, typename Cx, typename L>
        inline bool write_map(O& o, const C& c, Cx& cx, L element_write);
    template <typename X, typename C, typename O, typename II, typename Cx>
        inline bool write_map(O& o, II b, II e, Cx& cx);
    template <typename X, typename O, typename C, typename Cx>
        inline bool write_map(O& o, const C& c, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace cnt {

    namespace imp {

        template <typename X, typename Cr, typename II, typename Cx, typename EA>
            inline auto list_read_(II& i, II e, Cx& cx, EA element_add) -> enable_if_t<!X::allow_trailing_separator> {
                // expects non-empty list
                while (element_add() && consume<X>(i, e, cx) && consume<X>(Cr::sep, i, e)) ;
            }
        template <typename X, typename Cr, typename II, typename Cx, typename EA>
            inline auto list_read_(II& i, II e, Cx& cx, EA element_add) -> enable_if_t< X::allow_trailing_separator> {
                // expects non-empty list
                while (element_add() && consume<X>(i, e, cx) && consume<X>(Cr::sep, i, e) && (consume<X>(i, e, cx) && peek(i, e) != Cr::end)) ;
            }

        template <typename X, typename Cr, typename II, typename Cx, typename EA>
            inline bool read_(II& i, II e, Cx& cx, EA element_add) {
                if (!consume<X>(Cr::beg, i, e, cx)) return false;
                    if (!consume<X>(i, e, cx)) return false;
                        if (consume<X>(Cr::end, i, e)) return true;
                return list_read_<X, Cr>(i, e, cx, element_add), !cx.ec && consume<X>(Cr::end, i, e, cx);
            }

        template <typename X, typename Cr, typename O, typename II, typename Cx, typename L>
            inline void list_write_(O& o, II b, II e, Cx& cx, L element_write) {
                if (b != e && element_write(*b)) {
                    while (++b != e && poke<X>(o, Cr::sep, cx) && element_write(*b)) ;
                }
            }

        template <typename X, typename Xc, typename O, typename II, typename Cx, typename L>
            inline bool write_(O& o, II b, II e, Cx& cx, L element_write) {
                if (!poke<X>(o, Xc::beg, cx)) return false;
                return list_write_<X, Xc>(o, b, e, cx, element_write), !cx.ec && poke<X>(o, Xc::end, cx);
            }
        template <typename X, typename C, typename Xc, typename O, typename II, typename Cx>
            inline bool write_(O& o, II b, II e, Cx& cx) {
                return write_<X, Xc>(o, b, e, cx, [&](const decltype(*b)& e) {
                    return cxon::cnt::element_write<X, C>(o, e, cx);
                });
            }

    }

    template <typename X, typename C, typename II, typename Cx>
        inline bool read_list(C& c, II& i, II e, Cx& cx) {
            using Y = unbind_traits_t<X, key::quoted_traits>;
            return imp::read_<Y, typename Y::list>(i, e, cx, [&] {
                return cxon::cnt::element_read<Y>(c, i, e, cx);
            });
        }
    template <typename X, typename II, typename Cx, typename EA>
        inline bool read_list(II& i, II e, Cx& cx, EA element_add) {
            using Y = unbind_traits_t<X, key::quoted_traits>;
            return imp::read_<Y, typename Y::list>(i, e, cx, element_add);
        }

    template <typename X, typename C, typename II, typename Cx>
        inline bool read_map(C& c, II& i, II e, Cx& cx) {
            return imp::read_<X, typename X::map>(i, e, cx, [&] {
                return cxon::cnt::element_read<X>(c, i, e, cx);
            });
        }
    template <typename X, typename II, typename Cx, typename EA>
        inline bool read_map(II& i, II e, Cx& cx, EA element_add) {
            return imp::read_<X, typename X::map>(i, e, cx, element_add);
        }

    template <typename X, typename C, typename O, typename II, typename Cx, typename L>
        inline bool write_list(O& o, II b, II e, Cx& cx, L element_write) {
            using Y = unbind_traits_t<X, key::quoted_traits>;
            return imp::write_<Y, typename Y::list>(o, b, e, cx, element_write);
        }
    template <typename X, typename O, typename C, typename Cx, typename L>
        inline bool write_list(O& o, const C& c, Cx& cx, L element_write) {
            return write_list<X, C>(o, std::begin(c), std::end(c), cx, element_write);
        }
    template <typename X, typename C, typename O, typename II, typename Cx>
        inline bool write_list(O& o, II b, II e, Cx& cx) {
            using Y = unbind_traits_t<X, key::quoted_traits>;
            return imp::write_<Y, C, typename Y::list>(o, b, e, cx);
        }
    template <typename X, typename O, typename C, typename Cx>
        inline bool write_list(O& o, const C& c, Cx& cx) {
            return write_list<X, C>(o, std::begin(c), std::end(c), cx);
        }

    template <typename X, typename C, typename O, typename II, typename Cx, typename L>
        inline bool write_map(O& o, II b, II e, Cx& cx, L element_write) {
            return imp::write_<X, typename X::map>(o, b, e, cx, element_write);
        }
    template <typename X, typename O, typename C, typename Cx, typename L>
        inline bool write_map(O& o, const C& c, Cx& cx, L element_write) {
            return write_map<X, C>(o, std::begin(c), std::end(c), cx, element_write);
        }
    template <typename X, typename C, typename O, typename II, typename Cx>
        inline bool write_map(O& o, II b, II e, Cx& cx) {
            return imp::write_<X, C, typename X::map>(o, b, e, cx);
        }
    template <typename X, typename O, typename C, typename Cx>
        inline bool write_map(O& o, const C& c, Cx& cx) {
            return write_map<X, C>(o, std::begin(c), std::end(c), cx);
        }

}}}

#endif // CXON_CIO_CONTAINER_HXX_
