// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CIO_VALUE_HXX_
#define CXON_CIO_VALUE_HXX_

#include "cio.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cio { namespace val { // value parsing

    template <typename C = void>    struct sink; // expecting valid poke<O, char>(...)
    template <typename C, typename ...A>
        inline sink<C> make_sink(A&&... as);
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
                        if (*i == X::map::end)      return false;
                        CXON_IF_CONSTEXPR (X::list::end != X::map::end)
                            if (*i == X::list::end) return false;
                        if (*i == X::map::sep)      return false;
                        CXON_IF_CONSTEXPR (X::list::sep != X::map::sep)
                            if (*i == X::list::sep) return false;
                        if (*i == X::map::beg)      return skip_cnt_<X::map::beg, X::map::end>(o, i, e);
                        CXON_IF_CONSTEXPR (X::list::beg != X::map::beg)
                            if (*i == X::list::beg) return skip_cnt_<X::list::beg, X::list::end>(o, i, e);
                        if (*i == X::string::del)   return skip_str_(o, i, e);
                        else                        return skip_scl_(o, i, e);
                    }
            private:
#               define CXON_POKE(o, c) if (!poke(o, c)) return false
                    template <char CB, char CE, typename O, typename II>
                        static bool skip_cnt_(O& o, II& i, II e) {
                            if (!poke(o, *i)) return false;
                            unsigned r = 0;
                            for (++i; i != e; ++i) {
                                switch (*i) {
                                    case CB:                CXON_POKE(o, *i);
                                                            ++r; break;
                                    case CE:                CXON_POKE(o, *i);
                                                            if (r == 0) return ++i, true;
                                                            --r; break;
                                    case X::string::del:    if (!skip_s_(o, i, e)) return false;
                                                            break;
                                    default:                CXON_POKE(o, *i);
                                }
                            }
                            return false;
                        }
                    template <typename O, typename II>
                        static bool skip_str_(O& o, II& i, II e) {
                            return skip_s_(o, i, e) && (++i, true);
                        }
                    template <typename O, typename II>
                        static bool skip_s_(O& o, II& i, II e) {
                            if (!poke(o, *i)) return false;
                            for (++i; i != e; ++i) {
                                switch (*i)  {
                                    case '\\':              CXON_POKE(o, *i);
                                                            if (++i == e) return false;
                                                            CXON_POKE(o, *i); break;
                                    case X::string::del:    return poke(o, *i);
                                    default:                CXON_POKE(o, *i);
                                }
                            }
                            return false;
                        }
                    template <typename O, typename II>
                        static bool skip_scl_(O& o, II& i, II e) {
                            for ( ; i != e; ++i) {
                                if (*i == X::map::end)      return true;
                                CXON_IF_CONSTEXPR (X::list::end != X::map::end)
                                    if (*i == X::list::end) return true;
                                if (*i == X::map::sep)      return true;
                                CXON_IF_CONSTEXPR (X::list::sep != X::map::sep)
                                    if (*i == X::list::sep) return true;
                                CXON_POKE(o, *i);
                            }
                            return false;
                        }
#               undef CXON_POKE
            };

    }

    template <typename C>
        struct sink {
            C value;
            bool append(typename C::value_type c) { return poke(value, c); }
            bool operator ==(const sink& t) const { return value == t.value; }
            bool operator  <(const sink& t) const { return value  < t.value; }
        };
    template <>
        struct sink<void> {
            constexpr bool append(char) const { return true; }
        };
    template <typename C, typename ...A>
        inline sink<C> make_sink(A&&... as) {
            return { C(std::forward<A>(as)...) };
        }

    template <typename T>
        struct is_sink : std::false_type {};
    template <typename T>
        struct is_sink<sink<T>> : std::true_type {};

    template <typename X, typename T, typename II, typename Cx>
        inline bool sink_read(T& t, II& i, II e, Cx& cx) {
            return consume<X>(i, e, cx) && (imp::value_<X>::skip(t, i, e) || cx/X::read_error::unexpected);
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
