// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_FUNDAMENTAL_HXX_
#define CXON_JSON_FUNDAMENTAL_HXX_

#include "cxon/lang/common/chio/char.hxx"
#include "cxon/lang/common/chio/numeric.hxx"

namespace cxon { // nullptr_t

    template <typename X, typename II, typename Cx>
        inline auto read_value(std::nullptr_t& t, II& i, II e, Cx& cx) -> enable_for_t<X, JSON> {
            II const o = i;
            return (chio::consume<X>(X::id::nil, i, e) || (chio::rewind(i, o), cx|chio::read_error::unexpected)) &&
                    (t = nullptr, true)
            ;
        }

    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, std::nullptr_t, Cx& cx) -> enable_for_t<X, JSON> {
            return chio::poke<X>(o, X::id::nil, cx);
        }

}

namespace cxon { // bool

    template <typename X, typename II, typename Cx>
        inline auto read_value(bool& t, II& i, II e, Cx& cx) -> enable_for_t<X, JSON> {
            static_assert(*X::id::pos != *X::id::neg, "boolean literals ambiguous"); // for input-iterator, id must be consumed
            II const o = i;
                char const c = (chio::consume<X>(i, e), chio::peek(i, e));
                     if (c == *X::id::pos && chio::consume<X>(X::id::pos, i, e))  return t = true,  true;
                else if (c == *X::id::neg && chio::consume<X>(X::id::neg, i, e))  return t = false, true;
            return chio::rewind(i, o), cx|chio::read_error::boolean_invalid;
        }

    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, bool t, Cx& cx) -> enable_for_t<X, JSON> {
            return chio::poke<X>(o, t ? X::id::pos : X::id::neg, cx);
        }

}

namespace cxon { // character

    template <typename X, typename II, typename Cx>
        inline auto read_value(char& t, II& i, II e, Cx& cx) -> enable_for_t<X, JSON> {
            if (!chio::consume<X>(X::string::beg, i, e, cx)) return false;
                II const o = i;
                    char32_t const c32 = chio::chr::str_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF)  return chio::rewind(i, o), false;
                        if (c32 > 0XFF)         return chio::rewind(i, o), cx|chio::read_error::character_invalid;
            return chio::consume<X>(X::string::end, i, e, cx) && (t = char(c32), true);
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<is_same_format<X, JSON>::value && chio::chr::is_char16_t<T>::value, bool>
        {
            if (!chio::consume<X>(X::string::beg, i, e, cx)) return false;
                II const o = i;
                    char32_t const c32 = chio::chr::str_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF)  return chio::rewind(i, o), false;
                        if (c32 > 0XFFFF)       return chio::rewind(i, o), cx|chio::read_error::character_invalid;
            return chio::consume<X>(X::string::end, i, e, cx) && (t = T(c32), true);
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto read_value(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<is_same_format<X, JSON>::value && chio::chr::is_char32_t<T>::value, bool>
        {
            if (!chio::consume<X>(X::string::beg, i, e, cx)) return false;
                II const o = i;
                    char32_t const c32 = chio::chr::str_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF) return chio::rewind(i, o), false;
            return chio::consume<X>(X::string::end, i, e, cx) && (t = T(c32), true);
        }

    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, char t, Cx& cx) -> enable_for_t<X, JSON>      { return chio::chr::write<X>(o, t, cx); }
    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, char16_t t, Cx& cx) -> enable_for_t<X, JSON>  { return chio::chr::write<X>(o, t, cx); }
    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, char32_t t, Cx& cx) -> enable_for_t<X, JSON>  { return chio::chr::write<X>(o, t, cx); }
    template <typename X, typename O, typename Cx>
        inline auto write_value(O& o, wchar_t t, Cx& cx) -> enable_for_t<X, JSON>   { return chio::chr::write<X>(o, t, cx); }

}

namespace cxon { // numeric

#   define CXON_READ_DEF(T)\
        template <typename X, typename II, typename Cx>\
            inline auto read_value(T& t, II& i, II e, Cx& cx) -> enable_for_t<X, JSON> {\
                return chio::num::number_read<X>(t, i, e, cx);\
            }
        CXON_READ_DEF(signed char)
        CXON_READ_DEF(unsigned char)
        CXON_READ_DEF(short)
        CXON_READ_DEF(unsigned short)
        CXON_READ_DEF(int)
        CXON_READ_DEF(unsigned int)
        CXON_READ_DEF(long)
        CXON_READ_DEF(unsigned long)
        CXON_READ_DEF(long long)
        CXON_READ_DEF(unsigned long long)
        CXON_READ_DEF(float)
        CXON_READ_DEF(double)
        CXON_READ_DEF(long double)
#   undef CXON_READ_DEF

#   define CXON_WRITE_DEF(T)\
        template <typename X, typename O, typename Cx>\
            inline auto write_value(O& o, const T& t, Cx& cx) -> enable_for_t<X, JSON> {\
                return chio::num::number_write<X>(o, t, cx);\
            }
        CXON_WRITE_DEF(signed char)
        CXON_WRITE_DEF(unsigned char)
        CXON_WRITE_DEF(short)
        CXON_WRITE_DEF(unsigned short)
        CXON_WRITE_DEF(int)
        CXON_WRITE_DEF(unsigned int)
        CXON_WRITE_DEF(long)
        CXON_WRITE_DEF(unsigned long)
        CXON_WRITE_DEF(long long)
        CXON_WRITE_DEF(unsigned long long)
        CXON_WRITE_DEF(float)
        CXON_WRITE_DEF(double)
        CXON_WRITE_DEF(long double)
#   undef CXON_WRITE_DEF

}

#endif // CXON_JSON_FUNDAMENTAL_HXX_
