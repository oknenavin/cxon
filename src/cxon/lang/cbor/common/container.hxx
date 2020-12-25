// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMMON_CONTAINER_HXX_
#define CXON_CBOR_COMMON_CONTAINER_HXX_

#include "cxon/lang/cbor/fundamental.hxx"

// interface ///////////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cnt {

    template <typename X, typename T>
        struct reserve;
        //  static bool space(T& t, size_t n);
    template <typename X, typename T>
        inline bool reserve_space(T& t, size_t n);

    template <typename X, typename T>
        struct append;
        //  template <typename II, typename Cx>
        //      static bool value(T& t, II& i, II e, Cx& cx);
    template <typename X, typename T, typename II, typename Cx>
        inline bool append_value(T& t, II& i, II e, Cx& cx);

    template <typename X, typename T>
        struct append_element;
        //  template <typename II, typename Cx>
        //      static bool range(T& t, II f, II l, Cx& cx);
    template <typename X, typename T, typename II, typename Cx>
        inline bool append_element_range(T& t, size_t n, II& f, II l, Cx& cx);

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array(T& t, II& i, II e, Cx& cx);
    template <typename X, typename FI, typename II, typename Cx>
        inline bool read_array(FI f, FI l, II& i, II e, Cx& cx);
    
    template <typename X, typename FI, typename O, typename Cx>
        inline bool write_array(O& o, FI f, FI l, Cx& cx);

}}}

// implementation //////////////////////////////////////////////////////////////

namespace cxon { namespace cbor { namespace cnt {

    template <typename X, typename T>
        struct reserve {
            template <typename U = T>
                static auto space_(option<1>, U& t, size_t n)
                    -> decltype(t.reserve(n), bool())
                {
                    return t.reserve(n), true;
                }
            static constexpr bool space_(option<0>, T&, size_t) {
                return true;
            }

            static bool space(T& t, size_t n) {
                return space_(option<1>(), t, n);
            }
        };
    template <typename X, typename T>
        inline bool reserve_space(T& t, size_t n) {
            return reserve<X, T>::space(t, n);
        }

    template <typename X, typename T>
        struct append {
            template <typename II, typename Cx, typename U = T>
                static auto value_(option<2>, U& t, II& i, II e, Cx& cx)
                    -> enable_if_t<has_emplace_back<U>::value, bool>
                {
                    return read_value<X>(t.emplace_back(), i, e, cx);
                }
            template <typename II, typename Cx, typename U = T>
                static auto value_(option<1>, U& t, II& i, II e, Cx& cx)
                    -> enable_if_t<has_emplace_back_void<U>::value && has_back<U>::value, bool>
                {
                    return t.emplace_back(), read_value<X>(t.back(), i, e, cx);
                }
            template <typename II, typename Cx, typename U = T>
                static auto value_(option<0>, U& t, II& i, II e, Cx& cx)
                    -> enable_if_t<has_push_back<U>::value && has_back<U>::value, bool>
                {
                    return t.push_back({}), read_value<X>(t.back(), i, e, cx);
                }

            template <typename II, typename Cx>
                static bool value(T& t, II& i, II e, Cx& cx) {
                    return value_(option<2>(), t, i, e, cx);
                }
        };
    template <typename X, typename T, typename II, typename Cx>
        inline bool append_value(T& t, II& i, II e, Cx& cx) {
            return append<X, T>::value(t, i, e, cx);
        }

    namespace bits {

        template <typename X, typename T, typename II, typename Cx>
            inline auto append_element_range_(T& t, size_t n, II& f, II l, Cx& cx)
                -> enable_if_t< is_forward_iterator<II>::value, bool>
            {
                auto const m = std::min(n, static_cast<size_t>(std::distance(f, l)));
                II e = f; std::advance(e, m);
                return append_element<X, T>::range(t, f, e, cx) && (std::advance(f, m), m == n);
            }
        template <typename X, typename T, typename II, typename Cx>
            inline auto append_element_range_(T& t, size_t n, II& f, II l, Cx& cx)
                -> enable_if_t<!is_forward_iterator<II>::value, bool>
            {
                for ( ; n != 0 && append_value<X>(t, f, l, cx); --n)
                    ;
                return n == 0;
            }

    }

    template <typename X, typename T>
        struct append_element {
            template <typename II, typename Cx>
                static bool range(T& t, II f, II l, Cx& cx) {
                    for ( ; f != l && append_value<X>(t, f, l, cx); )
                        ;
                    return f == l;
                }
        };
    template <typename X, typename T, typename II, typename Cx>
        inline bool append_element_range(T& t, size_t n, II& f, II l, Cx& cx) {
            return bits::append_element_range_<X>(t, n, f, l, cx);
        }

}}}

namespace cxon { namespace cbor { namespace cnt {

    namespace bits {

