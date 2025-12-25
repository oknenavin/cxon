// Copyright (c) 2017-2026 oknenavin.
//
//  This file is part of CXON.
//
//  CXON is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  CXON is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License along with CXON. If not, see <https://www.gnu.org/licenses/>. 
//
// SPDX-License-Identifier: AGPL-3.0-or-later

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

    template <typename T = cxon::json::format_traits>
        struct allow_cxx_comments_traits : T {
            static constexpr bool allow_cxx_comments = true;
        };

    template <typename T = cxon::json::format_traits>
        struct allow_bash_comments_traits : T {
            static constexpr bool allow_bash_comments = true;
        };

    template <typename T = cxon::json::format_traits>
        struct allow_trailing_separators_traits : T {
            static constexpr bool allow_trailing_separators = true;
        };

    template <typename T = cxon::json::format_traits>
        struct allow_javascript_nans_traits : T {
            static constexpr bool allow_javascript_nans = true;
        };

}}

#endif // CXON_JSON_TEST_HXX_
