// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "json.node.hxx"

#ifdef CXON_TIME_NLOHMANN

#include "nlohmann/json.hpp"

#include <fstream>

////////////////////////////////////////////////////////////////////////////////

namespace test { namespace kind {

    void time_nlohmannjson(test& t) {
        std::ifstream is(t.source, std::ifstream::binary);
            if (!is) return t.error = "cannot be opened", void();
        std::string const json = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
        // size
            t.size = json.size() - 1;
        // read
            std::vector<nlohmann::json> vo;
            t.time.read.push_back(CXON_MEASURE(
                vo.emplace_back();
                vo.back() = nlohmann::json::parse(json);
            ));
        // write
            auto o = vo.back(); vo.clear();
            std::vector<std::string> vs;
            t.time.write.push_back(CXON_MEASURE(
                vs.emplace_back();
                vs.back() = o.dump();
            ));
    }

}}

#endif // CXON_TIME_NLOHMANN
