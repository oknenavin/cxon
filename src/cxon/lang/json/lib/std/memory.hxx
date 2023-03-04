// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_LIB_STD_MEMORY_HXX_
#define CXON_JSON_LIB_STD_MEMORY_HXX_

#include "cxon/lang/common/allocator.hxx"

namespace cxon {

    template <typename X, typename T, typename D>
        struct read<JSON<X>, std::unique_ptr<T, D>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::unique_ptr<T, D>& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<Y>(i, e, cx))
                        return false;
                    if (cio::peek(i, e) == *Y::id::nil) { // TODO: needs buffering
                        II const o = i;
                        return  (cio::consume<Y>(Y::id::nil, i, e) && (t.reset(), true)) ||
                                (cio::rewind(i, o), cx/json::read_error::unexpected)
                        ;
                    }
                    auto al = alc::make_context_allocator<T>(cx);
                    T *const n = al.create();
                        if (!read_value<Y>(*n, i, e, cx))
                            return al.release(n), false;
                    return t.reset(n), true;
                }
        };

    template <typename X, typename T, typename D>
        struct write<JSON<X>, std::unique_ptr<T, D>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::unique_ptr<T, D>& t, Cx& cx) {
                    return t ?
                        write_value<Y>(o, *t.get(), cx) :
                        cio::poke<Y>(o, X::id::nil, cx)
                    ;
                }
        };

    template <typename X, typename T>
        struct read<JSON<X>, std::shared_ptr<T>> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(std::shared_ptr<T>& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<Y>(i, e, cx))
                        return false;
                    if (cio::peek(i, e) == *Y::id::nil) { // TODO: needs buffering
                        II const o = i;
                        return  (cio::consume<Y>(Y::id::nil, i, e) && (t.reset(), true)) ||
                                (cio::rewind(i, o), cx/json::read_error::unexpected)
                        ;
                    }
                    auto al = alc::make_context_allocator<T>(cx);
                    T *const n = al.create();
                        if (!read_value<Y>(*n, i, e, cx))
                            return al.release(n), false;
                    return t.reset(n, [al](T* p) mutable { al.release(p); }), true;
                }
        };

    template <typename X, typename T>
        struct write<JSON<X>, std::shared_ptr<T>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::shared_ptr<T>& t, Cx& cx) {
                    return t ?
                        write_value<Y>(o, *t.get(), cx) :
                        cio::poke<Y>(o, X::id::nil, cx)
                    ;
                }
        };

}

#endif // CXON_JSON_LIB_STD_MEMORY_HXX_
