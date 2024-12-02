#pragma once

#include "util/types.h"
#include "util/log.h"

#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <unordered_map>
#include <memory>

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        enum class vertex_attribute_type
        {
            POSITION,
            NORMAL,
            TEXCOORD,
            COLOR,
            DIFFUSE_TEXTURE_INDEX
        };

        //-------------------------------------------------------------------------
        enum class vertex_attribute_data_type
        {
            FLOAT,
            UNSIGNED_INT
        };

        //-------------------------------------------------------------------------
        inline s32 size_in_bytes_for_data_type(vertex_attribute_data_type type)
        {
            switch (type)
            {
            case vertex_attribute_data_type::FLOAT: return 4;
            case vertex_attribute_data_type::UNSIGNED_INT: return 4;
            }
            return 0;  // Fallback to avoid compiler warnings
        }

        //-------------------------------------------------------------------------
        inline s32 component_count_for_vertex_attribute(vertex_attribute_type type)
        {
            switch (type)
            {
            case vertex_attribute_type::POSITION:               return 3;
            case vertex_attribute_type::NORMAL:                 return 3;
            case vertex_attribute_type::TEXCOORD:               return 2;
            case vertex_attribute_type::COLOR:                  return 4;
            case vertex_attribute_type::DIFFUSE_TEXTURE_INDEX:  return 1;
            }
            return 0;  // Fallback to avoid compiler warnings
        }

        //-------------------------------------------------------------------------
        struct vertex_attribute_layout
        {
            vertex_attribute_type type;             // type of the attribute, these are similar to the semantics in shaders (POSITION, NORMAL, TEXCOORD0)
            vertex_attribute_data_type data_type;   // data type of the attribute (FLOAT, UNSIGNED INT)

            s32 idx;        // attribute index 
            s32 count;      // attribute component count, for instance glm::vec3 component count == 3
            s32 normalized; // attribute values are normalized
            u64 stride;     // size the full vertex type ( so stride of a vertex with position (vec3) and color (vec4) would be (3 * 4) + (4 * 4) 
            u64 offset;     // offset in the vertex type ( so offset of color within a vertex with position (vec3) and color (vec4) would be 12

            u64 element_size_in_bytes() const { return size_in_bytes_for_data_type(data_type); }
            u64 total_size_in_bytes() const { return count * element_size_in_bytes(); }
        };

        //-------------------------------------------------------------------------
        using vertex_attribute_blob = std::vector<u8>;

        //-------------------------------------------------------------------------
        struct vertex_attribute
        {
            vertex_attribute_layout layout;
            vertex_attribute_blob blob;
        };

        //-------------------------------------------------------------------------
        class irender_item
        {
        public:
            virtual u64 vertex_count() const = 0;
            virtual u64 index_count() const = 0;
            virtual u64 texture_count() const = 0;

            virtual const std::vector<glm::vec3>& vertex_positions() const = 0;
            virtual const std::vector<glm::vec3>& vertex_normals() const = 0;
            virtual const std::vector<glm::vec2>& vertex_uvs() const = 0;

            virtual const std::vector<face>& faces() const = 0;
            virtual const std::vector<texture_id>& texture_ids() const = 0;

            virtual const u64 id() const = 0;
        };
    }
}