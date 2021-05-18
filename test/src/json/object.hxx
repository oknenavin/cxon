// Copyright (c) 2017-2021 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_OBJECT_HXX_
#define CXON_JSON_TEST_OBJECT_HXX_

#include "cxon/json.hxx"

#include "cxon/lib/node.ordered.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/utility.hxx"
#include "cxon/lib/std/vector.hxx"

namespace test {

    namespace blns {

        using object = std::vector<std::string>;

    }

    namespace emoji {

        struct entry {
            std::string emoji;
            std::string description;
            std::string category;
            std::vector<std::string> aliases;
            std::vector<std::string> tags;
            std::string unicode_version;
            std::string ios_version;

            CXON_JSON_CLS_MEMBER(entry,
                CXON_JSON_CLS_FIELD_ASIS(emoji),
                CXON_JSON_CLS_FIELD_ASIS(description),
                CXON_JSON_CLS_FIELD_ASIS(category),
                CXON_JSON_CLS_FIELD_ASIS(aliases),
                CXON_JSON_CLS_FIELD_ASIS(tags),
                CXON_JSON_CLS_FIELD_ASIS(unicode_version),
                CXON_JSON_CLS_FIELD_ASIS(ios_version)
            )
        };

        using object = std::vector<entry>;

    }

    namespace apache_builds {

        struct object {
            struct job {
                std::string name;
                std::string url;
                std::string color;

                CXON_JSON_CLS_MEMBER(job,
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(url),
                    CXON_JSON_CLS_FIELD_ASIS(color)
                )
            };

            struct view {
                std::string name;
                std::string url;

                CXON_JSON_CLS_MEMBER(view,
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(url)
                )
            };

            std::vector<std::map<std::string, std::string>> assignedLabels;
            std::string mode;
            std::string nodeDescription;
            std::string nodeName;
            unsigned numExecutors;
            std::string description;
            std::vector<job> jobs;
            std::map<std::string, std::string> overallLoad;
            std::map<std::string, std::string> primaryView;
            bool quietingDown;
            unsigned slaveAgentPort;
            std::map<std::string, std::string> unlabeledLoad;
            bool useCrumbs;
            bool useSecurity;
            std::vector<view> views;

            CXON_JSON_CLS_MEMBER(object,
                CXON_JSON_CLS_FIELD_ASIS(assignedLabels),
                CXON_JSON_CLS_FIELD_ASIS(mode),
                CXON_JSON_CLS_FIELD_ASIS(nodeDescription),
                CXON_JSON_CLS_FIELD_ASIS(nodeName),
                CXON_JSON_CLS_FIELD_ASIS(numExecutors),
                CXON_JSON_CLS_FIELD_ASIS(description),
                CXON_JSON_CLS_FIELD_ASIS(jobs),
                CXON_JSON_CLS_FIELD_ASIS(overallLoad),
                CXON_JSON_CLS_FIELD_ASIS(primaryView),
                CXON_JSON_CLS_FIELD_ASIS(quietingDown),
                CXON_JSON_CLS_FIELD_ASIS(slaveAgentPort),
                CXON_JSON_CLS_FIELD_ASIS(unlabeledLoad),
                CXON_JSON_CLS_FIELD_ASIS(useCrumbs),
                CXON_JSON_CLS_FIELD_ASIS(useSecurity),
                CXON_JSON_CLS_FIELD_ASIS(views)
            )
        };

    }

    namespace canada {

        struct object {
            struct feature {
                struct geometry {
                    std::string type;
                    std::vector<std::vector<std::pair<double, double>>> coordinates;

                    CXON_JSON_CLS_MEMBER(geometry,
                        CXON_JSON_CLS_FIELD_ASIS(type),
                        CXON_JSON_CLS_FIELD_ASIS(coordinates)
                    )
                };

                std::string type;
                std::map<std::string, std::string> properties;
                struct geometry geometry;

