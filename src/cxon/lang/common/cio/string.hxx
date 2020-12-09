// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_STRING_HXX_
#define CXON_CIO_STRING_HXX_

#include "cio.hxx"
#include "key.hxx"
#include "char.hxx"
#include <algorithm>

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace str { // string quoting: read

    template <typename X>
        struct is_str; // end, esc
    template <typename X, template <typename> class S>
        struct is_str<S<UQKEY<X>>>;

    template <typename X>
        struct consume_str; // beg, end, chr
    template <typename X, template <typename> class S>
        struct consume_str<S<UQKEY<X>>>;

}}}

namespace cxon { namespace cio { namespace str { // string quoting: write

    template <typename X>
        struct str; // beg, end
    template <typename X, template <typename> class S>
        struct str<S<UQKEY<X>>>;

}}}

namespace cxon { namespace cio { namespace str { // char arrays: read

    template <typename X, typename T, typename II, typename Cx>
        inline bool array_read(T* t, const T* te, II& i, II ie, Cx& cx);

    template <typename X>
        struct array; // read
    template <typename X, template <typename> class S>
        struct array<S<UQKEY<X>>>;

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
    template <typename X, typename O, typename T, typename Cx>
        inline bool uqkey_pointer_write(O& o, const T* t, size_t s, Cx& cx);
    template <typename X, typename O, typename T, typename Cx>
        inline bool uqkey_pointer_write(O& o, const T* t, Cx& cx);

    template <typename X>
        struct pointer; // write
    template <typename X, template <typename> class S>
        struct pointer<S<UQKEY<X>>>;

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace str {

    template <typename X>
        struct is_str {
            static           bool end(char c) noexcept  { return c == X::string::end; }
            static constexpr bool esc(char)             { return false; }
        };
    template <typename X, template <typename> class S>
        struct is_str<S<UQKEY<X>>> {
            static bool end(char c) noexcept { return chr::is<S<X>>::space(c) || c == S<X>::map::div; }
            static bool esc(char c) noexcept { return end(c); }
        };

    template <typename X>
        struct consume_str {
            template <typename II, typename Cx> static bool     beg(II& i, II e, Cx& cx) { return consume<X>(X::string::beg, i, e, cx); }
            template <typename II, typename Cx> static bool     end(II& i, II e, Cx& cx) { return consume<X>(X::string::end, i, e, cx); }
            template <typename II, typename Cx> static char32_t chr(II& i, II e, Cx& cx) {
                return  chr::str_to_utf32<X>(i, e, cx);
            }
        };
    template <typename X, template <typename> class S>
        struct consume_str<S<UQKEY<X>>> {
            template <typename II, typename Cx> static constexpr bool   beg(II&, II, Cx&)          { return true; }
            template <typename II, typename Cx> static constexpr bool   end(II&, II, Cx&)          { return true; }
            template <typename II, typename Cx> static char32_t         chr(II& i, II e, Cx& cx)   {
                if (peek(i, e) == '\\') {
                    char const c = next(i, e);
                    return !is_str<S<UQKEY<X>>>::esc(c) ?
                        chr::esc_to_utf32<S<X>>(i, e, cx) :
                        (++i, char32_t(c))
                    ;
                }
                return chr::str_to_utf32<S<X>>(i, e, cx);
            }
        };

}}}

namespace cxon { namespace cio { namespace str {

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

}}}

namespace cxon { namespace cio { namespace str {

