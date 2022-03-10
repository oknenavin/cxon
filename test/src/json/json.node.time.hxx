// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_TEST_JSON_NODE_TIME_HXX_
#define CXON_TEST_JSON_NODE_TIME_HXX_

#include "cxon/json.hxx"
#include "json.node.hxx"

struct time_traits : cxon::json::format_traits {};
using TIME = cxon::JSON<time_traits>;

#endif // CXON_TEST_JSON_NODE_TIME_HXX_
