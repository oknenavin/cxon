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
            return bio::poke<X>(o, X::nil, cx);
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
            return bio::poke<X>(o, t ? X::pos : X::neg, cx);
        }

}

namespace cxon { // numeric & character

    namespace bits {

        template <typename X, typename T, typename II>
            inline auto get_(T& t, unsigned n, II& i, II e, bio::byte m)
                -> enable_if_t<std::is_integral<T>::value, bool>
            {
                return bio::get(t, n, i, e) && (!(m & X::nint) || (t = -1 - t, true));
            }

    }

    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_integral<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            II const o = i;
            switch (bio::byte const b = bio::get(i, e)) {
                case  0: case  1: case  2: case  3: case  4: case  5: case  6: case  7:
                case  8: case  9: case 10: case 11: case 12: case 13: case 14: case 15:
                case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23:
                case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39:
                case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47:
                case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55:
                    return  t = static_cast<T>((b & X::nint) ? -1 - (b - X::nint) : b), true;
                case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31:
                    return  bits::get_<X>(t, b - 23, i, e, b) ||
                            (bio::rewind(i, o), cx|cbor::read_error::integer_invalid)
                    ;
                case 56: case 57: case 58: case 59: case 60: case 61: case 62: case 63:
                    return  bits::get_<X>(t, b - 55, i, e, b) ||
                            (bio::rewind(i, o), cx|cbor::read_error::integer_invalid)
                    ;
                default:
                    return  (bio::rewind(i, o), cx|cbor::read_error::integer_invalid);
            }
        }

    namespace bits {

        template <typename FP>  struct unsigned_;
        template <>             struct unsigned_<float> { using type = unsigned long; };
        template <>             struct unsigned_<double> { using type = unsigned long long; };
        template <typename T>
            using unsigned_type_ = typename unsigned_<T>::type;

        template <typename FP>  struct signed_;
        template <>             struct signed_<float> { using type = long; };
        template <>             struct signed_<double> { using type = long long; };
        template <typename T>
            using signed_type_ = typename signed_<T>::type;

    }

    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_floating_point<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            II const o = i;
            switch (bio::byte const b = bio::get(i, e)) {
                case  0: case  1: case  2: case  3: case  4: case  5: case  6: case  7:
                case  8: case  9: case 10: case 11: case 12: case 13: case 14: case 15:
                case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23:
                case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39:
                case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47:
                case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55:
                    return  t = static_cast<T>((b & X::nint) ? -1 - (b - X::nint) : b), true;
                case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: {
                    bits::unsigned_type_<T> tt;
                    return  (bits::get_<X>(tt, b - 23, i, e, b) && (t = static_cast<T>(tt), true)) ||
                            (bio::rewind(i, o), cx|cbor::read_error::integer_invalid)
                    ;
                }
                case 56: case 57: case 58: case 59: case 60: case 61: case 62: case 63:
                    bits::signed_type_<T> tt;
                    return  (bits::get_<X>(tt, b - 55, i, e, b) && (t = static_cast<T>(tt), true)) ||
                            (bio::rewind(i, o), cx|cbor::read_error::integer_invalid)
                    ;
                case X::fp16:
                    /*return  bio ::get<2>(t, i, e) ||
                            (bio::rewind(i, o), cx|cbor::read_error::floating_point_invalid)
                    ;*/
                    return  CXON_ASSERT(0, "not implemented"), false;
                case X::fp32:
                    return  bio ::get<4>(t, i, e) ||
                            (bio::rewind(i, o), cx|cbor::read_error::floating_point_invalid)
                    ;
                case X::fp64:
                    return  bio ::get<8>(t, i, e) ||
                            (bio::rewind(i, o), cx|cbor::read_error::floating_point_invalid)
                    ;
                default:
                    return  (bio::rewind(i, o), cx|cbor::read_error::floating_point_invalid);
            }
        }

    namespace bits {

        template <typename T>
            inline auto bytes_(T  ) -> enable_if_t<sizeof(T) == 1, unsigned> {
                return 1;
            }
        template <typename T>
            inline auto bytes_(T t) -> enable_if_t<sizeof(T) == 2, unsigned> {
                /*unsigned r, s;
                r = (t > 0x00FF) << 3, t >>= r;
                s = (t > 0x000F) << 2, t >>= s, r |= s;
                s = (t > 0x0003) << 1, t >>= s, r |= s;
                                                r |= (t >> 1), r >>= 3, ++r;
                return r;*/
                return t >> 8 ? 2 : 1;
            }
        template <typename T>
            inline auto bytes_(T t) -> enable_if_t<sizeof(T) == 4, unsigned> {
                /*unsigned r, s;
                r = (t > 0xFFFF) << 4, t >>= r;
                s = (t > 0x00FF) << 3, t >>= s, r |= s;
                s = (t > 0x000F) << 2, t >>= s, r |= s;
                s = (t > 0x0003) << 1, t >>= s, r |= s;
                                                r |= (t >> 1), r >>= 3, ++r;
                return r;*/
                return t >> 16 ? (t >> 24 ? 4 : 3) : (t >> 8 ? 2 : 1);
            }
        template <typename T>
            inline auto bytes_(T t) -> enable_if_t<sizeof(T) == 8, unsigned> {
                /*unsigned r, s;
                r = (t > 0xFFFFFFFF) << 5, t >>= r;
                s = (t > 0x0000FFFF) << 4, t >>= s, r |= s;
                s = (t > 0x000000FF) << 3, t >>= s, r |= s;
                s = (t > 0x0000000F) << 2, t >>= s, r |= s;
                s = (t > 0x00000003) << 1, t >>= s, r |= s;
                                                    r |= (t >> 1), r >>= 3, ++r;
                return r;*/
                return t >> 32 ? (t >> 48 ? (t >> 56 ? 8 : 7) : (t >> 40 ? 6 : 5)) : (t >> 16 ? (t >> 24 ? 4 : 3) : (t >> 8 ? 2 : 1));
            }

    }

    template <typename X, typename T, typename O, typename Cx>
        inline auto write_value(O& o, T t, Cx& cx)
            -> enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            unsigned const n = bits::bytes_(t);
            return t > 23 ?
                bio::poke<X>(o, bio::byte(X::pint + 23 + n), cx) && bio::poke<X>(o, t, n, cx) :
                bio::poke<X>(o, bio::byte(X::pint + t), cx)
            ;
        }
    template <typename X, typename T, typename O, typename Cx>
        inline auto write_value(O& o, T t, Cx& cx)
            -> enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            bio::byte const m = t < 0 ? (t = -1 - t, X::nint) : X::pint;
            unsigned const n = bits::bytes_(t);
            return t > 23 ?
                bio::poke<X>(o, bio::byte(m + 23 + n), cx) && bio::poke<X>(o, t, n, cx) :
                bio::poke<X>(o, bio::byte(m + t), cx)
            ;
        }


    template <typename X, typename T, typename O, typename Cx>
        inline auto write_value(O& o, T t, Cx& cx)
            -> enable_if_t<std::is_floating_point<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            static constexpr bio::byte m = sizeof(T) == sizeof(float) ? X::fp32 : X::fp64;
            return bio::poke<X>(o, m, cx) && bio::poke<X>(o, t, cx);
        }

}

#endif // CXON_CBOR_FUNDAMENTAL_HXX_
