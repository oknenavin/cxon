// Copyright (c) 2017-2025 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_NATIVE_EMOJI_HXX_
#define CXON_JSON_TEST_NATIVE_EMOJI_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"


namespace test { namespace emoji {

    struct entry {
        std::string emoji;
        std::string description;
        std::string category;
        std::vector<std::string> aliases;
        std::vector<std::string> tags;
        std::string unicode_version;
        std::string ios_version;
        bool skin_tones;

        CXON_JSON_CLS_MEMBER(entry,
            CXON_JSON_CLS_FIELD_ASIS_DFLT(emoji             , self.emoji.empty()),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(description       , self.description.empty()),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(category          , self.category.empty()),
            CXON_JSON_CLS_FIELD_ASIS(aliases),
            CXON_JSON_CLS_FIELD_ASIS(tags),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(unicode_version   , self.emoji.empty()),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(ios_version       , self.ios_version.empty()),
            CXON_JSON_CLS_FIELD_ASIS_DFLT(skin_tones        , !self.skin_tones)
        )
    };
    using object = std::vector<entry>;

}}

#endif // CXON_JSON_TEST_NATIVE_EMOJI_HXX_
