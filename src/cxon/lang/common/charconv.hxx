// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHARCONV_HXX_
#define CXON_CHARCONV_HXX_

#include "cxon/utility.hxx"

#include <cstdlib>
#include <cstdio>
#include <cerrno>

#include <algorithm> // reverse
#include <limits>
#include <type_traits>

#if __cplusplus >= 201703L
#   if defined(__has_include) && __has_include(<charconv>)
#       include <charconv>
#       ifdef __GLIBCXX__ // partial
            namespace std { enum class chars_format; }
#       endif
#       define CXON_HAS_CHARCONV
#   endif
#endif

namespace cxon { namespace charconv { namespace imp {

    enum class chars_format {
        scientific  = 1,
        fixed       = 2,
        hex         = 4,
        general     = fixed|scientific
    };

    struct from_chars_result {
        const char* ptr;
        std::errc ec;
    };

    namespace {

        template <typename T, typename F>
            constexpr auto clamp_(F t) noexcept -> enable_if_t<sizeof(T) == sizeof(F), T> {
                return t;
            }
        template <typename T, typename F>
            inline auto clamp_(F f) noexcept    -> enable_if_t<sizeof(T) < sizeof(F) && std::is_signed<T>(), T> {
                        if (f < std::numeric_limits<T>::min()) errno = ERANGE, f = std::numeric_limits<T>::min();
                else    if (f > std::numeric_limits<T>::max()) errno = ERANGE, f = std::numeric_limits<T>::max();
                return (T)f;
            }
        template <typename T, typename F>
            inline auto clamp_(F f) noexcept    -> enable_if_t<sizeof(T) < sizeof(F) && std::is_unsigned<T>(), T> {
                if (std::numeric_limits<T>::max() < f) errno = ERANGE, f = std::numeric_limits<T>::max();
                return (T)f;
            }

        template <typename T>
            inline auto is_sign_invalid_(char c) noexcept -> enable_if_t<std::is_signed<T>::value, bool> {
                return c == '+';
            }
        template <typename T>
            inline auto is_sign_invalid_(char c) noexcept -> enable_if_t<std::is_unsigned<T>::value, bool> {
                return c == '+' || c == '-';
            }

        template <typename T, typename Cv>
            inline from_chars_result number_from_chars_(const char* first, const char*, Cv convert, T& value, int base) noexcept {
                CXON_ASSERT(base >= 2 && base <= 36, "invalid base");
                    if (is_sign_invalid_<T>(*first)) return { first, std::errc::invalid_argument };
                char* end;
                errno = 0; auto const v = clamp_<T>(convert(first, &end, base));
                return { end, end != first ? errno != ERANGE ? (value = v, std::errc{}) : std::errc::result_out_of_range : std::errc::invalid_argument };
            }
        template <typename T, typename Cv>
            inline from_chars_result number_from_chars_(const char* first, const char*, Cv convert, T& value, chars_format) noexcept {
                    if (is_sign_invalid_<T>(*first)) return { first, std::errc::invalid_argument };
                char* end;
                errno = 0; auto const v = convert(first, &end);
                return { end, end != first ? errno != ERANGE ? (value = v, std::errc{}) : std::errc::result_out_of_range : std::errc::invalid_argument };
            }

    }

#   define CXON_FROM_CHARS(T, C)\
        inline from_chars_result from_chars(const char* first, const char* last, T& value, int base = 10) noexcept {\
            return number_from_chars_(first, last, C, value, base);\
        }
        CXON_FROM_CHARS(char, strtol)
        CXON_FROM_CHARS(signed char, strtol)
        CXON_FROM_CHARS(unsigned char, strtoul)
        CXON_FROM_CHARS(short, strtol)
        CXON_FROM_CHARS(unsigned short, strtoul)
        CXON_FROM_CHARS(int, strtol)
        CXON_FROM_CHARS(unsigned int, strtoul)
        CXON_FROM_CHARS(long, strtol)
        CXON_FROM_CHARS(unsigned long, strtoul)
        CXON_FROM_CHARS(long long, strtoll)
        CXON_FROM_CHARS(unsigned long long, strtoull)
#   undef CXON_FROM_CHARS
#   define CXON_FROM_CHARS(T, C)\
        inline from_chars_result from_chars(const char* first, const char* last, T& value, chars_format fmt = chars_format::general) noexcept {\
            return number_from_chars_(first, last, C, value, fmt);\
        }
        CXON_FROM_CHARS(float, strtof)
        CXON_FROM_CHARS(double, strtod)
        CXON_FROM_CHARS(long double, strtold)
#   undef CXON_FROM_CHARS

