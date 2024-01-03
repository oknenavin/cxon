// Copyright (c) 2017-2024 oknenavin.
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
        using vector = std::vector<fixture>;
        std::string label;
        std::vector<std::string> group;
        std::map<std::string, struct fix> fixup;
    };

}

namespace test { namespace kind {

    int self();

    int vector(const fixture::vector& fs);
    int roundtrip(const fixture::vector& fs);
    int time(const fixture::vector& fs);

}}

#endif // CXON_TEST_CBOR_NODE_HXX_
