// Copyright (c) 2017-2020 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_CHRONO_HXX_
#define CXON_CHRONO_HXX_

#include <chrono>

namespace cxon {

    template <typename X, typename R, typename P>
        struct read<X, std::chrono::duration<R, P>> {
            template <typename II, typename Cx>
                static bool value(std::chrono::duration<R, P>& t, II& i, II e, Cx& cx) {
                    R c{};
                    return read_value<X>(c, i, e, cx) && (t = std::chrono::duration<R, P>(c), true);
                }
        };

    template <typename X, typename R, typename P>
        struct write<X, std::chrono::duration<R, P>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::chrono::duration<R, P>& t, Cx& cx) {
                    return write_value<X>(o, t.count(), cx);
                }
        };

    template <typename X, typename C, typename D>
        struct read<X, std::chrono::time_point<C, D>> {
            template <typename II, typename Cx>
                static bool value(std::chrono::time_point<C, D>& t, II& i, II e, Cx& cx) {
                    D d{};
                    return read_value<X>(d, i, e, cx) && (t = std::chrono::time_point<C, D>(d), true);
                }
        };

    template <typename X, typename C, typename D>
        struct write<X, std::chrono::time_point<C, D>> {
            template <typename O, typename Cx>
                static bool value(O& o, const std::chrono::time_point<C, D>& t, Cx& cx) {
                    return write_value<X>(o, t.time_since_epoch(), cx);
                }
        };

}   // cxon

#endif // CXON_CHRONO_HXX_
