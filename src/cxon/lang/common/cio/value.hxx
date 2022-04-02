// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_VALUE_HXX_
#define CXON_CIO_VALUE_HXX_

#include "cio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace val { // value parsing

    template <typename C = void>    struct sink; // expecting valid poke<O, char>(...)
    template <typename T>           struct is_sink;

    template <typename X, typename T, typename II, typename Cx>
        inline bool sink_read(T& t, II& i, II e, Cx& cx);

    template <typename X, typename O, typename S, typename Cx>
        inline bool sink_write(O& o, const S& s, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace val {

    namespace imp {

        template <typename X>
            struct value_ {
                template <typename O, typename II>
                    static bool skip(O& o, II& i, II e) {
                        for ( ; i != e; ++i) {
                            char const c = *i;
                                 if (c == X::map::beg)      { if (!skip_<X::map::beg, X::map::end>(o, i, e))    return false; }
                            else if (c == X::list::beg)     { if (!skip_<X::list::beg, X::list::end>(o, i, e))  return false; }
                            else if (c == X::string::del)   { if (!skip_(o, i, e))                              return false; }
                            else if (c == X::map::sep)                                                          break;
                            else if (c == X::map::end)                                                          break;
                            else if (c == X::list::end)                                                         break;
                            else                            { if (!poke(o, *i))                                 return false; }
                        }
                        return true;
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
                                case X::string::del:    return poke(o, *i);
                                default:                if (!poke(o, *i))   return false;
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
                                case X::string::del:    if (!skip_(o, i, e))    return false;
                                                        break;
                                default:                if (!poke(o, *i))       return false;
                            }
                        }
                        return false;
                    }
            };

    }

    template <typename C>
        struct sink {
            using value_type = typename C::value_type; // for is_back_insertable
            void push_back(value_type c) { poke(value, c); }
            C value;
        };
    template <>
        struct sink<void> {
            using value_type = char; // for is_back_insertable
            void push_back(value_type) const {}
        };

    template <typename T>
        struct is_sink : std::false_type {};
    template <typename T>
        struct is_sink<sink<T>> : std::true_type {};

    template <typename X, typename T, typename II, typename Cx>
        inline bool sink_read(T& t, II& i, II e, Cx& cx) {
            return imp::value_<X>::skip(t, i, e) || cx/X::read_error::unexpected;
        }

    template <typename X, typename O, typename S, typename Cx>
        inline bool sink_write(O& o, const S& s, Cx& cx) {
            for (auto e : s.value) 
                if (!cio::poke<X>(o, e, cx))
                    return false;
            return true;
        }

}}}

#endif // CXON_CIO_VALUE_HXX_
