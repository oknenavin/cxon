// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMMON_CONTAINER_HXX_
#define CXON_CBOR_COMMON_CONTAINER_HXX_

#include "cxon/lang/cbor/fundamental.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cnt { // container mutation

    template <typename C>
        struct container_mutator;
        //  static bool reserve(C& c, size_t s);
        //  static auto emplace(C& c) -> typename C::reference;
        //  template <typename II>
        //      static bool append(C& c, II f, II l);
    template <typename C>
        inline bool container_reserve(C& c, size_t s);
    template <typename C>
        inline auto container_emplace(C& c) -> typename C::reference;
    template <typename C, typename II>
        inline bool container_append(C& c, II f, II l);

}}}

namespace cxon { namespace cbor { namespace cnt { // container element read/write

    template <typename X, typename C>
        struct element_reader {
            template <typename II, typename Cx>
                static bool read(C& c, II& i, II e, Cx& cx);
        };
    template <typename X, typename C, typename II, typename Cx>
        inline bool element_read(C& c, II& i, II e, Cx& cx);
    template <typename X, typename C, typename II, typename Cx>
        inline bool element_read(C& c, size_t n, II& f, II l, Cx& cx);

    template <typename X, typename C>
        struct element_writer {
            template <typename O, typename Cx>
                static bool write(O& o, const typename C::value_type& t, Cx& cx);
        };
    template <typename X, typename C, typename O, typename Cx>
        inline bool element_write(O& o, const typename C::value_type& t, Cx& cx);

}}}

namespace cxon { namespace cbor { namespace cnt {
        
    template <typename X, typename II, typename Cx>
        inline bool read_size(size_t& s, II& i, II e, Cx& cx);
    template <typename X, typename II, typename Cx>
        inline bool read_size_le(size_t& s, size_t n, II& i, II e, Cx& cx);
    template <typename X, typename II, typename Cx>
        inline bool read_size_eq(size_t n, II& i, II e, Cx& cx);

    template <typename X, typename O, typename Cx>
        inline bool write_size(O& o, bio::byte m, size_t s, Cx& cx);

}}}

namespace cxon { namespace cbor { namespace cnt {

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array(T& t, II& i, II e, Cx& cx);
    template <typename X, typename FI, typename II, typename Cx>
        inline bool read_array(FI f, FI l, II& i, II e, Cx& cx);

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array(T& t, size_t tag, II& i, II e, Cx& cx);
    template <typename X, typename FI, typename II, typename Cx>
        inline bool read_array(FI f, FI l, size_t tag, II& i, II e, Cx& cx);
    
    template <typename X, typename FI, typename O, typename Cx>
        inline bool write_array(O& o, FI f, FI l, Cx& cx);
    template <typename X, bio::byte M, typename T, typename O, typename Cx>
        inline bool write_array(O& o, const T& t, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cnt {

    template <typename A>
        struct adaptor : A {
            static const typename A::container_type& container(const A& a) noexcept { return ((adaptor&)a).c; }
            static       typename A::container_type& container(      A& a) noexcept { return ((adaptor&)a).c; }
        };

}}}

namespace cxon { namespace cbor { namespace cnt { // container mutation

    namespace bits {

        template <typename C>
            inline auto reserve_(option<2>, C& c, size_t s)
                -> enable_if_t<std::is_same<decltype(container_mutator<C>::reserve(c, s)), bool>::value, bool>
            {
                return container_mutator<C>::reserve(c, s);
            }
        template <typename C>
            inline auto reserve_(option<1>, C& c, size_t s)
                -> decltype(c.reserve(s), bool())
            {
                return c.reserve(s), true;
            }
        template <typename C>
            inline constexpr bool reserve_(option<0>, C&, size_t) {
                return true;
            }

    }
    template <typename C>
        inline bool container_reserve(C& c, size_t s) {
            return bits::reserve_(option<1>(), c, s);
        }


    namespace bits {

