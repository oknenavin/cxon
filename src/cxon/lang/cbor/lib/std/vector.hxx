// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_VECTOR_HXX_
#define CXON_CBOR_LIB_STD_VECTOR_HXX_

#include "cxon/lang/cbor/compound.hxx"

namespace cxon { namespace cbor { namespace cnt {
    
    template <typename X, typename T>
        inline bool reserve(T& t, size_t n);

    template <typename X, typename T, typename II, typename Cx>
        inline bool append(T& t, II& i, II e, Cx& cx);
    template <typename X, typename T, typename II, typename Cx>
        inline bool append(T& t, size_t n, II& i, II e, Cx& cx);

}}}

namespace cxon { namespace cbor { namespace cnt {

    namespace imp {

        template <typename X, typename T>
            struct reserve {
                template <typename U = T>
                    static auto make_(option<1>, U& t, size_t n)
                        -> decltype(t.reserve(n), bool())
                    {
                        return t.reserve(n), true;
                    }
                static constexpr bool make_(option<0>, T&, size_t) {
                    return true;
                }
                static bool make(T& t, size_t n) {
                    return make_(option<1>(), t, n);
                }
            };

    }
    template <typename X, typename T>
        inline bool reserve(T& t, size_t n) {
            return imp::reserve<X, T>::make(t, n);
        }

    namespace imp {

        template <typename X, typename T>
            struct append {
                template <typename II, typename Cx>
                    static auto make_(option<2>, T& t, II& i, II e, Cx& cx)
                        -> decltype(std::is_same<decltype(t.emplace_back()), typename T::reference>::value, bool())
                    {
                        return read_value<X>(t.emplace_back(), i, e, cx);
                    }
                template <typename II, typename Cx>
                    static auto make_(option<1>, T& t, II& i, II e, Cx& cx)
                        -> decltype(t.emplace_back(), t.back(), bool())
                    {
                        return t.emplace_back(), read_value<X>(t.back(), i, e, cx);
                    }
                template <typename II, typename Cx>
                    static auto make_(option<0>, T& t, II& i, II e, Cx& cx)
                        -> decltype(t.push_back(std::declval<typename T::value_type>()), t.back(), bool())
                    {
                        return t.push_back({}), read_value<X>(t.back(), i, e, cx);
                    }

                template <typename II, typename Cx>
                    static bool make(T& t, II& i, II e, Cx& cx) {
                        return make_(option<2>(), t, i, e, cx);
                    }
            };

    }
    template <typename X, typename T, typename II, typename Cx>
        inline bool append(T& t, II& i, II e, Cx& cx) {
            return imp::append<X, T>::make(t, i, e, cx);
        }

    namespace imp {

        template <typename X, typename T>
            struct append_n {
                template <typename II, typename Cx>
                    static bool make(T& t, size_t n, II& i, II e, Cx& cx) {
                        for ( ; n && cnt::append<X>(t, i, e, cx); --n)
                            ;
                        return n == 0;
                    }
            };

    }
    template <typename X, typename T, typename II, typename Cx>
        inline bool append(T& t, size_t n, II& i, II e, Cx& cx) {
            return imp::append_n<X, T>::make(t, n, i, e, cx);
        }

}}}

namespace cxon { namespace cbor { namespace cnt {

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array_w_fix_(T& t, II& i, II e, Cx& cx) {
            size_t n;
            return  bits::read_size_<X>(n, i, e, cx) &&
                    reserve<X>(t, n) &&
                    append<X>(t, n, i, e, cx)
            ;
        }
    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array_w_var_(T& t, II& i, II e, Cx& cx) {
            II const o = i;
                for (bio::get(i, e); bio::peek(i, e, 0) != X::brk; )
                    if (!append<X>(t, i, e, cx))
                        return false;
            return  (bio::get(i, e, 0) == X::brk) ||
                    (bio::rewind(i, o), cx|cbor::read_error::size_invalid)
            ;
        }
    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array_w_(T& t, bio::byte m, II& i, II e, Cx& cx) {
            switch (m & X::mnr) {
                case 0x1C: case 0x1D: case 0x1E:    return cx|cbor::read_error::size_invalid;
                case 0x1F:                          return read_array_w_var_<X>(t, i, e, cx);
                default:                            return read_array_w_fix_<X>(t, i, e, cx);
            }
        }

    template <typename X, typename T, typename II, typename Cx>
        inline bool read_array_(T& t, II& i, II e, Cx& cx) {
            auto const m = bio::peek(i, e);
            switch (m & X::mjr) {
                case X::arr:    return read_array_w_<X>(t, m, i, e, cx);
                default:        return cx|cbor::read_error::array_invalid;
            }
        }

}}}

namespace cxon {

    template <typename X, typename T, typename ...R>
        struct read<CBOR<X>, std::vector<T, R...>> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(std::vector<T, R...>& t, II& i, II e, Cx& cx) {
                    return cbor::cnt::read_array_<J>(t, i, e, cx);
                }
        };

    template <typename X, typename T, typename ...R>
        struct write<CBOR<X>, std::vector<T, R...>> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const std::vector<T, R...>& t, Cx& cx) {
                    return cbor::bits::write_array_<J>(o, t.begin(), t.end(), cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_VECTOR_HXX_
