// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct dependency {
        std::string name;
        bool optional;
        std::string version;

        CXON_JSON_CLS_MEMBER(dependency, CXON_JSON_CLS_FIELD_ASIS(name), CXON_JSON_CLS_FIELD_ASIS(optional), CXON_JSON_CLS_FIELD_ASIS(version))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct developer {
        std::string developerId;
        std::string email;
        std::string name;

        CXON_JSON_CLS_MEMBER(developer, CXON_JSON_CLS_FIELD_ASIS(developerId), CXON_JSON_CLS_FIELD_ASIS(email), CXON_JSON_CLS_FIELD_ASIS(name))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()

//        static inline unsigned int hash(const char *str, std::size_t len) {
//            static const unsigned char asso_values[] = {
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 10,  0,
//                1, 11, 23, 11, 23, 23, 23, 23,  4, 23,
//                5, 23,  0, 23,  0,  0,  3,  3,  0,  1,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
//                23, 23, 23, 23, 23, 23
//            };
//            return unsigned(len + asso_values[static_cast<unsigned char>(str[0])]);
//        }
//
//        template <typename X, typename II, typename Cx>
//            static bool read_value_(plugin& t, const char *str, std::size_t len, II& i, II e, Cx& cx) {
//                enum {
//                    TOTAL_KEYWORDS = 18,
//                    MIN_WORD_LENGTH = 3,
//                    MAX_WORD_LENGTH = 22,
//                    MIN_HASH_VALUE = 3,
//                    MAX_HASH_VALUE = 22
//                };
//
//                using read_ = bool (*)(plugin&, II&, II, Cx&);
//                struct field {
//                    char const* name;
//                    read_ call;
//                };
//
//#               define FIELD(field) {#field, [](plugin& t, II& i, II e, Cx& cx) -> bool { return cxon::cio::read_map_val<X>(t.field, i, e, cx); } }
//                static CXON_CXX17_CONSTEXPR struct field wordlist[] = {
//                    { nullptr }, {nullptr }, {nullptr },
//                    FIELD(scm),
//                    FIELD(sha1),
//                    FIELD(wiki),
//                    FIELD(url),
//                    FIELD(version),
//                    FIELD(title),
//                    FIELD(name),
//                    FIELD(labels),
//                    FIELD(developers),
//                    FIELD(requiredCore),
//                    FIELD(dependencies),
//                    FIELD(gav),
//                    FIELD(previousVersion),
//                    FIELD(releaseTimestamp),
//                    FIELD(previousTimestamp),
//                    FIELD(excerpt),
//                    FIELD(buildDate),
//                    { nullptr }, { nullptr },
//                    FIELD(compatibleSinceVersion)
//                };
//#               undef FIELD
//
//                if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
//                    unsigned int key = hash(str, len);
//                    if (key <= MAX_HASH_VALUE) {
//                        if (wordlist[key].call)
//                            return wordlist[key].call(t, i, e, cx);
//                    }
//                }
//                return false;
//            }
//
//
//        template <typename X, typename II, typename Cx>
//            static auto read_value(plugin& t, II& i, II e, Cx& cx) -> cxon::enable_for_t<X, cxon::JSON> {
//                if (!cxon::cio::consume<X>(X::map::beg, i, e, cx))
//                    return false;
//                for (char id[cxon::cio::ids_len_max::constant<cxon::napa_type<Cx>>(64)]; ; ) {
//                    if (!cxon::cio::consume<X>(i, e, cx))
//                        return false;
//                    if (cxon::cio::peek(i, e) == X::map::end)
//                        return ++i, true;
//                    II const o = i;
//                        using Y = cxon::bind_traits_t<X, cxon::cio::str::raw_traits>;
//                        if (!cxon::cio::read_map_key<Y>(id, i, e, cx))
//                            return cxon::cio::rewind(i, o), false;
//                        if (!read_value_<X>(t, id, std::strlen(id), i, e, cx))
//                            return cx && (cxon::cio::rewind(i, o), cx/X::read_error::unexpected);
//                        if (cxon::cio::cnt::consume_sep<X, typename X::map>(i, e, cx))
//                            continue;
//                    return cxon::cio::consume<X>(X::map::end, i, e, cx);
//                }
//                return true;
//            }
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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

}}

#endif // CXON_JSON_TEST_NATIVE_UPDATE_CENTER_HXX_
