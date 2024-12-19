#include "render/render_features.h"
#include "render/opengl/render_gl_error.h"

#include "glad/glad.h"

namespace ppp
{
    namespace render
    {
        //-------------------------------------------------------------------------
        static constexpr s32 _max_points = 9'000;
        static constexpr s32 _max_lines = 9'000;
        static constexpr s32 _max_triangles = 9'000;

        static constexpr s32 _max_texture_units = 8;

        //-------------------------------------------------------------------------
        u32 max_vertices(topology_type type)
        {
            s32 max_vertex_elements = 0;
            GL_CALL(glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_vertex_elements));

            s32 max_vertices = 0;
            switch (type)
            {
            case topology_type::POINTS:
                max_vertices = std::min(max_vertex_elements, _max_points);
                break;
            case topology_type::LINES:
                max_vertices = std::min(max_vertex_elements, _max_lines);
                break;
            case topology_type::TRIANGLES:
                max_vertices = std::min(max_vertex_elements, _max_triangles);
                break;
            }

            return max_vertices;
        }
        //-------------------------------------------------------------------------
        u32 max_indices(topology_type type)
        {
            s32 max_index_elements = 0;
            GL_CALL(glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &max_index_elements));

            s32 max_indices = 0;
            switch (type)
            {
            case topology_type::POINTS:
                max_indices = std::min(max_index_elements, _max_points * 1);
                break;
            case topology_type::LINES:
                max_indices = std::min(max_index_elements, _max_lines * 2);
                break;
            case topology_type::TRIANGLES:
                max_indices = std::min(max_index_elements, _max_triangles * 3);
                break;
            }

            return max_indices;
        }
        //-------------------------------------------------------------------------
        u32 max_textures()
        {
            s32 max_texture_units = 0;
            GL_CALL(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units));

            return std::min(max_texture_units, _max_texture_units);
        }
    }
}