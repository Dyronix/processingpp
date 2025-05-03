#include "render/opengl/render_gl_util.h"

#include "render/helpers/render_vertex_layouts.h"

#include "resources/shader_pool.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        GLenum gl_data_type(attribute_data_type type)
        {
            switch (type)
            {
            case attribute_data_type::FLOAT: return GL_FLOAT;
            case attribute_data_type::UNSIGNED_INT: return GL_UNSIGNED_INT;
            case attribute_data_type::INT: return GL_INT;
            }

            assert(false);
            return 0;  // Fallback to avoid compiler warnings
        }
        //-------------------------------------------------------------------------
        GLenum gl_topology_type(topology_type type)
        {
            switch (type)
            {
            case topology_type::POINTS: return GL_POINTS;
            case topology_type::LINES: return GL_LINES;
            case topology_type::TRIANGLES: return GL_TRIANGLES;
            }

            log::error("Invalid topology_type type specified, using GL_TRIANGLES");
            return GL_TRIANGLES;
        }
        //-------------------------------------------------------------------------
        GLenum gl_index_type()
        {
            if (sizeof(index) == sizeof(u32)) return GL_UNSIGNED_INT;
            if (sizeof(index) == sizeof(u16)) return GL_UNSIGNED_SHORT;

            log::error("Invalid index type specified: {}, using UNSIGNED_INT", typeid(index).name());
            return GL_UNSIGNED_INT;
        }

        //-------------------------------------------------------------------------
        const attribute_layout* get_attribute_layout(string::string_id shader_tag)
        {
            auto shader_program = shader_pool::get_shader_program(shader_tag);

            return fill_user_layout(shader_program->vertex_format());
        }
        //-------------------------------------------------------------------------
        u32 get_attribute_layout_count(string::string_id shader_tag)
        {
            auto shader_program = shader_pool::get_shader_program(shader_tag);
            auto layout_count = fill_user_layout_count(shader_program->vertex_format());

            assert(layout_count == static_cast<u32>(layout_count));

            return static_cast<u32>(layout_count);
        }
        //-------------------------------------------------------------------------
        const attribute_layout* layouts(string::string_id shader_tag)
        {
            return get_attribute_layout(shader_tag);
        }
        //-------------------------------------------------------------------------
        u32 layout_count(string::string_id shader_tag)
        {
            return get_attribute_layout_count(shader_tag);
        }

        //-------------------------------------------------------------------------
        void check_drawing_type(u32 index_count, GLenum type)
        {
            if (index_count == 0)
            {
                return;
            }

            switch (type)
            {
            case GL_LINES:
                if (index_count % 2 != 0)
                {
                    log::error("Trying to render invalid number of lines: {}", index_count);
                    return;
                }
                break;
            case GL_TRIANGLES:
                if (index_count % 3 != 0)
                {
                    log::error("Trying to render invalid number of triangles: {}", index_count);
                    return;
                }
                break;
            default:
                assert(false);
            }
        }
    }
}