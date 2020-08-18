// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHARIO_NUMBERS_HXX_
#define CXON_CHARIO_NUMBERS_HXX_

#include "chario.hxx"
#include "charconv.hxx"
#include <cmath> // isfinite, ...

namespace cxon { namespace nums { // number conversion: read

    namespace io = cxon::chario;
    using namespace cxon::charclass;

    template <typename T>
        inline auto is_sign(char c) -> enable_if_t<std::is_signed<T>::value, bool> {
            return c == '-';
        }
    template <typename T>
        constexpr auto is_sign(char) -> enable_if_t<std::is_unsigned<T>::value, bool> {
            return false;
        }

#   define CXON_NEXT() { if (f == l) return -1; *f = c, c = io::next(i, e), ++f; }

        template <typename X, typename T>
            struct number_consumer {
                // integral
                template <typename II, typename N = T>
                    static auto consume(char* f, const char* l, II& i, II e)
                        -> enable_if_t<std::is_integral<N>::value, int>
                    {
                        CXON_ASSERT(f && f < l, "unexpected");
                        int b = 2;
                        char c = io::peek(i, e);
                            if (is_sign<T>(c))              CXON_NEXT()
                            if (c == '0')                   goto trap_oct;
                        // trap_dec:
                               if (!is<X>::digit10(c))      return 0;
                            while ( is<X>::digit10(c))      CXON_NEXT()
                            b = 10;                         goto trap_end;
                        trap_oct:
                            ;                               c = io::next(i, e);
                            if (c == 'x')                   goto trap_hex;
                            if (c == 'b')                   goto trap_bin;
                            if (!is<X>::digit8(c)) {        *f = '0', ++f;
                                                            goto trap_end; }
                            while (is<X>::digit8(c))        CXON_NEXT()
                            b = 8;                          goto trap_end;
                        trap_hex:
                            ;                               c = io::next(i, e);
                               if (!is<X>::digit16(c))      return 0;
                            while ( is<X>::digit16(c))      CXON_NEXT()
                            b = 16;                         goto trap_end;
                        trap_bin:
                            ;                               c = io::next(i, e);
                               if (c != '0' && c != '1')    return 0;
                            while (c == '0' || c == '1')    CXON_NEXT()
                        trap_end:
                            return f != l ? (*f = '\0', b) : 0;
                    }
                // floating point
                template <typename II, typename N = T>
                    static auto consume(char* f, const char* l, II& i, II e)
                        -> enable_if_t<std::is_floating_point<N>::value, int>
                    {
                        CXON_ASSERT(f && f < l, "unexpected");
                        char c = io::peek(i, e);
                            if (is_sign<T>(c))          CXON_NEXT()
                            if (c == '.')               goto trap_fraction_0;
                            if (c == 'i')               goto trap_inf;
                            if (c == 'n')               goto trap_nan;
                        //trap_whole:
                               if (!is<X>::digit10(c))  return 0;
                            while ( is<X>::digit10(c))  CXON_NEXT()
                            if (c == '.')               goto trap_fraction_1;
                            if (c == 'e' || c == 'E')   goto trap_exponent;
                            ;                           goto trap_end;
                        trap_fraction_0:
                            ;                           CXON_NEXT()
                               if (!is<X>::digit10(c))  return 0;
                            while ( is<X>::digit10(c))  CXON_NEXT()
                            if (c == 'e' || c == 'E')   goto trap_exponent;
                            ;                           goto trap_end;
                        trap_fraction_1:
                            ;                           CXON_NEXT()
                            while (is<X>::digit10(c))   CXON_NEXT()
                            if (c == 'e' || c == 'E')   goto trap_exponent;
                            ;                           goto trap_end;
                        trap_exponent:
                            ;                           CXON_NEXT()
                            if (c == '-' || c == '+')   CXON_NEXT()
                               if (!is<X>::digit10(c))  return 0;
                            while ( is<X>::digit10(c))  CXON_NEXT()
                            ;                           goto trap_end;
                        trap_inf:
                            CXON_NEXT() if (c != 'n')   return 0;
                            CXON_NEXT() if (c != 'f')   return 0;
                            CXON_NEXT()                 goto trap_end;
                        trap_nan:
                            CXON_NEXT() if (c != 'a')   return 0;
                            CXON_NEXT() if (c != 'n')   return 0;
                            CXON_NEXT()
                        trap_end:
                            return f != l ? (*f = '\0', 1) : 0;
                    }
            };
        template <typename X, typename T>
            struct number_consumer<JSON<X>, T> {
            // integral
            template <typename II, typename N = T>
                static auto consume(char* f, const char* l, II& i, II e)
                    -> enable_if_t<std::is_integral<N>::value, int>
                {
                    CXON_ASSERT(f && f < l, "unexpected");
                    char c = io::peek(i, e);
                        if (is_sign<T>(c))                  CXON_NEXT()
                        if (c == '0') {                     CXON_NEXT()
                                                            goto trap_end; }
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                    trap_end:
                        return f != l ? (*f = '\0', 1) : 0;
                }
            template <typename N = T>
                static auto consume(const char*& i, const char* e)
                    -> enable_if_t<std::is_integral<N>::value, int>
                {
                    io::consume<JSON<X>>(i, e);
                    char c = io::peek(i, e);
                    if (is_sign<T>(c)) goto trap_neg;
                    // trap_pos
                        if (c == '0') return ++i, 1;
                        while (is<JSON<X>>::digit10(c)) c = io::next(i, e);
                        goto trap_end;
                    trap_neg:
                        c = io::next(i, e);
                            if (c == '0' || !is<JSON<X>>::digit10(c)) return 0;
                        while (is<JSON<X>>::digit10(c)) c = io::next(i, e);
                    trap_end:
                        return 1;
                }
            // floating point
            template <typename II, typename N = T>
                static auto consume(char* f, const char* l, II& i, II e)
                    -> enable_if_t<std::is_floating_point<N>::value, int>
                {   // as in RFC7159
                    CXON_ASSERT(f && f < l, "unexpected");
                    char c = io::peek(i, e);
                        if (c == '"')                       goto trap_spec_beg;
                        if (is_sign<T>(c))                  CXON_NEXT()
                        if (c == '0')                       goto trap_zero;
                    //trap_whole:
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                        if (c == '.')                       goto trap_fraction;
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_zero:
                        ;                                   CXON_NEXT()
                        if (c == '.')                       goto trap_fraction;
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_fraction:
                        ;                                   CXON_NEXT()
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_exponent:
                        ;                                   CXON_NEXT()
                        if (c == '-' || c == '+')           CXON_NEXT()
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    CXON_NEXT()
                        ;                                   goto trap_end;
                    trap_spec_beg:
                        ;                                   CXON_NEXT()
                        if (is_sign<T>(c))                  CXON_NEXT()
                        if (c == 'i')                       goto trap_inf;
                        if (c == 'n')                       goto trap_nan;
                        ;                                   return 0;
                    trap_inf:
                        CXON_NEXT() if (c != 'n')           return 0;
                        CXON_NEXT() if (c != 'f')           return 0;
                        ;                                   goto trap_spec_end;
                    trap_nan:
                        CXON_NEXT() if (c != 'a')           return 0;
                        CXON_NEXT() if (c != 'n')           return 0;
                    trap_spec_end:
                        CXON_NEXT() if (c != '"')           return 0;
                        CXON_NEXT()
                    trap_end:
                        return f != l ? (*f = '\0', 1) : 0;
                }
            template <typename N = T>
                static auto consume(const char*& i, const char* e)
                    -> enable_if_t<std::is_floating_point<N>::value, int>
                {   // as in RFC7159
                    char c = io::peek(i, e);
                        if (c == '"')                       goto trap_spec_beg;
                        if (is_sign<T>(c))                  c = io::next(i, e);
                        if (c == '0')                       goto trap_zero;
                    //trap_whole:
                            if (!is<JSON<X>>::digit10(c))   return 0;
                        while (is<JSON<X>>::digit10(c))     c = io::next(i, e);
                        if (c == '.')                       goto trap_fraction;
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_zero:
                        ;                                   c = io::next(i, e);
                        if (c == '.')                       goto trap_fraction;
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_fraction:
                        ;                                   c = io::next(i, e);
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    c = io::next(i, e);
                        if (c == 'e' || c == 'E')           goto trap_exponent;
                        ;                                   goto trap_end;
                    trap_exponent:
                        ;                                   c = io::next(i, e);
                        if (c == '-' || c == '+')           c = io::next(i, e);
                           if (!is<JSON<X>>::digit10(c))    return 0;
                        while ( is<JSON<X>>::digit10(c))    c = io::next(i, e);
                        ;                                   goto trap_end;
                    trap_spec_beg:
                        ;                                   c = io::next(i, e);
                        if (is_sign<T>(c))                  c = io::next(i, e);
                        if (c == 'i')                       goto trap_inf;
                        if (c == 'n')                       goto trap_nan;
                        ;                                   return 0;
                    trap_inf:
                        c = io::next(i, e); if (c != 'n')   return 0;
                        c = io::next(i, e); if (c != 'f')   return 0;
                        ;                                   goto trap_spec_end;
                    trap_nan:
                        c = io::next(i, e); if (c != 'a')   return 0;
                        c = io::next(i, e); if (c != 'n')   return 0;
                    trap_spec_end:
                        c = io::next(i, e); if (c != '"')   return 0;
                        ++i;
                    trap_end:
                        return 1;
                }
        };

#   undef CXON_NEXT

