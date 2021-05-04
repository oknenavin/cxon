// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_NUMERIC_HXX_
#define CXON_CIO_NUMERIC_HXX_

#include "cio.hxx"
#include "chcls.hxx"
#include "../charconv.hxx"

#include <cmath> // isfinite, ...

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace num { // number conversion

    template <typename X, typename T, typename II, typename Cx>
        inline bool number_read(T& t, II& i, II e, Cx& cx);

    template <typename X, typename T, typename O, typename Cx>
        inline bool number_write(O& o, T t, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace num { // read

    namespace imp {

        template <typename T>
            inline auto is_sign_(char c) -> enable_if_t<std::is_signed<T>::value, bool> {
                return c == '-';
            }
        template <typename T>
            constexpr auto is_sign_(char) -> enable_if_t<std::is_unsigned<T>::value, bool> {
                return false;
            }

#       define CXON_NEXT() { if (f == l) return -1; *f = c, c = next(i, e), ++f; }

        template <typename X, typename T, typename II>
            inline auto number_consume_(char* f, const char* l, II& i, II e)
                -> enable_if_t<std::is_integral<T>::value, int>
            {
                CXON_ASSERT(f && f < l, "unexpected");
                char c = peek(i, e);
                    if (is_sign_<T>(c))                 CXON_NEXT()
                    if (c == '0') {                     CXON_NEXT()
                                                        goto trap_end; }
                       if (!chr::is<X>::digit10(c))     return 0;
                    while ( chr::is<X>::digit10(c))     CXON_NEXT()
                trap_end:
                    return f != l ? (*f = '\0', 1) : 0;
            }
        template <typename X, typename T>
            inline auto number_consume_(const char*& i, const char* e)
                -> enable_if_t<std::is_integral<T>::value, int>
            {
                cio::consume<X>(i, e);
                char c = peek(i, e);
                if (is_sign_<T>(c)) goto trap_neg;
                // trap_pos
                    if (c == '0') return ++i, 1;
                    while (chr::is<X>::digit10(c)) c = next(i, e);
                    goto trap_end;
                trap_neg:
                    c = next(i, e);
                       if (c == '0' || !chr::is<X>::digit10(c)) return 0;
                    while (chr::is<X>::digit10(c)) c = next(i, e);
                trap_end:
                    return 1;
            }

        template <typename X, typename T, typename II>
            inline auto number_consume_(char* f, const char* l, II& i, II e)
                -> enable_if_t<std::is_floating_point<T>::value, int>
            {   // as in RFC7159
                CXON_ASSERT(f && f < l, "unexpected");
                char c = peek(i, e);
                    if (c == '"')                       goto trap_spec_beg;
                    if (is_sign_<T>(c))                 CXON_NEXT()
                    if (c == '0')                       goto trap_zero;
                //trap_whole:
                       if (!chr::is<X>::digit10(c))     return 0;
                    while ( chr::is<X>::digit10(c))     CXON_NEXT()
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
                       if (!chr::is<X>::digit10(c))     return 0;
                    while ( chr::is<X>::digit10(c))     CXON_NEXT()
                    if (c == 'e' || c == 'E')           goto trap_exponent;
                    ;                                   goto trap_end;
                trap_exponent:
                    ;                                   CXON_NEXT()
                    if (c == '-' || c == '+')           CXON_NEXT()
                       if (!chr::is<X>::digit10(c))     return 0;
                    while ( chr::is<X>::digit10(c))     CXON_NEXT()
                    ;                                   goto trap_end;
                trap_spec_beg:
                    ;                                   CXON_NEXT()
                    if (is_sign_<T>(c))                 CXON_NEXT()
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
        template <typename X, typename T>
            inline auto number_consume_(const char*& i, const char* e)
                -> enable_if_t<std::is_floating_point<T>::value, int>
            {   // as in RFC7159
                char c = peek(i, e);
                    if (c == '"')                       goto trap_spec_beg;
                    if (is_sign_<T>(c))                 c = next(i, e);
                    if (c == '0')                       goto trap_zero;
                //trap_whole:
                       if (!chr::is<X>::digit10(c))     return 0;
                    while (chr::is<X>::digit10(c))      c = next(i, e);
                    if (c == '.')                       goto trap_fraction;
                    if (c == 'e' || c == 'E')           goto trap_exponent;
                    ;                                   goto trap_end;
                trap_zero:
                    ;                                   c = next(i, e);
                    if (c == '.')                       goto trap_fraction;
                    if (c == 'e' || c == 'E')           goto trap_exponent;
                    ;                                   goto trap_end;
                trap_fraction:
                    ;                                   c = next(i, e);
                       if (!chr::is<X>::digit10(c))     return 0;
                    while ( chr::is<X>::digit10(c))     c = next(i, e);
                    if (c == 'e' || c == 'E')           goto trap_exponent;
                    ;                                   goto trap_end;
                trap_exponent:
                    ;                                   c = next(i, e);
                    if (c == '-' || c == '+')           c = next(i, e);
                       if (!chr::is<X>::digit10(c))     return 0;
                    while ( chr::is<X>::digit10(c))     c = next(i, e);
                    ;                                   goto trap_end;
                trap_spec_beg:
                    ;                                   c = next(i, e);
                    if (is_sign_<T>(c))                 c = next(i, e);
                    if (c == 'i')                       goto trap_inf;
                    if (c == 'n')                       goto trap_nan;
                    ;                                   return 0;
                trap_inf:
                    c = next(i, e); if (c != 'n')       return 0;
                    c = next(i, e); if (c != 'f')       return 0;
                    ;                                   goto trap_spec_end;
                trap_nan:
                    c = next(i, e); if (c != 'a')       return 0;
                    c = next(i, e); if (c != 'n')       return 0;
                trap_spec_end:
                    c = next(i, e); if (c != '"')       return 0;
                    ++i;
                trap_end:
                    return 1;
            }

#       undef CXON_NEXT

        template <typename X, typename T, typename II, typename Cx>
            inline auto number_read_(T& t, II& i, II e, Cx& cx)
                -> enable_if_t<std::is_integral<T>::value, bool>
            {
                II const o = i;
                    char s[num_len_max::constant<napa_type<Cx>>(32)];
                    int const b = number_consume_<X, T>(std::begin(s), std::end(s), i, e);
                    return  (b != -1                                                                || (rewind(i, o), cx/X::read_error::overflow)) &&
                            (b !=  0                                                                || (rewind(i, o), cx/X::read_error::integral_invalid)) &&
                            (charconv::from_chars(std::begin(s), std::end(s), t).ec == std::errc()  || (rewind(i, o), cx/X::read_error::integral_invalid))
                    ;
            }
        template <typename X, typename T, typename Cx>
            inline auto number_read_(T& t, const char*& i, const char* e, Cx& cx)
                -> enable_if_t<std::is_integral<T>::value && X::number::strict, bool>
            {
                auto const b = i;
                if (number_consume_<X, T>(i, e)) {
                    if (*b == '0' && i - b == 1) return t = 0, true;
                    auto const r = charconv::from_chars(b, i, t);
                    if (r.ec == std::errc() && r.ptr == i) return true;
                }
                return i = b, cx/X::read_error::integral_invalid;
            }
        template <typename X, typename T, typename Cx>
            inline auto number_read_(T& t, const char*& i, const char* e, Cx& cx)
                -> enable_if_t<std::is_integral<T>::value && !X::number::strict, bool>
            {
                auto const r = charconv::from_chars(i, e, t);
                return (r.ec == std::errc() && (i = r.ptr, true)) || cx/X::read_error::integral_invalid;
            }

        template <typename T>
            inline charconv::imp::from_chars_result from_chars_(const char* b, const char* e, T& t) {
                if (*b == '"') {
                    if (b[1] == '-' && e - b >= 6) { // (false-positive [&1]) warning: array subscript X is outside array bounds of 'char [4]' [-Warray-bounds]
                        if (b[2] == 'i') return b[3] == 'n' && b[4] == 'f' && b[5] == '"' ? t = -std::numeric_limits<T>::infinity(),
                            charconv::imp::from_chars_result{ b + 6, std::errc() } : charconv::imp::from_chars_result{ b, std::errc::invalid_argument }
                        ;
                        return b[2] == 'n' && b[3] == 'a' && b[4] == 'n' && b[5] == '"' ? t =  std::numeric_limits<T>::quiet_NaN(),
                            charconv::imp::from_chars_result{ b + 6, std::errc() } : charconv::imp::from_chars_result{ b, std::errc::invalid_argument }
                        ;
                    }
                    else if (e - b >= 5) { // (false-positive [&1]) warning: array subscript X is outside array bounds of 'char [4]' [-Warray-bounds]
                        if (b[1] == 'i') return b[2] == 'n' && b[3] == 'f' && b[4] == '"' ? t =  std::numeric_limits<T>::infinity(),
                            charconv::imp::from_chars_result{ b + 5, std::errc() } : charconv::imp::from_chars_result{ b, std::errc::invalid_argument }
                        ;
                        return b[1] == 'n' && b[2] == 'a' && b[3] == 'n' && b[4] == '"' ? t =  std::numeric_limits<T>::quiet_NaN(),
                            charconv::imp::from_chars_result{ b + 5, std::errc() } : charconv::imp::from_chars_result{ b, std::errc::invalid_argument }
                        ;
                    }
                }
                auto const r = charconv::from_chars(b, e, t);
                return { r.ptr, r.ec };
            }
        template <typename X, typename T, typename II, typename Cx>
            inline auto number_read_(T& t, II& i, II e, Cx& cx)
                -> enable_if_t<std::is_floating_point<T>::value, bool>
            {
                II const o = i;
                    char s[num_len_max::constant<napa_type<Cx>>(64)];
                    int const b = number_consume_<X, T>(std::begin(s), std::end(s), i, e);
                        // coverity[overrun-buffer-val] - if b > 0 and the input is a string, the possible values are \"nan\", \"inf\" and \"-inf\" (see [&1])
                        return  (b != -1                                                        || (rewind(i, o), cx/X::read_error::overflow)) &&
                                (b !=  0                                                        || (rewind(i, o), cx/X::read_error::floating_point_invalid)) &&
                                (from_chars_(std::begin(s), std::end(s), t).ec == std::errc()   || (rewind(i, o), cx/X::read_error::floating_point_invalid))
                        ;
            }
        template <typename X, typename T, typename Cx>
            inline auto number_read_(T& t, const char*& i, const char* e, Cx& cx)
                -> enable_if_t<std::is_floating_point<T>::value && X::number::strict, bool>
            {
                auto const b = i;
                if (number_consume_<X, T>(i, e)) {
                    if (*b == '0' && i - b == 1) return t = 0, true;
                    auto const r = from_chars_(b, i, t);
                    if (r.ec == std::errc() && r.ptr == i) return true;
                }
                return i = b, cx/X::read_error::floating_point_invalid;
            }
        template <typename X, typename T, typename Cx>
            inline auto number_read_(T& t, const char*& i, const char* e, Cx& cx)
                -> enable_if_t<std::is_floating_point<T>::value && !X::number::strict, bool>
            {
                auto const r = from_chars_(i, e, t);
                return  (r.ec == std::errc() || cx/X::read_error::floating_point_invalid) &&
                        (i = r.ptr, true)
                ;
            }

    }
    template <typename X, typename T, typename II, typename Cx>
        inline bool number_read(T& t, II& i, II e, Cx& cx) {
            consume<X>(i, e);
            return imp::number_read_<X>(t, i, e, cx);
        }

}}}

namespace cxon { namespace cio { namespace num { // write

    namespace imp {

        template <typename X, typename T, typename O, typename Cx>
            inline auto number_write_(O& o, T t, Cx& cx) -> enable_if_t<std::is_integral<T>::value, bool> {
                char s[std::numeric_limits<T>::digits10 + 3];
                auto const r = charconv::to_chars(std::begin(s), std::end(s), t);
                return (r.ec == std::errc() || cx/X::write_error::argument_invalid) &&
                        poke<X>(o, s, r.ptr, cx)
                ;
            }

        template <typename X, typename T, typename O, typename Cx>
            inline auto number_write_(O& o, T t, Cx& cx) -> enable_if_t<std::is_floating_point<T>::value, bool> {
                if (std::isinf(t)) {
                    if (!poke<X>(o, X::string::beg, cx)) return false;
                    if (std::signbit(t) && !poke<X>(o, '-', cx)) return false;
                    return poke<X>(o, "inf", cx) && poke<X>(o, X::string::end, cx);
                }
                if (std::isnan(t))
                    return poke<X>(o, X::string::beg, cx) && poke<X>(o, "nan", cx) && poke<X>(o, X::string::end, cx);
                CXON_ASSERT(std::isfinite(t), "unexpected");
                char s[std::numeric_limits<T>::max_digits10 * 2];
                auto const r = charconv::to_chars(
                    std::begin(s), std::end(s), t, fp_precision::constant<napa_type<Cx>>(std::numeric_limits<T>::max_digits10)
                );
                return (r.ec == std::errc() || cx/X::write_error::argument_invalid) &&
                        poke<X>(o, s, r.ptr, cx)
                ;
            }

    }
    template <typename X, typename T, typename O, typename Cx>
        inline bool number_write(O& o, T t, Cx& cx) {
            return imp::number_write_<X>(o, t, cx);
        }

}}}

#endif // CXON_CIO_NUMERIC_HXX_
