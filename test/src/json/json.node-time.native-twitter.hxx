// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_NATIVE_TWITTER_HXX_
#define CXON_JSON_TEST_NATIVE_TWITTER_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"


#ifdef CXON_HAS_LIB_STD_OPTIONAL

namespace test { namespace twitter {

    struct metadata {
        std::string result_type;
        std::string iso_language_code;

        CXON_JSON_CLS_MEMBER(metadata, CXON_JSON_CLS_FIELD_ASIS(result_type), CXON_JSON_CLS_FIELD_ASIS(iso_language_code))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct url {
        std::string url_;
        std::string expanded_url;
        std::string display_url;
        std::vector<unsigned> indices;

        CXON_JSON_CLS_MEMBER(url, CXON_JSON_CLS_FIELD_NAME("url", url_), CXON_JSON_CLS_FIELD_ASIS(expanded_url), CXON_JSON_CLS_FIELD_ASIS(display_url), CXON_JSON_CLS_FIELD_ASIS(indices))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct user_mention {
        std::string screen_name;
        std::string name;
        unsigned long long id;
        std::string id_str;
        std::vector<unsigned> indices;

        CXON_JSON_CLS_MEMBER(user_mention,
            CXON_JSON_CLS_FIELD_ASIS(screen_name),
            CXON_JSON_CLS_FIELD_ASIS(name),
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(id_str),
            CXON_JSON_CLS_FIELD_ASIS(indices)
        )
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct size {
        unsigned w;
        unsigned h;
        std::string resize;

        CXON_JSON_CLS_MEMBER(size, CXON_JSON_CLS_FIELD_ASIS(w), CXON_JSON_CLS_FIELD_ASIS(h), CXON_JSON_CLS_FIELD_ASIS(resize))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct sizes {
        size medium;
        size large;
        size thumb;
        size small;

        CXON_JSON_CLS_MEMBER(sizes, CXON_JSON_CLS_FIELD_ASIS(medium), CXON_JSON_CLS_FIELD_ASIS(large), CXON_JSON_CLS_FIELD_ASIS(thumb), CXON_JSON_CLS_FIELD_ASIS(small))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct media {
        unsigned long long id;
        std::string id_str;
        std::vector<unsigned> indices;
        std::string media_url;
        std::string media_url_https;
        std::string url;
        std::string display_url;
        std::string expanded_url;
        std::string type;
        struct sizes sizes;
        unsigned long long source_status_id;
        std::string source_status_id_str;

        CXON_JSON_CLS_MEMBER(media,
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(id_str),
            CXON_JSON_CLS_FIELD_ASIS(indices),
            CXON_JSON_CLS_FIELD_ASIS(media_url),
            CXON_JSON_CLS_FIELD_ASIS(media_url_https),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(display_url),
            CXON_JSON_CLS_FIELD_ASIS(expanded_url),
            CXON_JSON_CLS_FIELD_ASIS(type),
            CXON_JSON_CLS_FIELD_ASIS(sizes),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(source_status_id      , self.source_status_id == 0),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(source_status_id_str  , self.source_status_id_str.empty())
        )
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct description {
        std::vector<url> urls;

        CXON_JSON_CLS_MEMBER(description, CXON_JSON_CLS_FIELD_ASIS(urls))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct urls {
        std::vector<url> urls_;

        CXON_JSON_CLS_MEMBER(urls, CXON_JSON_CLS_FIELD_NAME("urls", urls_))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct hashtag {
        std::string text;
        std::vector<unsigned> indices;

        CXON_JSON_CLS_MEMBER(hashtag, CXON_JSON_CLS_FIELD_ASIS(text), CXON_JSON_CLS_FIELD_ASIS(indices))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct entity {
        std::vector<hashtag> hashtags;
        std::vector<std::string> symbols;
        std::vector<struct url> urls;
        std::vector<user_mention> user_mentions;
        struct urls url;
        struct description description;
        std::vector<struct media> media;

        CXON_JSON_CLS_MEMBER(entity,
            CXON_JSON_CLS_FIELD_ASIS_DFLT(hashtags          , self.hashtags.empty()),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(symbols           , self.symbols.empty()),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(urls              , self.urls.empty()),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(user_mentions     , self.user_mentions.empty()),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(url               , self.url.urls_.empty()),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(description       , self.description.urls.empty()),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(media             , self.media.empty())
        )
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct user {
        unsigned long long id;
        std::string id_str;
        std::string name;
        std::string screen_name;
        std::string location;
        std::string description;
        std::optional<std::string> url;
        entity entities;
        bool protected_;
        unsigned followers_count;
        unsigned friends_count;
        unsigned listed_count;
        std::string created_at;
        unsigned favourites_count;
        std::optional<signed> utc_offset;
        std::optional<std::string> time_zone;
        bool geo_enabled;
        bool verified;
        unsigned statuses_count;
        std::string lang;
        bool contributors_enabled;
        bool is_translator;
        bool is_translation_enabled;
        std::string profile_background_color;
        std::string profile_background_image_url;
        std::string profile_background_image_url_https;
        bool profile_background_tile;
        std::string profile_image_url;
        std::string profile_image_url_https;
        std::string profile_banner_url;
        std::string profile_link_color;
        std::string profile_sidebar_border_color;
        std::string profile_sidebar_fill_color;
        std::string profile_text_color;
        bool profile_use_background_image;
        bool default_profile;
        bool default_profile_image;
        bool following;
        bool follow_request_sent;
        bool notifications;

        CXON_JSON_CLS_MEMBER(user,
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(id_str),
            CXON_JSON_CLS_FIELD_ASIS(name),
            CXON_JSON_CLS_FIELD_ASIS(screen_name),
            CXON_JSON_CLS_FIELD_ASIS(location),
            CXON_JSON_CLS_FIELD_ASIS(description),
            CXON_JSON_CLS_FIELD_ASIS(url),
            CXON_JSON_CLS_FIELD_ASIS(entities),
            CXON_JSON_CLS_FIELD_NAME("protected", protected_),
            CXON_JSON_CLS_FIELD_ASIS(followers_count),
            CXON_JSON_CLS_FIELD_ASIS(friends_count),
            CXON_JSON_CLS_FIELD_ASIS(listed_count),
            CXON_JSON_CLS_FIELD_ASIS(created_at),
            CXON_JSON_CLS_FIELD_ASIS(favourites_count),
            CXON_JSON_CLS_FIELD_ASIS(utc_offset),
            CXON_JSON_CLS_FIELD_ASIS(time_zone),
            CXON_JSON_CLS_FIELD_ASIS(geo_enabled),
            CXON_JSON_CLS_FIELD_ASIS(verified),
            CXON_JSON_CLS_FIELD_ASIS(statuses_count),
            CXON_JSON_CLS_FIELD_ASIS(lang),
            CXON_JSON_CLS_FIELD_ASIS(contributors_enabled),
            CXON_JSON_CLS_FIELD_ASIS(is_translator),
            CXON_JSON_CLS_FIELD_ASIS(is_translation_enabled),
            CXON_JSON_CLS_FIELD_ASIS(profile_background_color),
            CXON_JSON_CLS_FIELD_ASIS(profile_background_image_url),
            CXON_JSON_CLS_FIELD_ASIS(profile_background_image_url_https),
            CXON_JSON_CLS_FIELD_ASIS(profile_background_tile),
            CXON_JSON_CLS_FIELD_ASIS(profile_image_url),
            CXON_JSON_CLS_FIELD_ASIS(profile_image_url_https),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(profile_banner_url, self.profile_banner_url.empty()),
            CXON_JSON_CLS_FIELD_ASIS(profile_link_color),
            CXON_JSON_CLS_FIELD_ASIS(profile_sidebar_border_color),
            CXON_JSON_CLS_FIELD_ASIS(profile_sidebar_fill_color),
            CXON_JSON_CLS_FIELD_ASIS(profile_text_color),
            CXON_JSON_CLS_FIELD_ASIS(profile_use_background_image),
            CXON_JSON_CLS_FIELD_ASIS(default_profile),
            CXON_JSON_CLS_FIELD_ASIS(default_profile_image),
            CXON_JSON_CLS_FIELD_ASIS(following),
            CXON_JSON_CLS_FIELD_ASIS(follow_request_sent),
            CXON_JSON_CLS_FIELD_ASIS(notifications)
        )
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()

#       ifdef CXON_USE_GPERF
            static unsigned hash(const char *str, std::size_t len) {
                static const unsigned char asso_values[] = {
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 29, 45, 27,  7, 17,
                     1,  3, 45,  2, 45, 10, 45, 45,  5, 45, 18,  6,  3, 45,  6,  2,  1, 11, 45, 45,
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
                    45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45
                };
                unsigned hval = (unsigned)len;
                    switch (hval) {
                        default:
                            hval += asso_values[static_cast<unsigned char>(str[8])];
                            CXON_FALLTHROUGH;
                        case 8: case 7: case 6: case 5: case 4: case 3: case 2:
                            break;
                    }
                return hval + asso_values[static_cast<unsigned char>(str[len - 1])];
            }

            template <typename X, typename II, typename Cx>
                static bool read_field(user& t, const char *str, std::size_t len, II& i, II e, Cx& cx) {
                    enum {
                        TOTAL_KEYWORDS = 40,
                        MIN_WORD_LENGTH = 2,
                        MAX_WORD_LENGTH = 34,
                        MIN_HASH_VALUE = 3,
                        MAX_HASH_VALUE = 44
                    };

                    struct field {
                        using read = bool (*)(user&, II&, II, Cx&);
                        char const* name;
                        read call;
                    };

#                   define CXON_FIELD(field) {#field, [](user& t, II& i, II e, Cx& cx) -> bool { return cxon::cio::read_map_val<X>(t.field, i, e, cx); } }
                        static CXON_CXX17_CONSTEXPR struct field wordlist[] = {
                            {(char*)0}, {(char*)0}, {(char*)0},
                            CXON_FIELD(id),
                            {(char*)0}, {(char*)0},
                            CXON_FIELD(lang),
                            CXON_FIELD(name),
                            CXON_FIELD(url),
                            CXON_FIELD(verified),
                            CXON_FIELD(entities),
                            {"protected", [](user& t, II& i, II e, Cx& cx) -> bool { return cxon::cio::read_map_val<X>(t.protected_, i, e, cx); } },
                            CXON_FIELD(id_str),
                            CXON_FIELD(following),
                            CXON_FIELD(utc_offset),
                            CXON_FIELD(time_zone),
                            CXON_FIELD(notifications),
                            CXON_FIELD(geo_enabled),
                            CXON_FIELD(followers_count),
                            CXON_FIELD(listed_count),
                            CXON_FIELD(favourites_count),
                            CXON_FIELD(default_profile),
                            CXON_FIELD(contributors_enabled),
                            CXON_FIELD(follow_request_sent),
                            CXON_FIELD(is_translator),
                            CXON_FIELD(profile_text_color),
                            CXON_FIELD(location),
                            CXON_FIELD(default_profile_image),
                            CXON_FIELD(is_translation_enabled),
                            CXON_FIELD(profile_link_color),
                            CXON_FIELD(profile_banner_url),
                            CXON_FIELD(friends_count),
                            CXON_FIELD(profile_image_url),
                            CXON_FIELD(profile_background_tile),
                            CXON_FIELD(profile_sidebar_fill_color),
                            CXON_FIELD(profile_image_url_https),
                            CXON_FIELD(profile_sidebar_border_color),
                            CXON_FIELD(profile_background_color),
                            CXON_FIELD(created_at),
                            CXON_FIELD(description),
                            CXON_FIELD(profile_background_image_url),
                            CXON_FIELD(screen_name),
                            CXON_FIELD(profile_use_background_image),
                            CXON_FIELD(profile_background_image_url_https),
                            CXON_FIELD(statuses_count)
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

    struct retweeted_status {
        struct metadata metadata;
        std::string created_at;
        unsigned long long id;
        std::string id_str;
        std::string text;
        std::string source;
        bool truncated;
        std::optional<unsigned long long> in_reply_to_status_id;
        std::optional<std::string> in_reply_to_status_id_str;
        std::optional<unsigned long long> in_reply_to_user_id;
        std::optional<std::string> in_reply_to_user_id_str;
        std::optional<std::string> in_reply_to_screen_name;
        struct user user;
        std::nullptr_t geo;
        std::nullptr_t coordinates;
        std::nullptr_t place;
        std::nullptr_t contributors;
        unsigned retweet_count;
        unsigned favorite_count;
        entity entities;
        bool favorited;
        bool retweeted;
        bool possibly_sensitive;
        std::string lang;

        CXON_JSON_CLS_MEMBER(retweeted_status,
            CXON_JSON_CLS_FIELD_ASIS(metadata),
            CXON_JSON_CLS_FIELD_ASIS(created_at),
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(id_str),
            CXON_JSON_CLS_FIELD_ASIS(text),
            CXON_JSON_CLS_FIELD_ASIS(source),
            CXON_JSON_CLS_FIELD_ASIS(truncated),
            CXON_JSON_CLS_FIELD_ASIS(in_reply_to_status_id),
            CXON_JSON_CLS_FIELD_ASIS(in_reply_to_status_id_str),
            CXON_JSON_CLS_FIELD_ASIS(in_reply_to_user_id),
            CXON_JSON_CLS_FIELD_ASIS(in_reply_to_user_id_str),
            CXON_JSON_CLS_FIELD_ASIS(in_reply_to_screen_name),
            CXON_JSON_CLS_FIELD_ASIS(user),
            CXON_JSON_CLS_FIELD_ASIS(geo),
            CXON_JSON_CLS_FIELD_ASIS(coordinates),
            CXON_JSON_CLS_FIELD_ASIS(place),
            CXON_JSON_CLS_FIELD_ASIS(contributors),
            CXON_JSON_CLS_FIELD_ASIS(retweet_count),
            CXON_JSON_CLS_FIELD_ASIS(favorite_count),
            CXON_JSON_CLS_FIELD_ASIS(entities),
            CXON_JSON_CLS_FIELD_ASIS(favorited),
            CXON_JSON_CLS_FIELD_ASIS(retweeted),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(possibly_sensitive, !self.possibly_sensitive),
            CXON_JSON_CLS_FIELD_ASIS(lang)
        )
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()

#       ifdef CXON_USE_GPERF
            static unsigned hash(const char *str, std::size_t len) {
                static const unsigned char asso_values[] = {
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  0,
                    28, 11,  6, 15, 28,  1, 28, 28, 12,  0,  0, 28,  9, 28,  4,  0,  0,  1, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28
                };
                unsigned hval = (unsigned)len;
                    switch (hval) {
                        default:
                            hval += asso_values[static_cast<unsigned char>(str[12])];
                            CXON_FALLTHROUGH;
                        case 12: case 11: case 10: case 9: case 8: case 7: case 6: case 5: case 4: case 3: case 2: case 1:
                            hval += asso_values[static_cast<unsigned char>(str[0])];
                            break;
                    }
                return hval;
            }

            template <typename X, typename II, typename Cx>
                static bool read_field(retweeted_status& t, const char *str, std::size_t len, II& i, II e, Cx& cx) {
                    enum {
                        TOTAL_KEYWORDS = 24,
                        MIN_WORD_LENGTH = 2,
                        MAX_WORD_LENGTH = 25,
                        MIN_HASH_VALUE = 3,
                        MAX_HASH_VALUE = 27
                    };

                    struct field {
                        using read = bool (*)(retweeted_status&, II&, II, Cx&);
                        char const* name;
                        read call;
                    };

#                   define CXON_FIELD(field) {#field, [](retweeted_status& t, II& i, II e, Cx& cx) -> bool { return cxon::cio::read_map_val<X>(t.field, i, e, cx); } }
                        static CXON_CXX17_CONSTEXPR struct field wordlist[] = {
                            {(char*)0}, {(char*)0}, {(char*)0},
                            CXON_FIELD(id),
                            CXON_FIELD(text),
                            CXON_FIELD(user),
                            CXON_FIELD(source),
                            CXON_FIELD(id_str),
                            CXON_FIELD(metadata),
                            CXON_FIELD(truncated),
                            CXON_FIELD(created_at),
                            CXON_FIELD(coordinates),
                            CXON_FIELD(contributors),
                            CXON_FIELD(retweeted),
                            CXON_FIELD(place),
                            CXON_FIELD(favorited),
                            CXON_FIELD(lang),
                            CXON_FIELD(retweet_count),
                            CXON_FIELD(geo),
                            CXON_FIELD(entities),
                            CXON_FIELD(favorite_count),
                            CXON_FIELD(in_reply_to_user_id),
                            CXON_FIELD(in_reply_to_status_id),
                            {(char*)0},
                            CXON_FIELD(in_reply_to_screen_name),
                            CXON_FIELD(in_reply_to_user_id_str),
                            CXON_FIELD(in_reply_to_status_id_str),
                            CXON_FIELD(possibly_sensitive)
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

    struct status {
        struct metadata metadata;
        std::string created_at;
        unsigned long long id;
        std::string id_str;
        std::string text;
        std::string source;
        bool truncated;
        std::optional<unsigned long long> in_reply_to_status_id;
        std::optional<std::string> in_reply_to_status_id_str;
        std::optional<unsigned long long> in_reply_to_user_id;
        std::optional<std::string> in_reply_to_user_id_str;
        std::optional<std::string> in_reply_to_screen_name;
        struct user user;
        std::nullptr_t geo;
        std::nullptr_t coordinates;
        std::nullptr_t place;
        std::nullptr_t contributors;
        struct retweeted_status retweeted_status; 
        unsigned retweet_count;
        unsigned favorite_count;
        entity entities;
        bool favorited;
        bool retweeted;
        bool possibly_sensitive;
        std::string lang;

        CXON_JSON_CLS_MEMBER(status,
            CXON_JSON_CLS_FIELD_ASIS(metadata),
            CXON_JSON_CLS_FIELD_ASIS(created_at),
            CXON_JSON_CLS_FIELD_ASIS(id),
            CXON_JSON_CLS_FIELD_ASIS(id_str),
            CXON_JSON_CLS_FIELD_ASIS(text),
            CXON_JSON_CLS_FIELD_ASIS(source),
            CXON_JSON_CLS_FIELD_ASIS(truncated),
            CXON_JSON_CLS_FIELD_ASIS(in_reply_to_status_id),
            CXON_JSON_CLS_FIELD_ASIS(in_reply_to_status_id_str),
            CXON_JSON_CLS_FIELD_ASIS(in_reply_to_user_id),
            CXON_JSON_CLS_FIELD_ASIS(in_reply_to_user_id_str),
            CXON_JSON_CLS_FIELD_ASIS(in_reply_to_screen_name),
            CXON_JSON_CLS_FIELD_ASIS(user),
            CXON_JSON_CLS_FIELD_ASIS(geo),
            CXON_JSON_CLS_FIELD_ASIS(coordinates),
            CXON_JSON_CLS_FIELD_ASIS(place),
            CXON_JSON_CLS_FIELD_ASIS(contributors),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(retweeted_status      , self.retweeted_status.created_at.empty()),
            CXON_JSON_CLS_FIELD_ASIS(retweet_count),
            CXON_JSON_CLS_FIELD_ASIS(favorite_count),
            CXON_JSON_CLS_FIELD_ASIS(entities),
            CXON_JSON_CLS_FIELD_ASIS(favorited),
            CXON_JSON_CLS_FIELD_ASIS(retweeted),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(possibly_sensitive    , !self.possibly_sensitive),
            CXON_JSON_CLS_FIELD_ASIS(lang)
        )
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()

#       ifdef CXON_USE_GPERF
            static unsigned hash(const char *str, std::size_t len) {
                static const unsigned char asso_values[] = {
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  3, 28,  0,
                    28, 11,  6, 15, 28,  1, 28, 28, 12,  0,  0, 28,  9, 28,  4,  0,  0,  1, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28
                };
                unsigned hval = (unsigned)len;
                    switch (hval) {
                        default:
                            hval += asso_values[static_cast<unsigned char>(str[12])];
                            CXON_FALLTHROUGH;
                        case 12: case 11: case 10: case 9: case 8: case 7: case 6: case 5: case 4: case 3: case 2: case 1:
                            hval += asso_values[static_cast<unsigned char>(str[0])];
                            break;
                    }
                return hval;
            }

            template <typename X, typename II, typename Cx>
                static bool read_field(status& t, const char *str, std::size_t len, II& i, II e, Cx& cx) {
                    enum {
                        TOTAL_KEYWORDS = 25,
                        MIN_WORD_LENGTH = 2,
                        MAX_WORD_LENGTH = 25,
                        MIN_HASH_VALUE = 3,
                        MAX_HASH_VALUE = 27
                    };

                    struct field {
                        using read = bool (*)(status&, II&, II, Cx&);
                        char const* name;
                        read call;
                    };

    #               define CXON_FIELD(field) {#field, [](status& t, II& i, II e, Cx& cx) -> bool { return cxon::cio::read_map_val<X>(t.field, i, e, cx); } }
                        static CXON_CXX17_CONSTEXPR struct field wordlist[] = {
                            {(char*)0}, {(char*)0}, {(char*)0},
                            CXON_FIELD(id),
                            CXON_FIELD(text),
                            CXON_FIELD(user),
                            CXON_FIELD(source),
                            CXON_FIELD(id_str),
                            CXON_FIELD(metadata),
                            CXON_FIELD(truncated),
                            CXON_FIELD(created_at),
                            CXON_FIELD(coordinates),
                            CXON_FIELD(contributors),
                            CXON_FIELD(retweeted),
                            CXON_FIELD(place),
                            CXON_FIELD(favorited),
                            CXON_FIELD(lang),
                            CXON_FIELD(retweet_count),
                            CXON_FIELD(geo),
                            CXON_FIELD(entities),
                            CXON_FIELD(favorite_count),
                            CXON_FIELD(in_reply_to_user_id),
                            CXON_FIELD(in_reply_to_status_id),
                            CXON_FIELD(retweeted_status),
                            CXON_FIELD(in_reply_to_screen_name),
                            CXON_FIELD(in_reply_to_user_id_str),
                            CXON_FIELD(in_reply_to_status_id_str),
                            CXON_FIELD(possibly_sensitive)
                        };
    #               undef CXON_FIELD

                    if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
                        unsigned int key = hash(str, len);
                        if (key <= MAX_HASH_VALUE && wordlist[key].call)
                            return wordlist[key].call(t, i, e, cx);
                    }
                    return false;
                }
#       endif // CXON_USE_GPERF
    };
    
    struct search_metadata {
        float completed_in;
        unsigned long long max_id;
        std::string max_id_str;
        std::string next_results;
        std::string query;
        std::string refresh_url;
        unsigned count;
        unsigned since_id;
        std::string since_id_str;

        CXON_JSON_CLS_MEMBER(search_metadata,
            CXON_JSON_CLS_FIELD_ASIS(completed_in),
            CXON_JSON_CLS_FIELD_ASIS(max_id),
            CXON_JSON_CLS_FIELD_ASIS(max_id_str),
            CXON_JSON_CLS_FIELD_ASIS(next_results),
            CXON_JSON_CLS_FIELD_ASIS(query),
            CXON_JSON_CLS_FIELD_ASIS(refresh_url),
            CXON_JSON_CLS_FIELD_ASIS(count),
            CXON_JSON_CLS_FIELD_ASIS(since_id),
            CXON_JSON_CLS_FIELD_ASIS(since_id_str)
        )
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct object {
        std::vector<status> statuses;
        struct search_metadata search_metadata;

        CXON_JSON_CLS_MEMBER(object, CXON_JSON_CLS_FIELD_ASIS(statuses), CXON_JSON_CLS_FIELD_ASIS(search_metadata))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

}}

#endif // CXON_HAS_LIB_STD_OPTIONAL

#endif // CXON_JSON_TEST_NATIVE_TWITTER_HXX_
