#pragma once

#include "util/types.h"

#include <glm/glm.hpp>

#include <vector>

namespace ppp
{
    namespace geometry
    {
        static glm::vec2 circular_center_translation(f32 w, f32 h)
        {
            return glm::vec2(w, h);
        }

        static glm::vec2 triangular_center_translation(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)
        {
            // Find the minimum and maximum coordinates
            glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3 }), std::min({ y1, y2, y3 }), 0.0f);
            glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3 }), std::max({ y1, y2, y3 }), 0.0f);

            f32 width = max_coord.x - min_coord.x;
            f32 height = max_coord.y - min_coord.y;

            // Center the shape
            return glm::vec2(width / 2.0f, height / 2.0f);
        }

        static glm::vec2 rectanglular_center_translation(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
        {
            // Find the minimum and maximum coordinates
            glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3, x4 }), std::min({ y1, y2, y3, y4 }), 0.0f);
            glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3, x4 }), std::max({ y1, y2, y3, y4 }), 0.0f);

            f32 width = max_coord.x - min_coord.x;
            f32 height = max_coord.y - min_coord.y;

            // Center the shape
            return glm::vec2(width / 2.0f, height / 2.0f);
        }

        static glm::vec2 rectanglular_center_translation(f32 x, f32 y, f32 w, f32 h)
        {
            f32 x1 = x;
            f32 y1 = y;

            f32 x2 = x + w;
            f32 y2 = y;

            f32 x3 = x + w;
            f32 y3 = y + h;

            f32 x4 = x;
            f32 y4 = y + h;
            
            return rectanglular_center_translation(x1, y1, x2, y2, x3, y3, x4, y4);
        }

        template<typename TVertexType>
        static void extrude_vertices(std::vector<glm::vec3>& new_vertices, const glm::mat4& world, const TVertexType& center, const TVertexType* vertices, s32 vertex_count, f32 extrusion_width)
        {
            for (s32 i = 0; i < vertex_count; ++i)
            {
                glm::vec4 world_vertex_i = world * glm::vec4(vertices[i], 1.0f);
                glm::vec4 world_center = world * glm::vec4(center, 1.0f);

                glm::vec4 dir = glm::normalize(world_vertex_i - world_center);

                glm::vec4 p1 = world_vertex_i;
                glm::vec4 p2 = p1 + (dir * extrusion_width);

                // We need to switch winding order if the inner stroke is enabled
                if (extrusion_width < 0.0f)
                {
                    // We will always render 2d geometry with an orthographic projection matrix
                    // Putting the z to 1.0f makes sure the depth buffer does not cull the inner stroke
                    p1.z = 10.0f;
                    p2.z = 10.0f;

                    new_vertices.push_back({ p2.x, p2.y, p2.z });
                    new_vertices.push_back({ p1.x, p1.y, p1.z });
                }
                else
                {
                    new_vertices.push_back({ p1.x, p1.y, p1.z });
                    new_vertices.push_back({ p2.x, p2.y, p2.z });
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