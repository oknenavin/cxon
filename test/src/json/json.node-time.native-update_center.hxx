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

#ifndef CXON_JSON_TEST_NATIVE_UPDATE_CENTER_HXX_
#define CXON_JSON_TEST_NATIVE_UPDATE_CENTER_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include <cstring>


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

#       ifdef CXON_USE_GPERF
            static unsigned hash(const char *str, std::size_t len) {
                static const unsigned char asso_values[] = {
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 10,  0,
                     1, 11, 23, 11, 23, 23, 23, 23,  4, 23,  5, 23,  0, 23,  0,  0,  3,  3,  0,  1,
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23
                };
                return unsigned(len + asso_values[static_cast<unsigned char>(str[0])]);
            }

            template <typename X, typename II, typename Cx>
                static bool read_field(plugin& t, const char *str, std::size_t len, II& i, II e, Cx& cx) {
                    enum {
                        TOTAL_KEYWORDS = 18,
                        MIN_WORD_LENGTH = 3,
                        MAX_WORD_LENGTH = 22,
                        MIN_HASH_VALUE = 3,
                        MAX_HASH_VALUE = 22
                    };

                    struct field {
                        using read = bool (*)(plugin&, II&, II, Cx&);
                        char const* name;
                        read call;
                    };

#                   define CXON_FIELD(field) {#field, [](plugin& t, II& i, II e, Cx& cx) -> bool { return cxon::cio::read_map_val<X>(t.field, i, e, cx); } }
                        static CXON_CXX17_CONSTEXPR struct field wordlist[] = {
                            { nullptr }, {nullptr }, {nullptr },
                            CXON_FIELD(scm),
                            CXON_FIELD(sha1),
                            CXON_FIELD(wiki),
                            CXON_FIELD(url),
                            CXON_FIELD(version),
                            CXON_FIELD(title),
                            CXON_FIELD(name),
                            CXON_FIELD(labels),
                            CXON_FIELD(developers),
                            CXON_FIELD(requiredCore),
                            CXON_FIELD(dependencies),
                            CXON_FIELD(gav),
                            CXON_FIELD(previousVersion),
                            CXON_FIELD(releaseTimestamp),
                            CXON_FIELD(previousTimestamp),
                            CXON_FIELD(excerpt),
                            CXON_FIELD(buildDate),
                            { nullptr }, { nullptr },
                            CXON_FIELD(compatibleSinceVersion)
                        };
#                   undef CXON_FIELD

                    if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
                        unsigned int key = hash(str, len);
                        if (key <= MAX_HASH_VALUE && wordlist[key].call)
                            return wordlist[key].call(t, i, e, cx);
                    }
                    return false;
                }
#       endif // CXON_USE_GPERF
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