                CXON_JSON_CLS_MEMBER(feature,
                    CXON_JSON_CLS_FIELD_ASIS(type),
                    CXON_JSON_CLS_FIELD_ASIS(properties),
                    CXON_JSON_CLS_FIELD_ASIS(geometry)
                )
            };

            std::string type;
            std::vector<feature> features;

            CXON_JSON_CLS_MEMBER(object,
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(features)
            )
        };

    }

    #ifdef CXON_HAS_LIB_STD_OPTIONAL
        namespace citm_catalog {

            struct object {
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

                struct performance {
                    struct price {
                        unsigned long long amount;
                        unsigned long long audienceSubCategoryId;
                        unsigned long long seatCategoryId;

                        CXON_JSON_CLS_MEMBER(price,
                            CXON_JSON_CLS_FIELD_ASIS(amount),
                            CXON_JSON_CLS_FIELD_ASIS(audienceSubCategoryId),
                            CXON_JSON_CLS_FIELD_ASIS(seatCategoryId)
                        )
                    };

                    struct seat {
                        struct area {
                            unsigned long long areaId;
                            std::vector<unsigned long long> blockIds;

                            CXON_JSON_CLS_MEMBER(area,
                                CXON_JSON_CLS_FIELD_ASIS(areaId),
                                CXON_JSON_CLS_FIELD_ASIS(blockIds)
                            )
                        };

                        std::vector<area> areas;
                        unsigned long long seatCategoryId;

                        CXON_JSON_CLS_MEMBER(seat,
                            CXON_JSON_CLS_FIELD_ASIS(areas),
                            CXON_JSON_CLS_FIELD_ASIS(seatCategoryId)
                        )
                    };

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

                cxon::ordered::object<std::string, std::string> areaNames;
                cxon::ordered::object<std::string, std::string> audienceSubCategoryNames;
                cxon::ordered::object<std::string, std::string> blockNames;
                cxon::ordered::object<std::string, event> events;
                std::vector<performance> performances;
                cxon::ordered::object<std::string, std::string> seatCategoryNames;
                cxon::ordered::object<std::string, std::string> subTopicNames;
                cxon::ordered::object<std::string, std::string> subjectNames;
                cxon::ordered::object<std::string, std::string> topicNames;
                cxon::ordered::object<std::string, std::vector<unsigned long long>> topicSubTopics;
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

        }
    #endif

    namespace gsoc_2018 {

        struct participant {
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

                CXON_JSON_CLS_MEMBER(author,
                    CXON_JSON_CLS_FIELD_NAME("@type", type),
                    CXON_JSON_CLS_FIELD_ASIS(name)
                )
            };

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

    }

    #ifdef CXON_HAS_LIB_STD_OPTIONAL
        namespace instruments {

            struct object {
                struct instrument {
                    struct envelope {
                        struct node {
                            unsigned tick;
                            unsigned value;

                            CXON_JSON_CLS_MEMBER(node,
                                CXON_JSON_CLS_FIELD_ASIS(tick),
                                CXON_JSON_CLS_FIELD_ASIS(value)
                            )
                        };

                        unsigned loop_end;
                        unsigned loop_start;
                        std::vector<node> nodes;
                        unsigned release_node;
                        unsigned sustain_end;
                        unsigned sustain_start;

                        CXON_JSON_CLS_MEMBER(envelope,
                            CXON_JSON_CLS_FIELD_ASIS(loop_end),
                            CXON_JSON_CLS_FIELD_ASIS(loop_start),
                            CXON_JSON_CLS_FIELD_ASIS(nodes),
                            CXON_JSON_CLS_FIELD_ASIS(release_node),
                            CXON_JSON_CLS_FIELD_ASIS(sustain_end),
                            CXON_JSON_CLS_FIELD_ASIS(sustain_start)
                        )
                    };

