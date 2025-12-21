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

#ifndef CXON_TEST_JSON_NODE_TIME_HXX_
#define CXON_TEST_JSON_NODE_TIME_HXX_

#include "cxon/json.hxx"
#include "json.node.hxx"

struct time_traits : cxon::json::format_traits {
    static constexpr bool validate_string_encoding  = false;
    static constexpr bool validate_string_escapes   = false;
};
//using TIME = cxon::JSON<time_traits>;
using TIME = cxon::JSON<>;

#endif // CXON_TEST_JSON_NODE_TIME_HXX_
