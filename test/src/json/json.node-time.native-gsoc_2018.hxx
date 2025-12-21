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

    using object = cxon::ordered::object<unsigned, participant>;

}}

#endif // CXON_JSON_TEST_NATIVE_GSOC_2018_HXX_
