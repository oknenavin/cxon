// Copyright (c) 2017-2024 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_NATIVE_INSTRUMENTS_HXX_
#define CXON_JSON_TEST_NATIVE_INSTRUMENTS_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"


#ifdef CXON_HAS_LIB_STD_OPTIONAL

namespace test { namespace instruments {

    struct envelope_node {
        unsigned tick;
        unsigned value;

        CXON_JSON_CLS_MEMBER(envelope_node, CXON_JSON_CLS_FIELD_ASIS(tick), CXON_JSON_CLS_FIELD_ASIS(value))
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct envelope {
        unsigned loop_end;
        unsigned loop_start;
        std::vector<envelope_node> nodes;
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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct instrument {
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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct pattern_data {
        unsigned channel;
        unsigned fxcmd;
        unsigned fxparam;
        unsigned instr;
        unsigned note;
        unsigned row;
        unsigned volcmd;
        unsigned volval;

        CXON_JSON_CLS_MEMBER(pattern_data,
            CXON_JSON_CLS_FIELD_ASIS(channel),
            CXON_JSON_CLS_FIELD_ASIS(fxcmd),
            CXON_JSON_CLS_FIELD_ASIS(fxparam),
            CXON_JSON_CLS_FIELD_ASIS(instr),
            CXON_JSON_CLS_FIELD_ASIS(note),
            CXON_JSON_CLS_FIELD_ASIS(row),
            CXON_JSON_CLS_FIELD_ASIS(volcmd),
            CXON_JSON_CLS_FIELD_ASIS(volval)
        )
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct pattern {
        std::optional<std::vector<pattern_data>> data;
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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

    struct object {
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
        CXON_JSON_CLS_SIMPLE_KEY_MEMBER()
    };

}}

#endif // CXON_HAS_LIB_STD_OPTIONAL

#endif // CXON_JSON_TEST_NATIVE_INSTRUMENTS_HXX_
