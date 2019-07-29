// Copyright (c) 2017-2019 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_BITS_CHARCONV_HXX_
#define CXON_BITS_CHARCONV_HXX_

#include <cstdlib>
#include <cstdio>
#include <cerrno>

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

namespace cxon { namespace bits { namespace charconv {

    using namespace std;

    enum class chars_format {
        scientific  = 1,
        fixed       = 2,
        hex         = 4,
        general     = fixed|scientific
    };

    struct from_chars_result {
        const char* ptr;
        errc ec;
    };

    namespace bits {

        template <typename T, typename F>
            constexpr auto clamp(F t)       -> enable_if_t<sizeof(T) == sizeof(F), T> {
                return t;
            }
        template <typename T, typename F>
            inline auto clamp(F f) noexcept -> enable_if_t<sizeof(T) < sizeof(F) && std::is_signed<T>(), T> {
                        if (f < numeric_limits<T>::min()) errno = ERANGE, f = numeric_limits<T>::min();
                else    if (f > numeric_limits<T>::max()) errno = ERANGE, f = numeric_limits<T>::max();
                return (T)f;
            }
        template <typename T, typename F>
            inline auto clamp(F f) noexcept -> enable_if_t<sizeof(T) < sizeof(F) && std::is_unsigned<T>(), T> {
                if (numeric_limits<T>::max() < f) errno = ERANGE, f = numeric_limits<T>::max();
                return (T)f;
            }

        template <typename T>
            inline auto is_sign_invalid(char c) noexcept -> enable_if_t<std::is_signed<T>::value, bool> {
                return c == '+';
            }
        template <typename T>
            inline auto is_sign_invalid(char c) noexcept -> enable_if_t<std::is_unsigned<T>::value, bool> {
                return c == '+' || c == '-';
            }

        template <typename T, typename Cv>
            inline from_chars_result number_from_chars(const char* first, const char*, Cv convert, T& value, int base) {
                CXON_ASSERT(base == 2 || base == 8 || base == 10 || base == 16, "unsupported base");
                    if (is_sign_invalid<T>(*first)) return { first, errc::invalid_argument };
                char* end;
                errno = 0; auto const v = clamp<T>(convert(first, &end, base));
                return { end, end != first ? errno != ERANGE ? (value = v, errc{}) : errc::result_out_of_range : errc::invalid_argument };
            }
        template <typename T, typename Cv>
            inline from_chars_result number_from_chars(const char* first, const char*, Cv convert, T& value, chars_format) {
                    if (is_sign_invalid<T>(*first)) return { first, errc::invalid_argument };
                char* end;
                errno = 0; auto const v = convert(first, &end);
                return { end, end != first ? errno != ERANGE ? (value = v, errc{}) : errc::result_out_of_range : errc::invalid_argument };
            }

    }

#   define CXON_FROM_CHARS(T, C)\
        inline from_chars_result from_chars(const char* first, const char* last, T& value, int base = 10) {\
            return bits::number_from_chars(first, last, C, value, base);\
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
        inline from_chars_result from_chars(const char* first, const char* last, T& value, chars_format fmt = chars_format::general) {\
            return bits::number_from_chars(first, last, C, value, fmt);\
        }
        CXON_FROM_CHARS(float, strtof)
        CXON_FROM_CHARS(double, strtod)
        CXON_FROM_CHARS(long double, strtold)
#   undef CXON_FROM_CHARS

    struct to_chars_result {
        char* ptr;
        errc ec;
    };

    namespace bits {

        template <typename> struct fmt;
        template <> struct fmt<char>                // %c writes a single character but, the argument is first converted to unsigned char
                                                    { static constexpr char const*const str = "%hhu";   }; // no char[], clang complains ...
        template <> struct fmt<signed char>         { static constexpr char const*const str = "%hhi";   };
        template <> struct fmt<unsigned char>       { static constexpr char const*const str = "%hhu";   };
        template <> struct fmt<short>               { static constexpr char const*const str = "%hi";    };
        template <> struct fmt<unsigned short>      { static constexpr char const*const str = "%hu";    };
        template <> struct fmt<int>                 { static constexpr char const*const str = "%i";     };
        template <> struct fmt<unsigned int>        { static constexpr char const*const str = "%u";     };
        template <> struct fmt<long>                { static constexpr char const*const str = "%li";    };
        template <> struct fmt<unsigned long>       { static constexpr char const*const str = "%lu";    };
        template <> struct fmt<long long>           { static constexpr char const*const str = "%lli";   };
        template <> struct fmt<unsigned long long>  { static constexpr char const*const str = "%llu";   };
        template <> struct fmt<float>               { static constexpr char const*const str = "%.*g";   };
        template <> struct fmt<double>              { static constexpr char const*const str = "%.*g";   };
        template <> struct fmt<long double>         { static constexpr char const*const str = "%.*Lg";  };

        // to_chars: on error, returns a value of type to_chars_result holding errc::value_too_large in ec,
        //  a copy of the value last in ptr, and leaves the contents of the range [first, last) in unspecified state,
        //  but we don't care, the buffer must be sufficient
        template <typename T>
            inline to_chars_result number_to_chars(char* first, char* last, T value) {
                size_t const l = last - first;
                int const w = snprintf(first, l, fmt<T>::str, value); CXON_ASSERT(w > 0 && (size_t)w < l, "conversion failed");
                return { first + w, errc{} };
            }
        template <typename T>
            inline to_chars_result number_to_chars(char* first, char* last, T value, int precision) {
                size_t const l = last - first;
                int const w = snprintf(first, l, fmt<T>::str, precision, value); CXON_ASSERT(w > 0 && (size_t)w < l, "conversion failed");
                return { first + w, errc{} };
            }

    }

#   define CXON_TO_CHARS(T)\
        inline to_chars_result to_chars(char* first, char* last, T value) {\
            return bits::number_to_chars<T>(first, last, value);\
        }
        CXON_TO_CHARS(char)
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
        inline to_chars_result to_chars(char* first, char* last, T value, int precision) {\
            return bits::number_to_chars<T>(first, last, value, precision);\
        }
        CXON_TO_CHARS(float)
        CXON_TO_CHARS(double)
        CXON_TO_CHARS(long double)
#   undef CXON_TO_CHARS

}}}   // cxon::bits::charconv

namespace cxon { namespace bits { // <charconv>

