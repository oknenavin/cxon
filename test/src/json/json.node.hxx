// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

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

    bool parse_cl(int argc, char *argv[], cases& pass, cases& fail, cases& diff, cases& time_node, cases& time_native);

}

namespace test { namespace kind {

    int self();

    int pass(cases& cases);
    int fail(cases& cases);
    int diff(cases& cases);

}}

namespace test { namespace kind {

    template <typename TIME_CXON>
        void time(cases& cases, TIME_CXON time_cxon);

    void time_cxon_node(test&);
    void time_cxon_type(test&);
    void time_boostjson(test&);
    void time_rapidjson(test&);
    void time_nlohmannjson(test&);

    void time_print(const std::vector<std::string>& names, const cases& c);

}}

#endif // CXON_TEST_JSON_NODE_HXX_
