// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_STRING_HXX_
#define CXON_CIO_STRING_HXX_

#include "cio.hxx"
#include "char.hxx"
#include "cxon/lang/common/allocator.hxx"
#include "cxon/lang/common/container.hxx"
#include <algorithm>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace str { // char arrays: read

    template <typename X, typename C, typename II, typename Cx>
        inline auto string_read(C& c, II& i, II e, Cx& cx)      -> enable_if_t<is_char<typename C::value_type>::value, bool>;

    template <typename X, typename T, typename II, typename Cx>
        inline auto array_read(T* f, T* l, II& i, II e, Cx& cx) -> enable_if_t<is_char<T>::value, bool>;

    template <typename X, typename T, typename II, typename Cx>
        inline auto pointer_read(T*& t, II& i, II e, Cx& cx)    -> enable_if_t<is_char<T>::value, bool>;

}}}

namespace cxon { namespace cio { namespace str { // char arrays: write

    template <typename X, typename O, typename T, typename Cx>
        inline auto array_write(O& o, const T* t, const T* te, Cx& cx)      -> enable_if_t<is_char<T>::value, bool>;

    template <typename X, typename O, typename T, typename Cx>
        inline auto pointer_write(O& o, const T* t, std::size_t s, Cx& cx)  -> enable_if_t<is_char<T>::value, bool>;
    template <typename X, typename O, typename T, typename Cx>
        inline auto pointer_write(O& o, const T* t, Cx& cx)                 -> enable_if_t<is_char<T>::value, bool>;

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace str {

    namespace imp {

        template <typename C, typename T = typename C::value_type>
            inline auto char_append_(C& c, char32_t c32) -> enable_if_t<chr::is_char_8<T>::value, bool> {
                T b[4]; int const n = chr::utf32_to_utf8(b, c32); CXON_ASSERT(n > 0 && n <=4, "unexpected");
                return cxon::cnt::append(c, &b[0], &b[0] + n);
            }
        template <typename C, typename T = typename C::value_type>
            inline auto char_append_(C& c, char32_t c32) -> enable_if_t<chr::is_char_16<T>::value, bool> {
                if (c32 > 0xFFFF) {
                    c32 -= 0x10000;
                    return cxon::cnt::append(c, T(0xD800 | (c32 >> 10))) && cxon::cnt::append(c, T(0xDC00 | (c32 & 0x3FF)));
                }
                return cxon::cnt::append(c, T(c32));
            }
        template <typename C, typename T = typename C::value_type>
            inline auto char_append_(C& c, char32_t c32) -> enable_if_t<chr::is_char_32<T>::value, bool> {
                return cxon::cnt::append(c, T(c32));
            }

        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
            inline bool char_read_(C& c, II& i, II e, Cx& cx) {
                II const o = i;
                    char32_t const c32 = chr::utf8_to_utf32<X>(i, e, cx);
                        if (c32 == chr::bad_utf32) return rewind(i, o), false;
                return char_append_(c, c32) || (rewind(i, o), cx/X::read_error::overflow);
            }

        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
            inline auto string_read_tail_(C& c, II& i, II e, Cx& cx)
                -> enable_if_t<!chr::is_char_8<T>::value || !is_forward_iterator<II>::value, bool>
            {
                constexpr bool HANDLE_ESCAPES = is_unquoted_key_context<X>::value || !X::assume_no_escapes;
                while (i != e) {
                    if (HANDLE_ESCAPES && *i == '\\') {
                        II const o = i; ++i;
                        CXON_IF_CONSTEXPR (is_unquoted_key_context<X>::value) {
                            if (i != e && *i == X::string::del)
                                return ++i, true;
                        }
                        char32_t c32 = chr::esc_to_utf32<X>(i, e, cx);
                            if (c32 == chr::bad_utf32) return rewind(i, o), false;
                        if (!char_append_(c, c32))
                            return rewind(i, o), cx/X::read_error::overflow;
                    }
                    else {
                        CXON_IF_CONSTEXPR (!is_unquoted_key_context<X>::value) {
                            if (delim_en_check<X>(*i))      return delim_en_read<X>(i, e);
                        }
                        CXON_IF_CONSTEXPR (X::validate_string_escapes) {
                            if (chr::is<X>::ctrl(*i))       return cx/X::read_error::unexpected;
                        }
                        if (!char_read_<X>(c, i, e, cx))    return false;
                    }
                }
                return cx/X::read_error::unexpected;
            }
        namespace imp { // str::raw_traits
            template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
                inline auto string_read_tail_(C& c, II& i, II e, Cx& cx)
                    -> enable_if_t<!has_traits<X, str::raw_traits>::value, bool>
                {
                    II l = i;
                        constexpr bool HANDLE_ESCAPES = is_unquoted_key_context<X>::value || !X::assume_no_escapes;
                        for ( ; i != e; ++i) {
                            if (HANDLE_ESCAPES && *i == '\\') {
                                if (l != i && !cxon::cnt::append(c, l, i))
                                    return rewind(i, l), cx/X::read_error::overflow;
                                CXON_IF_CONSTEXPR (is_unquoted_key_context<X>::value) {
                                    II const f = i;
                                        if (++i != e && *i == X::string::del)
                                            return ++i, true;
                                    i = f;
                                }
                                II const o = i;
                                    char32_t const c32 = chr::esc_to_utf32<X>(++i, e, cx);
                                        if (c32 == chr::bad_utf32) return rewind(i, o), false;
                                    if (!char_append_(c, c32))
                                        return rewind(i, o), cx/X::read_error::overflow;
                                l = i, --i;
                            }
                            else {
                                CXON_IF_CONSTEXPR (!is_unquoted_key_context<X>::value) {
                                    if (delim_en_check<X>(*i)) {
                                        if (l == i || cxon::cnt::append(c, l, i))
                                            return delim_en_read<X>(i, e);
                                        return rewind(i, l), cx/X::read_error::overflow;
                                    }
                                }
                                CXON_IF_CONSTEXPR (X::validate_string_encoding) {
                                    if ((unsigned char)*i > 0x7F) {
                                        auto bs = chr::utf8_check(i, e);
                                        if (bs == 0)
                                            return cx/X::read_error::character_invalid;
                                        while ((bs = chr::utf8_check(i += bs, e)))
                                            ;
                                        --i;
                                    }
                                    else CXON_IF_CONSTEXPR (X::validate_string_escapes) {
                                        if (chr::is<X>::ctrl(*i))
                                            return cx/X::read_error::unexpected;
                                    }
                                }
                                else CXON_IF_CONSTEXPR (X::validate_string_escapes) {
                                    if (chr::is<X>::ctrl(*i))
                                        return cx/X::read_error::unexpected;
                                }
                            }
                        }
                    return cx/X::read_error::unexpected;
                }
            template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
                inline auto string_read_tail_(C& c, II& i, II e, Cx& cx)
                    -> enable_if_t< has_traits<X, str::raw_traits>::value, bool>
                {
                    II l = i;
                        for ( ; i != e; ++i) {
                            if (delim_en_check<X>(*i)) {
                                if (l == i || cxon::cnt::append(c, l, i))
                                    return delim_en_read<X>(i, e);
                                return rewind(i, l), cx/X::read_error::overflow;
                            }
                        }
                    return cx/X::read_error::unexpected;
                }
        }
        template <typename X, typename C, typename II, typename Cx, typename T = typename C::value_type>
            inline auto string_read_tail_(C& c, II& i, II e, Cx& cx)
                -> enable_if_t< chr::is_char_8<T>::value &&  is_forward_iterator<II>::value, bool>
            {
                return imp::string_read_tail_<X>(c, i, e, cx);
            }

        template <typename X, typename C, typename II, typename Cx>
            inline bool string_read_head_(C&, II& i, II e, Cx& cx) {
                return consume<X>(delim_be_read<X, II>, i, e, cx);
            }

    }

    template <typename X, typename C, typename II, typename Cx>
        inline auto string_read(C& c, II& i, II e, Cx& cx)
             -> enable_if_t<is_char<typename C::value_type>::value, bool>
        {
            return  imp::string_read_head_<X>(c, i, e, cx) &&
                    imp::string_read_tail_<X>(c, i, e, cx)
            ;
        }

    template <typename X, typename T, typename II, typename Cx>
        inline auto array_read(T* f, T* l, II& i, II e, Cx& cx)
            -> enable_if_t<is_char<T>::value, bool>
        {
            auto c = cxon::cnt::make_range_container(f, l);
            return consume<X>(i, e, cx) && string_read<X>(c, i, e, cx) && (cxon::cnt::append(c, {}), true);
        }

    template <typename X, typename T, typename II, typename Cx>
        inline auto pointer_read(T*& t, II& i, II e, Cx& cx)
            -> enable_if_t<is_char<T>::value, bool>
        {
            if (!consume<X>(i, e, cx))
                return false;
            if (peek(i, e) == *X::id::nil && consume<X>(X::id::nil, i, e))
                return true;
            auto c = cxon::cnt::make_pointer_container<X, T>(cx);
            return string_read<X>(c, i, e, cx) && (c.push_back({}), t = c.release(), true);
        }

}}}

namespace cxon { namespace cio { namespace str {