        template <typename X, typename II, typename Cx>
            inline bool read_size_(size_t& s, II& i, II e, Cx& cx) {
                return cbor::bits::read_unsigned_<X>(s, i, e, cx) || cx|cbor::read_error::size_invalid;
            }
        template <typename X, typename II, typename Cx>
            inline bool read_size_le_(size_t& s, size_t n, II& i, II e, Cx& cx) {
                II const o = i; size_t t;
                return  (read_size_<X>(t, i, e, cx) && t <= n && (s = t, true)) ||
                        (bio::rewind(i, o), cx|cbor::read_error::size_invalid)
                ;
            }

    }

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
                return  (append_element_range<X>(t, n, i, e, cx)) ||
                        (cx|cbor::read_error::unexpected)
                ;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_b_fix_(T& t, II& i, II e, Cx& cx) {
                size_t n;
                return  read_size_le_<X>(n, t.max_size() - size_(t), i, e, cx) &&
                        reserve_space<X>(t, size_(t) + n) &&
                        read_bytes_<X>(t, n, i, e, cx)
                ;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_b_var_(T& t, II& i, II e, Cx& cx) {
                bio::byte const m = bio::get(i, e) & X::mjr;
                    for (auto b = bio::peek(i, e, 0); b != X::brk; b = bio::peek(i, e, 0)) {
                        if (m != (b & X::mjr))
                            return cx|cbor::read_error::array_invalid;
                        if (!read_array_b_fix_<X>(t, i, e, cx))
                            return false;
                    }
                return bio::get(i, e, 0), true;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_b_(T& t, bio::byte m, II& i, II e, Cx& cx) {
                switch (m & X::mnr) {
                    case 0x1C: case 0x1D: case 0x1E:    return cx|cbor::read_error::size_invalid;
                    case 0x1F:                          return read_array_b_var_<X>(t, i, e, cx);
                    default:                            return read_array_b_fix_<X>(t, i, e, cx);
                }
            }

    }

    namespace bits { // value arrays

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_elements_(T& t, size_t n, II& i, II e, Cx& cx) {
                for ( ; n != 0 && append_value<X>(t, i, e, cx); --n) 
                    ;
                return n == 0;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_fix_(T& t, II& i, II e, Cx& cx) {
                size_t n;
                return  read_size_le_<X>(n, t.max_size() - size_(t), i, e, cx) &&
                        reserve_space<X>(t, n) &&
                        read_elements_<X>(t, n, i, e, cx)
                ;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_var_(T& t, II& i, II e, Cx& cx) {
                II const o = i;
                    for (bio::get(i, e); size_(t) < t.max_size() && bio::peek(i, e, 0) != X::brk; )
                        if (!append_value<X>(t, i, e, cx))
                            return false;
                return  (bio::get(i, e, 0) == X::brk) ||
                        (bio::rewind(i, o), cx|cbor::read_error::size_invalid)
                ;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_(T& t, bio::byte m, II& i, II e, Cx& cx) {
                switch (m & X::mnr) {
                    case 0x1C: case 0x1D: case 0x1E:    return cx|cbor::read_error::size_invalid;
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
                    default:                    return cx|cbor::read_error::array_invalid;
                }
            }
        template <typename X, typename T, typename II, typename Cx>
            inline auto read_array_(T& t, II& i, II e, Cx& cx)
                -> enable_if_t<!std::is_assignable<decltype(*std::begin(t)), decltype(*i)>::value, bool>
            {
                auto const m = bio::peek(i, e);
                switch (m & X::mjr) {
                    case X::arr:    return bits::read_array_w_<X>(t, m, i, e, cx);
                    default:        return cx|cbor::read_error::array_invalid;
                }
            }

    }
    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array(T& t, II& i, II e, Cx& cx) {
            return bits::read_array_<X>(t, i, e, cx);
        }

    namespace bits {

        template <typename FI>
            struct range_container {
                using value_type = typename std::iterator_traits<FI>::value_type;
                using reference = value_type&;

                range_container(FI f, FI l) : f_(f), l_(l), e_(f) {}

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
            range_container<FI> make_range_container(FI f, FI l) {
                return {f, l};
            }

    }
    template <typename X, typename FI, typename II, typename Cx>
        inline bool read_array(FI f, FI l, II& i, II e, Cx& cx) {
            auto c = bits::make_range_container(f, l);
            return read_array<X>(c, i, e, cx);
        }

}}}

namespace cxon { namespace cbor { namespace cnt {

    namespace bits {

        template <typename X, typename O, typename Cx>
            inline bool write_size_(O& o, bio::byte m, size_t s, Cx& cx) {
                unsigned const p = cbor::bits::power_(s);
                return s > 0x17 ?
                    bio::poke<X>(o, bio::byte(m + 0x18 + p), cx) && bio::poke<X>(o, s, 1 << p, cx) :
                    bio::poke<X>(o, bio::byte(m + s), cx)
                ;
            }

        template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
            inline auto write_array_(O& o, FI f, FI l, Cx& cx) -> enable_if_t<sizeof(T) == 1 &&  is_char<T>::value, bool> {
                size_t const n = std::distance(f, l);
                return  write_size_<X>(o, X::tstr, n, cx) &&
                        bio::poke<X>(o, f, n, cx)
                ;
            }
        template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
            inline auto write_array_(O& o, FI f, FI l, Cx& cx) -> enable_if_t<sizeof(T) == 1 && !is_char<T>::value, bool> {
                size_t const n = std::distance(f, l);
                return  write_size_<X>(o, X::bstr, n, cx) &&
                        bio::poke<X>(o, f, n, cx)
                ;
            }
        template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
            inline auto write_array_(O& o, FI f, FI l, Cx& cx) -> enable_if_t<sizeof(T) != 1, bool> {
                if (write_size_<X>(o, X::arr, std::distance(f, l), cx))
                    for ( ; f != l && write_value<X>(o, *f, cx); ++f) ;
                return f == l;
            }

    }
    template <typename X, typename FI, typename O, typename Cx>
        inline bool write_array(O& o, FI f, FI l, Cx& cx) {
            return bits::write_array_<X>(o, f, l, cx);
        }

}}}

#endif // CXON_CBOR_COMMON_CONTAINER_HXX_
