#pragma once

#include "render/render_item.h"
#include "render/render_types.h"

#include "string/string_id.h"

#include <glad/glad.h>

namespace ppp
{
    namespace render
    {
        // opengl data type conversions
        GLenum gl_data_type(attribute_data_type type);
        GLenum gl_topology_type(topology_type type);
        GLenum gl_index_type();

        // opengl attribute layout helpers
        const attribute_layout* get_attribute_layout(string::string_id shader_tag);
        u32                     get_attribute_layout_count(string::string_id shader_tag);

        const attribute_layout* layouts(string::string_id shader_tag);
        u32                     layout_count(string::string_id shader_tag);

        // opengl check for correct index drawing type
        void check_drawing_type(u32 index_count, GLenum type);
    }
}