                    unsigned default_filter_cutoff;
                    bool default_filter_cutoff_enabled;
                    unsigned default_filter_mode;
                    unsigned default_filter_resonance;
                    bool default_filter_resonance_enabled;
                    unsigned default_pan;
                    unsigned duplicate_check_type;
                    unsigned duplicate_note_action;
                    unsigned fadeout;
                    unsigned global_volume;
                    unsigned graph_insert;
                    std::string legacy_filename;
                    unsigned midi_bank;
                    unsigned midi_channel;
                    unsigned midi_drum_set;
                    unsigned midi_program;
                    std::string name;
                    unsigned new_note_action;
                    std::nullptr_t note_map;
                    envelope panning_envelope;
                    envelope pitch_envelope;
                    unsigned pitch_pan_center;
                    unsigned pitch_pan_separation;
                    unsigned pitch_to_tempo_lock;
                    unsigned random_cutoff_weight;
                    unsigned random_pan_weight;
                    unsigned random_resonance_weight;
                    unsigned random_volume_weight;
                    std::nullptr_t sample_map;
                    std::nullptr_t tuning;
                    envelope volume_envelope;
                    unsigned volume_ramp_down;
                    unsigned volume_ramp_up;

                    CXON_JSON_CLS_MEMBER(instrument,
                        CXON_JSON_CLS_FIELD_ASIS(default_filter_cutoff),
                        CXON_JSON_CLS_FIELD_ASIS(default_filter_cutoff_enabled),
                        CXON_JSON_CLS_FIELD_ASIS(default_filter_mode),
                        CXON_JSON_CLS_FIELD_ASIS(default_filter_resonance),
                        CXON_JSON_CLS_FIELD_ASIS(default_filter_resonance_enabled),
                        CXON_JSON_CLS_FIELD_ASIS(default_pan),
                        CXON_JSON_CLS_FIELD_ASIS(duplicate_check_type),
                        CXON_JSON_CLS_FIELD_ASIS(duplicate_note_action),
                        CXON_JSON_CLS_FIELD_ASIS(fadeout),
                        CXON_JSON_CLS_FIELD_ASIS(global_volume),
                        CXON_JSON_CLS_FIELD_ASIS(graph_insert),
                        CXON_JSON_CLS_FIELD_ASIS(legacy_filename),
                        CXON_JSON_CLS_FIELD_ASIS(midi_bank),
                        CXON_JSON_CLS_FIELD_ASIS(midi_channel),
                        CXON_JSON_CLS_FIELD_ASIS(midi_drum_set),
                        CXON_JSON_CLS_FIELD_ASIS(midi_program),
                        CXON_JSON_CLS_FIELD_ASIS(name),
                        CXON_JSON_CLS_FIELD_ASIS(new_note_action),
                        CXON_JSON_CLS_FIELD_ASIS(note_map),
                        CXON_JSON_CLS_FIELD_ASIS(panning_envelope),
                        CXON_JSON_CLS_FIELD_ASIS(pitch_envelope),
                        CXON_JSON_CLS_FIELD_ASIS(pitch_pan_center),
                        CXON_JSON_CLS_FIELD_ASIS(pitch_pan_separation),
                        CXON_JSON_CLS_FIELD_ASIS(pitch_to_tempo_lock),
                        CXON_JSON_CLS_FIELD_ASIS(random_cutoff_weight),
                        CXON_JSON_CLS_FIELD_ASIS(random_pan_weight),
                        CXON_JSON_CLS_FIELD_ASIS(random_resonance_weight),
                        CXON_JSON_CLS_FIELD_ASIS(random_volume_weight),
                        CXON_JSON_CLS_FIELD_ASIS(sample_map),
                        CXON_JSON_CLS_FIELD_ASIS(tuning),
                        CXON_JSON_CLS_FIELD_ASIS(volume_envelope),
                        CXON_JSON_CLS_FIELD_ASIS(volume_ramp_down),
                        CXON_JSON_CLS_FIELD_ASIS(volume_ramp_up)
                    )
                };

