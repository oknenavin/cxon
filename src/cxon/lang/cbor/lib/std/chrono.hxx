// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CBOR_LIB_STD_CHRONO_HXX_
#define CXON_CBOR_LIB_STD_CHRONO_HXX_

namespace cxon {

    template <typename X, typename R, typename P>
        struct read<CBOR<X>, std::chrono::duration<R, P>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::chrono::duration<R, P>& t, II& i, II e, Cx& cx) {
                    R c{};
                    return read_value<Y>(c, i, e, cx) && (t = std::chrono::duration<R, P>(c), true);
                }
        };

    template <typename X, typename R, typename P>
        struct write<CBOR<X>, std::chrono::duration<R, P>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::chrono::duration<R, P>& t, Cx& cx) {
                    return write_value<Y>(o, t.count(), cx);
                }
        };

    template <typename X, typename C, typename D>
        struct read<CBOR<X>, std::chrono::time_point<C, D>> {
            template <typename II, typename Cx, typename Y = CBOR<X>>
                static bool value(std::chrono::time_point<C, D>& t, II& i, II e, Cx& cx) {
                    D d{};
                    return read_value<Y>(d, i, e, cx) && (t = std::chrono::time_point<C, D>(d), true);
                }
        };

    template <typename X, typename C, typename D>
        struct write<CBOR<X>, std::chrono::time_point<C, D>> {
            template <typename O, typename Cx, typename Y = CBOR<X>>
                static bool value(O& o, const std::chrono::time_point<C, D>& t, Cx& cx) {
                    return write_value<Y>(o, t.time_since_epoch(), cx);
                }
        };

}

#endif // CXON_CBOR_LIB_STD_CHRONO_HXX_
