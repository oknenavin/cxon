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

    template <typename II>
        inline auto position(II b, II e)
            -> typename std::enable_if<!std::is_same<typename std::iterator_traits<II>::iterator_category, std::input_iterator_tag>::value, std::pair<std::size_t, std::size_t>>::type
        {
            std::size_t l = 1, c = 1;
            for ( ; b != e; ++b, ++c)
                if (*b == '\n')
                    ++l, c = 0;
            return std::make_pair(l, c);
        }
    template <typename II>
        inline auto position(II b, II e)
            -> typename std::enable_if< std::is_same<typename std::iterator_traits<II>::iterator_category, std::input_iterator_tag>::value, std::pair<std::size_t, std::size_t>>::type
        {
            return std::make_pair(1, std::distance(b, e));
        }

    template <typename R, typename I>
        static std::string format_error(const R& r, I b) {
            auto const p = position(b, r.end);
            return std::string()
                .append(r.ec.category().name())
                .append(":(")
                .append(std::to_string(p.first)).append(",").append(std::to_string(p.second))
                .append("): ")
                .append(r.ec.message())
            ;
        }

}

#endif // CXON_TEST_UTILITY_HXX_