                struct pattern {
                    struct data {
                        unsigned channel;
                        unsigned fxcmd;
                        unsigned fxparam;
                        unsigned instr;
                        unsigned note;
                        unsigned row;
                        unsigned volcmd;
                        unsigned volval;

                        CXON_JSON_CLS_MEMBER(data,
                            CXON_JSON_CLS_FIELD_ASIS(channel),
                            CXON_JSON_CLS_FIELD_ASIS(fxcmd),
                            CXON_JSON_CLS_FIELD_ASIS(fxparam),
                            CXON_JSON_CLS_FIELD_ASIS(instr),
                            CXON_JSON_CLS_FIELD_ASIS(note),
                            CXON_JSON_CLS_FIELD_ASIS(row),
                            CXON_JSON_CLS_FIELD_ASIS(volcmd),
                            CXON_JSON_CLS_FIELD_ASIS(volval)
                        )
                    };

                    std::optional<std::vector<data>> data;
                    std::string name;
                    unsigned rows;
                    unsigned rows_per_beat;
                    unsigned rows_per_measure;

                    CXON_JSON_CLS_MEMBER(pattern,
                        CXON_JSON_CLS_FIELD_ASIS(data),
                        CXON_JSON_CLS_FIELD_ASIS(name),
                        CXON_JSON_CLS_FIELD_ASIS(rows),
                        CXON_JSON_CLS_FIELD_ASIS(rows_per_beat),
                        CXON_JSON_CLS_FIELD_ASIS(rows_per_measure)
                    )
                };

                struct sample {
                    unsigned c5_samplerate;
                    unsigned global_volume;
                    std::string legacy_filename;
                    unsigned length;
                    unsigned loop_end;
                    unsigned loop_start;
                    std::string name;
                    unsigned pan;
                    unsigned sustain_end;
                    unsigned sustain_start;
                    unsigned vibrato_depth;
                    unsigned vibrato_rate;
                    unsigned vibrato_sweep;
                    unsigned vibrato_type;
                    unsigned volume;

                    CXON_JSON_CLS_MEMBER(sample,
                        CXON_JSON_CLS_FIELD_ASIS(c5_samplerate),
                        CXON_JSON_CLS_FIELD_ASIS(global_volume),
                        CXON_JSON_CLS_FIELD_ASIS(legacy_filename),
                        CXON_JSON_CLS_FIELD_ASIS(length),
                        CXON_JSON_CLS_FIELD_ASIS(loop_end),
                        CXON_JSON_CLS_FIELD_ASIS(loop_start),
                        CXON_JSON_CLS_FIELD_ASIS(name),
                        CXON_JSON_CLS_FIELD_ASIS(pan),
                        CXON_JSON_CLS_FIELD_ASIS(sustain_end),
                        CXON_JSON_CLS_FIELD_ASIS(sustain_start),
                        CXON_JSON_CLS_FIELD_ASIS(vibrato_depth),
                        CXON_JSON_CLS_FIELD_ASIS(vibrato_rate),
                        CXON_JSON_CLS_FIELD_ASIS(vibrato_sweep),
                        CXON_JSON_CLS_FIELD_ASIS(vibrato_type),
                        CXON_JSON_CLS_FIELD_ASIS(volume)
                    )
                };

                std::nullptr_t graphstate;
                std::vector<instrument> instruments;
                std::optional<std::string> message;
                std::string name;
                std::nullptr_t orderlist;
                std::vector<pattern> patterns;
                std::nullptr_t pluginstate;
                std::vector<sample> samples;
                unsigned short version;

                CXON_JSON_CLS_MEMBER(object,
                    CXON_JSON_CLS_FIELD_ASIS(graphstate),
                    CXON_JSON_CLS_FIELD_ASIS(instruments),
                    CXON_JSON_CLS_FIELD_ASIS(message),
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(orderlist),
                    CXON_JSON_CLS_FIELD_ASIS(patterns),
                    CXON_JSON_CLS_FIELD_ASIS(pluginstate),
                    CXON_JSON_CLS_FIELD_ASIS(samples),
                    CXON_JSON_CLS_FIELD_ASIS(version)
                )
            };

        }
    #endif

