// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMMON_CONTAINER_HXX_
#define CXON_CBOR_COMMON_CONTAINER_HXX_

#include "cxon/lang/cbor/compound.hxx"

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
        struct append_n;
        //  template <typename II, typename Cx>
        //      static bool value(T& t, II& i, II e, Cx& cx);
    template <typename X, typename T, typename II, typename Cx>
        inline bool append_value(T& t, size_t n, II& i, II e, Cx& cx);

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
                    -> enable_if_t<std::is_same<decltype(t.emplace_back()), typename U::reference>::value, bool>
                {
                    return read_value<X>(t.emplace_back(), i, e, cx);
                }
            template <typename II, typename Cx>
                static auto value_(option<1>, T& t, II& i, II e, Cx& cx)
                    -> decltype(t.emplace_back(), t.back(), bool())
                {
                    return t.emplace_back(), read_value<X>(t.back(), i, e, cx);
                }
            template <typename II, typename Cx>
                static auto value_(option<0>, T& t, II& i, II e, Cx& cx)
                    -> decltype(t.push_back(std::declval<typename T::value_type>()), t.back(), bool())
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

    template <typename X, typename T>
        struct append_n {
            template <typename II, typename Cx>
                static bool value(T& t, II& i, II e, Cx& cx) {
                    for ( ; i != e && append_value<X>(t, i, e, cx); )
                        ;
                    return i == e;
                }
        };
    template <typename X, typename T, typename II, typename Cx>
        inline bool append_value(T& t, size_t n, II& i, II e, Cx& cx) {
            auto const m = std::min(n, static_cast<size_t>(std::distance(i, e)));
            return append_n<X, T>::value(t, i, i + m, cx); // TODO: i + m => handle input-iterators
        }

}}}

namespace cxon { namespace cbor { namespace cnt {

    namespace bits {

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
                return  (append_value<X>(t, n, i, e, cx)) ||
                        (cx|cbor::read_error::unexpected)
                ;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_b_fix_(T& t, II& i, II e, Cx& cx) {
                size_t n;
                return  cbor::bits::read_size_<X>(n, i, e, cx) &&
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

    namespace bits {

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_elements_(T& t, size_t n, II& i, II e, Cx& cx) {
                for ( ; n != 0 && append_value<X>(t, i, e, cx); --n) 
                    ;
                return n == 0;
            }

        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_fix_(T& t, II& i, II e, Cx& cx) {
                size_t n;
                return  cbor::bits::read_size_<X>(n, i, e, cx) &&
                        reserve_space<X>(t, n) &&
                        read_elements_<X>(t, n, i, e, cx)
                ;
            }
        template <typename X, typename T, typename II, typename Cx>
            inline bool read_array_w_var_(T& t, II& i, II e, Cx& cx) {
                II const o = i;
                    for (bio::get(i, e); bio::peek(i, e, 0) != X::brk; )
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

    template <typename X, typename T, typename II, typename Cx>
        inline auto read_array(T& t, II& i, II e, Cx& cx)
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
        inline auto read_array(T& t, II& i, II e, Cx& cx)
            -> enable_if_t<!std::is_assignable<decltype(*std::begin(t)), decltype(*i)>::value, bool>
        {
            auto const m = bio::peek(i, e);
            switch (m & X::mjr) {
                case X::arr:    return bits::read_array_w_<X>(t, m, i, e, cx);
                default:        return cx|cbor::read_error::array_invalid;
            }
        }

}}}

namespace cxon { namespace cbor { namespace cnt {
    
    template <typename X, typename FI, typename O, typename Cx, typename T = typename std::iterator_traits<FI>::value_type>
        inline bool write_array(O& o, FI f, FI l, Cx& cx) {
            return cbor::bits::write_array_<X>(o, f, l, cx);
        }

}}}

#endif // CXON_CBOR_COMMON_CONTAINER_HXX_
