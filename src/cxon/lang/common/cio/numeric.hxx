// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_NUMERIC_HXX_
#define CXON_CIO_NUMERIC_HXX_

#include "cio.hxx"
#include "chcls.hxx"
#include "../charconv.hxx"

#include <cmath> // isfinite, ...

#ifdef CXON_USE_FAST_FLOAT
#   include "fast_float/fast_float.h"
#   define CXON_HAS_FAST_FLOAT
#endif

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

#       define CXON_READ() *f = c, c = next(i, e), ++f;
#       define CXON_NEXT() { if (f == l) return -1; CXON_READ() }

        template <typename X, typename T, typename II>
            inline auto number_consume_(char* f, const char* l, II& i, II e)
                -> enable_if_t<std::is_integral<T>::value, int>
            {
                CXON_ASSERT(f && f < l, "unexpected");
                char c = peek(i, e);
                    if (is_sign_<T>(c))                 CXON_READ()
                    if (c == '0') {                     CXON_NEXT()
                                                        goto trap_end; }
                       if (!chr::is<X>::digit10(c))     return 0;
                    while ( chr::is<X>::digit10(c))     CXON_NEXT()
                trap_end:
                    return f != l ? (*f = '\0', 1) : -1;
            }
        template <typename X, typename T>
            inline auto number_consume_(const char*& i, const char* e)
                -> enable_if_t<std::is_integral<T>::value, int>
            {
                char c = *i;
                    if (is_sign_<T>(c)) goto trap_neg;
                // trap_pos
                    if (c == '0') return ++i, 1;
                       if (!chr::is<X>::digit10(c)) return 0;
                    while ( chr::is<X>::digit10(c)) c = next(i, e);
                    goto trap_end;
                trap_neg:
                    c = next(i, e);
                       if (!chr::is<X>::digit10(c)) return 0;
                    while ( chr::is<X>::digit10(c)) c = next(i, e);
                trap_end:
                    return 1;
            }

        template <typename X, typename T, typename II>
            inline auto number_consume_(char* f, const char* l, II& i, II e)
                -> enable_if_t<std::is_floating_point<T>::value, int>
            {   // as in RFC7159 (except nans)
                CXON_ASSERT(f && f < l, "unexpected");
                char c = peek(i, e);
                    CXON_IF_CONSTEXPR(!X::allow_javascript_nans) {
                        if (c == '"') {
                            ;                               CXON_READ()
                            if (is_sign_<T>(c))             CXON_NEXT()
                            if (c == 'i') {
                                CXON_NEXT() if (c != 'n')   return 0;
                                CXON_NEXT() if (c != 'f')   return 0;
                            }
                            else if (c == 'n') {
                                CXON_NEXT() if (c != 'a')   return 0;
                                CXON_NEXT() if (c != 'n')   return 0;
                            }
                            else                            return 0;
                            CXON_NEXT() if (c != '"')       return 0;
                            CXON_NEXT()                     goto trap_end;
                        }
                    }
                    if (is_sign_<T>(c))                     CXON_READ()
                    CXON_IF_CONSTEXPR( X::allow_javascript_nans) {
                        if (c == 'I') {
                            CXON_NEXT() if (c != 'n')       return 0;
                            CXON_NEXT() if (c != 'f')       return 0;
                            CXON_NEXT() if (c != 'i')       return 0;
                            CXON_NEXT() if (c != 'n')       return 0;
                            CXON_NEXT() if (c != 'i')       return 0;
                            CXON_NEXT() if (c != 't')       return 0;
                            CXON_NEXT() if (c != 'y')       return 0;
                            CXON_NEXT()                     goto trap_end;
                        }
                        if (c == 'N') {
                            CXON_NEXT() if (c != 'a')       return 0;
                            CXON_NEXT() if (c != 'N')       return 0;
                            CXON_NEXT()                     goto trap_end;
                        }
                    }
                    if (c == '0')                           goto trap_zero;
                //trap_whole:
                       if (!chr::is<X>::digit10(c))         return 0;
                    while ( chr::is<X>::digit10(c))         CXON_NEXT()
                    if (c == '.')                           goto trap_fraction;
                    if (c == 'e' || c == 'E')               goto trap_exponent;
                    ;                                       goto trap_end;
                trap_zero:
                    ;                                       CXON_NEXT()
                    if (c == '.')                           goto trap_fraction;
                    if (c == 'e' || c == 'E')               goto trap_exponent;
                    ;                                       goto trap_end;
                trap_fraction:
                    ;                                       CXON_NEXT()
                       if (!chr::is<X>::digit10(c))         return 0;
                    while ( chr::is<X>::digit10(c))         CXON_NEXT()
                    if (c == 'e' || c == 'E')               goto trap_exponent;
                    ;                                       goto trap_end;
                trap_exponent:
                    ;                                       CXON_NEXT()
                    if (c == '-' || c == '+')               CXON_NEXT()
                       if (!chr::is<X>::digit10(c))         return 0;
                    while ( chr::is<X>::digit10(c))         CXON_NEXT()
                    ;                                       goto trap_end;
                trap_end:
                    return f != l ? (*f = '\0', 1) : -1;
            }
        template <typename X, typename T>
            inline auto number_consume_(const char*& i, const char* e)
                -> enable_if_t<std::is_floating_point<T>::value, int>
            {   // as in RFC7159
                char c = peek(i, e);
                    CXON_IF_CONSTEXPR(!X::allow_javascript_nans) {
                        if (c == '"') {
                            ;                           c = next(i, e);
                            if (is_sign_<T>(c))         c = next(i, e);
                            if (c == 'i') {
                                if (next(i, e) != 'n')  return 0;
                                if (next(i, e) != 'f')  return 0;
                            }
                            else if (c == 'n') {
                                if (next(i, e) != 'a')  return 0;
                                if (next(i, e) != 'n')  return 0;
                            }
                            else                        return 0;
                            if (next(i, e) != '"')      return 0;
                            return ++i, 1;
                        }
                    }
                    if (is_sign_<T>(c))                 c = next(i, e);
                    CXON_IF_CONSTEXPR( X::allow_javascript_nans) {
                        if (c == 'I') {
                            if (next(i, e) != 'n')      return 0;
                            if (next(i, e) != 'f')      return 0;
                            if (next(i, e) != 'i')      return 0;
                            if (next(i, e) != 'n')      return 0;
                            if (next(i, e) != 'i')      return 0;
                            if (next(i, e) != 't')      return 0;
                            if (next(i, e) != 'y')      return 0;
                            return ++i, 1;
                        }
                        if (c == 'N') {
                            if (next(i, e) != 'a')      return 0;
                            if (next(i, e) != 'N')      return 0;
                            return ++i, 1;
                        }
                    }
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
                trap_end:
                    return 1;
            }

