// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_BITS_STRINGS_HXX_
#define CXON_BITS_STRINGS_HXX_

#include "chio.hxx"
#include "key.hxx"
#include "chars.hxx"
#include <algorithm>

namespace cxon { namespace strs { // string quoting: read

    using namespace cxon::chcls;
    using namespace chio::bits; // TODO

    template <typename X>
        struct is_str {
            static           bool end(char c) noexcept  { return c == X::string::end; }
            static constexpr bool esc(char)             { return false; }
        };
    template <typename X, template <typename> class S>
        struct is_str<S<UQKEY<X>>> {
            static bool end(char c) noexcept { return is<S<X>>::space(c) || c == S<X>::map::div; }
            static bool esc(char c) noexcept { return end(c); }
        };

    template <typename X>
        struct consume_str {
            template <typename II, typename Cx> static bool      beg(II& i, II e, Cx& cx) { return chio::consume<X>(X::string::beg, i, e, cx); }
            template <typename II, typename Cx> static bool      end(II& i, II e, Cx& cx) { return chio::consume<X>(X::string::end, i, e, cx); }
            template <typename II, typename Cx> static char32_t  chr(II& i, II e, Cx& cx) {
                return  chars::str_to_utf32<X>(i, e, cx);
            }
        };
    template <typename X, template <typename> class S>
        struct consume_str<S<UQKEY<X>>> {
            template <typename II, typename Cx> static constexpr bool    beg(II&, II, Cx&)          { return true; }
            template <typename II, typename Cx> static constexpr bool    end(II&, II, Cx&)          { return true; }
            template <typename II, typename Cx> static char32_t          chr(II& i, II e, Cx& cx)   {
                if (chio::peek(i, e) == '\\') {
                    char const c = chio::next(i, e);
                    return !is_str<S<UQKEY<X>>>::esc(c) ?
                        chars::esc_to_utf32<S<X>>(i, e, cx) :
                        (++i, char32_t(c))
                    ;
                }
                return chars::str_to_utf32<S<X>>(i, e, cx);
            }
        };

}}  // cxon::strs string quoting: read

namespace cxon { namespace strs { // string quoting: write

    template <typename X>
        struct str {
            static constexpr char                   beg = X::string::beg;
            static constexpr char                   end = X::string::end;
        };
    template <typename X, template <typename> class S>
        struct str<S<UQKEY<X>>> {
            static constexpr std::nullptr_t         beg = nullptr;
            static constexpr std::nullptr_t         end = nullptr;
        };

}}  // cxon::strs string quoting: write

namespace cxon { namespace strs { // char arrays: read

    template <typename X, typename II, typename Cx> // TODO: common with std::basic_string?
        inline bool array_char_read(char*& t, const char* te, II& i, II ie, Cx& cx) {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return chio::rewind(i, o), false;
                char b[4]; int const n = chars::utf32_to_utf8(b, c32);
                    if (n == 0 || t + n > te) return cx|read_error::overflow;
                std::copy_n(b, n, t);
            return t += n, true;
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto array_char_read(T*& t, const T* te, II& i, II ie, Cx& cx)
            -> enable_if_t<std::is_same<T, char16_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char16_t)), bool>
        {
            II const o = i;
                char32_t c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return chio::rewind(i, o), false;
                if (c32 > 0xFFFF) {
                    c32 -= 0x10000;
                    *t = T(0xD800 | (c32 >> 10));   if (++t == te) return cx|read_error::overflow;
                    *t = T(0xDC00 | (c32 & 0x3FF));
                }
                else {
                    *t = T(c32);
                }
            return ++t, true;
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto array_char_read(T*& t, const T*, II& i, II ie, Cx& cx)
            -> enable_if_t<std::is_same<T, char32_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char32_t)), bool>
        {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return chio::rewind(i, o), false;
            return *t = T(c32), ++t, true;
        }

    template <typename X, typename T, typename II, typename Cx>
        inline bool array_read(T* t, const T* te, II& i, II ie, Cx& cx) {
            II const o = i;
                if (!consume_str<X>::beg(i, ie, cx)) return false;
                    while (t < te) {
                        if (!is<X>::real(chio::peek(i, ie)))        return consume_str<X>::end(i, ie, cx);
                        if (is_str<X>::end(chio::peek(i, ie)))      return *t = '\0', consume_str<X>::end(i, ie, cx);
                        if (!array_char_read<X>(t, te, i, ie, cx))  return cx.ec == read_error::overflow && (chio::rewind(i, o), false);
                    }
                return (is_str<X>::end(chio::peek(i, ie)) || (chio::rewind(i, o), cx|read_error::overflow)) &&
                        consume_str<X>::end(i, ie, cx)
                ;
        }

    template <typename X>
        struct array {
            template <typename T, typename II, typename Cx>
                static bool read(T* t, const T* te, II& i, II ie, Cx& cx) {
                    chio::consume<X>(i, ie);
                    return array_read<X>(t, te, i, ie, cx);
                }
        };
    template <typename X, template <typename> class S>
        struct array<S<UQKEY<X>>> {
            template <typename T, typename II, typename Cx>
                static bool read(T* t, const T* te, II& i, II ie, Cx& cx) {
                    chio::consume<S<X>>(i, ie);
                    return chio::peek(i, ie) == S<X>::string::beg ?
                        array_read<S<X>>(t, te, i, ie, cx) :
                        array_read<S<UQKEY<X>>>(t, te, i, ie, cx)
                    ;
                }
        };

    template <typename X, typename T, typename II, typename Cx>
        inline bool pointer_read(T*& t, II& i, II e, Cx& cx) {
            chio::consume<X>(i, e);
            if (chio::peek(i, e) == *X::id::nil && chio::consume<X>(X::id::nil, i, e)) return true;
                if (!consume_str<X>::beg(i, e, cx)) return false;
            auto ax = allocator::value(cx.ps, std::allocator<T>());
            typename std::allocator_traits<decltype(ax)>::template rebind_alloc<T> at;
                using al = std::allocator_traits<decltype(at)>;
            T *b = al::allocate(at, 4), *p = b; T* be = b + 4;
            for ( ; ; ) {
                if (p + 4 > be) {
                    T *const n = al::allocate(at, 2 * (be - b));
                        std::copy_n(b, p - b, n);
                    p = n + (p - b);
                    al::deallocate(at, b, be - b);
                    be = n + 2 * (be - b), b = n;
                }
                if (!is<X>::real(chio::peek(i, e)))           goto err;
                if (is_str<X>::end(chio::peek(i, e)))         return *p = '\0', t = b, consume_str<X>::end(i, e, cx);
                if (!array_char_read<X>(p, be, i, e, cx))   goto err;
            }
            err: return al::deallocate(at, b, be - b), cx|read_error::unexpected;
        }

}}  // cxon::strs char arrays: read

