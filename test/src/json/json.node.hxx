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
    };

    using cases = std::vector<test>;

}

namespace self {
    unsigned test();
}

namespace test { namespace benchmark {

    void cxon_node_time_run(test&);
    void cxon_type_time_run(test&);
    void boostjson_time_run(test&);
    void rapidjson_time_run(test&);
    void nlohmannjson_time_run(test&);

}}

#endif // CXON_TEST_JSON_NODE_HXX_