#       undef CXON_NEXT
#       undef CXON_READ

        template <typename X, typename T, typename II, typename Cx>
            inline auto number_read_(T& t, II& i, II e, Cx& cx)
                -> enable_if_t<std::is_integral<T>::value, bool>
            {
                II const o = i;
                    char s[num_len_max::constant<napa_type<Cx>>(32)];
                    switch (number_consume_<X, T>(std::begin(s), std::end(s), i, e)) {
                        case -1: return rewind(i, o), cx/X::read_error::overflow;
                        case  0: return rewind(i, o), cx/X::read_error::integral_invalid;
                        default: {
                            auto const r = charconv::from_chars(std::begin(s), std::end(s), t); CXON_ASSERT(r.ec == std::errc(), "unexpected");
                            return true;
                        }
                    }
            }
        template <typename X, typename T, typename Cx>
            inline auto number_read_(T& t, const char*& i, const char* e, Cx& cx)
                -> enable_if_t<std::is_integral<T>::value && X::number::strict, bool>
            {
                auto const b = i;
                if (!number_consume_<X, T>(i, e))
                    return i = b, cx/X::read_error::integral_invalid;
                if (i - b == 1)
                    return t = *b - '0', true;
                auto const r = charconv::from_chars(b, i, t); CXON_ASSERT(r.ec == std::errc() && r.ptr == i, "unexpected");
                return true;
            }
        template <typename X, typename T, typename Cx>
            inline auto number_read_(T& t, const char*& i, const char* e, Cx& cx)
                -> enable_if_t<std::is_integral<T>::value && !X::number::strict, bool>
            {
                auto const r = charconv::from_chars(i, e, t);
                    if (r.ec != std::errc()) return cx/X::read_error::integral_invalid;
                return i = r.ptr, true;
            }

        template <typename X, typename T>
            inline charconv::imp::from_chars_result from_chars_(const char* b, const char* e, T& t) noexcept {
                if (e - b >= 3) {
                    CXON_IF_CONSTEXPR (!X::allow_javascript_nans) {
                        if (b[0] == '"') {
                            auto s = b[1] == '-' ? ++b + 1 : ++b;
                            switch (s[0]) {
                                case 'i':
                                    if (e - s >= 4 && s[1] == 'n' && s[2] == 'f' && s[3] == '"') {
                                        t = s != b ? -std::numeric_limits<T>::infinity() : std::numeric_limits<T>::infinity();
                                        return charconv::imp::from_chars_result{ s + 4, std::errc() };
                                    }
                                    break;
                                case 'n':
                                    if (e - s >= 4 && s[1] == 'a' && s[2] == 'n' && s[3] == '"') {
                                        t = s != b ? -std::numeric_limits<T>::quiet_NaN() : std::numeric_limits<T>::quiet_NaN();
                                        return charconv::imp::from_chars_result{ s + 4, std::errc() };
                                    }
                                    break;
                            }
                            return charconv::imp::from_chars_result { s != b ? --b : b, std::errc::invalid_argument };
                        }
                    }
                    CXON_IF_CONSTEXPR ( X::allow_javascript_nans) {
                        auto s = b[0] == '-' ? b + 1 : b;
                        switch (s[0]) {
                            case 'I':
                                if (e - s >= 8 && std::char_traits<char>::compare(s + 1, "nfinity", 7) == 0) {
                                    t = s != b ? -std::numeric_limits<T>::infinity() : std::numeric_limits<T>::infinity();
                                    return charconv::imp::from_chars_result { s + 8, std::errc() };
                                }
                                return charconv::imp::from_chars_result { b, std::errc::invalid_argument };
                            case 'N':
                                if (e - s >= 3 && s[1] == 'a' && s[2] == 'N') {
                                    t = s != b ? -std::numeric_limits<T>::quiet_NaN() : std::numeric_limits<T>::quiet_NaN();
                                    return charconv::imp::from_chars_result { s + 3, std::errc() };
                                }
                                return charconv::imp::from_chars_result { b, std::errc::invalid_argument };
                        }
                    }
                }
#               ifdef CXON_HAS_FAST_FLOAT
                    CXON_IF_CONSTEXPR (!(std::is_same<T, long double>::value)) {
                        auto const r = fast_float::from_chars(b, e, t);
                        return { r.ptr, r.ec };
                    }
                    else {
                        auto const r = charconv::from_chars(b, e, t);
                        return { r.ptr, r.ec };
                    }
#               else
                    auto const r = charconv::from_chars(b, e, t);
                    return { r.ptr, r.ec };
#               endif
            }
        template <typename X, typename T, typename II, typename Cx>
            inline auto number_read_(T& t, II& i, II e, Cx& cx)
                -> enable_if_t<std::is_floating_point<T>::value, bool>
            {
                II const o = i;
                    char s[num_len_max::constant<napa_type<Cx>>(64)];
                    switch (number_consume_<X, T>(std::begin(s), std::end(s), i, e)) {
                        case -1: return rewind(i, o), cx/X::read_error::overflow;
                        case  0: return rewind(i, o), cx/X::read_error::floating_point_invalid;
                        default: {
                            auto const r = from_chars_<X>(std::begin(s), std::end(s), t); CXON_ASSERT(r.ec == std::errc(), "unexpected");
                            return true;
                        }
                    }
            }
        template <typename X, typename T, typename Cx>
            inline auto number_read_(T& t, const char*& i, const char* e, Cx& cx)
                -> enable_if_t<std::is_floating_point<T>::value &&  X::number::strict, bool>
            {
                auto const b = i;
                if (!number_consume_<X, T>(i, e))
                    return i = b, cx/X::read_error::floating_point_invalid;
                if (i - b == 1)
                    return t = T(*b - '0'), true;
                auto const r = from_chars_<X>(b, i, t); CXON_ASSERT(r.ec == std::errc() && r.ptr == i, "unexpected");
                return true;
            }
        template <typename X, typename T, typename Cx>
            inline auto number_read_(T& t, const char*& i, const char* e, Cx& cx)
                -> enable_if_t<std::is_floating_point<T>::value && !X::number::strict, bool>
            {
                auto const r = from_chars_<X>(i, e, t);
                    if (r.ec != std::errc()) return cx/X::read_error::floating_point_invalid;
                return i = r.ptr, true;
            }

    }
    template <typename X, typename T, typename II, typename Cx>
        inline bool number_read(T& t, II& i, II e, Cx& cx) {
            return consume<X>(i, e, cx) && imp::number_read_<X>(t, i, e, cx);
        }

}}}

