// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_MEMORY_HXX_
#define CXON_CBOR_LIB_STD_MEMORY_HXX_

#include "cxon/lang/common/allocator.hxx"

namespace cxon {

    template <typename X, typename T, typename D>
        struct read<CBOR<X>, std::unique_ptr<T, D>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::unique_ptr<T, D>& t, II& i, II e, Cx& cx) {
                        T* p;
                    return read_value<Y>(p, i, e, cx) && (t.reset(p), true);
                }
        };

    template <typename X, typename T, typename D>
        struct write<CBOR<X>, std::unique_ptr<T, D>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::unique_ptr<T, D>& t, Cx& cx) {
                    return write_value<Y>(o, t.get(), cx);
                }
        };

    template <typename X, typename T>
        struct read<CBOR<X>, std::shared_ptr<T>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::shared_ptr<T>& t, II& i, II e, Cx& cx) {
                    auto al = alc::make_context_allocator<T>(cx);
                        T* p;
                    return read_value<Y>(p, i, e, cx) && (t.reset(p, [al](T* p) mutable { al.release(p); }), true);
                }
        };

    template <typename X, typename T>
        struct write<CBOR<X>, std::shared_ptr<T>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::shared_ptr<T>& t, Cx& cx) {
                    return write_value<Y>(o, t.get(), cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_MEMORY_HXX_