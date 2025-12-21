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

#ifndef CXON_JSON_TEST_NATIVE_APACHE_BUILDS_HXX_
#define CXON_JSON_TEST_NATIVE_APACHE_BUILDS_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"


namespace test { namespace apache_builds {

    struct job {
        std::string name;
        std::string url;
        std::string color;

        CXON_JSON_CLS_MEMBER(job, CXON_JSON_CLS_FIELD_ASIS(name), CXON_JSON_CLS_FIELD_ASIS(url), CXON_JSON_CLS_FIELD_ASIS(color))
    };

    struct view {
        std::string name;
        std::string url;

        CXON_JSON_CLS_MEMBER(view, CXON_JSON_CLS_FIELD_ASIS(name), CXON_JSON_CLS_FIELD_ASIS(url))
    };

    struct object {
        std::vector<std::map<std::string, std::string>> assignedLabels;
        std::string mode;
        std::string nodeDescription;
        std::string nodeName;
        unsigned numExecutors;
        std::string description;
        std::vector<job> jobs;
        std::map<std::string, std::string> overallLoad;
        std::map<std::string, std::string> primaryView;
        bool quietingDown;
        unsigned slaveAgentPort;
        std::map<std::string, std::string> unlabeledLoad;
        bool useCrumbs;
        bool useSecurity;
        std::vector<view> views;

        CXON_JSON_CLS_MEMBER(object,
            CXON_JSON_CLS_FIELD_ASIS(assignedLabels),
            CXON_JSON_CLS_FIELD_ASIS(mode),
            CXON_JSON_CLS_FIELD_ASIS(nodeDescription),
            CXON_JSON_CLS_FIELD_ASIS(nodeName),
            CXON_JSON_CLS_FIELD_ASIS(numExecutors),
            CXON_JSON_CLS_FIELD_ASIS(description),
            CXON_JSON_CLS_FIELD_ASIS(jobs),
            CXON_JSON_CLS_FIELD_ASIS(overallLoad),
            CXON_JSON_CLS_FIELD_ASIS(primaryView),
            CXON_JSON_CLS_FIELD_ASIS(quietingDown),
            CXON_JSON_CLS_FIELD_ASIS(slaveAgentPort),
            CXON_JSON_CLS_FIELD_ASIS(unlabeledLoad),
            CXON_JSON_CLS_FIELD_ASIS(useCrumbs),
            CXON_JSON_CLS_FIELD_ASIS(useSecurity),
            CXON_JSON_CLS_FIELD_ASIS(views)
        )
    };

}}

#endif // CXON_JSON_TEST_NATIVE_APACHE_BUILDS_HXX_
