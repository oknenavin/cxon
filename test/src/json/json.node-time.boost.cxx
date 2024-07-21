// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifdef CXON_TIME_BOOST_JSON

#include "json.node.hxx"
#include "boost/json.hpp"
#include <fstream>

////////////////////////////////////////////////////////////////////////////////

namespace test { namespace kind {

    void time_boostjson(test& t) {
        std::ifstream is(t.source, std::ifstream::binary);
            if (!is) return t.error = "cannot be opened", void();
        std::string const json = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
        // size
            t.size = json.size() - 1;
        // read
            std::vector<boost::json::value> vo;
                boost::json::parse_options opt;
                    opt.numbers = boost::json::number_precision::precise;
            t.time.read.push_back(CXON_MEASURE(
                vo.emplace_back();
                boost::system::error_code ec;
                    vo.back() = boost::json::parse(json, ec, {}, opt);
                if (ec) t.error = std::string("Boost/JSON: ") + ec.category().name() + ": " + ec.message();
            ));
        // write
            auto o = vo.back(); vo.clear();
            std::vector<std::string> vs;
            t.time.write.push_back(CXON_MEASURE(
                vs.emplace_back();
                vs.back() = boost::json::serialize(o);
            ));
    }

}}

#endif // CXON_TIME_BOOST_JSON
