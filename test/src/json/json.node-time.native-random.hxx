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

#ifndef CXON_JSON_TEST_NATIVE_RANDOM_HXX_
#define CXON_JSON_TEST_NATIVE_RANDOM_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"


namespace test { namespace random {

    struct friend_ {
        unsigned id;
        std::string name;
        std::string phone;

        CXON_JSON_CLS_MEMBER(friend_, CXON_JSON_CLS_FIELD_ASIS(id), CXON_JSON_CLS_FIELD_ASIS(name), CXON_JSON_CLS_FIELD_ASIS(phone))
    };

    struct result {
        unsigned id;
        std::string avatar;
        unsigned age;
        bool admin;
        std::string name;
        std::string company;
        std::string phone;
        std::string email;
        std::string birthDate;
        std::vector<friend_> friends;
        std::string field;

        CXON_JSON_CLS_MEMBER(result,
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(avatar),
            CXON_JSON_CLS_FIELD_ASIS(age),
            CXON_JSON_CLS_FIELD_ASIS(admin),
            CXON_JSON_CLS_FIELD_ASIS(name),
            CXON_JSON_CLS_FIELD_ASIS(company),
            CXON_JSON_CLS_FIELD_ASIS(phone),
            CXON_JSON_CLS_FIELD_ASIS(email),
            CXON_JSON_CLS_FIELD_ASIS(birthDate),
            CXON_JSON_CLS_FIELD_ASIS(friends),
            CXON_JSON_CLS_FIELD_ASIS(field)
        )
    };

    struct object {
        unsigned id;
        std::string jsonrpc;
        unsigned total;
        std::vector<struct result> result;

        CXON_JSON_CLS_MEMBER(object, CXON_JSON_CLS_FIELD_ASIS(id), CXON_JSON_CLS_FIELD_ASIS(jsonrpc), CXON_JSON_CLS_FIELD_ASIS(total), CXON_JSON_CLS_FIELD_ASIS(result))
    };

}}

#endif // CXON_JSON_TEST_NATIVE_RANDOM_HXX_
