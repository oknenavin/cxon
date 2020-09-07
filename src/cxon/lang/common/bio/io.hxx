// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_BIO_IO_HXX_
#define CXON_BIO_IO_HXX_

#include "cxon/utility.hxx"
//#include "error.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace bio {

    using byte = unsigned char;

    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t<!cxon::is_forward_iterator<II>::value>;
    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t< cxon::is_forward_iterator<II>::value>;

    // input

    template <typename II>
        inline byte peek(II i, II e);
    template <typename II>
        inline byte next(II& i, II e);

    template <typename X, typename II>
        inline bool consume(byte b, II& i, II e);
    template <typename X, typename II, typename Cx>
        inline bool consume(byte b, II& i, II e, Cx& cx);

    // output

    template <typename O>
        inline bool poke(O& o, byte b);

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, byte b, Cx& cx);

}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace bio {

    template<typename II>
        inline auto rewind(II&, II) noexcept     -> enable_if_t<!cxon::is_forward_iterator<II>::value> {}
    template<typename II>
        inline auto rewind(II& i, II o) noexcept -> enable_if_t< cxon::is_forward_iterator<II>::value> { i = o; }

    // input

    template <typename II>
        inline byte peek(II i, II e) {
            return i != e ? *i : 0xFF;
        }
    template <typename II>
        inline byte next(II& i, II e) {
            return ++i, peek(i, e);
        }

    template <typename X, typename II>
        inline bool consume(byte b, II& i, II e) {
            return b == peek(i, e) && (next(i, e), true);
        }
    template <typename X, typename II, typename Cx>
        inline bool consume(byte b, II& i, II e, Cx& cx) {
            return (b == peek(i, e) && (next(i, e), true)) || false/*(cx|read_error::unexpected)*/;
        }

    // output

    namespace bits {

        template <typename O>
            inline auto push_(O& o, byte b) -> enable_if_t<is_output_iterator<O>::value> {
                *o = b, ++o;
            }
        template <typename O>
            inline auto push_(O& o, byte b) -> enable_if_t<is_back_insertable<O>::value> {
                o.push_back(b);
            }

        template <typename O, typename ...P>
            inline auto poke_(option<2>, O& o, P... p) -> decltype((bool)o) {
                return push_(o, p...), o;
            }
        template <typename O, typename ...P>
            inline auto poke_(option<1>, O& o, P... p) -> decltype(o.good()) {
                return push_(o, p...), o.good();
            }
        template <typename O, typename ...P>
            inline bool poke_(option<0>, O& o, P... p) {
                return push_(o, p...), true;
            }
        template <typename O, typename ...P>
            inline bool poke_(O& o, P... p) {
                return poke_(option<2>(), o, p...);
            }

        template <typename X, typename O, typename Cx, typename ...P>
            inline bool poke_(O& o, Cx& cx, P... p) {
                return poke_(option<2>(), o, p...) || false/*(cx|write_error::output_failure)*/;
            }

    }

    template <typename O>
        inline bool poke(O& o, byte b)              { return bits::poke_(o, b); }

    template <typename X, typename O, typename Cx>
        inline bool poke(O& o, byte b, Cx& cx)      { return bits::poke_<X>(o, cx, b); }

}}

#endif // CXON_BIO_IO_HXX_