namespace cxon { namespace cio { namespace num { // write

    namespace imp {

        template <typename X, typename T, typename O, typename Cx>
            inline auto number_write_(O& o, T t, Cx& cx) -> enable_if_t<std::is_integral<T>::value, bool> {
                char s[std::numeric_limits<T>::digits10 + 3];
                    auto const r = charconv::to_chars(std::begin(s), std::end(s), t); CXON_ASSERT(r.ec == std::errc(), "unexpected");
#               if __cplusplus >= 202002L && defined(__GNUC__) && __GNUC__ >= 11 && !defined(__clang__)
#                   pragma GCC diagnostic push
                    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100366
#                   pragma GCC diagnostic ignored "-Wrestrict"
#               endif
                    return poke<X>(o, s, r.ptr, cx);
#               if __cplusplus >= 202002L && defined(__GNUC__) && __GNUC__ >= 11 && !defined(__clang__)
#                   pragma GCC diagnostic pop
#               endif
            }

        template <typename X, typename T, typename O, typename Cx>
            inline auto number_write_(O& o, T t, Cx& cx) -> enable_if_t<std::is_floating_point<T>::value, bool> {
                if (std::isfinite(t)) {
                    char s[std::numeric_limits<T>::max_digits10 * 2];
                        constexpr auto fp_precision = fp_precision::constant<napa_type<Cx>>(std::numeric_limits<T>::max_digits10);
                        auto const r = charconv::to_chars(std::begin(s), std::end(s), t, fp_precision); CXON_ASSERT(r.ec == std::errc(), "unexpected");
                    return poke<X>(o, s, r.ptr, cx);
                }
                else {
                    CXON_IF_CONSTEXPR (!X::allow_javascript_nans) {
                        CXON_IF_CONSTEXPR (!is_unquoted_key_context<X>::value) {
                            if (!poke<X>(o, X::string::del, cx))                                return false;
                        }
                        if (std::isinf(t)) {
                            if (!poke<X>(o, std::signbit(t) ? "-inf" : "inf", cx))              return false;
                        }
                        else {
                            if (!poke<X>(o, std::signbit(t) ? "-nan" : "nan", cx))              return false;
                        }
                        CXON_IF_CONSTEXPR (!is_unquoted_key_context<X>::value) {
                            if (!poke<X>(o, X::string::del, cx))                                return false;
                        }
                    }
                    CXON_IF_CONSTEXPR ( X::allow_javascript_nans) {
                        if (std::isinf(t)) {
                            if (!poke<X>(o, std::signbit(t) ? "-Infinity" : "Infinity", cx))    return false;
                        }
                        else {
                            if (!poke<X>(o, std::signbit(t) ? "-NaN" : "NaN", cx))              return false;
                        }
                    }
                    return true;
                }
            }

    }
    template <typename X, typename T, typename O, typename Cx>
        inline bool number_write(O& o, T t, Cx& cx) {
            return imp::number_write_<X>(o, t, cx);
        }

}}}

#endif // CXON_CIO_NUMERIC_HXX_
