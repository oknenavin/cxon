// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_VALARRAY_HXX_
#define CXON_CBOR_LIB_STD_VALARRAY_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon {

    namespace cbor { namespace imp {

        template <typename T>
            struct valarray_container {
                using value_type = T;
                using reference = value_type&;
                using iterator = decltype(std::begin(std::declval<std::valarray<T>&>()));

                valarray_container(std::valarray<T>& a) : a_(a), i_() {}

                size_t size() const     { return a_.size(); }
                size_t max_size() const { return std::numeric_limits<size_t>::max(); }

                iterator begin()    { return std::begin(a_); }
                iterator end()      { return std::end(a_); }

                reference emplace_back()            { return grow(), a_[i_++]; }
                void push_back(const value_type& t) { grow(), a_[i_++] = t; }
                void push_back(value_type&& t)      { grow(), a_[i_++] = std::move(t); }

                void reserve(size_t n)              { n > a_.size() ? grow(n) : void(); }

                private:
                    void grow()                     { i_ == a_.size() ? grow(a_.size() + 1) : void(); }
                    
                    void grow(size_t n) {
                        CXON_ASSERT(n > a_.size(), "unexpected");
                        std::valarray<T> a(n);
#                           ifdef _MSC_VER // std::begin/std::end broken for empty std::valarray
                                for (size_t i = 0, is = a_.size(); i != is; ++i)
                                    a[i] = a_[i];
#                           else
                                std::copy(std::begin(a_), std::end(a_), std::begin(a));
#                           endif
                        a_.swap(a);
                    }

                private:
                    std::valarray<T>& a_;
                    size_t i_;
            };

    }}

    template <typename X, typename T>
        struct read<CBOR<X>, std::valarray<T>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::valarray<T>& t, II& i, II e, Cx& cx) {
                    auto c = cbor::imp::valarray_container<T> {t};
                    return cbor::cnt::read_array<Y>(c, i, e, cx);
                }
        };

    template <typename X, typename T>
        struct write<CBOR<X>, std::valarray<T>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::valarray<T>& t, Cx& cx) {
                    return (t.size() && cbor::cnt::write_array<Y>(o, std::begin(t), std::end(t), cx)) ||
                           (cbor::cnt::write_size<Y>(o, Y::arr, 0, cx))
                    ;
                }
        };

}

#endif // CXON_CBOR_LIB_STD_VALARRAY_HXX_
