#pragma once

#include "render/render.h"

#include "util/types.h"

#include <glm/glm.hpp>

#include <vector>

namespace ppp
{
    namespace geometry
    {
        namespace image
        {
            render::render_item extrude_image(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width);
            
            render::render_item make_image(bool from_corner, f32 x, f32 y, f32 w, f32 h, s32 image_id);
        }

        namespace font
        {
            render::render_item make_font(bool from_corner, f32 x, f32 y, f32 w, f32 h, f32 uv_start_x, f32 uv_start_y, f32 uv_end_x, f32 uv_end_y, s32 image_id);
        }

        template<typename TVertexType>
        static void extrude_vertices(std::vector<glm::vec3>& new_vertices, const TVertexType& center, const TVertexType* vertices, s32 vertex_count, f32 extrusion_width)
        {
            for (s32 i = 0; i < vertex_count; ++i)
            {
                glm::vec3 dir = glm::normalize(vertices[i] - center);

                glm::vec3 p1 = vertices[i];
                glm::vec3 p2 = p1 + (dir * extrusion_width);

                // We need to switch winding order if the inner stroke is enabled
                if (extrusion_width < 0.0f)
                {
                    // We will always render 2d geometry with an orthographic projection matrix
                    // Putting the z to 1.0f makes sure the depth buffer does not cull the inner stroke
                    p1.z = 1.0f;
                    p2.z = 1.0f;

                    new_vertices.push_back({ p2 });
                    new_vertices.push_back({ p1 });
                }
                else
                {
                    new_vertices.push_back({ p1 });
                    new_vertices.push_back({ p2 });
                }

            }
        }

        static void extrude_indices(std::vector<render::face>& faces, s32 original_vertex_count)
        {
            constexpr s32 vertices_per_extrusion_segment = 2;

            const s32 new_vertex_count = original_vertex_count * vertices_per_extrusion_segment;

            for (s32 i = 0; i < new_vertex_count; i += vertices_per_extrusion_segment)
            {
                render::face tri_1;

                tri_1[0] = ((i + 0) % (new_vertex_count));
                tri_1[1] = ((i + 1) % (new_vertex_count));
                tri_1[2] = ((i + 2) % (new_vertex_count));

                render::face tri_2;

                tri_2[0] = ((i + 2) % (new_vertex_count));
                tri_2[1] = ((i + 1) % (new_vertex_count));
                tri_2[2] = ((i + 3) % (new_vertex_count));

                faces.push_back(std::move(tri_1));
                faces.push_back(std::move(tri_2));
            }
        }
    }
}