    namespace marine_ik {

        struct image {
            std::string url;
            std::string uuid;
            std::string name;

            CXON_JSON_CLS_MEMBER(image,
                CXON_JSON_CLS_FIELD_ASIS(url),
                CXON_JSON_CLS_FIELD_ASIS(uuid),
                CXON_JSON_CLS_FIELD_ASIS(name)
            )
        };

        struct data {
            struct frame {
                struct element {
                    struct key {
                        std::vector<float> pos;
                        float time;
                        std::vector<float> scl;
                        std::vector<float> rot;

                        CXON_JSON_CLS_MEMBER(key,
                            CXON_JSON_CLS_FIELD_ASIS(pos),
                            CXON_JSON_CLS_FIELD_ASIS(time),
                            CXON_JSON_CLS_FIELD_ASIS(scl),
                            CXON_JSON_CLS_FIELD_ASIS(rot)
                        )
                    };
                    int parent;
                    std::vector<key> keys;

                    CXON_JSON_CLS_MEMBER(element,
                        CXON_JSON_CLS_FIELD_ASIS(parent),
                        CXON_JSON_CLS_FIELD_ASIS(keys)
                    )
                };

                std::vector<element> hierarchy;
                float length;
                unsigned fps;
                std::string name;

                CXON_JSON_CLS_MEMBER(frame,
                    CXON_JSON_CLS_FIELD_ASIS(hierarchy),
                    CXON_JSON_CLS_FIELD_ASIS(length),
                    CXON_JSON_CLS_FIELD_ASIS(fps),
                    CXON_JSON_CLS_FIELD_ASIS(name)
                )
            };

            struct metadata {
                unsigned uvs;
                unsigned version;
                unsigned faces;
                std::string generator;
                unsigned normals;
                unsigned bones;
                unsigned vertices;

                CXON_JSON_CLS_MEMBER(metadata,
                    CXON_JSON_CLS_FIELD_ASIS(uvs),
                    CXON_JSON_CLS_FIELD_ASIS(version),
                    CXON_JSON_CLS_FIELD_ASIS(faces),
                    CXON_JSON_CLS_FIELD_ASIS(generator),
                    CXON_JSON_CLS_FIELD_ASIS(normals),
                    CXON_JSON_CLS_FIELD_ASIS(bones),
                    CXON_JSON_CLS_FIELD_ASIS(vertices)
                )
            };

            struct bone {
                int parent;
                std::vector<float> pos;
                std::vector<float> rotq;
                std::vector<float> scl;
                std::string name;

                CXON_JSON_CLS_MEMBER(bone,
                    CXON_JSON_CLS_FIELD_ASIS(parent),
                    CXON_JSON_CLS_FIELD_ASIS(pos),
                    CXON_JSON_CLS_FIELD_ASIS(rotq),
                    CXON_JSON_CLS_FIELD_ASIS(scl),
                    CXON_JSON_CLS_FIELD_ASIS(name)
                )
            };

            std::vector<std::vector<float>> uvs;
            std::vector<frame> animations;
            std::vector<float> vertices;
            struct metadata metadata;
            std::string name;
            std::vector<float> skinWeights;
            std::vector<unsigned> skinIndices;
            unsigned influencesPerVertex;
            std::vector<float> normals;
            std::vector<bone> bones;
            std::vector<unsigned> faces;

