// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_NATIVE_GSOC_2018_HXX_
#define CXON_JSON_TEST_NATIVE_GSOC_2018_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/node.ordered.hxx"
#include "cxon/lib/std/string.hxx"

namespace test { namespace gsoc_2018 {

    struct sponsor {
        std::string type;
        std::string name;
        std::string disambiguatingDescription;
        std::string description;
        std::string url;
        std::string logo;

        CXON_JSON_CLS_MEMBER(sponsor,
            CXON_JSON_CLS_FIELD_NAME("@type", type),
            CXON_JSON_CLS_FIELD_ASIS(name),
            CXON_JSON_CLS_FIELD_ASIS(disambiguatingDescription),
            CXON_JSON_CLS_FIELD_ASIS(description),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(logo)
        )
    };

    struct author {
        std::string type;
        std::string name;

        CXON_JSON_CLS_MEMBER(author, CXON_JSON_CLS_FIELD_NAME("@type", type), CXON_JSON_CLS_FIELD_ASIS(name))
    };

    struct participant {
        std::string context;
        std::string type;
        std::string name;
        std::string description;
        struct sponsor sponsor;
        struct author author;

        CXON_JSON_CLS_MEMBER(participant,
            CXON_JSON_CLS_FIELD_NAME("@context", context),
            CXON_JSON_CLS_FIELD_NAME("@type", type),
            CXON_JSON_CLS_FIELD_ASIS(name),
            CXON_JSON_CLS_FIELD_ASIS(description),
            CXON_JSON_CLS_FIELD_ASIS(sponsor),
            CXON_JSON_CLS_FIELD_ASIS(author)
        )
    };

    using object = cxon::ordered::object<std::string, participant>;

}}

#endif // CXON_JSON_TEST_NATIVE_GSOC_2018_HXX_
