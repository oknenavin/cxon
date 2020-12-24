// Copyright (c) 2017-2020 oknenavin
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_ARRAY_HXX_
#define CXON_CBOR_LIB_STD_ARRAY_HXX_

#include "cxon/lang/cbor/compound.hxx"
#include "cxon/lang/cbor/common/container.hxx"

namespace cxon {

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

                reference emplace_back() { return e_ != l_ ? *e_++ : *e_; }

                private:
                    FI f_, l_, e_;
            };

    }

    template <typename X, typename T, size_t N>
        struct read<CBOR<X>, std::array<T, N>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::array<T, N>& t, II& i, II e, Cx& cx) {
                    auto c = bits::range_container<typename std::array<T, N>::iterator>(t.begin(), t.end());
                    return cbor::cnt::read_array<Y>(c, i, e, cx);
                    //return cbor::bits::read_array_<J>(t.begin(), t.end(), i, e, cx);
                }
        };
    template <typename X, typename T>
        struct read<CBOR<X>, std::array<T, 0>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::array<T, 0>& t, II& i, II e, Cx& cx) {
                    auto c = bits::range_container<typename std::array<T, 0>::iterator>(t.begin(), t.end());
                    return cbor::cnt::read_array<Y>(c, i, e, cx);
                    //return cbor::bits::read_array_<Y>(t.begin(), t.end(), i, e, cx);
                }
        };

    template <typename X, typename T, size_t N>
        struct write<CBOR<X>, std::array<T, N>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::array<T, N>& t, Cx& cx) {
                    return cbor::cnt::write_array<Y>(o, t.begin(), t.end(), cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_ARRAY_HXX_
