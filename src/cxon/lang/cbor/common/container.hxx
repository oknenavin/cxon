// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMMON_CONTAINER_HXX_
#define CXON_CBOR_COMMON_CONTAINER_HXX_

#include "cxon/lang/common/container.hxx"
#include "cxon/lang/cbor/fundamental.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cnt {
        
    template <typename X, typename II, typename Cx>
        inline bool read_size(std::size_t& s, II& i, II e, Cx& cx);
    template <typename X, typename II, typename Cx>
        inline bool read_size_le(std::size_t& s, std::size_t n, II& i, II e, Cx& cx);
    template <typename X, typename II, typename Cx>
        inline bool read_size_eq(std::size_t n, II& i, II e, Cx& cx);

    template <typename X, typename O, typename Cx>
        inline bool write_size(O& o, bio::byte m, std::size_t s, Cx& cx);

}}}

namespace cxon { namespace cbor { namespace cnt {

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array(T& t, II& i, II e, Cx& cx);
    template <typename X, typename FI, typename II, typename Cx>
        inline bool read_array(FI f, FI l, II& i, II e, Cx& cx);

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array(T& t, std::size_t tag, II& i, II e, Cx& cx);
    template <typename X, typename FI, typename II, typename Cx>
        inline bool read_array(FI f, FI l, std::size_t tag, II& i, II e, Cx& cx);

    template <typename X, typename FI, typename O, typename Cx>
        inline bool write_array(O& o, FI f, FI l, Cx& cx);
    template <typename X, typename Iterable, typename O, typename Cx>
        inline auto write_array(O& o, const Iterable& i, Cx& cx) -> decltype(std::begin(i), bool());
    template <typename X, typename T, typename O, typename Cx>
        inline bool write_map(O& o, const T& t, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cnt {

    template <typename X, typename II, typename Cx>
        inline bool read_size(std::size_t& s, II& i, II e, Cx& cx) {
            return  cbor::imp::read_unsigned_<X>(s, i, e, cx) ||
                    cx/cbor::read_error::size_invalid
            ;
        }
    template <typename X, typename II, typename Cx>
        inline bool read_size_le(std::size_t& s, std::size_t n, II& i, II e, Cx& cx) {
            II const o = i; std::size_t t;
            return  (read_size<X>(t, i, e, cx) && t <= n && (s = t, true)) ||
                    (bio::rewind(i, o), cx/cbor::read_error::size_invalid)
            ;
        }
    template <typename X, typename II, typename Cx>
        inline bool read_size_eq(std::size_t n, II& i, II e, Cx& cx) {
            II const o = i; std::size_t s;
            return  (read_size<X>(s, i, e, cx) && s == n) ||
                    (bio::rewind(i, o), cx/cbor::read_error::size_invalid)
            ;
        }

    template <typename X, typename O, typename Cx>
        inline bool write_size(O& o, bio::byte m, std::size_t s, Cx& cx) {
            unsigned const p = cbor::imp::power_(s);
            return s > 0x17 ?
                bio::poke<X>(o, bio::byte(m + 0x18 + p), cx) && bio::poke<X>(o, s, 1 << p, cx) :
                bio::poke<X>(o, bio::byte(m + s), cx)
            ;
        }

}}}

namespace cxon { namespace cbor { namespace cnt {

    namespace imp { // byte/text arrays

        template <typename C>
            inline auto size_(option<1>, const C& c) -> decltype(c.size(), std::size_t()) {
                return c.size();
            }
        template <typename C>
            inline auto size_(option<0>, const C& c) -> decltype(std::distance(c.begin(), c.end()), std::size_t()) {
                return std::distance(c.begin(), c.end());
            }
        template <typename C>
            inline std::size_t size_(const C& c) {
                return size_(option<1>(), c);
            }

