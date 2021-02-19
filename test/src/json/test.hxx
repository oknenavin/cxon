// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_HXX_
#define CXON_JSON_TEST_HXX_

#include "cxon/json.hxx"
#include "../test.hxx"

namespace cxon { namespace test {

    struct input_iterator_traits : json::format_traits {
        static constexpr bool force_input_iterator = true;
    };

}}

#endif // CXON_JSON_TEST_HXX_
