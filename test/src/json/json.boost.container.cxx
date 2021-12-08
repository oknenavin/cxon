// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "test.hxx"

#include "cxon/lib/boost/container/stable_vector.hxx"
#include "cxon/lib/boost/container/small_vector.hxx"
#include "cxon/lib/boost/container/static_vector.hxx"

////////////////////////////////////////////////////////////////////////////////

TEST_BEG(stable_vector, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::stable_vector<int>
        R_TEST((stable_vector<int>{}), "[]");
        W_TEST("[]", (stable_vector<int>{}));
        R_TEST((stable_vector<int>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (stable_vector<int>({1, 2, 3})));
TEST_END()


TEST_BEG(small_vector, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::small_vector<int>
        R_TEST((small_vector<int, 8>{}), "[]");
        W_TEST("[]", (small_vector<int, 8>{}));
        R_TEST((small_vector<int, 2>({1, 2, 3})), "[1, 2, 3]");
        W_TEST("[1,2,3]", (small_vector<int, 2>({1, 2, 3})));
TEST_END()


TEST_BEG(static_vector, cxon::JSON<>, "/boost")
    using namespace boost::container;
    // boost::container::static_vector<int, 0>
        R_TEST((static_vector<int, 0>{}), "[]");
        W_TEST("[]", (static_vector<int, 0>{}));
        R_TEST((static_vector<int, 0>{}), "", json::read_error::unexpected, 0);
        R_TEST((static_vector<int, 0>{}), "]", json::read_error::unexpected, 0);
        R_TEST((static_vector<int, 0>{}), "[", json::read_error::unexpected, 1);
    // boost::container::static_vector<int, 3>
        R_TEST((static_vector<int, 3>{{1, 2, 3}}), "[1, 2, 3]");
        W_TEST("[1,2,3]", (static_vector<int, 3>{{1, 2, 3}}));
        R_TEST((static_vector<int, 4>{{1, 2, 3}}), "[1, 2, 3]");
        W_TEST("[1,2,3,4]", (static_vector<int, 4>{{1, 2, 3, 4}}));
        R_TEST((static_vector<int, 2>{}), "[1, 2, 3]", json::read_error::overflow, 0);
TEST_END()