    template <typename X, typename T>
        struct number_reader {
            // integral
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(32)];
                            int const b = number_consumer<X, T>::consume(s, s + sizeof(s), i, e);
                            return  (b != -1                                                    || (io::rewind(i, o), cx|read_error::overflow)) &&
                                    (b !=  0                                                    || (io::rewind(i, o), cx|read_error::integral_invalid)) &&
                                    (bits::from_chars(s, s + sizeof(s), t, b).ec == std::errc() || (io::rewind(i, o), cx|read_error::integral_invalid))
                            ;
                    }
                // no optimization for const char* because of numeric bases (0, 0b, 0x)
            // floating point
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<std::is_floating_point<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(64)];
                            int const b = number_consumer<X, T>::consume(s, s + sizeof(s), i, e);
                            return  (b != -1                                                    || (io::rewind(i, o), cx|read_error::overflow)) &&
                                    (b !=  0                                                    || (io::rewind(i, o), cx|read_error::floating_point_invalid)) &&
                                    (bits::from_chars(s, s + sizeof(s), t).ec == std::errc()    || (io::rewind(i, o), cx|read_error::floating_point_invalid))
                            ;
                    }
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_floating_point<N>::value, bool>
                    {
                        auto const r = bits::from_chars(i, e, t);
                        return  (r.ec == std::errc() || (cx|read_error::floating_point_invalid)) &&
                                (i = r.ptr, true);
                    }
        };
    template <typename X, typename T>
        struct number_reader<JSON<X>, T> {
            // integral
                // strict
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(32)];
                            int const b = number_consumer<JSON<X>, T>::consume(s, s + sizeof(s), i, e);
                            return  (b != -1                                                    || (io::rewind(i, o), cx|read_error::overflow)) &&
                                    (b !=  0                                                    || (io::rewind(i, o), cx|read_error::integral_invalid)) &&
                                    (bits::from_chars(s, s + sizeof(s), t).ec == std::errc()    || (io::rewind(i, o), cx|read_error::integral_invalid))
                            ;
                    }
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value && X::number::strict, bool>
                    {
                        auto const b = i;
                        if (number_consumer<JSON<X>, T>::consume(i, e)) {
                            if (*b == '0' && i - b == 1) return t = 0, true;
                            auto const r = bits::from_chars(b, i, t);
                            if (r.ec == std::errc() && r.ptr == i) return true;
                        }
                        return i = b, cx|read_error::integral_invalid;
                    }
                // not strict
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_integral<N>::value && !X::number::strict, bool>
                    {
                        auto const r = bits::from_chars(i, e, t);
                        return (r.ec == std::errc() && (i = r.ptr, true)) || (cx|read_error::integral_invalid);
                    }
            // floating point
                template <typename N>
                    static bits::charconv::from_chars_result from_chars(const char* b, const char* e, N& t) {
                        if (*b == '"') {
                            if (b[1] == '-') {
                                if (b[2] == 'i') return b[3] == 'n' && b[4] == 'f' && b[5] == '"' ? t = -std::numeric_limits<N>::infinity(),
                                    bits::charconv::from_chars_result{ b + 6, std::errc() } : bits::charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                                return b[2] == 'n' && b[3] == 'a' && b[4] == 'n' && b[5] == '"' ? t =  std::numeric_limits<N>::quiet_NaN(),
                                    bits::charconv::from_chars_result{ b + 6, std::errc() } : bits::charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                            }
                            else {
                                if (b[1] == 'i') return b[2] == 'n' && b[3] == 'f' && b[4] == '"' ? t =  std::numeric_limits<N>::infinity(),
                                    bits::charconv::from_chars_result{ b + 5, std::errc() } : bits::charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                                return b[1] == 'n' && b[2] == 'a' && b[3] == 'n' && b[4] == '"' ? t =  std::numeric_limits<N>::quiet_NaN(),
                                    bits::charconv::from_chars_result{ b + 5, std::errc() } : bits::charconv::from_chars_result{ b, std::errc::invalid_argument }
                                ;
                            }
                        }
                        auto const r = bits::from_chars(b, e, t);
                        return { r.ptr, r.ec };
                    }
                // strict
                template <typename II, typename N, typename Cx>
                    static auto read(N& t, II& i, II e, Cx& cx)
                        -> enable_if_t<std::is_floating_point<N>::value, bool>
                    {
                        II const o = i;
                            char s[num_len_max::constant<prms_type<Cx>>(64)];
                            int const b = number_consumer<JSON<X>, T>::consume(s, s + sizeof(s), i, e);
                                return  (b != -1                                            || (io::rewind(i, o), cx|read_error::overflow)) &&
                                        (b !=  0                                            || (io::rewind(i, o), cx|read_error::floating_point_invalid)) &&
                                        (from_chars(s, s + sizeof(s), t).ec == std::errc()  || (io::rewind(i, o), cx|read_error::floating_point_invalid))
                                ;
                    }
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_floating_point<N>::value && X::number::strict, bool>
                    {
                        auto const b = i;
                        if (number_consumer<JSON<X>, T>::consume(i, e)) {
                            if (*b == '0' && i - b == 1) return t = 0, true;
                            auto const r = from_chars(b, i, t);
                            if (r.ec == std::errc() && r.ptr == i) return true;
                        }
                        return i = b, cx|read_error::floating_point_invalid;
                    }
                // not strict
                template <typename N, typename Cx>
                    static auto read(N& t, const char*& i, const char* e, Cx& cx)
                        -> enable_if_t<std::is_floating_point<N>::value && !X::number::strict, bool>
                    {
                        auto const r = from_chars(i, e, t);
                        return  (r.ec == std::errc() || (cx|read_error::floating_point_invalid)) &&
                                (i = r.ptr, true)
                        ;
                    }
        };

    template <typename X, typename T, typename II, typename Cx>
        inline bool number_read(T& t, II& i, II e, Cx& cx) {
            io::consume<X>(i, e);
            return number_reader<X, T>::read(t, i, e, cx);
        }

}}  // cxon::nums number conversion: read