    struct to_chars_result {
        char* ptr;
        std::errc ec;
    };

    namespace {

        template <typename T>
            inline auto itoa_(char* b, T t, int base) noexcept -> typename std::enable_if<std::is_unsigned<T>::value, unsigned>::type {
                char    *p = b;
                do      *p = "0123456789abcdefghijklmnopqrstuvwxyz"[t % (T)base], ++p;
                while   (t /= (T)base);
                return  std::reverse(b, p), unsigned(p - b);
            }
        template <typename T>
            inline auto itoa_(char* b, T t, int base) noexcept -> typename std::enable_if<std::is_signed<T>::value, unsigned>::type {
                using U = typename std::make_unsigned<T>::type;
                return t < 0 ?
                    *b = '-', itoa_<U>(++b, -t, base) + 1 :
                              itoa_<U>(  b,  t, base)
                ;
            }

        // to_chars: on error, returns a value of type to_chars_result holding errc::value_too_large in ec,
        //  a copy of the value last in ptr, and leaves the contents of the range [first, last) in unspecified state,
        //  but we don't care, the buffer must be sufficient
        template <typename T>
            inline auto number_to_chars_(char* first, char* last, T value, int base) noexcept
                 -> typename std::enable_if<std::is_integral<T>::value, to_chars_result>::type
            {
                CXON_ASSERT(base >= 2 && base <= 36, "invalid base");
                CXON_ASSERT(last > first && std::size_t(last - first) >= std::numeric_limits<T>::digits10 + 1, "buffer too small");
                unsigned const w = itoa_(first, value, base);
                return { first + w, std::errc{} };
            }

        template <typename> struct fmt_;
        template <> struct fmt_<float>          { static constexpr char const*const str = "%.*g";   };
        template <> struct fmt_<double>         { static constexpr char const*const str = "%.*g";   };
        template <> struct fmt_<long double>    { static constexpr char const*const str = "%.*Lg";  };

        template <typename T>
            inline auto number_to_chars_(char* first, char* last, T value, int precision) noexcept
                 -> typename std::enable_if<std::is_floating_point<T>::value, to_chars_result>::type
            {
                std::size_t const l = last - first;
                int const w = std::snprintf(first, l, fmt_<T>::str, precision, value); CXON_ASSERT(w > 0 && (std::size_t)w < l, "conversion failed");
                return { first + w, std::errc{} };
            }

    }

#   define CXON_TO_CHARS(T)\
        inline to_chars_result to_chars(char* first, char* last, T value, int base = 10) noexcept {\
            return number_to_chars_<T>(first, last, value, base);\
        }
        CXON_TO_CHARS(signed char)
        CXON_TO_CHARS(unsigned char)
        CXON_TO_CHARS(short)
        CXON_TO_CHARS(unsigned short)
        CXON_TO_CHARS(int)
        CXON_TO_CHARS(unsigned int)
        CXON_TO_CHARS(long)
        CXON_TO_CHARS(unsigned long)
        CXON_TO_CHARS(long long)
        CXON_TO_CHARS(unsigned long long)
#   undef CXON_TO_CHARS
#   define CXON_TO_CHARS(T)\
        inline to_chars_result to_chars(char* first, char* last, T value, int precision) noexcept {\
            return number_to_chars_<T>(first, last, value, precision);\
        }
        CXON_TO_CHARS(float)
        CXON_TO_CHARS(double)
        CXON_TO_CHARS(long double)
#   undef CXON_TO_CHARS

}}}

