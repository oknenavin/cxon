// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_FUNDAMENTAL_HXX_
#define CXON_CBOR_FUNDAMENTAL_HXX_

#include "cxon/lang/common/bio/bio.hxx"

namespace cxon { // nullptr_t

    template <typename X, typename II, typename Cx>
        inline auto read_value(std::nullptr_t& t, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {
            II const o = i;
            return (bio::consume<X>(X::nil, i, e) || (bio::rewind(i, o), cx|cbor::read_error::null_invalid)) &&
                    (t = nullptr, true)
            ;
        }

    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, std::nullptr_t, Cx& cx) -> enable_for_t<X, CBOR> {
            return bio::poke<X>(o, X::nil, cx);
        }

}

namespace cxon { // bool

    template <typename X, typename II, typename Cx>
        inline auto read_value(bool& t, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {
            II const o = i;
                auto const b = bio::get(i, e);
            return b != X::neg && b != X::pos ?
                (bio::rewind(i, o), cx|cbor::read_error::boolean_invalid) :
                (t = bool(b - X::neg),  true) // F5 - F4, F4 - F4
            ;
        }

    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, bool t, Cx& cx) -> enable_for_t<X, CBOR> {
            return bio::poke<X>(o, t ? X::pos : X::neg, cx);
        }

}

#endif // CXON_CBOR_FUNDAMENTAL_HXX_
