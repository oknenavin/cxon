// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHIO_VALUE_HXX_
#define CXON_CHIO_VALUE_HXX_

#include "chio.hxx"

namespace cxon { namespace chio { namespace val { // value parsing

    // TODO: add output iterator variant

    template <typename X, typename T, size_t N, typename II, typename Cx>
        inline bool read(T (&t)[N], II& i, II e, Cx& cx);

    template <typename X, typename II, typename Cx>
        inline bool read(II& i, II e, Cx& cx);

}}}

// implementation /////////////////////////////////////////////////////////////

namespace cxon { namespace chio { namespace val {

    namespace bits {

        template <typename X>
            struct value {
                template <typename BA, typename II>
                    static bool skip(BA& o, II& i, II e) {
                        if (!o.add(*i)) return false;
                        for (++i; i != e; ++i) {
                            switch (*i)  {
                                case '\\':              if (!o.add(*i)) return false;
                                                        if (++i == e)   return false;
                                                        break;
                                case X::string::end:    return o.add(*i);
                            }
                        }
                        return false;
                    }
                template <char CB, char CE, typename BA, typename II>
                    static bool skip(BA& o, II& i, II e) {
                        if (!o.add(*i)) return false;
                        unsigned r = 0;
                        for (++i; i != e; ++i) {
                            switch (*i) {
                                case CB:                if (!o.add(*i))     return false;
                                                        ++r; break;
                                case CE:                if (!o.add(*i))     return false;
                                                        if (r == 0)         return true;
                                                        --r; break;
                                case X::string::beg:    if (!skip(o, i, e)) return false;
                            }
                        }
                        return false;
                    }
                template <typename BA, typename II>
                    static bool read(BA o, II& i, II e) {
                        for ( ; i != e; ++i) {
                            char const c = *i;
                                 if (c == X::map::beg)      { if (!skip<X::map::beg, X::map::end>(o, i, e))     return false; }
                            else if (c == X::list::beg)     { if (!skip<X::list::beg, X::list::end>(o, i, e))   return false; }
                            else if (c == X::string::beg)   { if (!skip(o, i, e))                               return false; }
                            else if (c == X::map::sep)                                                          break;
                            else if (c == X::map::end)                                                          break;
                            else if (c == X::list::end)                                                         break;
                            else                            { if (!o.add(*i))                                   return false; }
                        }
                        return o.add('\0');
                    }
            };

        template <typename T, typename Cx>
            struct array_adder {
                T *f, *l;
                Cx& cx;
                template <size_t N>
                    array_adder(T (&t)[N], Cx& cx) : f(t), l(t + N), cx(cx) {}
                bool add(T e) { return f != l ? (*f = e, ++f, true) : cx|read_error::overflow; }
            };

        template <typename T>
            struct black_adder {
                constexpr bool add(T) const { return true; }
            };

    }

    template <typename X, typename T, size_t N, typename II, typename Cx>
        inline bool read(T (&t)[N], II& i, II e, Cx& cx) {
            II const o = i;
                if (!bits::value<X>::read(bits::array_adder<T, Cx>(t, cx), i, e)) {
                    return cx.ec == read_error::overflow ? (rewind(i, o), false) : cx|read_error::unexpected;
                }
            return true;
        }

    template <typename X, typename II, typename Cx>
        inline bool read(II& i, II e, Cx& cx) {
            return bits::value<X>::read(bits::black_adder<decltype(*i)>(), i, e) || (cx|read_error::unexpected);
        }

}}}

#endif // CXON_CHIO_VALUE_HXX_
