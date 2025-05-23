#pragma once

#include "util/types.h"
#include "util/log.h"

#include <glm/glm.hpp>

#include <vector>
#include <numeric>

namespace ppp
{
    namespace resources
    {
        class imaterial;
    }

    namespace render
    {
        //-------------------------------------------------------------------------
        enum class attribute_type
        {
            POSITION,
            NORMAL,
            TEXCOORD,
            COLOR,
            MATERIAL_INDEX,
            WORLD_MATRIX
        };

        //-------------------------------------------------------------------------
        inline s32 component_count_for_attribute(attribute_type type)
        {
            switch (type)
            {
            case attribute_type::POSITION:               return 3;
            case attribute_type::NORMAL:                 return 3;
            case attribute_type::TEXCOORD:               return 2;
            case attribute_type::COLOR:                  return 4;

            case attribute_type::MATERIAL_INDEX:  return 1;
            }
            return 0;  // Fallback to avoid compiler warnings
        }

        //-------------------------------------------------------------------------
        enum class attribute_data_type
        {
            FLOAT,
            UNSIGNED_INT,
            INT
        };

        //-------------------------------------------------------------------------
        inline s32 size_in_bytes_for_data_type(attribute_data_type type)
        {
            switch (type)
            {
            case attribute_data_type::FLOAT:               return 4;
            case attribute_data_type::UNSIGNED_INT:        return 4;
            case attribute_data_type::INT:                 return 4;
            }

            assert(false);
            return 0;  // Fallback to avoid compiler warnings
        }

        //-------------------------------------------------------------------------
        struct attribute_layout
        {
            attribute_type type;             // type of the attribute, these are similar to the semantics in shaders (POSITION, NORMAL, TEXCOORD0)
            attribute_data_type data_type;   // data type of the attribute (FLOAT, UNSIGNED INT, VEC2, VEC4, ...)

            s32 idx;        // attribute index 
            s32 count;      // attribute component count, for instance glm::vec3 component count == 3
            s32 span;       // attribute span count, for instance glm::vec4 has a span of 1, glm::mat4 has a span of 4
            s32 normalized; // attribute values are normalized
            u64 stride;     // size of the full vertex type ( so stride of a vertex with position (vec3) and color (vec4) would be (3 * 4) + (4 * 4) 
            u64 offset;     // offset in the vertex type ( so offset of color within a vertex with position (vec3) and color (vec4) would be 12

            u64 element_size_in_bytes() const { return size_in_bytes_for_data_type(data_type); }
            u64 total_size_in_bytes() const { return span * count * element_size_in_bytes(); }
        };

        //-------------------------------------------------------------------------
        inline u64 calculate_total_size_layout(const attribute_layout* layouts, u64 layout_count)
        {
            return std::accumulate(layouts, layouts + layout_count, 0ULL,
                [](u64 sum, const attribute_layout& layout)
            {
                return sum + layout.total_size_in_bytes();
            });
        }

        //-------------------------------------------------------------------------
        class irender_item
        {
        public:
            virtual ~irender_item() = default;

            virtual bool has_smooth_normals() const = 0;
            virtual bool has_textures() const = 0;
            virtual bool cast_shadows() const = 0;

            virtual u32 vertex_count() const = 0;
            virtual u32 index_count() const = 0;

            virtual const std::vector<glm::vec3>& vertex_positions() const = 0;
            virtual const std::vector<glm::vec3>& vertex_normals() const = 0;
            virtual const std::vector<glm::vec2>& vertex_uvs() const = 0;

            virtual const std::vector<face>& faces() const = 0;

            virtual const u64 geometry_id() const = 0;
            virtual const u64 material_id() const = 0;

            virtual const resources::imaterial* material() const = 0;
        };

        namespace conversions
        {
            //-------------------------------------------------------------------------
            inline std::string to_string(attribute_type type)
            {
                switch (type)
                {
                case attribute_type::POSITION:                 return "POSITION|VEC3";
                case attribute_type::NORMAL:                   return "NORMAL|VEC3";
                case attribute_type::TEXCOORD:                 return "TEXCOORD|VEC2";
                case attribute_type::COLOR:                    return "COLOR|VEC4";
                case attribute_type::MATERIAL_INDEX:           return "DIFFUSE TEXTURE INDEX|INT";
                case attribute_type::WORLD_MATRIX:             return "WORLD MATRIX|MAT4";
                default:
                    assert(false && "Unknown attribute type");
                    return {};
                }
            }
        }
    }
}