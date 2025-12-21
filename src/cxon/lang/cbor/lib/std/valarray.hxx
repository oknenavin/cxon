// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef CXON_CBOR_LIB_STD_VALARRAY_HXX_
#define CXON_CBOR_LIB_STD_VALARRAY_HXX_

#include "cxon/lang/cbor/common/container.hxx"

namespace cxon {

    namespace cbor { namespace imp {

        template <typename T>
            struct valarray_container_ {
                using value_type = T;
                using reference = value_type&;
                using iterator = decltype(std::begin(std::declval<std::valarray<T>&>()));

                valarray_container_(std::valarray<T>& a) : a_(a), i_() {}

                std::size_t size() const     { return a_.size(); }
                std::size_t max_size() const { return std::numeric_limits<std::size_t>::max(); }

                iterator begin()    { return std::begin(a_); }
                iterator end()      { return std::end(a_); }

                reference emplace_back()            { return grow(), a_[i_++]; }
                //void push_back(const value_type& t) { grow(), a_[i_++] = t; }
                void push_back(value_type&& t)      { grow(), a_[i_++] = std::move(t); }

                void reserve(std::size_t n)         { n > a_.size() ? grow(n) : void(); }

                private:
                    void grow()                     { i_ == a_.size() ? grow(a_.size() + 1) : void(); }
                    
                    void grow(std::size_t n) {
                        CXON_ASSERT(n > a_.size(), "unexpected");
                        std::valarray<T> a(n);
#                           ifdef _MSC_VER // std::begin/std::end broken for empty std::valarray
                                for (std::size_t i = 0, is = a_.size(); i != is; ++i)
                                    a[i] = a_[i];
#                           else
                                std::copy(std::begin(a_), std::end(a_), std::begin(a));
#                           endif
                        a_.swap(a);
                    }

                private:
                    std::valarray<T>& a_;
                    std::size_t i_;
            };

    }}

    template <typename X, typename T>
        struct read<CBOR<X>, std::valarray<T>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::valarray<T>& t, II& i, II e, Cx& cx) {
                    auto c = cbor::imp::valarray_container_<T> {t};
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