            CXON_JSON_CLS_MEMBER(data,
                CXON_JSON_CLS_FIELD_ASIS(uvs),
                CXON_JSON_CLS_FIELD_ASIS(animations),
                CXON_JSON_CLS_FIELD_ASIS(vertices),
                CXON_JSON_CLS_FIELD_ASIS(metadata),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(skinWeights),
                CXON_JSON_CLS_FIELD_ASIS(skinIndices),
                CXON_JSON_CLS_FIELD_ASIS(influencesPerVertex),
                CXON_JSON_CLS_FIELD_ASIS(normals),
                CXON_JSON_CLS_FIELD_ASIS(bones),
                CXON_JSON_CLS_FIELD_ASIS(faces)
            )
        };


        struct geometry {
            std::string type;
            std::string uuid;
            struct data data;

            CXON_JSON_CLS_MEMBER(geometry,
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(uuid),
                CXON_JSON_CLS_FIELD_ASIS(data)
            )
        };

        struct texture {
            std::vector<int> repeat;
            std::vector<int> wrap;
            unsigned anisotropy;
            std::string image;
            std::string name;
            unsigned mapping;
            unsigned minFilter;
            std::string uuid;
            unsigned magFilter;

            CXON_JSON_CLS_MEMBER(texture,
                CXON_JSON_CLS_FIELD_ASIS(repeat),
                CXON_JSON_CLS_FIELD_ASIS(wrap),
                CXON_JSON_CLS_FIELD_ASIS(anisotropy),
                CXON_JSON_CLS_FIELD_ASIS(image),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(mapping),
                CXON_JSON_CLS_FIELD_ASIS(minFilter),
                CXON_JSON_CLS_FIELD_ASIS(uuid),
                CXON_JSON_CLS_FIELD_ASIS(magFilter)
            )
        };

        struct metadata {
            std::string sourceFile;
            std::string generator;
            std::string type;
            float version;

            CXON_JSON_CLS_MEMBER(metadata,
                CXON_JSON_CLS_FIELD_ASIS(sourceFile),
                CXON_JSON_CLS_FIELD_ASIS(generator),
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(version)
            )
        };

        struct material {
            unsigned vertexColors;
            std::string name;
            std::string type;
            std::string uuid;
            std::string blending;
            std::string map;
            bool transparent;
            bool depthTest;
            unsigned color;
            unsigned shininess;
            unsigned emissive;
            bool depthWrite;
            unsigned specular;

            CXON_JSON_CLS_MEMBER(material,
                CXON_JSON_CLS_FIELD_ASIS(vertexColors),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(uuid),
                CXON_JSON_CLS_FIELD_ASIS(blending),
                CXON_JSON_CLS_FIELD_ASIS(map),
                CXON_JSON_CLS_FIELD_ASIS(transparent),
                CXON_JSON_CLS_FIELD_ASIS(depthTest),
                CXON_JSON_CLS_FIELD_ASIS(color),
                CXON_JSON_CLS_FIELD_ASIS(shininess),
                CXON_JSON_CLS_FIELD_ASIS(emissive),
                CXON_JSON_CLS_FIELD_ASIS(depthWrite),
                CXON_JSON_CLS_FIELD_ASIS(specular)
            )
        };

        struct object_ {
            struct child {
                std::string name;
                std::string uuid;
                std::vector<int> matrix;
                bool visible;
                std::string type;
                std::string material;
                bool castShadow;
                bool receiveShadow;
                std::string geometry;

                CXON_JSON_CLS_MEMBER(child,
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(uuid),
                    CXON_JSON_CLS_FIELD_ASIS(matrix),
                    CXON_JSON_CLS_FIELD_ASIS(visible),
                    CXON_JSON_CLS_FIELD_ASIS(type),
                    CXON_JSON_CLS_FIELD_ASIS(material),
                    CXON_JSON_CLS_FIELD_ASIS(castShadow),
                    CXON_JSON_CLS_FIELD_ASIS(receiveShadow),
                    CXON_JSON_CLS_FIELD_ASIS(geometry)
                )
            };

            std::vector<child> children;
            std::string type;
            std::vector<int> matrix;
            std::string uuid;