        template <typename C>
            static auto emplace_(option<3>, C& c)
                -> enable_if_t<std::is_same<decltype(container_mutator<C>::emplace(c)), typename C::reference>::value, typename C::reference>
            {
                return container_mutator<C>::emplace(c);
            }
        template <typename C>
            static auto emplace_(option<2>, C& c)
                -> enable_if_t<has_emplace_back<C>::value, typename C::reference>
            {
                return c.emplace_back();
            }
        template <typename C>
            static auto emplace_(option<1>, C& c)
                -> enable_if_t<has_emplace_back_void<C>::value && has_back<C>::value, typename C::reference>
            {
                return c.emplace_back(), c.back();
            }
        template <typename C>
            static auto emplace_(option<0>, C& c)
                -> enable_if_t<has_push_back<C>::value && has_back<C>::value, typename C::reference>
            {
                return c.push_back({}), c.back();
            }

    }
    template <typename C>
        inline auto container_emplace(C& c) -> typename C::reference {
            return bits::emplace_(option<3>(), c);
        }

    namespace bits {

        template <typename C, typename II>
            inline auto append_(option<1>, C& c, II f, II l)
                -> enable_if_t<std::is_same<decltype(container_mutator<C>::append(c, f, l)), bool>::value, bool>
            {
                return container_mutator<C>::append(c, f, l);
            }
        template <typename C, typename II>
            inline auto append_(option<0>, C& c, II f, II l)
                -> enable_if_t<std::is_same<decltype(container_emplace(c)), typename C::reference>::value, bool>
            {
                for ( ; f != l; ++f)
                    container_emplace(c) = *f;
                return f == l;
            }

    }
    template <typename C, typename II>
        inline bool container_append(C& c, II f, II l) {
            return bits::append_(option<1>(), c, f, l);
        }

}}}

namespace cxon { namespace cbor { namespace cnt { // container element read/write

    template <typename X, typename C>
        template <typename II, typename Cx>
            bool element_reader<X, C>::read(C& c, II& i, II e, Cx& cx) {
                return read_value<X>(container_emplace(c), i, e, cx);
            }

    template <typename X, typename C, typename II, typename Cx>
        inline bool element_read(C& c, II& i, II e, Cx& cx) {
            return element_reader<X, C>::read(c, i, e, cx);
        }

    namespace bits {

        template <typename X, typename C, typename II, typename Cx>
            inline auto element_read_(C& c, size_t n, II& f, II l, Cx&)
                -> enable_if_t< is_forward_iterator<II>::value, bool>
            {
                auto const m = std::min(n, static_cast<size_t>(std::distance(f, l)));
                II e = f; std::advance(e, m);
                return container_append(c, f, e) && (std::advance(f, m), m == n);
            }
        template <typename X, typename C, typename II, typename Cx>
            inline auto element_read_(C& c, size_t n, II& f, II l, Cx&)
                -> enable_if_t<!is_forward_iterator<II>::value, bool>
            {
                for ( ; n != 0 && f != l; --n, ++f)
                    container_emplace(c) = *f;
                return n == 0;
            }

    }
    template <typename X, typename C, typename II, typename Cx>
        inline bool element_read(C& c, size_t n, II& f, II l, Cx& cx) {
            return bits::element_read_<X>(c, n, f, l, cx);
        }

}}}

namespace cxon { namespace cbor { namespace cnt {

    template <typename X, typename II, typename Cx>
        inline bool read_size(size_t& s, II& i, II e, Cx& cx) {
            return  cbor::bits::read_unsigned_<X>(s, i, e, cx) ||
                    cx/cbor::read_error::size_invalid
            ;
        }
    template <typename X, typename II, typename Cx>
        inline bool read_size_le(size_t& s, size_t n, II& i, II e, Cx& cx) {
            II const o = i; size_t t;
            return  (read_size<X>(t, i, e, cx) && t <= n && (s = t, true)) ||
                    (bio::rewind(i, o), cx/cbor::read_error::size_invalid)
            ;
        }
    template <typename X, typename II, typename Cx>
        inline bool read_size_eq(size_t n, II& i, II e, Cx& cx) {
            II const o = i; size_t s;
            return  (read_size<X>(s, i, e, cx) && s == n) ||
                    (bio::rewind(i, o), cx/cbor::read_error::size_invalid)
            ;
        }

