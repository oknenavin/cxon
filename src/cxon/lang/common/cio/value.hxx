// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_VALUE_HXX_
#define CXON_CIO_VALUE_HXX_

#include "cio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace val { // value parsing

    template <typename O = void>    struct skip_t; // expecting valid poke<O, char>(...)
    template <typename T>           struct is_skip_t;

    template <typename X, typename T, typename II, typename Cx>
        inline bool skip(T& t, II& i, II e, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace val {

    namespace bits {

        template <typename X>
            struct value {
                template <typename O, typename II>
                    static bool skip(O& o, II& i, II e) {
                        for ( ; i != e; ++i) {
                            char const c = *i;
                                 if (c == X::map::beg)      { if (!skip_<X::map::beg, X::map::end>(o, i, e))    return false; }
                            else if (c == X::list::beg)     { if (!skip_<X::list::beg, X::list::end>(o, i, e))  return false; }
                            else if (c == X::string::beg)   { if (!skip_(o, i, e))                              return false; }
                            else if (c == X::map::sep)                                                          break;
                            else if (c == X::map::end)                                                          break;
                            else if (c == X::list::end)                                                         break;
                            else                            { if (!poke(o, *i))                                 return false; }
                        }
                        return poke(o, *i);
                    }
            private:
                template <typename O, typename II>
                    static bool skip_(O& o, II& i, II e) {
                        if (!poke(o, *i)) return false;
                        for (++i; i != e; ++i) {
                            switch (*i)  {
                                case '\\':              if (!poke(o, *i))   return false;
                                                        if (++i == e)       return false;
                                                        break;
                                case X::string::end:    return poke(o, *i);
                            }
                        }
                        return false;
                    }
                template <char CB, char CE, typename O, typename II>
                    static bool skip_(O& o, II& i, II e) {
                        if (!poke(o, *i)) return false;
                        unsigned r = 0;
                        for (++i; i != e; ++i) {
                            switch (*i) {
                                case CB:                if (!poke(o, *i))       return false;
                                                        ++r; break;
                                case CE:                if (!poke(o, *i))       return false;
                                                        if (r == 0)             return true;
                                                        --r; break;
                                case X::string::beg:    if (!skip_(o, i, e))    return false;
                            }
                        }
                        return false;
                    }
            };

    }

    template <typename O>
        struct skip_t {
            O value;
            using value_type = char; // for is_back_insertable
                                        // TODO: consider O's value type
            void push_back(value_type c) const { poke(value, c); }
        };
    template <>
        struct skip_t<void> {
            using value_type = char; // for is_back_insertable
            void push_back(value_type) const {}
        };

    template <typename T>
        struct is_skip_t : std::false_type {};
    template <typename T>
        struct is_skip_t<skip_t<T>> : std::true_type {};

    template <typename X, typename T, typename II, typename Cx>
        inline bool skip(T& t, II& i, II e, Cx& cx) {
            return bits::value<X>::skip(t, i, e) || (cx|read_error::unexpected);
        }

}}}

#endif // CXON_CIO_VALUE_HXX_
