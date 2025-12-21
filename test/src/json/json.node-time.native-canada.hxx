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

#ifndef CXON_JSON_TEST_NATIVE_CANADA_HXX_
#define CXON_JSON_TEST_NATIVE_CANADA_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/utility.hxx"
#include "cxon/lib/std/vector.hxx"


namespace test { namespace canada {

    struct geometry {
        std::string type;
        std::vector<std::vector<std::pair<double, double>>> coordinates;

        CXON_JSON_CLS_MEMBER(geometry, CXON_JSON_CLS_FIELD_ASIS(type), CXON_JSON_CLS_FIELD_ASIS(coordinates))
    };

    struct feature {
        std::string type;
        std::map<std::string, std::string> properties;
        struct geometry geometry;

        CXON_JSON_CLS_MEMBER(feature, CXON_JSON_CLS_FIELD_ASIS(type), CXON_JSON_CLS_FIELD_ASIS(properties), CXON_JSON_CLS_FIELD_ASIS(geometry))
    };

    struct object {
        std::string type;
        std::vector<feature> features;

        CXON_JSON_CLS_MEMBER(object, CXON_JSON_CLS_FIELD_ASIS(type), CXON_JSON_CLS_FIELD_ASIS(features))
    };

}}

#endif // CXON_JSON_TEST_NATIVE_CANADA_HXX_