namespace cxon { namespace nums { // number conversion: write

    template <typename X>
        struct opqt {
            static constexpr std::nullptr_t const beg = nullptr;
            static constexpr std::nullptr_t const end = nullptr;
        };
    template <typename X>
        struct opqt<JSON<X>> : JSON<X>::string {};

    template <typename X, typename T, typename O, typename Cx>
        inline auto number_write(O& o, const T& t, Cx& cx) -> enable_if_t<std::is_integral<T>::value, bool> {
            char s[std::numeric_limits<T>::digits10 + 3];
            auto const r = bits::to_chars(s, s + sizeof(s) / sizeof(char), t);
            return (r.ec == std::errc() || (cx|write_error::argument_invalid)) &&
                    io::poke<X>(o, s, r.ptr - s, cx)
            ;
        }

    template <typename X, typename T, typename O, typename Cx>
        inline auto number_write(O& o, const T& t, Cx& cx) -> enable_if_t<std::is_floating_point<T>::value, bool> {
            if (std::isinf(t)) {
                if (!io::poke<X>(o, opqt<X>::beg, cx)) return false;
                if (std::signbit(t) && !io::poke<X>(o, '-', cx)) return false;
                return io::poke<X>(o, "inf", cx) && io::poke<X>(o, opqt<X>::end, cx);
            }
            if (std::isnan(t))
                return io::poke<X>(o, opqt<X>::beg, cx) && io::poke<X>(o, "nan", cx) && io::poke<X>(o, opqt<X>::end, cx);
            CXON_ASSERT(std::isfinite(t), "unexpected");
            char s[std::numeric_limits<T>::max_digits10 * 2];
            auto const r = bits::to_chars(
                s, s + sizeof(s) / sizeof(char), t, fp_precision::constant<prms_type<Cx>>(std::numeric_limits<T>::max_digits10)
            );
            return (r.ec == std::errc() || (cx|write_error::argument_invalid)) &&
                    io::poke<X>(o, s, r.ptr - s, cx)
            ;
        }

}}  // cxon::nums number conversion: write

#endif // CXON_CHARIO_NUMBERS_HXX_
