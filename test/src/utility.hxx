// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_TEST_UTILITY_HXX_
#define CXON_TEST_UTILITY_HXX_

#include <utility>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <system_error>

// interface ///////////////////////////////////////////////////////////////////

#ifndef CXON_TIME_RUN_FOR
#   define CXON_TIME_RUN_FOR 250
#endif

namespace test {

    template <typename B>
        static double measure(B block);
#   define CXON_MEASURE(...) ::test::measure([&] { __VA_ARGS__ })

    template <typename R, typename II>
        static std::string format_error(const R& r, II b);

}

// implementation //////////////////////////////////////////////////////////////

namespace test {

    namespace imp {

        template <typename C>
            inline auto median_(const C& c) -> typename C::value_type {
                std::sort(std::begin(c), std::end(c));
                return c.size() % 2 == 0 ? (c[c.size() / 2] + c[c.size() / 2 - 1]) / 2 : c[c.size() / 2];
            }
        template <typename C>
            inline auto median_(C& c) -> typename C::value_type {
                auto d = c;
                    std::sort(std::begin(d), std::end(d));
                return d.size() % 2 == 0 ? (d[d.size() / 2] + d[d.size() / 2 - 1]) / 2 : d[d.size() / 2];
            }
        template <typename C>
            inline auto average_(const C& c) -> typename C::value_type {
                return std::accumulate(c.begin(), c.end(), typename C::value_type {0}) / c.size();
            }
        template <typename C>
            inline auto min_(const C& c) -> typename C::value_type {
                return *std::min_element(c.begin(), c.end());
            }
        template <typename C>
            inline auto max_(const C& c) -> typename C::value_type {
                return *std::max_element(c.begin(), c.end());
            }

    }

    template <typename B>
        static double measure(B block) {
            using clock = std::chrono::steady_clock;
            using ns = std::chrono::nanoseconds;
            std::vector<ns> sl;
            {   ns  rt = ns {0}, rm = ns {std::chrono::milliseconds {CXON_TIME_RUN_FOR}} ;
                do {
                    auto const s = clock::now();
                        block();
                    sl.emplace_back(std::chrono::duration_cast<ns>(clock::now() - s));
                    rt += sl.back();
                }   while (rt < rm || sl.size() < 17);
            }
            auto const md = imp::median_(sl);
            return std::chrono::duration<double, std::milli>(md).count();
        }
#   define CXON_MEASURE(...) ::test::measure([&] { __VA_ARGS__ })

    namespace imp {

        template <typename II>
            inline auto position_(II b, II e)
                -> typename std::enable_if<!std::is_same<typename std::iterator_traits<II>::iterator_category, std::input_iterator_tag>::value, std::pair<std::size_t, std::size_t>>::type
            {
                std::size_t l = 1, c = 1;
                for ( ; b != e; ++b, ++c)
                    if (*b == '\n')
                        ++l, c = 0;
                return std::make_pair(l, c);
            }
        template <typename II>
            inline auto position_(II b, II e)
                -> typename std::enable_if< std::is_same<typename std::iterator_traits<II>::iterator_category, std::input_iterator_tag>::value, std::pair<std::size_t, std::size_t>>::type
            {
                return std::make_pair(1, std::distance(b, e));
            }

    }

    template <typename R, typename II>
        static std::string format_error(const R& r, II b) {
            auto const p = imp::position_(b, r.end);
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
