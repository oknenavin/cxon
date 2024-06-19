// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct object {
        unsigned id;
        std::string jsonrpc;
        unsigned total;
        std::vector<struct result> result;

        CXON_JSON_CLS_MEMBER(object, CXON_JSON_CLS_FIELD_ASIS(id), CXON_JSON_CLS_FIELD_ASIS(jsonrpc), CXON_JSON_CLS_FIELD_ASIS(total), CXON_JSON_CLS_FIELD_ASIS(result))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

}}

#endif // CXON_JSON_TEST_NATIVE_RANDOM_HXX_
