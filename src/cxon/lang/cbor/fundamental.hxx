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
            return  (bio::get(i, e) == X::nil || (bio::rewind(i, o), cx|cbor::read_error::null_invalid)) &&
                    (t = nullptr, true)
            ;
        }

    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, std::nullptr_t, Cx& cx) -> enable_for_t<X, CBOR> {
            return bio::poke(o, X::nil) || (cx|X::write_error::output_failure);
        }

}

namespace cxon { // bool

    template <typename X, typename II, typename Cx>
        inline auto read_value(bool& t, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {
            II const o = i;
                auto const b = bio::get(i, e);
            return  ((b == X::neg || b == X::pos) || (bio::rewind(i, o), cx|cbor::read_error::boolean_invalid)) &&
                    (t = bool(b - X::neg),  true) // F5 - F4, F4 - F4
            ;
        }

    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, bool t, Cx& cx) -> enable_for_t<X, CBOR> {
            return bio::poke(o, t ? X::pos : X::neg) || (cx|X::write_error::output_failure);
        }

}

namespace cxon { // character

}

namespace cxon { // numeric

    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            II const o = i;
            switch (bio::byte const b = bio::get(i, e)) {
                case  0: case  1: case  2: case  3: case  4: case  5: case  6: case  7:
                case  8: case  9: case 10: case 11: case 12: case 13: case 14: case 15:
                case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23:
                    return  t = T(b), true;
                case 24: // 8
                    return  (bio::get<1>(t, i, e) || (bio::rewind(i, o), cx|cbor::read_error::u8_invalid));
                case 25: // 16
                    return  (bio::get<2>(t, i, e) || (bio::rewind(i, o), cx|cbor::read_error::u16_invalid));
                case 26: // 32
                    return  (bio::get<4>(t, i, e) || (bio::rewind(i, o), cx|cbor::read_error::u32_invalid));
                case 27: // 64
                    return  (bio::get<8>(t, i, e) || (bio::rewind(i, o), cx|cbor::read_error::u32_invalid));
                default:
                    return  bio::rewind(i, o), cx|cbor::read_error::u_invalid;
            }
        }

    namespace bits {

        template <typename T>
            inline auto bytes(T t) -> enable_if_t<sizeof(T) == 1, unsigned> {
                return 1;
            }
        template <typename T>
            inline auto bytes(T t) -> enable_if_t<sizeof(T) == 2, unsigned> {
                return t >> 8 ? 2 : 1;
            }
        template <typename T>
            inline auto bytes(T t) -> enable_if_t<sizeof(T) == 4, unsigned> {
                return t >> 16 ? (t >> 24 ? 4 : 3) : (t >> 8 ? 2 : 1);
            }
        template <typename T>
            inline auto bytes(T t) -> enable_if_t<sizeof(T) == 8, unsigned> {
                return t >> 32 ? (t >> 48 ? (t >> 56 ? 8 : 7) : (t >> 40 ? 6 : 5)) : (t >> 16 ? (t >> 24 ? 4 : 3) : (t >> 8 ? 2 : 1));
            }

    }

    template <typename X, typename T, typename O, typename Cx>
        inline auto write_value(O& o, const T& t, Cx&/* cx*/)
            -> enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            unsigned const c = bits::bytes(t);
            return t > 23 ?
                bio::poke(o, bio::byte(X::uint + 23 + c)) && bio::poke(o, t, c) :
                bio::poke(o, bio::byte(X::uint + t))
            ;
        }

}

#endif // CXON_CBOR_FUNDAMENTAL_HXX_