namespace cxon { namespace charconv { // <charconv>

#   ifdef CXON_HAS_CHARCONV
        template <typename T>
            inline auto from_chars_i_(option<1>, const char* f, const char* l, T& t, int base = 10) noexcept
                ->  decltype(std::from_chars(f, l, t, base), std::from_chars_result())
            {
                return std::from_chars(f, l, t, base);
            }
        template <typename T>
            inline auto from_chars_i_(option<0>, const char* f, const char* l, T& t, int base = 10) noexcept
                ->  std::from_chars_result
            {
                auto const r = imp::from_chars(f, l, t, base);
                return { r.ptr, r.ec };
            }
        template <typename T>
            inline auto from_chars(const char* f, const char* l, T& t, int base = 10) noexcept
                ->  enable_if_t<std::is_integral<T>::value, std::from_chars_result>
            {
                return from_chars_i_(option<1>(), f, l, t, base);
            }
        std::from_chars_result from_chars(const char*, const char*, bool&, int = 10) = delete;

        template <typename T>
            inline auto from_chars_f_(option<1>, const char* f, const char* l, T& t) noexcept
                ->  decltype(std::from_chars(f, l, t), std::from_chars_result())
            {
                return std::from_chars(f, l, t);
            }
        template <typename T>
            inline auto from_chars_f_(option<0>, const char* f, const char* l, T& t) noexcept
                ->  std::from_chars_result
            {
                auto const r = imp::from_chars(f, l, t);
                return { r.ptr, r.ec };
            }
        template <typename T>
            inline auto from_chars(const char* f, const char* l, T& t) noexcept
                ->  enable_if_t<std::is_floating_point<T>::value, std::from_chars_result>
            {
                return from_chars_f_(option<1>(), f, l, t);
            }

        template <typename T>
            inline auto to_chars_i_(option<1>, char* f, char* l, T t, int base = 10) noexcept
                ->  decltype(std::to_chars(f, l, t, base), std::to_chars_result())
            {
                return std::to_chars(f, l, t, base);
            }
        template <typename T>
            inline auto to_chars_i_(option<0>, char* f, char* l, T t, int base = 10) noexcept
                ->  std::to_chars_result
            {
                auto const r = imp::to_chars(f, l, t, base);
                return { r.ptr, r.ec };
            }
        template <typename T>
            inline auto to_chars(char* f, char* l, T t, int base = 10) noexcept
                ->  enable_if_t<std::is_integral<T>::value, std::to_chars_result>
            {
                return to_chars_i_(option<1>(), f, l, t, base);
            }
        std::to_chars_result to_chars(char*, char*, bool, int = 10) = delete;

        // g++: error: 'general' is not a member of 'std::chars_format'
        template <typename T, typename = void>
            struct general {
                static constexpr T      value = (T)imp::chars_format::general;
            };
        template <typename T>
            struct general<T, enable_if_t<sizeof(T::general)>> {
                static constexpr T      value = T::general;
            };

        template <typename T>
            inline auto to_chars_f_(option<2>, char* f, char* l, T t, int precision) noexcept
                ->  decltype(std::to_chars(f, l, t, (std::chars_format)0, precision), std::to_chars_result())
            {
#               if defined(_MSC_VER) && _MSC_VER <= 1923
                    auto const r = imp::to_chars(f, l, t, precision);
                    return { r.ptr, r.ec };
#               else
                    return std::to_chars(f, l, t, general<std::chars_format>::value, precision);
#               endif
            }
        template <typename T>
            inline auto to_chars_f_(option<1>, char* f, char* l, T t, int) noexcept
                ->  decltype(std::to_chars(f, l, t), std::to_chars_result())
            {
                return std::to_chars(f, l, t);
            }
        template <typename T>
            inline auto to_chars_f_(option<0>, char* f, char* l, T t, int precision) noexcept
                ->  std::to_chars_result
            {
                auto const r = imp::to_chars(f, l, t, precision);
                return { r.ptr, r.ec };
            }
        template <typename T>
            inline auto to_chars(char* f, char* l, T t, int precision) noexcept
                ->  enable_if_t<std::is_floating_point<T>::value, std::to_chars_result>
            {
                return to_chars_f_(option<2>(), f, l, t, precision);
            }
#   else
        using imp::from_chars;
        using imp::to_chars;
#   endif

}}

#endif // CXON_CHARCONV_HXX_
