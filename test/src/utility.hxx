// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_TEST_UTILITY_HXX_
#define CXON_TEST_UTILITY_HXX_

#include <utility>
#include <limits>
#include <chrono>
#include <string>
#include <system_error>

////////////////////////////////////////////////////////////////////////////////

#ifndef CXON_TIME_RUN_FOR
#   define CXON_TIME_RUN_FOR 250
#endif

namespace test {

    template <typename B>
        static double measure(B block) {
            using clock = std::chrono::steady_clock;
            using std::chrono::duration_cast;
            using std::chrono::nanoseconds;
            double dur = 0;
            double t = std::numeric_limits<double>::max();
                do {
                    auto const s = clock::now();
                    block();
                    auto const n = duration_cast<nanoseconds>(clock::now() - s).count() / 1e6;
                    dur += n;
                    t = std::min(t, n);
                }   while (dur < CXON_TIME_RUN_FOR);
            return t;
        }
#   define CXON_MEASURE(...) ::test::measure([&] { __VA_ARGS__ })

    template <typename R, typename I>
        static std::string format_error(const R& r, I b) {
            return std::string()
                .append(r.ec.category().name())
                .append(":")
                .append(std::to_string(std::distance(b, r.end)))
                .append(": ")
                .append(r.ec.message())
            ;
        }

}

#endif // CXON_TEST_UTILITY_HXX_
