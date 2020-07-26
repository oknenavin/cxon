// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHARIO_KEY_HXX_
#define CXON_CHARIO_KEY_HXX_

#include "chario.hxx"
#include "bits/key.hxx"

namespace cxon { namespace chario { // key read/write helpers

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_key(T& t, II& i, II e, Cx& cx) {
            return bits::key_read<X>::value(t, i, e, cx) && chario::consume<X>(X::map::div, i, e, cx);
        }

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_key(O& o, const T& t, Cx& cx) {
            return bits::key_write<X>::value(o, t, cx) && chario::poke<X>(o, X::map::div, cx);
        }

}}  //cxon::chario key read/write helpers

#endif // CXON_CHARIO_KEY_HXX_
