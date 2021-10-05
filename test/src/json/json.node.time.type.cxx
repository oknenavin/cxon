// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#include "json.node.hxx"

#include "cxon/json.hxx"
#include "cxon/lib/node.ordered.hxx"

#include "object.hxx"

#include <fstream>

////////////////////////////////////////////////////////////////////////////////

using node = cxon::json::ordered_node;

namespace test { namespace benchmark {

    template <typename T>
        static void cxon_time_run(test& t) {
            std::ifstream is(t.source, std::ifstream::binary);
                if (!is) return t.error = "cannot be opened", void();
            std::string const json = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            // size
                t.size = json.size() - 1;
            // read
                std::vector<T> vo;
                t.time.read.push_back(CXON_MEASURE(
                    vo.emplace_back();
                    auto const r = cxon::from_bytes(vo.back(), json);
                    if (!r) t.error = format_error(r, json.begin());
                ));
            // write
                auto o = vo.back(); vo.clear();
                std::vector<std::string> vs;
                t.time.write.push_back(CXON_MEASURE(
                    vs.emplace_back();
                    cxon::to_bytes(vs.back(), o);
                ));
        }

    using executor = void (*)(test& t);

    static std::map<std::string, executor> executors_ = {
        { "blns.json",              &cxon_time_run<blns::object> },
        { "emoji.json",             &cxon_time_run<emoji::object> },
        { "apache_builds.json",     &cxon_time_run<apache_builds::object> },
        { "canada.json",            &cxon_time_run<canada::object> },
#       ifdef CXON_HAS_LIB_STD_OPTIONAL
        { "citm_catalog.json",      &cxon_time_run<citm_catalog::object> },
#       endif
        //{ "github_events.json",     &cxon_time_run<node> },
        { "gsoc-2018.json",         &cxon_time_run<gsoc_2018::object> },
#       ifdef CXON_HAS_LIB_STD_OPTIONAL
        { "instruments.json",       &cxon_time_run<instruments::object> },
#       endif
        { "marine_ik.json",         &cxon_time_run<marine_ik::object> },
        { "mesh.json",              &cxon_time_run<mesh::object> },
        { "mesh.pretty.json",       &cxon_time_run<mesh::object> },
        { "numbers.json",           &cxon_time_run<numbers::object> },
        { "random.json",            &cxon_time_run<random::object> },
        //{ "twitter.json",           &cxon_time_run<node> },
        //{ "twitterescaped.json",    &cxon_time_run<node> },
        { "update-center.json",     &cxon_time_run<update_center::object> },
    };

    void cxon_type_time_run(test& t) {
        auto i = benchmark::executors_.find(t.source.substr(t.source.rfind('/') + 1));
        i != benchmark::executors_.end() ?
            i->second(t) :
            (benchmark::cxon_node_time_run(t), t.source.push_back('*'))
        ;
    }

}}
