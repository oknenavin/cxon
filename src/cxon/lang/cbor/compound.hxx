// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_COMPOUND_HXX_
#define CXON_CBOR_COMPOUND_HXX_

#include "cxon/lang/common/bio/bio.hxx"

namespace cxon { // pointer

    template <typename X, typename T>
        struct read<CBOR<X>, T*> {
            template <typename II, typename Cx, typename J = CBOR<X>>
                static bool value(T*& t, II& i, II e, Cx& cx) {
                    auto const b = bio::peak(i, e);
                    if (b == X::nil)
                        return bio::get(i, e), t = nullptr, true;
                    auto ax = bio::allocator::value(cx.px, std::allocator<T>());
                    typename std::allocator_traits<decltype(ax)>::template rebind_alloc<T> at;
                        using al = std::allocator_traits<decltype(at)>;
                    T *const n = al::allocate(at, 1); al::construct(at, n);
                        if (!read_value<J>(*n, i, e, cx))
                            return al::destroy(at, n), al::deallocate(at, n, 1), false;
                    return t = n, true;
                }
        };

    template <typename X, typename T>
        struct write<CBOR<X>, T*> {
            template <typename O, typename Cx, typename J = CBOR<X>>
                static bool value(O& o, const T* t, Cx& cx) {
                    return t ? write_value<J>(o, *t, cx) : bio::poke<J>(o, X::nil, cx);
                }
        };

}

namespace cxon { // array

}

namespace cxon { // character pointer & array

}

#endif // CXON_CBOR_COMPOUND_HXX_
