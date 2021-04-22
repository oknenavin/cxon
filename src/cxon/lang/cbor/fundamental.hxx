// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_FUNDAMENTAL_HXX_
#define CXON_CBOR_FUNDAMENTAL_HXX_

#include "cxon/lang/common/bio/bio.hxx"
#include "cxon/lang/cbor/common/tag.hxx"

namespace cxon { // nullptr_t

    template <typename X, typename II, typename Cx>
        inline auto read_value(std::nullptr_t& t, II& i, II e, Cx& cx) -> enable_for_t<X, CBOR> {
            II const o = i;
            return  (bio::get(i, e) == X::nil && (t = nullptr, true)) ||
                    (bio::rewind(i, o), cx/cbor::read_error::null_invalid)
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
            return  ((b == X::neg || b == X::pos) && (t = bool(b - X::neg),  true)) ||
                    (bio::rewind(i, o), cx/cbor::read_error::boolean_invalid)
            ;
        }

    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, bool t, Cx& cx) -> enable_for_t<X, CBOR> {
            return bio::poke<X>(o, t ? X::pos : X::neg, cx);
        }

}

namespace cxon { // numeric|character/read

    namespace cbor { namespace imp {

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_unsigned_(T& t, II& i, II e, Cx& cx) {
                II const o = i;
                switch (auto const b = bio::get(i, e) & X::mnr) {
                    case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
                    case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F:
                    case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
                        return  (t = T(b), true);
                    case 0x18: case 0x19: case 0x1A: case 0x1B:
                        return  (bio::get(t, 1 << (b - 0x18), i, e)) ||
                                (bio::rewind(i, o), cx/cbor::read_error::integer_invalid)
                        ;
                    case 0x1C: case 0x1D: case 0x1E: case 0x1F:
                        return  (bio::rewind(i, o), cx/cbor::read_error::integer_invalid);
                    default:                                            // LCOV_EXCL_LINE
                        return  CXON_ASSERT(0, "unexpected"), false;    // LCOV_EXCL_LINE
                }
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_signed_(T& t, II& i, II e, Cx& cx) {
                return read_unsigned_<X>(t, i, e, cx) && (t = -1 - t, true);
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_integral_(T& t, II& i, II e, Cx& cx) {
                switch (bio::peek(i, e) & X::mjr) {
                    case 0x00:  return read_unsigned_<X>(t, i, e, cx);
                    case 0x20:  return read_signed_<X>(t, i, e, cx);
                    default:    return cx/cbor::read_error::integer_invalid;
                }
            }

    }}

    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_integral<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            return  cbor::tag::read<X>(i, e, cx) &&
                    cbor::imp::read_integral_<X>(t, i, e, cx)
            ;
        }

    namespace cbor { namespace imp {

        template <typename T>   struct unsigned_;
        template <>             struct unsigned_<float>     { using type = unsigned long; };
        template <>             struct unsigned_<double>    { using type = unsigned long long; };
        template <typename T>
            using unsigned_type_ = typename unsigned_<T>::type;

