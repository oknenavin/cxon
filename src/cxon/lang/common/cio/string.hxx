// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_STRING_HXX_
#define CXON_CIO_STRING_HXX_

#include "cio.hxx"
#include "key.hxx"
#include "char.hxx"
#include "cxon/lang/common/allocator.hxx"
#include "cxon/lang/common/container.hxx"
#include <algorithm>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace str { // char arrays: read

    template <typename X, typename C, typename II, typename Cx>
        inline bool string_read(C& c, II& i, II e, Cx& cx);

    template <typename X, typename T, typename II, typename Cx>
        inline bool array_read(T* f, T* l, II& i, II e, Cx& cx);

    template <typename X, typename T, typename II, typename Cx>
        inline bool pointer_read(T*& t, II& i, II e, Cx& cx);

}}}

namespace cxon { namespace cio { namespace str { // char arrays: write

    template <typename X, typename O, typename T, typename Cx>
        inline bool array_write(O& o, const T* t, const T* te, Cx& cx);

    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, std::size_t s, Cx& cx);
    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace str {

    namespace imp {

        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
            inline auto char_read_(C& c, II& i, II e, Cx& cx)
                -> enable_if_t<chr::is_char_8<T>::value, bool>
            {
                II const o = i;
                    char32_t const c32 = chr::utf8_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
                    T b[4]; int const n = chr::utf32_to_utf8(b, c32);
                        if (n == 0) return cx/X::read_error::character_invalid;
                return cnt::append(c, &b[0], &b[0] + n) || cx/X::read_error::overflow;
            }
        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
            inline auto char_read_(C& c, II& i, II e, Cx& cx)
                -> enable_if_t<chr::is_char_16<T>::value, bool>
            {
                II const o = i;
                    char32_t c32 = chr::utf8_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
                    if (c32 > 0xFFFF) {
                        c32 -= 0x10000;
                        return (cnt::append(c, T(0xD800 | (c32 >> 10))) && 
                                cnt::append(c, T(0xDC00 | (c32 & 0x3FF)))) ||
                                cx/X::read_error::overflow
                        ;
                    }
                return cnt::append(c, T(c32)) || cx/X::read_error::overflow;
            }
        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
            inline auto char_read_(C& c, II& i, II e, Cx& cx)
                -> enable_if_t<chr::is_char_32<T>::value, bool>
            {
                II const o = i;
                    char32_t const c32 = chr::utf8_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
                return cnt::append(c, T(c32)) || cx/X::read_error::overflow;
            }

        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
            inline auto chars_read_(C& c, II& i, II e, Cx& cx)
                -> enable_if_t<!chr::is_char_8<T>::value || !is_forward_iterator<II>::value, bool>
            {
                while (i != e && *i != X::string::end) {
                    if ( chr::is<X>::ctrl(*i))          return cx/X::read_error::unexpected;
                    if (!char_read_<X>(c, i, e, cx))    return false;
                }
                return true;
            }
        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
            inline auto chars_read_(C& c, II& i, II e, Cx& cx)
                -> enable_if_t< chr::is_char_8<T>::value &&  is_forward_iterator<II>::value, bool>
            {
                II const o = i; II l = i;
                for ( ; i != e && *i != X::string::end; ++i) {
                    if (*i == '\\') {
                        if (l != i) if (!cnt::append(c, l, i))
                            return rewind(i, o), cx/X::read_error::overflow;
                        char32_t const c32 = chr::esc_to_utf32<X>(++i, e, cx);
                            if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
                        T bf[4]; auto const n = chr::utf32_to_utf8(bf, c32);
                        if (!cnt::append(c, &bf[0], &bf[0] + n))
                            return rewind(i, o), cx/X::read_error::overflow;
                        l = i, --i;
                    }
                    else {
                        CXON_IF_CONSTEXPR (X::read_validate_string_utf8) {
                            if ((unsigned char)*i > 0x7F) {
                                auto const bs = chr::utf8_check(i, e);
                                if (bs > 3)
                                    return rewind(i, o), cx/X::read_error::character_invalid;
                                i += bs;
                            }
                            else CXON_IF_CONSTEXPR (X::read_validate_string_ctrl) {
                                if (chr::is<X>::ctrl(*i))
                                    return rewind(i, o), cx/X::read_error::unexpected;
                            }
                        }
                        else CXON_IF_CONSTEXPR (X::read_validate_string_ctrl) {
                            if ((unsigned char)*i <= 0x7F && chr::is<X>::ctrl(*i))
                                return rewind(i, o), cx/X::read_error::unexpected;
                        }
                    }
                }
                return l == i || cnt::append(c ,l , i) || (rewind(i, o), cx/X::read_error::overflow);
            }

    }

    template <typename X, typename C, typename II, typename Cx>
        inline bool string_read(C& c, II& i, II e, Cx& cx) {
            II const o = i;
            if (!consume<X>(X::string::beg, i, e, cx)) return false;
                if (!imp::chars_read_<X>(c, i, e, cx))
                    return cx.ec == X::read_error::overflow && (rewind(i, o), false);
            return consume<X>(X::string::end, i, e, cx);
        }

    template <typename X, typename T, typename II, typename Cx>
        inline bool array_read(T* f, T* l, II& i, II e, Cx& cx) {
            consume<X>(i, e);
            auto c = cnt::make_range_container(f, l);
            return string_read<X>(c, i, e, cx) && (c.append({}), true);
        }

    template <typename X, typename T, typename II, typename Cx>
        inline bool pointer_read(T*& t, II& i, II e, Cx& cx) {
            consume<X>(i, e);
            if (peek(i, e) == *X::id::nil && consume<X>(X::id::nil, i, e))
                return true;
            auto c = cnt::make_pointer_container<X, T>(cx);
            return string_read<X>(c, i, e, cx) && (c.push_back({}), t = c.release());
        }

}}}

namespace cxon { namespace cio { namespace str {

    template <typename X, typename O, typename T, typename Cx>
        inline bool array_write(O& o, const T* f, const T* l, Cx& cx) {
            if (!poke<X>(o, X::string::beg, cx)) return false;
                if (*(l - 1) == T(0)) --l;
            return chr::encode_range<X>(o, f, l, cx) && poke<X>(o, X::string::end, cx);
        }

    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, std::size_t s, Cx& cx) {
            return  poke<X>(o, X::string::beg, cx) &&
                        chr::encode_range<X>(o, t, t + s, cx) &&
                    poke<X>(o, X::string::end, cx)
            ;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, Cx& cx) {
            return t ?
                pointer_write<X>(o, t, std::char_traits<T>::length(t), cx) :
                poke<X>(o, X::id::nil, cx)
            ;
        }

}}}

#endif // CXON_CIO_STRING_HXX_