    template <bool C, typename T = void>
        using enable_if_t = typename std::enable_if<C, T>::type;

#   ifdef CXON_HAS_CHARCONV
#       define HAS_FUNC_DEF(name, R, F)\
            template <typename, typename = void>\
                struct has_##name : std::false_type {};\
            template <typename T>\
                struct has_##name<T, enable_if_t<std::is_same<R, decltype(F)>::value>> : std::true_type {}

        HAS_FUNC_DEF(from_chars_i, std::from_chars_result, std::from_chars(std::declval<char*>(), std::declval<char*>(), std::declval<T&>(), 0));
        template <typename T>
            inline auto from_chars(const char* f, const char* l, T& t, int base) noexcept   -> enable_if_t< has_from_chars_i<T>::value, std::from_chars_result> {
                return std::from_chars(f, l, t, base);
            }
        template <typename T>
            inline auto from_chars(const char* f, const char* l, T& t, int base)            -> enable_if_t<!has_from_chars_i<T>::value, std::from_chars_result> {
                auto const r = charconv::from_chars(f, l, t, base);
                return { r.ptr, r.ec };
            }

        HAS_FUNC_DEF(from_chars_f, std::from_chars_result, std::from_chars(std::declval<char*>(), std::declval<char*>(), std::declval<T&>()));
        template <typename T>
            inline auto from_chars(const char* f, const char* l, T& t) noexcept -> enable_if_t< has_from_chars_f<T>::value, std::from_chars_result> {
                return std::from_chars(f, l, t);
            }
        template <typename T>
            inline auto from_chars(const char* f, const char* l, T& t)          -> enable_if_t<!has_from_chars_f<T>::value, std::from_chars_result> {
                auto const r = charconv::from_chars(f, l, t);
                return { r.ptr, r.ec };
            }

        HAS_FUNC_DEF(to_chars, std::to_chars_result, std::to_chars(std::declval<char*>(), std::declval<char*>(), std::declval<T>()));
        HAS_FUNC_DEF(to_chars_p, std::to_chars_result, std::to_chars(std::declval<char*>(), std::declval<char*>(), std::declval<T>(), (std::chars_format)0, 0));

        template <typename T>
            inline auto to_chars(char* f, char* l, T t) noexcept    -> enable_if_t< has_to_chars<T>::value, std::to_chars_result> {
                return std::to_chars(f, l, t);
            }
        template <typename T>
            inline auto to_chars(char* f, char* l, T t)             -> enable_if_t<!has_to_chars<T>::value, std::to_chars_result> {
                auto const r = charconv::to_chars(f, l, t);
                return { r.ptr, r.ec };
            }

        template <typename, typename = void>
            struct general {
                static constexpr int    value = 7;
            };
        template <typename T>
            struct general<T, enable_if_t<sizeof(T::general) != 0>> {
                static constexpr T      value = T::general;
            };

        template <typename T>
            inline auto to_chars(char* f, char* l, T t, int precision) noexcept
                -> enable_if_t< has_to_chars_p<T>::value, std::to_chars_result>
            {
#               if defined(_MSC_VER) && _MSC_VER <= 1922
                    auto const r = charconv::to_chars(f, l, t, precision);
                    return { r.ptr, r.ec };
#               else
                    return std::to_chars(f, l, t, general<std::chars_format>::value, precision);
#               endif
            }
        template <typename T>
            inline auto to_chars(char* f, char* l, T t, int) noexcept
                -> enable_if_t<!has_to_chars_p<T>::value &&  has_to_chars<T>::value, std::to_chars_result>
            {
                return std::to_chars(f, l, t);
            }
        template <typename T>
            inline auto to_chars(char* f, char* l, T t, int precision)
                -> enable_if_t<!has_to_chars_p<T>::value && !has_to_chars<T>::value, std::to_chars_result>
            {
                auto const r = charconv::to_chars(f, l, t, precision);
                return { r.ptr, r.ec };
            }

#       undef HAS_FUNC_DEF
#   else
        template <typename T>
            inline charconv::from_chars_result from_chars(const char* f, const char* l, T& t, int base) {
                return charconv::from_chars(f, l, t, base);
            }
        template <typename T>
            inline charconv::from_chars_result from_chars(const char* f, const char* l, T& t) {
                return charconv::from_chars(f, l, t);
            }

        template <typename T>
            inline charconv::to_chars_result to_chars(char* f, char* l, T t) {
                return charconv::to_chars(f, l, t);
            }
        template <typename T>
            inline charconv::to_chars_result to_chars(char* f, char* l, T t, int precision) {
                return charconv::to_chars(f, l, t, precision);
            }
#   endif

}}  //cxon::bits <charconv>

#endif // CXON_BITS_CHARCONV_HXX_
