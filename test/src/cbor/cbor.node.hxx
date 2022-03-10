// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_TEST_CBOR_NODE_HXX_
#define CXON_TEST_CBOR_NODE_HXX_

#include "../utility.hxx"

#include <string>
#include <vector>
#include <map>

namespace test {
    
    struct fix {
        std::string act;
        std::string data;
        std::string fail;
    };
    struct fixture {
        std::string type;
        std::vector<std::string> in;
        std::map<std::string, struct fix> fix;
    };

    struct result {
        int err = 0;
        int all = 0;
    };

}

namespace test { namespace kind {

    int self();

    result vector(const fixture& fixture);
    result roundtrip(const fixture& fixture);
    result time(const fixture& fixture);

}}

#endif // CXON_TEST_CBOR_NODE_HXX_