            CXON_JSON_CLS_MEMBER(object_,
                CXON_JSON_CLS_FIELD_ASIS(children),
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(matrix),
                CXON_JSON_CLS_FIELD_ASIS(uuid)
            )
        };

        struct animation {
            std::vector<int> tracks;
            unsigned fps;
            std::string name;

            CXON_JSON_CLS_MEMBER(animation,
                CXON_JSON_CLS_FIELD_ASIS(tracks),
                CXON_JSON_CLS_FIELD_ASIS(fps),
                CXON_JSON_CLS_FIELD_ASIS(name)
            )
        };

        struct object {
            std::vector<image> images;
            std::vector<geometry> geometries;
            std::vector<texture> textures;
            struct metadata metadata;
            std::vector<material> materials;
            struct object_ object_;
            std::vector<animation> animations;

            CXON_JSON_CLS_MEMBER(object,
                CXON_JSON_CLS_FIELD_ASIS(images),
                CXON_JSON_CLS_FIELD_ASIS(geometries),
                CXON_JSON_CLS_FIELD_ASIS(textures),
                CXON_JSON_CLS_FIELD_ASIS(metadata),
                CXON_JSON_CLS_FIELD_ASIS(materials),
                CXON_JSON_CLS_FIELD_NAME("object", object_),
                CXON_JSON_CLS_FIELD_ASIS(animations)
            )
        };

    }

    namespace mesh {

        struct object {
            struct batch {
                std::pair<unsigned, unsigned> indexRange;
                std::pair<unsigned, unsigned> vertexRange;
                std::vector<unsigned> usedBones;

                CXON_JSON_CLS_MEMBER(batch,
                    CXON_JSON_CLS_FIELD_ASIS(indexRange),
                    CXON_JSON_CLS_FIELD_ASIS(vertexRange),
                    CXON_JSON_CLS_FIELD_ASIS(usedBones)
                )
            };

            struct morph_targets {
                bool dummy; // avoid compiler warning
                            // TODO: fix the macros to handle empty structs
                CXON_JSON_CLS_MEMBER(morph_targets,
                    CXON_JSON_CLS_FIELD_ASIS(dummy)
                )
            };

            std::vector<batch> batches;
            morph_targets morphTargets;
            std::vector<double> positions;
            std::vector<double> tex0;
            std::vector<unsigned> colors;
            std::vector<std::pair<float, unsigned>> influences;
            std::vector<double> normals;
            std::vector<unsigned> indices;

            CXON_JSON_CLS_MEMBER(object,
                CXON_JSON_CLS_FIELD_ASIS(batches),
                CXON_JSON_CLS_FIELD_ASIS(morphTargets),
                CXON_JSON_CLS_FIELD_ASIS(positions),
                CXON_JSON_CLS_FIELD_ASIS(tex0),
                CXON_JSON_CLS_FIELD_ASIS(colors),
                CXON_JSON_CLS_FIELD_ASIS(influences),
                CXON_JSON_CLS_FIELD_ASIS(normals),
                CXON_JSON_CLS_FIELD_ASIS(indices)
            )
        };

    }

    namespace numbers {

        using object = std::vector<double>;

    }

    namespace random {

        struct object {
            struct result {
                struct friend_ {
                    unsigned id;
                    std::string name;
                    std::string phone;

                    CXON_JSON_CLS_MEMBER(friend_,
                        CXON_JSON_CLS_FIELD_ASIS(id),
                        CXON_JSON_CLS_FIELD_ASIS(name),
                        CXON_JSON_CLS_FIELD_ASIS(phone)
                    )
                };

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

            unsigned id;
            std::string jsonrpc;
            unsigned total;
            std::vector<result> result;

            CXON_JSON_CLS_MEMBER(object,
                CXON_JSON_CLS_FIELD_ASIS(id),
                CXON_JSON_CLS_FIELD_ASIS(jsonrpc),
                CXON_JSON_CLS_FIELD_ASIS(total),
                CXON_JSON_CLS_FIELD_ASIS(result)
            )
        };

    }

