// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_TEST_JSON_NODE_TIME_HXX_
#define CXON_TEST_JSON_NODE_TIME_HXX_

#include "cxon/json.hxx"
#include "json.node.hxx"

struct time_traits : cxon::json::format_traits {};
using TIME = cxon::JSON<time_traits>;

#ifdef CXON_TIME_FAST_FLOAT

#   include "fast_float/fast_float.h"

    namespace cxon {

        namespace imp {

            template <typename X, typename T, typename II, typename Cx>
                inline auto read_value_(T& t, II& i, II e, Cx& cx)
                    -> enable_if_t<is_same_format<X, JSON>::value && std::is_floating_point<T>::value, bool>
                {
                    cio::consume<TIME>(i, e);
                    auto const r = fast_float::from_chars(i, e, t);
                    return (r.ec == std::errc() && (i = r.ptr, true)) || cx/json::read_error::floating_point_invalid;
                }

        }
        template <> struct read<TIME, float> {
            template <typename II, typename Cx>
                static bool value(float& t, II& i, II e, Cx& cx) {
                    return imp::read_value_<TIME>(t, i, e, cx);
                }
        };
        template <> struct read<TIME, double> {
            template <typename II, typename Cx>
                static bool value(double& t, II& i, II e, Cx& cx) {
                    return imp::read_value_<TIME>(t, i, e, cx);
                }
        };

    }

#endif

#endif // CXON_TEST_JSON_NODE_TIME_HXX_