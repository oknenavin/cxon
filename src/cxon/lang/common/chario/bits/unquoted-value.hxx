// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHARIO_BITS_UNQUOTED_VALUE_HXX_
#define CXON_CHARIO_BITS_UNQUOTED_VALUE_HXX_

namespace cxon { namespace unquoted { namespace bits { // unquoted value

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
                             if (c == X::map::beg)      { if (!skip<map<X>::beg, map<X>::end>(o, i, e))     return false; }
                        else if (c == X::list::beg)     { if (!skip<list<X>::beg, list<X>::end>(o, i, e))   return false; }
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

}}} // cxon::unquoted::bits unquoted value

#endif // CXON_CHARIO_BITS_UNQUOTED_VALUE_HXX_