        template <typename T>   struct signed_;
        template <>             struct signed_<float>       { using type = long; };
        template <>             struct signed_<double>      { using type = long long; };
        template <typename T>
            using signed_type_ = typename signed_<T>::type;

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_floating_point_(T& t, II& i, II e, Cx& cx) {
                switch (bio::peek(i, e) & X::mjr) {
                    case X::svn: {
                        II const o = i;
                        switch (bio::get(i, e)) {
                            case X::fp16:
                                return  (bio ::get<2>(t, i, e)) ||
                                        (bio::rewind(i, o), cx/cbor::read_error::floating_point_invalid)
                                ;
                            case X::fp32:
                                return  (bio ::get<4>(t, i, e)) ||
                                        (bio::rewind(i, o), cx/cbor::read_error::floating_point_invalid)
                                ;
                            case X::fp64:
                                return  (bio ::get<8>(t, i, e)) ||
                                        (bio::rewind(i, o), cx/cbor::read_error::floating_point_invalid)
                                ;
                            default:
                                return  (bio::rewind(i, o), cx/cbor::read_error::floating_point_invalid);
                        }
                        break;
                    }
                    // from integral
                    case X::pint: {
                        unsigned_type_<T> u;
                        return read_unsigned_<X>(u, i, e, cx) && (t = T(u), true);
                    }
                    case X::nint: {
                        signed_type_<T> u;
                        return read_signed_<X>(u, i, e, cx) && (t = T(u), true);
                    }
                    default:
                        return cx/cbor::read_error::floating_point_invalid;
                }
            }

    }}

    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_floating_point<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            return  cbor::tag::read<X>(i, e, cx) &&
                    cbor::imp::read_floating_point_<X>(t, i, e, cx)
            ;
        }

}

namespace cxon { // numeric|character/write

    namespace cbor { namespace imp {

        template <typename T>
            inline auto power_(T  ) -> enable_if_t<sizeof(T) == 1, unsigned> { return 0; }
        template <typename T>
            inline auto power_(T t) -> enable_if_t<sizeof(T) == 2, unsigned> { return !(t >> 8) ? 0 : 1; }
        template <typename T>
            inline auto power_(T t) -> enable_if_t<sizeof(T) == 4, unsigned> { return !(t >> 16) ? !(t >> 8) ? 0 : 1 : 2; }
        template <typename T>
            inline auto power_(T t) -> enable_if_t<sizeof(T) == 8, unsigned> { return !(t >> 32) ? !(t >> 16) ? !(t >> 8) ? 0 : 1 : 2 : 3; }

        template <typename T>
            struct is_quantum_  { static constexpr bool value = std::is_same<T, char>::value || std::is_same<T, wchar_t>::value; };
        template <typename T>
            struct is_signed_   { static constexpr bool value = std::is_signed<T>::value   && !is_quantum_<T>::value; };
        template <typename T>
            struct is_unsigned_ { static constexpr bool value = std::is_unsigned<T>::value && !is_quantum_<T>::value; };

    }}

    template <typename X, typename T, typename O, typename Cx>
        inline auto write_value(O& o, T t, Cx& cx)
            -> enable_if_t<std::is_integral<T>::value && cbor::imp::is_unsigned_<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            auto const p = cbor::imp::power_(t);
            return t > 0x17 ?
                bio::poke<X>(o, bio::byte(X::pint + 0x18 + p), cx) && bio::poke<X>(o, t, 1 << p, cx) :
                bio::poke<X>(o, bio::byte(X::pint + t), cx)
            ;
        }
    template <typename X, typename T, typename O, typename Cx>
        inline auto write_value(O& o, T t, Cx& cx)
            -> enable_if_t<std::is_integral<T>::value && cbor::imp::is_quantum_<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            using Q = typename std::make_unsigned<T>::type;
            auto const p = cbor::imp::power_(t);
            return Q(t) > 0x17 ?
                bio::poke<X>(o, bio::byte(X::pint + 0x18 + p), cx) && bio::poke<X>(o, Q(t), 1 << p, cx) :
                bio::poke<X>(o, bio::byte(X::pint + Q(t)), cx)
            ;
        }
    template <typename X, typename T, typename O, typename Cx>
        inline auto write_value(O& o, T t, Cx& cx)
            -> enable_if_t<std::is_integral<T>::value && cbor::imp::is_signed_<T>::value && is_same_format<X, CBOR>::value, bool>
        {
            auto const m = t < 0 ? (t = -1 - t, X::nint) : X::pint;
            auto const p = cbor::imp::power_(t);
            return t > 0x17 ?
                bio::poke<X>(o, bio::byte(m + 0x18 + p), cx) && bio::poke<X>(o, t, 1 << p, cx) :
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
