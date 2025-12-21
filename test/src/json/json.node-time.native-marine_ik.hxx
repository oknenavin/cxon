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

#ifndef CXON_JSON_TEST_NATIVE_MARINE_IK_HXX_
#define CXON_JSON_TEST_NATIVE_MARINE_IK_HXX_

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/array.hxx"
#include "cxon/lib/std/vector.hxx"


namespace test { namespace marine_ik {

    struct image {
        std::string url;
        std::string uuid;
        std::string name;

        CXON_JSON_CLS_MEMBER(image, CXON_JSON_CLS_FIELD_ASIS(url), CXON_JSON_CLS_FIELD_ASIS(uuid), CXON_JSON_CLS_FIELD_ASIS(name))
    };

    struct key {
        std::array<float, 3> pos;
        float time;
        std::array<float, 3> scl;
        std::array<float, 4> rot;

        CXON_JSON_CLS_MEMBER(key, CXON_JSON_CLS_FIELD_ASIS(pos), CXON_JSON_CLS_FIELD_ASIS(time), CXON_JSON_CLS_FIELD_ASIS(scl), CXON_JSON_CLS_FIELD_ASIS(rot))
    };

    struct element {
        int parent;
        std::vector<key> keys;

        CXON_JSON_CLS_MEMBER(element, CXON_JSON_CLS_FIELD_ASIS(parent), CXON_JSON_CLS_FIELD_ASIS(keys))
    };

    struct data_metadata {
        unsigned uvs;
        unsigned version;
        unsigned faces;
        std::string generator;
        unsigned normals;
        unsigned bones;
        unsigned vertices;

        CXON_JSON_CLS_MEMBER(data_metadata,
            CXON_JSON_CLS_FIELD_ASIS(uvs),
            CXON_JSON_CLS_FIELD_ASIS(version),
            CXON_JSON_CLS_FIELD_ASIS(faces),
            CXON_JSON_CLS_FIELD_ASIS(generator),
            CXON_JSON_CLS_FIELD_ASIS(normals),
            CXON_JSON_CLS_FIELD_ASIS(bones),
            CXON_JSON_CLS_FIELD_ASIS(vertices)
        )
    };

    struct frame {
        std::vector<element> hierarchy;
        float length;
        unsigned fps;
        std::string name;

        CXON_JSON_CLS_MEMBER(frame, CXON_JSON_CLS_FIELD_ASIS(hierarchy), CXON_JSON_CLS_FIELD_ASIS(length), CXON_JSON_CLS_FIELD_ASIS(fps), CXON_JSON_CLS_FIELD_ASIS(name))
    };

    struct bone {
        int parent;
        std::array<float, 3> pos;
        std::array<float, 4> rotq;
        std::array<float, 3> scl;
        std::string name;

        CXON_JSON_CLS_MEMBER(bone,
            CXON_JSON_CLS_FIELD_ASIS(parent),
            CXON_JSON_CLS_FIELD_ASIS(pos),
            CXON_JSON_CLS_FIELD_ASIS(rotq),
            CXON_JSON_CLS_FIELD_ASIS(scl),
            CXON_JSON_CLS_FIELD_ASIS(name)
        )
    };

    struct data {
        std::vector<std::vector<float>> uvs;
        std::vector<frame> animations;
        std::vector<float> vertices;
        struct data_metadata metadata;
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

        CXON_JSON_CLS_MEMBER(geometry, CXON_JSON_CLS_FIELD_ASIS(type), CXON_JSON_CLS_FIELD_ASIS(uuid), CXON_JSON_CLS_FIELD_ASIS(data))
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

    struct object_metadata {
        std::string sourceFile;
        std::string generator;
        std::string type;
        float version;

        CXON_JSON_CLS_MEMBER(object_metadata, CXON_JSON_CLS_FIELD_ASIS(sourceFile), CXON_JSON_CLS_FIELD_ASIS(generator), CXON_JSON_CLS_FIELD_ASIS(type), CXON_JSON_CLS_FIELD_ASIS(version))
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

    struct object_ {
        std::vector<child> children;
        std::string type;
        std::vector<int> matrix;
        std::string uuid;

        CXON_JSON_CLS_MEMBER(object_, CXON_JSON_CLS_FIELD_ASIS(children), CXON_JSON_CLS_FIELD_ASIS(type), CXON_JSON_CLS_FIELD_ASIS(matrix), CXON_JSON_CLS_FIELD_ASIS(uuid))
    };

    struct animation {
        std::vector<int> tracks;
        unsigned fps;
        std::string name;

        CXON_JSON_CLS_MEMBER(animation, CXON_JSON_CLS_FIELD_ASIS(tracks), CXON_JSON_CLS_FIELD_ASIS(fps), CXON_JSON_CLS_FIELD_ASIS(name))
    };

    struct object {
        std::vector<image> images;
        std::vector<geometry> geometries;
        std::vector<texture> textures;
        struct object_metadata metadata;
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

}}

#endif // CXON_JSON_TEST_NATIVE_MARINE_IK_HXX_