        template <typename X, typename C, typename II, typename Cx>
            inline auto elements_read_(C& c, std::size_t n, II& f, II l, Cx&)
                -> enable_if_t< is_forward_iterator<II>::value, bool>
            {
                auto const m = std::min(n, static_cast<std::size_t>(std::distance(f, l)));
                II e = f; std::advance(e, m);
                return cxon::cnt::append(c, f, e) && (std::advance(f, m), m == n);
            }
        template <typename X, typename C, typename II, typename Cx>
            inline auto elements_read_(C& c, std::size_t n, II& f, II l, Cx&)
                -> enable_if_t<!is_forward_iterator<II>::value, bool>
            {
                for ( ; n != 0 && f != l; --n, ++f)
                    cxon::cnt::append(c, *f);
                return n == 0;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_bytes_(T& t, std::size_t n, II& i, II e, Cx& cx) {
                return  elements_read_<X>(t, n, i, e, cx) ||
                        cx/cbor::read_error::unexpected
                ;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_b_fix_(T& t, II& i, II e, Cx& cx) {
                std::size_t n;
                return  read_size_le<X>(n, t.max_size() - size_(t), i, e, cx) &&
                        cxon::cnt::reserve(t, size_(t) + n) &&
                        read_bytes_<X>(t, n, i, e, cx)
                ;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_b_var_(T& t, II& i, II e, Cx& cx) {
                bio::byte const m = bio::get(i, e) & X::mjr;
                    for (auto b = bio::peek(i, e, 0); b != X::brk; b = bio::peek(i, e, 0)) {
                        if (m != (b & X::mjr))
                            return cx/cbor::read_error::array_invalid;
                        if (!read_array_b_fix_<X>(t, i, e, cx))
                            return false;
                    }
                return bio::get(i, e, 0), true;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_b_(T& t, bio::byte m, II& i, II e, Cx& cx) {
                switch (m & X::mnr) {
                    case 0x1C: case 0x1D: case 0x1E:    return cx/cbor::read_error::size_invalid;
                    case 0x1F:                          return read_array_b_var_<X>(t, i, e, cx);
                    default:                            return read_array_b_fix_<X>(t, i, e, cx);
                }
            }

    }

    namespace imp { // value arrays

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_elements_(T& t, std::size_t n, II& i, II e, Cx& cx) {
                for ( ; n != 0 && cxon::cnt::element_read<X>(t, i, e, cx); --n) 
                    ;
                return n == 0;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_fix_(T& t, II& i, II e, Cx& cx) {
                std::size_t n;
                return  read_size_le<X>(n, t.max_size() - size_(t), i, e, cx) &&
                        cxon::cnt::reserve(t, n) &&
                        read_elements_<X>(t, n, i, e, cx)
                ;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_var_(T& t, II& i, II e, Cx& cx) {
                II const o = i;
                    for (bio::get(i, e); size_(t) < t.max_size() && bio::peek(i, e, 0) != X::brk; )
                        if (!cxon::cnt::element_read<X>(t, i, e, cx))
                            return false;
                return  (bio::get(i, e, 0) == X::brk) ||
                        (bio::rewind(i, o), cx/cbor::read_error::size_invalid)
                ;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_(T& t, bio::byte m, II& i, II e, Cx& cx) {
                switch (m & X::mnr) {
                    case 0x1C: case 0x1D: case 0x1E:    return cx/cbor::read_error::size_invalid;
                    case 0x1F:                          return imp::read_array_w_var_<X>(t, i, e, cx);
                    default:                            return imp::read_array_w_fix_<X>(t, i, e, cx);
                }
            }

    }

    namespace imp {

        template <typename X, typename T, typename II, typename Cx>
            inline auto read_array_(T& t, II& i, II e, Cx& cx)
                -> enable_if_t< std::is_assignable<decltype(*std::begin(t)), decltype(*i)>::value, bool>
            {
                auto const m = bio::peek(i, e);
                switch (m & X::mjr) {
                    case X::bstr: case X::tstr: return imp::read_array_b_<X>(t, m, i, e, cx);
                    case X::arr:                return imp::read_array_w_<X>(t, m, i, e, cx);
                    default:                    return cx/cbor::read_error::array_invalid;
                }
            }
        template <typename X, typename T, typename II, typename Cx>
            inline auto read_array_(T& t, II& i, II e, Cx& cx)
                -> enable_if_t<!std::is_assignable<decltype(*std::begin(t)), decltype(*i)>::value, bool>
            {
                auto const m = bio::peek(i, e);
                switch (m & X::mjr) {
                    case X::arr: case X::map:   return imp::read_array_w_<X>(t, m, i, e, cx);
                    default:                    return cx/cbor::read_error::array_invalid;
                }
            }

    }
    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array(T& t, II& i, II e, Cx& cx) {
            std::size_t tag;
            if (!tag::read<X>(tag, i, e, cx))
                return false;
            return read_array<X>(t, tag, i, e, cx);
        }
    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array(T& t, std::size_t/* tag*/, II& i, II e, Cx& cx) {
            // TODO: tags - e.g. typed-arrays (RFC8746)
            return imp::read_array_<X>(t, i, e, cx);
        }

    template <typename X, typename FI, typename II, typename Cx>
        inline bool read_array(FI f, FI l, II& i, II e, Cx& cx) {
            std::size_t tag;
            if (!tag::read<X>(tag, i, e, cx))
                return false;
            return read_array<X>(f, l, tag, i, e, cx);
        }
    template <typename X, typename FI, typename II, typename Cx>
        inline bool read_array(FI f, FI l, std::size_t tag, II& i, II e, Cx& cx) {
            // TODO: tags - e.g. typed-arrays (RFC8746)
            auto c = cxon::cnt::make_range_container(f, l);
            return read_array<X>(c, tag, i, e, cx);
        }

}}}

namespace cxon { namespace cbor { namespace cnt {

    namespace imp {

        template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
            inline auto write_array_(O& o, FI f, FI l, Cx& cx) -> enable_if_t<sizeof(T) == 1 &&  is_char<T>::value, bool> {
                return  write_size<X>(o, X::tstr, std::distance(f, l), cx) &&
                        bio::poke<X>(o, f, l, cx)
                ;
            }
        template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
            inline auto write_array_(O& o, FI f, FI l, Cx& cx) -> enable_if_t<sizeof(T) == 1 && !is_char<T>::value, bool> {
                return  write_size<X>(o, X::bstr, std::distance(f, l), cx) &&
                        bio::poke<X>(o, f, l, cx)
                ;
            }
        template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
            inline auto write_array_(O& o, FI f, FI l, Cx& cx) -> enable_if_t<sizeof(T) != 1, bool> {
                if (write_size<X>(o, X::arr, std::distance(f, l), cx))
                    for ( ; f != l && write_value<X>(o, *f, cx); ++f) ;
                return f == l;
            }

    }
    template <typename X, typename FI, typename O, typename Cx>
        inline bool write_array(O& o, FI f, FI l, Cx& cx) {
            return imp::write_array_<X>(o, f, l, cx);
        }

    template <typename X, typename I, typename O, typename Cx>
        inline auto write_array(O& o, const I& i, Cx& cx) -> decltype(std::begin(i), bool()) {
            auto const c = cxon::cnt::continuous<I>::range(i);
            return imp::write_array_<X>(o, c.first, c.second, cx);
        }

    template <typename X, typename T, typename O, typename Cx>
        inline bool write_map(O& o, const T& t, Cx& cx) {
            auto f = std::begin(t), l = std::end(t);
            if (write_size<X>(o, X::map, std::distance(f, l), cx))
                for ( ; f != l && cxon::cnt::element_write<X, T>(o, *f, cx); ++f) ;
            return f == l;
        }

}}}

#endif // CXON_CBOR_COMMON_CONTAINER_HXX_