    template <typename X, typename O, typename T, typename Cx>
        inline auto array_write(O& o, const T* f, const T* l, Cx& cx)
             -> enable_if_t<is_char<T>::value, bool>
        {
            if (!poke<X>(o, delim_be_write<X, O>, cx)) return false;
                if (*(l - 1) == T(0)) --l;
            return chr::encode_range<X>(o, f, l, cx) && poke<X>(o, delim_en_write<X, O>, cx);
        }

    namespace imp { // str::raw_traits
        template <typename X, typename O, typename T, typename Cx>
            inline auto pointer_write_(O& o, const T* t, std::size_t s, Cx& cx)
                 -> enable_if_t<!has_traits<X, str::raw_traits>::value, bool>
            {
                return  poke<X>(o, delim_be_write<X, O>, cx) &&
                            chr::encode_range<X>(o, t, t + s, cx) &&
                        poke<X>(o, delim_en_write<X, O>, cx)
                ;
            }
        template <typename X, typename O, typename T, typename Cx>
            inline auto pointer_write_(O& o, const T* t, std::size_t s, Cx& cx)
                 -> enable_if_t< has_traits<X, str::raw_traits>::value, bool>
            {
                return  poke<X>(o, delim_be_write<X, O>, cx) &&
                            poke<X>(o, t, s, cx) &&
                        poke<X>(o, delim_en_write<X, O>, cx)
                ;
            }
    }
    template <typename X, typename O, typename T, typename Cx>
        inline auto pointer_write(O& o, const T* t, std::size_t s, Cx& cx)
             -> enable_if_t<is_char<T>::value, bool>
        {
            return imp::pointer_write_<X>(o, t, s, cx);
        }
    template <typename X, typename O, typename T, typename Cx>
        inline auto pointer_write(O& o, const T* t, Cx& cx)
            -> enable_if_t<is_char<T>::value, bool>
        {
            return t ?
                pointer_write<X>(o, t, std::char_traits<T>::length(t), cx) :
                poke<X>(o, X::id::nil, cx)
            ;
        }

}}}

#endif // CXON_CIO_STRING_HXX_
