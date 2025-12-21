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

#ifndef CXON_JSON_TEST_NATIVE_MESH_HXX_
#define CXON_JSON_TEST_NATIVE_MESH_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/utility.hxx"
#include "cxon/lib/std/vector.hxx"


namespace test { namespace mesh {

    struct batch {
        std::pair<unsigned, unsigned> indexRange;
        std::pair<unsigned, unsigned> vertexRange;
        std::vector<unsigned> usedBones;

        CXON_JSON_CLS_MEMBER(batch, CXON_JSON_CLS_FIELD_ASIS(indexRange), CXON_JSON_CLS_FIELD_ASIS(vertexRange), CXON_JSON_CLS_FIELD_ASIS(usedBones))
    };

    struct morph_targets {
        CXON_JSON_CLS_MEMBER(morph_targets)
    };

    struct object {
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

}}

#endif // CXON_JSON_TEST_NATIVE_MESH_HXX_