    template <typename X, typename O, typename Cx>
        inline bool write_size(O& o, bio::byte m, size_t s, Cx& cx) {
            unsigned const p = cbor::bits::power_(s);
            return s > 0x17 ?
                bio::poke<X>(o, bio::byte(m + 0x18 + p), cx) && bio::poke<X>(o, s, 1 << p, cx) :
                bio::poke<X>(o, bio::byte(m + s), cx)
            ;
        }

}}}

namespace cxon { namespace cbor { namespace cnt {

    namespace bits { // byte/text arrays

        template <typename C>
            inline auto size__(option<1>, const C& c)
                -> decltype(c.size(), size_t())
            {
                return c.size();
            }
        template <typename C>
            inline auto size__(option<0>, const C& c)
                -> decltype(std::distance(c.begin(), c.end()), size_t())
            {
                return std::distance(c.begin(), c.end());
            }
        template <typename C>
            inline size_t size_(const C& c) {
                return size__(option<1>(), c);
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_bytes_(T& t, size_t n, II& i, II e, Cx& cx) {
                return  element_read<X>(t, n, i, e, cx) ||
                        cx/cbor::read_error::unexpected
                ;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_b_fix_(T& t, II& i, II e, Cx& cx) {
                size_t n;
                return  read_size_le<X>(n, t.max_size() - size_(t), i, e, cx) &&
                        container_reserve(t, size_(t) + n) &&
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

    namespace bits { // value arrays

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_elements_(T& t, size_t n, II& i, II e, Cx& cx) {
                for ( ; n != 0 && element_read<X>(t, i, e, cx); --n) 
                    ;
                return n == 0;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_fix_(T& t, II& i, II e, Cx& cx) {
                size_t n;
                return  read_size_le<X>(n, t.max_size() - size_(t), i, e, cx) &&
                        container_reserve(t, n) &&
                        read_elements_<X>(t, n, i, e, cx)
                ;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_var_(T& t, II& i, II e, Cx& cx) {
                II const o = i;
                    for (bio::get(i, e); size_(t) < t.max_size() && bio::peek(i, e, 0) != X::brk; )
                        if (!element_read<X>(t, i, e, cx))
                            return false;
                return  (bio::get(i, e, 0) == X::brk) ||
                        (bio::rewind(i, o), cx/cbor::read_error::size_invalid)
                ;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_(T& t, bio::byte m, II& i, II e, Cx& cx) {
                switch (m & X::mnr) {
                    case 0x1C: case 0x1D: case 0x1E:    return cx/cbor::read_error::size_invalid;
                    case 0x1F:                          return bits::read_array_w_var_<X>(t, i, e, cx);
                    default:                            return bits::read_array_w_fix_<X>(t, i, e, cx);
                }
            }

    }

    namespace bits {

        template <typename X, typename T, typename II, typename Cx>
            inline auto read_array_(T& t, II& i, II e, Cx& cx)
                -> enable_if_t< std::is_assignable<decltype(*std::begin(t)), decltype(*i)>::value, bool>
            {
                auto const m = bio::peek(i, e);
                switch (m & X::mjr) {
                    case X::bstr: case X::tstr: return bits::read_array_b_<X>(t, m, i, e, cx);
                    case X::arr:                return bits::read_array_w_<X>(t, m, i, e, cx);
                    default:                    return cx/cbor::read_error::array_invalid;
                }
            }
        template <typename X, typename T, typename II, typename Cx>
            inline auto read_array_(T& t, II& i, II e, Cx& cx)
                -> enable_if_t<!std::is_assignable<decltype(*std::begin(t)), decltype(*i)>::value, bool>
            {
                auto const m = bio::peek(i, e);
                switch (m & X::mjr) {
                    case X::arr: case X::map:   return bits::read_array_w_<X>(t, m, i, e, cx);
                    default:                    return cx/cbor::read_error::array_invalid;
                }
            }

    }
    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array(T& t, II& i, II e, Cx& cx) {
            size_t tag;
            if (!tag::read<X>(tag, i, e, cx))
                return false;
            return read_array<X>(t, tag, i, e, cx);
        }
    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array(T& t, size_t/* tag*/, II& i, II e, Cx& cx) {
            // TODO: tags - e.g. typed-arrays (RFC8746)
            return bits::read_array_<X>(t, i, e, cx);
        }

    namespace bits {

        template <typename FI>
            struct range_container_ {
                using value_type = typename std::iterator_traits<FI>::value_type;
                using reference = value_type&;

                range_container_(FI f, FI l) : f_(f), l_(l), e_(f) {}

                size_t size() const     { return std::distance(f_, e_); }
                size_t max_size() const { return std::distance(f_, l_); }

                FI begin()  { return f_; }
                FI end()    { return e_; }

                reference emplace_back()            { return *e_++; }
                void push_back(const value_type& t) { *e_ = t, ++e_; }
                void push_back(value_type&& t)      { *e_ = std::move(t), ++e_; }

                private:
                    FI f_, l_, e_;
            };
        template <typename FI>
            range_container_<FI> make_range_container_(FI f, FI l) {
                return {f, l};
            }

    }
    template <typename X, typename FI, typename II, typename Cx>
        inline bool read_array(FI f, FI l, II& i, II e, Cx& cx) {
            size_t tag;
            if (!tag::read<X>(tag, i, e, cx))
                return false;
            return read_array<X>(f, l, tag, i, e, cx);
        }
    template <typename X, typename FI, typename II, typename Cx>
        inline bool read_array(FI f, FI l, size_t/* tag*/, II& i, II e, Cx& cx) {
            // TODO: tags - e.g. typed-arrays (RFC8746)
            auto c = bits::make_range_container_(f, l);
            return read_array<X>(c, i, e, cx);
        }

}}}

namespace cxon { namespace cbor { namespace cnt {

    template <typename X, typename C>
        template <typename O, typename Cx>
            bool element_writer<X, C>::write(O& o, const typename C::value_type& t, Cx& cx) {
                return write_value<X>(o, t, cx);
            }

    template <typename X, typename C, typename O, typename Cx>
        inline bool element_write(O& o, const typename C::value_type& t, Cx& cx) {
            return element_writer<X, C>::write(o, t, cx);
        }

}}}

namespace cxon { namespace cbor { namespace cnt {

    namespace bits {

        template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
            inline auto write_array_(O& o, FI f, FI l, Cx& cx) -> enable_if_t<sizeof(T) == 1 &&  is_char<T>::value, bool> {
                size_t const n = std::distance(f, l);
                return  write_size<X>(o, X::tstr, n, cx) &&
                        bio::poke<X>(o, f, n, cx)
                ;
            }
        template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
            inline auto write_array_(O& o, FI f, FI l, Cx& cx) -> enable_if_t<sizeof(T) == 1 && !is_char<T>::value, bool> {
                size_t const n = std::distance(f, l);
                return  write_size<X>(o, X::bstr, n, cx) &&
                        bio::poke<X>(o, f, n, cx)
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
            return bits::write_array_<X>(o, f, l, cx);
        }

    template <typename X, bio::byte M, typename T, typename O, typename Cx>
        inline bool write_array(O& o, const T& t, Cx& cx) {
            auto f = std::begin(t), l = std::end(t);
            if (write_size<X>(o, M, std::distance(f, l), cx))
                for ( ; f != l && element_write<X, T>(o, *f, cx); ++f) ;
            return f == l;
        }

}}}

#endif // CXON_CBOR_COMMON_CONTAINER_HXX_
