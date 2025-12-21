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

#ifndef CXON_TEST_JSON_NODE_HXX_
#define CXON_TEST_JSON_NODE_HXX_

#include "../utility.hxx"

#include <string>
#include <vector>

namespace test {

    struct time {
        std::vector<double> read;
        std::vector<double> write;
    };

    struct test {
        std::string source;
        std::size_t size;
        struct time time;
        std::string error;
        bool flag;
    };

    using cases = std::vector<test>;

}

namespace test { namespace kind {

    int self();

    int pass(cases& cases);
    int fail(cases& cases);
    int diff_node(cases& cases);
    int diff_native(cases& cases);

}}

namespace test { namespace kind {

    template <typename TIME_CXON>
        void time(cases& cases, TIME_CXON time_cxon);

    void time_cxon_node(test&);
    void time_cxon_native(test&);
    void time_boostjson(test&);
    void time_rapidjson(test&);
    void time_nlohmannjson(test&);

    void time_print(const std::vector<std::string>& names, const cases& c);

}}

#endif // CXON_TEST_JSON_NODE_HXX_