    //namespace twitter {
    //
    //    struct object {
    //        struct status {
    //            struct metadata {
    //                std::string result_type;
    //                std::string iso_language_code;
    //            };
    //
    //            struct user {
    //                struct entity {
    //                };
    //
    //                unsigned long long id;
    //                std::string id_str;
    //                std::string name;
    //                std::string screen_name;
    //                std::string location;
    //                std::string description;
    //                std::optional<std::string> url;
    //                entity entities;
    //                bool protected_;
    //                unsigned followers_count;
    //                unsigned friends_count;
    //                unsigned listed_count;
    //                std::string created_at;
    //                unsigned favourites_count;
    //                std::optional<unsigned> utc_offset;
    //                std::optional<std::string> time_zone;
    //                bool geo_enabled;
    //                bool verified;
    //                unsigned statuses_count;
    //                std::string lang;
    //                bool contributors_enabled;
    //                bool is_translator;
    //                bool is_translation_enabled;
    //                std::string profile_background_color;
    //                std::string profile_background_image_url;
    //                std::string profile_background_image_url_https;
    //                bool profile_background_tile;
    //                std::string profile_image_url;
    //                std::string profile_image_url_https;
    //                std::string profile_banner_url;
    //                std::string profile_link_color;
    //                std::string profile_sidebar_border_color;
    //                std::string profile_sidebar_fill_color;
    //                std::string profile_text_color;
    //                bool profile_use_background_image;
    //                bool default_profile;
    //                bool default_profile_image;
    //                bool following;
    //                bool follow_request_sent;
    //                bool notifications;
    //            };
    //
    //            struct entity {
    //            };
    //
    //            metadata metadata;
    //            std::string created_at;
    //            unsigned long long id;
    //            std::string id_str;
    //            std::string text;
    //            std::string source;
    //            bool truncated;
    //            std::optional<unsigned long long> in_reply_to_status_id;
    //            std::optional<std::string> in_reply_to_status_id_str;
    //            std::optional<unsigned long long> in_reply_to_user_id;
    //            std::optional<std::string> in_reply_to_user_id_str;
    //            std::optional<std::string> in_reply_to_screen_name;
    //            user user;
    //            std::nullptr_t geo;
    //            std::nullptr_t coordinates;
    //            std::nullptr_t place;
    //            std::nullptr_t contributors;
    //            unsigned retweet_count;
    //            unsigned favorite_count;
    //            entity entities;
    //            bool favorited;
    //            bool retweeted;
    //            std::string lang;
    //        };
    //
    //        struct search_metadata {
    //            float completed_in;
    //            unsigned long long max_id;
    //            std::string max_id_str;
    //            std::string next_results;
    //            std::string query;
    //            std::string refresh_url;
    //            unsigned count;
    //            unsigned since_id;
    //            std::string since_id_str;
    //        };
    //
    //        std::vector<status> statuses;
    //        search_metadata search_metadata;
    //    };
    //
    //}

    namespace update_center {

        struct object {
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

            struct plugin {
                struct dependency {
                    std::string name;
                    bool optional;
                    std::string version;

                    CXON_JSON_CLS_MEMBER(dependency,
                        CXON_JSON_CLS_FIELD_ASIS(name),
                        CXON_JSON_CLS_FIELD_ASIS(optional),
                        CXON_JSON_CLS_FIELD_ASIS(version)
                    )
                };

                struct developer {
                    std::string developerId;
                    std::string email;
                    std::string name;

                    CXON_JSON_CLS_MEMBER(developer,
                        CXON_JSON_CLS_FIELD_ASIS(developerId),
                        CXON_JSON_CLS_FIELD_ASIS(email),
                        CXON_JSON_CLS_FIELD_ASIS(name)
                    )
                };

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

    }

}

#endif // CXON_JSON_TEST_OBJECT_HXX_
