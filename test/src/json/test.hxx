// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_HXX_
#define CXON_JSON_TEST_HXX_

#include "cxon/json.hxx"
#include "../test.hxx"

namespace cxon { namespace test {

    template <typename T = json::format_traits>
        struct input_iterator_traits : T {
            static constexpr bool force_input_iterator = true;
        };

    template <typename T = json::format_traits>
        struct unquoted_keys_traits : T {
            static constexpr bool quote_unquoted_keys = false;
        };

    template <typename T = cxon::json::format_traits>
        struct unquoted_quoted_keys_traits : T {
            static constexpr bool unquote_quoted_keys = true;
        };

}}

#endif // CXON_JSON_TEST_HXX_
