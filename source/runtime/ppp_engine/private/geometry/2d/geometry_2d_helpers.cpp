#include "geometry/2d/geometry_2d_helpers.h"

#include "constants.h"

#include <glm/gtc/constants.hpp>

#include <vector>
#include <array>

namespace ppp
{
    namespace geometry
    {
        namespace internal
        {           
            namespace image
            {
                std::tuple<std::array<glm::vec3, 4>, std::array<glm::vec2, 4>> make_quad_vertices(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4, f32 uvsx, f32 uvsy, f32 uvex, f32 uvey)
                {
                    _image_data.vertex_position[0] = glm::vec3(x1, y1, 0);
                    _image_data.vertex_position[1] = glm::vec3(x2, y2, 0);
                    _image_data.vertex_position[2] = glm::vec3(x3, y3, 0);
                    _image_data.vertex_position[3] = glm::vec3(x4, y4, 0);

                    _image_data.vertex_texcoords[0] = glm::vec2(uvsx, uvey);
                    _image_data.vertex_texcoords[1] = glm::vec2(uvex, uvey);
                    _image_data.vertex_texcoords[2] = glm::vec2(uvex, uvsy);
                    _image_data.vertex_texcoords[3] = glm::vec2(uvsx, uvsy);

                    if (from_corner == false)
                    {
                        // Find the minimum and maximum coordinates
                        glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3, x4 }), std::min({ y1, y2, y3, y4 }), 0.0f);
                        glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3, x4 }), std::max({ y1, y2, y3, y4 }), 0.0f);

                        f32 width = max_coord.x - min_coord.x;
                        f32 height = max_coord.y - min_coord.y;

                        // Center the shape
                        for (glm::vec3& vertex_pos : _image_data.vertex_position)
                        {
                            vertex_pos.x = vertex_pos.x - (width / 2.0f);
                            vertex_pos.y = vertex_pos.y - (height / 2.0f);
                        }
                    }

                    return { _image_data.vertex_position, _image_data.vertex_texcoords };
                }

                std::tuple<std::array<glm::vec3, 4>, std::array<glm::vec2, 4>> make_quad_vertices(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
                {
                    return make_quad_vertices(from_corner, x1, y1, x2, y2, x3, y3, x4, y4, 0.0, 1.0, 1.0, 0.0f);
                }

                std::array<render::index, 6>& make_image_indices()
                {
                    return _image_data.indices;
                }
            }
        }

        namespace image
        {
            render::render_item extrude_image(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width)
            {
                constexpr s32 vertices_per_triangle = 4;
                constexpr s32 vertices_per_extrusion_segment = 2;

                internal::_image_data.extrude_vertices.clear();
                internal::_image_data.extrude_vertices.reserve(vertex_count * vertices_per_extrusion_segment);

                internal::_image_data.extrude_indices.clear();
                internal::_image_data.extrude_indices.reserve(vertex_count * vertices_per_extrusion_segment * vertices_per_triangle);

                glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
                for (s32 i = 0; i < vertex_count; ++i)
                {
                    center += vertices[i];
                }
                center /= vertex_count;

                internal::extrude_vertices(internal::_image_data.extrude_vertices, { center }, vertices, vertex_count, extrusion_width);
                internal::extrude_indices(internal::_image_data.extrude_indices, vertex_count);

                render::render_item new_render_item;

                auto vert_comp = render::make_vertex_component(internal::_image_data.extrude_vertices.size());
                vert_comp->add_attribute(render::vertex_attribute_type::POSITION, internal::_image_data.extrude_vertices.data());

                auto idx_comp = render::make_index_component(internal::_image_data.extrude_indices.data(), internal::_image_data.extrude_indices.size());

                new_render_item.add_component(std::move(vert_comp));
                new_render_item.add_component(std::move(idx_comp));

                return new_render_item;
            }

            render::render_item make_image(bool from_corner, f32 x, f32 y, f32 w, f32 h, s32 image_id)
            {
                auto& vertices = internal::image::make_quad_vertices(from_corner, x, y, x + w, y, x + w, y + h, x, y + h);
                auto& indices = internal::image::make_image_indices();

                render::render_item item;

                assert(std::get<0>(vertices).size() == std::get<1>(vertices).size());

                auto vert_comp = render::make_vertex_component(std::get<0>(vertices).size());
                vert_comp->add_attribute(render::vertex_attribute_type::POSITION, std::get<0>(vertices).data());
                vert_comp->add_attribute(render::vertex_attribute_type::TEXCOORD, std::get<1>(vertices).data());
            
                auto idx_comp = render::make_index_component(indices.data(), indices.size());
                auto tex_comp = render::make_texture_component(image_id);

                item.add_component(std::move(vert_comp));
                item.add_component(std::move(idx_comp));
                item.add_component(std::move(tex_comp));

                return item;
            }
        }

        namespace font
        {
            render::render_item make_font(bool from_corner, f32 x, f32 y, f32 w, f32 h, f32 uv_start_x, f32 uv_start_y, f32 uv_end_x, f32 uv_end_y, s32 image_id)
            {
                auto& vertices = internal::image::make_quad_vertices(from_corner, x, y, x + w, y, x + w, y + h, x, y + h, uv_start_x, uv_start_y, uv_end_x, uv_end_y);
                auto& indices = internal::image::make_image_indices();

                render::render_item item;

                assert(std::get<0>(vertices).size() == std::get<1>(vertices).size());

                auto vert_comp = render::make_vertex_component(std::get<0>(vertices).size());
                vert_comp->add_attribute(render::vertex_attribute_type::POSITION, std::get<0>(vertices).data());
                vert_comp->add_attribute(render::vertex_attribute_type::TEXCOORD, std::get<1>(vertices).data());
            
                auto idx_comp = render::make_index_component(indices.data(), indices.size());
                auto tex_comp = render::make_texture_component(image_id);

                item.add_component(std::move(vert_comp));
                item.add_component(std::move(idx_comp));
                item.add_component(std::move(tex_comp));

                return item;
            }
        }

        render::render_item extrude_point(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width)
        {
            const bool from_corner = false;

            return geometry::make_ellipse(from_corner, vertices[0].x, vertices[0].y, extrusion_width, extrusion_width);
        }

        render::render_item extrude_line(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width)
        {
            const bool from_corner = true;

            const f32 stroke_x1 = vertices[0].x - extrusion_width; 
            const f32 stroke_y1 = vertices[0].y - extrusion_width; 

            const f32 stroke_x2 = vertices[1].x + extrusion_width; 
            const f32 stroke_y2 = vertices[1].y - extrusion_width; 

            const f32 stroke_x3 = vertices[1].x + extrusion_width;
            const f32 stroke_y3 = vertices[1].y + extrusion_width;

            const f32 stroke_x4 = vertices[0].x - extrusion_width;
            const f32 stroke_y4 = vertices[0].y + extrusion_width;

            return geometry::make_polygon(from_corner, stroke_x1, stroke_y1, stroke_x2, stroke_y2, stroke_x3, stroke_y3, stroke_x4, stroke_y4);
        }

        render::render_item extrude_ellipse(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width)
        {
            constexpr s32 vertices_per_triangle = 3;
            constexpr s32 vertices_per_extrusion_segment = 2;

            internal::_ellipse_data.extrude_vertices.clear();
            internal::_ellipse_data.extrude_vertices.reserve(vertex_count * vertices_per_extrusion_segment);

            internal::_ellipse_data.extrude_indices.clear();
            internal::_ellipse_data.extrude_indices.reserve(vertex_count * vertices_per_extrusion_segment * vertices_per_triangle);

            const glm::vec3& center = vertices[0];

            internal::extrude_vertices(internal::_ellipse_data.extrude_vertices, center, &vertices[1], vertex_count - 1, extrusion_width);
            internal::extrude_indices(internal::_ellipse_data.extrude_indices, vertex_count - 1);

            render::render_item new_render_item;

            auto vert_comp = render::make_vertex_component(internal::_ellipse_data.extrude_vertices.size());
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, internal::_ellipse_data.extrude_vertices.data());

            auto idx_comp = render::make_index_component(internal::_ellipse_data.extrude_indices.data(), internal::_ellipse_data.extrude_indices.size());

            new_render_item.add_component(std::move(vert_comp));
            new_render_item.add_component(std::move(idx_comp));

            return new_render_item;
        }

        render::render_item extrude_polygon(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width)
        {
            constexpr s32 vertices_per_triangle = 4;
            constexpr s32 vertices_per_extrusion_segment = 2;

            internal::_polygon_data.extrude_vertices.clear();
            internal::_polygon_data.extrude_vertices.reserve(vertex_count * vertices_per_extrusion_segment);

            internal::_polygon_data.extrude_indices.clear();
            internal::_polygon_data.extrude_indices.reserve(vertex_count * vertices_per_extrusion_segment * vertices_per_triangle);

            glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
            for (s32 i = 0; i < vertex_count; ++i)
            {
                center += vertices[i];
            }
            center /= vertex_count;

            internal::extrude_vertices(internal::_polygon_data.extrude_vertices, { center }, vertices, vertex_count, extrusion_width);
            internal::extrude_indices(internal::_polygon_data.extrude_indices, vertex_count);

            render::render_item new_render_item;

            auto vert_comp = render::make_vertex_component(internal::_polygon_data.extrude_vertices.size());
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, internal::_polygon_data.extrude_vertices.data());

            auto idx_comp = render::make_index_component(internal::_polygon_data.extrude_indices.data(), internal::_polygon_data.extrude_indices.size());

            new_render_item.add_component(std::move(vert_comp));
            new_render_item.add_component(std::move(idx_comp));

            return new_render_item;
        }

        render::render_item extrude_triangle(const glm::vec3* vertices, s32 vertex_count, f32 extrusion_width)
        {
            constexpr s32 vertices_per_triangle = 4;
            constexpr s32 vertices_per_extrusion_segment = 2;

            internal::_triangle_data.extrude_vertices.clear();
            internal::_triangle_data.extrude_vertices.reserve(vertex_count * vertices_per_extrusion_segment);

            internal::_triangle_data.extrude_indices.clear();
            internal::_triangle_data.extrude_indices.reserve(vertex_count * vertices_per_extrusion_segment * vertices_per_triangle);

            glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
            for (s32 i = 0; i < vertex_count; ++i)
            {
                center += vertices[i];
            }
            center /= vertex_count;

            internal::extrude_vertices(internal::_triangle_data.extrude_vertices, { center }, vertices, vertex_count, extrusion_width);
            internal::extrude_indices(internal::_triangle_data.extrude_indices, vertex_count);

            render::render_item new_render_item;

            auto vert_comp = render::make_vertex_component(internal::_triangle_data.extrude_vertices.size());
            vert_comp->add_attribute(render::vertex_attribute_type::POSITION, internal::_triangle_data.extrude_vertices.data());

            auto idx_comp = render::make_index_component(internal::_triangle_data.extrude_indices.data(), internal::_triangle_data.extrude_indices.size());

            new_render_item.add_component(std::move(vert_comp));
            new_render_item.add_component(std::move(idx_comp));

            return new_render_item;
        }

        template<typename TVertexType>
        void extrude_vertices(std::vector<glm::vec3>& new_vertices, const TVertexType& center, const TVertexType* vertices, s32 vertex_count, f32 extrusion_width)
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

        void extrude_indices(std::vector<render::index>& indices, s32 original_vertex_count)
        {
            constexpr s32 vertices_per_extrusion_segment = 2;

            const s32 new_vertex_count = original_vertex_count * vertices_per_extrusion_segment;

            for (s32 i = 0; i < new_vertex_count; i += vertices_per_extrusion_segment)
            {
                indices.push_back((i + 0) % (new_vertex_count));
                indices.push_back((i + 1) % (new_vertex_count));
                indices.push_back((i + 2) % (new_vertex_count));

                indices.push_back((i + 2) % (new_vertex_count));
                indices.push_back((i + 1) % (new_vertex_count));
                indices.push_back((i + 3) % (new_vertex_count));
            }
        }
    }
}