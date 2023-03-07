// Copyright (c) 2017-2023 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_NATIVE_UPDATE_CENTER_HXX_
#define CXON_JSON_TEST_NATIVE_UPDATE_CENTER_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"


namespace test { namespace update_center {

    struct core {
        std::string buildDate;
        std::string name;
        std::string sha1;
        std::string url;
        std::string version;

        CXON_JSON_CLS_MEMBER(core,
            CXON_JSON_CLS_FIELD_ASIS(buildDate),
            CXON_JSON_CLS_FIELD_ASIS(name),
            CXON_JSON_CLS_FIELD_ASIS(sha1),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(version)
        )
    };

    struct dependency {
        std::string name;
        bool optional;
        std::string version;

        CXON_JSON_CLS_MEMBER(dependency, CXON_JSON_CLS_FIELD_ASIS(name), CXON_JSON_CLS_FIELD_ASIS(optional), CXON_JSON_CLS_FIELD_ASIS(version))
    };

    struct developer {
        std::string developerId;
        std::string email;
        std::string name;

        CXON_JSON_CLS_MEMBER(developer, CXON_JSON_CLS_FIELD_ASIS(developerId), CXON_JSON_CLS_FIELD_ASIS(email), CXON_JSON_CLS_FIELD_ASIS(name))
    };

    struct plugin {
        std::string buildDate;
        std::string compatibleSinceVersion;
        std::vector<dependency> dependencies;
        std::vector<developer> developers;
        std::string excerpt;
        std::string gav;
        std::vector<std::string> labels;
        std::string name;
        std::string previousTimestamp;
        std::string previousVersion;
        std::string releaseTimestamp;
        std::string requiredCore;
        std::string scm;
        std::string sha1;
        std::string title;
        std::string url;
        std::string version;
        std::string wiki;

        CXON_JSON_CLS_MEMBER(plugin,
            CXON_JSON_CLS_FIELD_ASIS(buildDate),
            CXON_JSON_CLS_FIELD_ASIS(compatibleSinceVersion),
            CXON_JSON_CLS_FIELD_ASIS(dependencies),
            CXON_JSON_CLS_FIELD_ASIS(developers),
            CXON_JSON_CLS_FIELD_ASIS(excerpt),
            CXON_JSON_CLS_FIELD_ASIS(gav),
            CXON_JSON_CLS_FIELD_ASIS(labels),
            CXON_JSON_CLS_FIELD_ASIS(name),
            CXON_JSON_CLS_FIELD_ASIS(previousTimestamp),
            CXON_JSON_CLS_FIELD_ASIS(previousVersion),
            CXON_JSON_CLS_FIELD_ASIS(releaseTimestamp),
            CXON_JSON_CLS_FIELD_ASIS(requiredCore),
            CXON_JSON_CLS_FIELD_ASIS(scm),
            CXON_JSON_CLS_FIELD_ASIS(sha1),
            CXON_JSON_CLS_FIELD_ASIS(title),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(version),
            CXON_JSON_CLS_FIELD_ASIS(wiki)
        )
    };

    struct signature_ {
        std::vector<std::string> certificates;
        std::string correct_digest;
        std::string correct_signature;
        std::string digest;
        std::string signature;

        CXON_JSON_CLS_MEMBER(signature_,
            CXON_JSON_CLS_FIELD_ASIS(certificates),
            CXON_JSON_CLS_FIELD_ASIS(correct_digest),
            CXON_JSON_CLS_FIELD_ASIS(correct_signature),
            CXON_JSON_CLS_FIELD_ASIS(digest),
            CXON_JSON_CLS_FIELD_ASIS(signature)
        )
    };

    struct object {
        std::string connectionCheckUrl;
        struct core core;
        std::string id;
        cxon::ordered::object<std::string, plugin> plugins;
        signature_ signature;
        std::string updateCenterVersion;

        CXON_JSON_CLS_MEMBER(object,
            CXON_JSON_CLS_FIELD_ASIS(connectionCheckUrl),
            CXON_JSON_CLS_FIELD_ASIS(core),
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(plugins),
            CXON_JSON_CLS_FIELD_ASIS(signature),
            CXON_JSON_CLS_FIELD_ASIS(updateCenterVersion)
        )
    };

}}

#endif // CXON_JSON_TEST_NATIVE_UPDATE_CENTER_HXX_
