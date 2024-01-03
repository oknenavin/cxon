// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_NATIVE_CITM_CATALOG_HXX_
#define CXON_JSON_TEST_NATIVE_CITM_CATALOG_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/node.ordered.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"


#ifdef CXON_HAS_LIB_STD_OPTIONAL

namespace test { namespace citm_catalog {

    struct event {
        std::optional<std::string> description;
        unsigned long long id;
        std::optional<std::string> logo;
        std::optional<std::string> name;
        std::vector<unsigned long long> subTopicIds;
        std::optional<std::string> subjectCode;
        std::optional<std::string> subtitle;
        std::vector<unsigned long long> topicIds;

        CXON_JSON_CLS_MEMBER(event,
            CXON_JSON_CLS_FIELD_ASIS(description),
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(logo),
            CXON_JSON_CLS_FIELD_ASIS(name),
            CXON_JSON_CLS_FIELD_ASIS(subTopicIds),
            CXON_JSON_CLS_FIELD_ASIS(subjectCode),
            CXON_JSON_CLS_FIELD_ASIS(subtitle),
            CXON_JSON_CLS_FIELD_ASIS(topicIds)
        )
    };

    struct price {
        unsigned long long amount;
        unsigned long long audienceSubCategoryId;
        unsigned long long seatCategoryId;

        CXON_JSON_CLS_MEMBER(price, CXON_JSON_CLS_FIELD_ASIS(amount), CXON_JSON_CLS_FIELD_ASIS(audienceSubCategoryId), CXON_JSON_CLS_FIELD_ASIS(seatCategoryId))
    };

    struct area {
        unsigned long long areaId;
        std::vector<unsigned long long> blockIds;

        CXON_JSON_CLS_MEMBER(area, CXON_JSON_CLS_FIELD_ASIS(areaId), CXON_JSON_CLS_FIELD_ASIS(blockIds))
    };

    struct seat {
        std::vector<area> areas;
        unsigned long long seatCategoryId;

        CXON_JSON_CLS_MEMBER(seat, CXON_JSON_CLS_FIELD_ASIS(areas), CXON_JSON_CLS_FIELD_ASIS(seatCategoryId))
    };

    struct performance {
        unsigned long long eventId;
        unsigned long long id;
        std::optional<std::string> logo;
        std::optional<std::string> name;
        std::vector<price> prices;
        std::vector<seat> seatCategories;
        std::optional<std::string> seatMapImage;
        unsigned long long start;
        std::string venueCode;

        CXON_JSON_CLS_MEMBER(performance,
            CXON_JSON_CLS_FIELD_ASIS(eventId),
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(logo),
            CXON_JSON_CLS_FIELD_ASIS(name),
            CXON_JSON_CLS_FIELD_ASIS(prices),
            CXON_JSON_CLS_FIELD_ASIS(seatCategories),
            CXON_JSON_CLS_FIELD_ASIS(seatMapImage),
            CXON_JSON_CLS_FIELD_ASIS(start),
            CXON_JSON_CLS_FIELD_ASIS(venueCode)
        )
    };

    struct object {
        cxon::ordered::object<unsigned long long, std::string> areaNames;
        cxon::ordered::object<unsigned long long, std::string> audienceSubCategoryNames;
        cxon::ordered::object<std::string, std::string> blockNames;
        cxon::ordered::object<unsigned long long, event> events;
        std::vector<performance> performances;
        cxon::ordered::object<unsigned long long, std::string> seatCategoryNames;
        cxon::ordered::object<unsigned long long, std::string> subTopicNames;
        cxon::ordered::object<std::string, std::string> subjectNames;
        cxon::ordered::object<unsigned long long, std::string> topicNames;
        cxon::ordered::object<unsigned long long, std::vector<unsigned long long>> topicSubTopics;
        cxon::ordered::object<std::string, std::string> venueNames;

        CXON_JSON_CLS_MEMBER(object,
            CXON_JSON_CLS_FIELD_ASIS(areaNames),
            CXON_JSON_CLS_FIELD_ASIS(audienceSubCategoryNames),
            CXON_JSON_CLS_FIELD_ASIS(blockNames),
            CXON_JSON_CLS_FIELD_ASIS(events),
            CXON_JSON_CLS_FIELD_ASIS(performances),
            CXON_JSON_CLS_FIELD_ASIS(seatCategoryNames),
            CXON_JSON_CLS_FIELD_ASIS(subTopicNames),
            CXON_JSON_CLS_FIELD_ASIS(subjectNames),
            CXON_JSON_CLS_FIELD_ASIS(topicNames),
            CXON_JSON_CLS_FIELD_ASIS(topicSubTopics),
            CXON_JSON_CLS_FIELD_ASIS(venueNames)
        )
    };

}}

#endif // CXON_HAS_LIB_STD_OPTIONAL

#endif // CXON_JSON_TEST_NATIVE_CITM_CATALOG_HXX_