namespace cxon { namespace strs { // char arrays: write

    template <typename T>
        inline size_t ptrlen(const T* t) noexcept {
            const T* e = t;
                while (*e) ++e;
            return e - t;
        }

    template <typename X, typename O, typename T, typename Cx>
        inline bool array_write(O& o, const T* t, const T* te, Cx& cx) {
            if (!chio::poke<X>(o, X::string::beg, cx)) return false;
                if (*(te - 1) == T(0)) --te;
            return chars::encode<X, T>::range(o, t, te, cx) && chio::poke<X>(o, X::string::end, cx);
        }

    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, size_t s, Cx& cx) {
            return  chio::poke<X>(o, X::string::beg, cx) &&
                        chars::encode<X, T>::range(o, t, t + s, cx) &&
                    chio::poke<X>(o, X::string::end, cx)
            ;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, Cx& cx) {
            return t ?
                pointer_write<X>(o, t, ptrlen(t), cx) :
                chio::poke<X>(o, X::id::nil, cx)
            ;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool uqkey_pointer_write(O& o, const T* t, size_t s, Cx& cx) {
            for (auto e = t + s; t != e; ++t) {
                switch (*t) {
                    case ' ':           if (!chio::poke<X>(o, '\\', cx) || !chio::poke<X>(o, ' ', cx))          return false;   break;
                    case '"':           if (!chio::poke<X>(o, '"', cx))                                         return false;   break;
                    case '\'':          if (!chio::poke<X>(o, '\'', cx))                                        return false;   break;
                    case X::map::div:   if (!chio::poke<X>(o, '\\', cx) || !chio::poke<X>(o, X::map::div, cx))  return false;   break;
                    default:            if (!chars::encode<X, T>::value(o, t, e, cx))                           return false;
                }
            }
            return true;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool uqkey_pointer_write(O& o, const T* t, Cx& cx) {
            return t ?
                uqkey_pointer_write<X>(o, t, ptrlen(t), cx) :
                chio::poke<X>(o, X::id::nil, cx)
            ;
        }

    template <typename X>
        struct pointer {
            template <typename O, typename T, typename Cx>
                static bool write(O& o, const T* t, Cx& cx) {
                    return pointer_write<X>(o, t, cx);
                }
        };
    template <typename X, template <typename> class S>
        struct pointer<S<strs::UQKEY<X>>> {
            template <typename O, typename T, typename Cx>
                static bool write(O& o, const T* t, Cx& cx) {
                    return uqkey_pointer_write<S<X>>(o, t, cx);
                }
        };

}}  // cxon::strs char arrays: write

namespace cxon { namespace strs { // std::basic_string read

    template <typename X, typename ...R, typename II, typename Cx> // TODO: common with arrays?
        inline bool basic_string_char_read(std::basic_string<char, R...>& t, II& i, II e, Cx& cx) {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return chio::rewind(i, o), false;
                char b[4]; t.append(b, chars::utf32_to_utf8(b, c32));
            return true;
        }
    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline auto basic_string_char_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<T, char16_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char16_t)), bool>
        {
            II const o = i;
                char32_t c32 = consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return chio::rewind(i, o), false;
                if (c32 > 0xFFFF) {
                    c32 -= 0x10000;
                    t.push_back(char16_t(0xD800 | (c32 >> 10)));
                    t.push_back(char16_t(0xDC00 | (c32 & 0x3FF)));
                }
                else {
                    t.push_back(char16_t(c32));
                }
            return true;
        }
    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline auto basic_string_char_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx)
            -> enable_if_t<std::is_same<T, char32_t>::value || (std::is_same<T, wchar_t>::value && sizeof(wchar_t) == sizeof(char32_t)), bool>
        {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, e, cx);
                    if (c32 == 0xFFFFFFFF) return chio::rewind(i, o), false;
            return t.push_back(T(c32)), true;
        }

    template <typename X, typename T, typename ...R, typename II, typename Cx>
        inline bool basic_string_read(std::basic_string<T, R...>& t, II& i, II e, Cx& cx) {
            if (!consume_str<X>::beg(i, e, cx)) return false;
                for (char c = chio::peek(i, e); is<X>::real(c); c = chio::peek(i, e)) {
                    if (is_str<X>::end(c))                          return consume_str<X>::end(i, e, cx);
                    if (!basic_string_char_read<X>(t, i, e, cx))    return false;
                }
            return cx|read_error::unexpected;
        }

}}  // cxon::strs std::basic_string read

#endif // CXON_BITS_STRINGS_HXX_
