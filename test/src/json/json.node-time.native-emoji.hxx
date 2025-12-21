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
