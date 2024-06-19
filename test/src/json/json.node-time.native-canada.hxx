// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct feature {
        std::string type;
        std::map<std::string, std::string> properties;
        struct geometry geometry;

        CXON_JSON_CLS_MEMBER(feature, CXON_JSON_CLS_FIELD_ASIS(type), CXON_JSON_CLS_FIELD_ASIS(properties), CXON_JSON_CLS_FIELD_ASIS(geometry))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct object {
        std::string type;
        std::vector<feature> features;

        CXON_JSON_CLS_MEMBER(object, CXON_JSON_CLS_FIELD_ASIS(type), CXON_JSON_CLS_FIELD_ASIS(features))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

}}

#endif // CXON_JSON_TEST_NATIVE_CANADA_HXX_
