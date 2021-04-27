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

    template <typename X, typename T, typename II, typename Cx>
        inline bool array_read(T* t, T* te, II& i, II ie, Cx& cx);

    template <typename X, typename T, typename II, typename Cx>
        inline bool pointer_read(T*& t, II& i, II e, Cx& cx);

}}}

namespace cxon { namespace cio { namespace str { // char arrays: write

    template <typename X, typename O, typename T, typename Cx>
        inline bool array_write(O& o, const T* t, const T* te, Cx& cx);

    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, size_t s, Cx& cx);
    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace str {

    namespace imp {

        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type> // TODO: common with std::basic_string?
            inline auto char_read_(C& c, II& i, II e, Cx& cx)
                -> enable_if_t<chr::is_char_8<T>::value, bool>
            {
                II const o = i;
                    char32_t const c32 = chr::utf8_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
                    T b[4]; int const n = chr::utf32_to_utf8(b, c32);
                        if (n == 0) return cx/X::read_error::character_invalid;
                return container_append(c, &b[0], &b[0] + n) || cx/X::read_error::overflow;
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
                        return (container_append(c, T(0xD800 | (c32 >> 10))) && 
                                container_append(c, T(0xDC00 | (c32 & 0x3FF)))) ||
                                cx/X::read_error::overflow
                        ;
                    }
                return container_append(c, T(c32)) || cx/X::read_error::overflow;
            }
        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
            inline auto char_read_(C& c, II& i, II e, Cx& cx)
                -> enable_if_t<chr::is_char_32<T>::value, bool>
            {
                II const o = i;
                    char32_t const c32 = chr::utf8_to_utf32<X>(i, e, cx);
                        if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
                return container_append(c, T(c32)) || cx/X::read_error::overflow;
            }

        template <typename X, typename C, typename II, typename Cx>
            inline bool chars_read_(C& c, II& i, II e, Cx& cx) {
                while (i != e && *i != X::string::end) {
                    if (!chr::is<X>::real(*i))
                        return cx/X::read_error::unexpected;
                    if (!char_read_<X>(c, i, e, cx))
                        return false;
                }
                return true;
            }

        template <typename X, typename C, typename II, typename Cx>
            inline bool string_read_(C& c, II& i, II e, Cx& cx) {
                II const o = i;
                if (!consume<X>(X::string::beg, i, e, cx)) return false;
                    if (!chars_read_<X>(c, i, e, cx))
                        return cx.ec == X::read_error::overflow && (rewind(i, o), false);
                return consume<X>(X::string::end, i, e, cx);
            }

    }

    template <typename X, typename T, typename II, typename Cx>
        inline bool array_read(T* f, T* l, II& i, II e, Cx& cx) {
            consume<X>(i, e);
            auto c = make_range_container(f, l);
            return  imp::string_read_<X>(c, i, e, cx) &&
                    ((c.size() < c.max_size() && (c.push_back({}), true)) || true)
            ;
        }

    template <typename X, typename T, typename II, typename Cx>
        inline bool pointer_read(T*& t, II& i, II e, Cx& cx) {
            consume<X>(i, e);
            if (peek(i, e) == *X::id::nil && consume<X>(X::id::nil, i, e))
                return true;
            auto c = make_pointer_container<X, T>(cx);
            return imp::string_read_<X>(c, i, e, cx) && (c.push_back({}), t = c.release());
        }

}}}

namespace cxon { namespace cio { namespace str {

    template <typename T>
        inline size_t ptrlen(const T* t) noexcept {
            const T* e = t;
                while (*e) ++e;
            return e - t;
        }

    template <typename X, typename O, typename T, typename Cx>
        inline bool array_write(O& o, const T* t, const T* te, Cx& cx) {
            if (!poke<X>(o, X::string::beg, cx)) return false;
                if (*(te - 1) == T(0)) --te;
            return chr::encode_range<X>(o, t, te, cx) && poke<X>(o, X::string::end, cx);
        }

    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, size_t s, Cx& cx) {
            return  poke<X>(o, X::string::beg, cx) &&
                        chr::encode_range<X>(o, t, t + s, cx) &&
                    poke<X>(o, X::string::end, cx)
            ;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, Cx& cx) {
            return t ?
                pointer_write<X>(o, t, ptrlen(t), cx) :
                poke<X>(o, X::id::nil, cx)
            ;
        }

}}}

#endif // CXON_CIO_STRING_HXX_
