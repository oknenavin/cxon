// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT


#include "json.node.time.hxx"
#include "object.hxx"

#include "cxon/lib/node.ordered.hxx"

#include <fstream>

////////////////////////////////////////////////////////////////////////////////

using node = cxon::json::ordered_node;

namespace test { namespace kind {

    template <typename T>
        static void time_run(test& t) {
            std::ifstream is(t.source, std::ifstream::binary);
                if (!is) return t.error = "cannot be opened", void();
            std::string const json = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            // size
                t.size = json.size() - 1;
            // read
                std::vector<T> vo;
                t.time.read.push_back(CXON_MEASURE(
                    vo.emplace_back();
                    auto const r = cxon::from_bytes<TIME>(vo.back(), json);
                    if (!r) t.error = format_error(r, json.begin());
                ));
            // write
                auto o = vo.back(); vo.clear();
                std::vector<std::string> vs;
                t.time.write.push_back(CXON_MEASURE(
                    vs.emplace_back();
                    cxon::to_bytes<TIME>(vs.back(), o);
                ));
        }

    using executor = void (*)(test& t);

    static std::map<std::string, executor> executors_ = {
        { "blns.json",              &time_run<blns::object> },
        { "emoji.json",             &time_run<emoji::object> },
        { "apache_builds.json",     &time_run<apache_builds::object> },
        { "canada.json",            &time_run<canada::object> },
#       ifdef CXON_HAS_LIB_STD_OPTIONAL
        { "citm_catalog.json",      &time_run<citm_catalog::object> },
#       endif
        //{ "github_events.json",     &time_run<node> },
        { "gsoc-2018.json",         &time_run<gsoc_2018::object> },
#       ifdef CXON_HAS_LIB_STD_OPTIONAL
        { "instruments.json",       &time_run<instruments::object> },
#       endif
        { "marine_ik.json",         &time_run<marine_ik::object> },
        { "mesh.json",              &time_run<mesh::object> },
        { "mesh.pretty.json",       &time_run<mesh::object> },
        { "numbers.json",           &time_run<numbers::object> },
        { "random.json",            &time_run<random::object> },
#       ifdef CXON_HAS_LIB_STD_OPTIONAL
        { "twitter.json",           &time_run<twitter::object> },
        { "twitterescaped.json",    &time_run<twitter::object> },
#       endif
        { "update-center.json",     &time_run<update_center::object> },
    };

    void time_cxon_type(test& t) {
        auto i = executors_.find(t.source.substr(t.source.rfind('/') + 1));
        i != executors_.end() ?
            i->second(t) :
            (time_cxon_node(t), t.flag = true, void())
        ;
    }

}}