    template <typename X, typename T, typename II, typename Cx> // TODO: common with std::basic_string?
        inline auto array_char_read(T*& t, const T* te, II& i, II ie, Cx& cx)
            -> enable_if_t<chr::is_char_8<T>::value, bool>
        {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
                T b[4]; int const n = chr::utf32_to_utf8(b, c32);
                    if (n == 0 || t + n > te) return cx|read_error::overflow;
                std::copy_n(b, n, t);
            return t += n, true;
        }
    template <typename X, typename T, typename II, typename Cx>
        inline auto array_char_read(T*& t, const T* te, II& i, II ie, Cx& cx)
            -> enable_if_t<chr::is_char_16<T>::value, bool>
        {
            II const o = i;
                char32_t c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
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
            -> enable_if_t<chr::is_char_32<T>::value, bool>
        {
            II const o = i;
                char32_t const c32 = consume_str<X>::chr(i, ie, cx);
                    if (c32 == 0xFFFFFFFF) return rewind(i, o), false;
            return *t = T(c32), ++t, true;
        }

    template <typename X, typename T, typename II, typename Cx>
        inline bool array_read(T* t, const T* te, II& i, II ie, Cx& cx) {
            II const o = i;
                if (!consume_str<X>::beg(i, ie, cx)) return false;
                    while (t < te) {
                        if (!chr::is<X>::real(peek(i, ie)))         return consume_str<X>::end(i, ie, cx);
                        if (is_str<X>::end(peek(i, ie)))            return *t = '\0', consume_str<X>::end(i, ie, cx);
                        if (!array_char_read<X>(t, te, i, ie, cx))  return cx.ec == read_error::overflow && (rewind(i, o), false);
                    }
                return (is_str<X>::end(peek(i, ie)) || (rewind(i, o), cx|read_error::overflow)) &&
                        consume_str<X>::end(i, ie, cx)
                ;
        }

    template <typename X>
        struct array {
            template <typename T, typename II, typename Cx>
                static bool read(T* t, const T* te, II& i, II ie, Cx& cx) {
                    consume<X>(i, ie);
                    return array_read<X>(t, te, i, ie, cx);
                }
        };
    template <typename X, template <typename> class S>
        struct array<S<UQKEY<X>>> {
            template <typename T, typename II, typename Cx>
                static bool read(T* t, const T* te, II& i, II ie, Cx& cx) {
                    consume<S<X>>(i, ie);
                    return peek(i, ie) == S<X>::string::beg ?
                        array_read<S<X>>(t, te, i, ie, cx) :
                        array_read<S<UQKEY<X>>>(t, te, i, ie, cx)
                    ;
                }
        };

    template <typename X, typename T, typename II, typename Cx>
        inline bool pointer_read(T*& t, II& i, II e, Cx& cx) {
            consume<X>(i, e);
            if (peek(i, e) == *X::id::nil && consume<X>(X::id::nil, i, e)) return true;
                if (!consume_str<X>::beg(i, e, cx)) return false;
            auto ax = allocator::value(cx.px, std::allocator<T>());
            typename std::allocator_traits<decltype(ax)>::template rebind_alloc<T> at;
                using al = std::allocator_traits<decltype(at)>;
            T *b = al::allocate(at, 4), *p = b, *be = b + 4;
            for ( ; ; ) {
                if (p + 4 > be) {
                    T *const n = al::allocate(at, 2 * (be - b));
                        std::copy_n(b, p - b, n);
                    p = n + (p - b);
                    al::deallocate(at, b, be - b);
                    be = n + 2 * (be - b), b = n;
                }
                if (!chr::is<X>::real(peek(i, e)))          goto err;
                if (is_str<X>::end(peek(i, e)))             return *p = '\0', t = b, consume_str<X>::end(i, e, cx);
                if (!array_char_read<X>(p, be, i, e, cx))   goto err;
            }
            err: return al::deallocate(at, b, be - b), cx|read_error::unexpected;
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
            return chr::encode<X, T>::range(o, t, te, cx) && poke<X>(o, X::string::end, cx);
        }

    template <typename X, typename O, typename T, typename Cx>
        inline bool pointer_write(O& o, const T* t, size_t s, Cx& cx) {
            return  poke<X>(o, X::string::beg, cx) &&
                        chr::encode<X, T>::range(o, t, t + s, cx) &&
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
    template <typename X, typename O, typename T, typename Cx>
        inline bool uqkey_pointer_write(O& o, const T* t, size_t s, Cx& cx) {
            for (auto e = t + s; t != e; ++t) {
                switch (*t) {
                    case ' ':           if (!poke<X>(o, '\\', cx) || !poke<X>(o, ' ', cx))          return false;   break;
                    case '"':           if (!poke<X>(o, '"', cx))                                   return false;   break;
                    case '\'':          if (!poke<X>(o, '\'', cx))                                  return false;   break;
                    case X::map::div:   if (!poke<X>(o, '\\', cx) || !poke<X>(o, X::map::div, cx))  return false;   break;
                    default:            if (!chr::encode<X, T>::value(o, t, e, cx))               return false;
                }
            }
            return true;
        }
    template <typename X, typename O, typename T, typename Cx>
        inline bool uqkey_pointer_write(O& o, const T* t, Cx& cx) {
            return t ?
                uqkey_pointer_write<X>(o, t, ptrlen(t), cx) :
                poke<X>(o, X::id::nil, cx)
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
        struct pointer<S<UQKEY<X>>> {
            template <typename O, typename T, typename Cx>
                static bool write(O& o, const T* t, Cx& cx) {
                    return uqkey_pointer_write<S<X>>(o, t, cx);
                }
        };

}}}

#endif // CXON_CIO_STRING_HXX_
