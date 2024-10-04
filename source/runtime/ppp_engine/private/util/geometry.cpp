#include "util/geometry.h"

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
            struct point_data
            {
                render::VertexPos vertex;
                render::Index index = { 0 };
            } _point_data;

            struct line_data
            {
                std::array<render::VertexPos, 2> vertices;
                std::array<render::Index, 2> indices = { 0, 1 };
            } _line_data;

            struct triangle_data
            {
                std::array<render::VertexPos, 3> vertices;
                std::array<render::Index, 3> indices = { 0, 1, 2 };

                std::vector<render::VertexPos> extrude_vertices;
                std::vector<render::Index> extrude_indices;
            } _triangle_data;

            struct polygon_data
            {
                std::array<render::VertexPos, 4> vertices;
                std::array<render::Index, 6> indices = { 0, 1 ,3, 1, 3, 2 };

                std::vector<render::VertexPos> extrude_vertices;
                std::vector<render::Index> extrude_indices;
            } _polygon_data;

            struct ellipse_data
            {
                render::VertexPosArr vertices;
                render::Indices indices;

                render::VertexPosArr extrude_vertices;
                render::Indices extrude_indices;

            } _ellipse_data;

            struct image_data
            {
                std::array<render::VertexPosTex, 4> vertices;
                std::array<render::Index, 6> indices = { 0, 1 ,3, 1, 3, 2 };

                std::vector<render::VertexPos> extrude_vertices;
                std::vector<render::Index> extrude_indices;;
            } _image_data;

            struct box_data
            {
                std::array<render::VertexPos, 24> vertices;
                std::array<render::Index, 36> indices = 
                {
                    // Front face
                    0, 1, 2, 0, 2, 3,
                    // Back face
                    4, 6, 5, 4, 7, 6,
                    // Left face
                    8, 9, 10, 8, 10, 11,
                    // Right face
                    12, 14, 13, 12, 15, 14,
                    // Top face
                    16, 17, 18, 16, 18, 19,
                    // Bottom face
                    20, 21, 22, 20, 22, 23
                };

            } _box_data;

            struct cylinder_data
            {
                render::VertexPosArr vertices;
                render::Indices indices;
            } _cylinder_data;

            struct plane_data
            {
                std::array<render::VertexPos, 4> vertices;
                std::array<render::Index, 6> indices = { 0, 1 ,3, 1, 3, 2 };
            } _plane_data;

            template<typename TVertexType>
            void extrude_vertices(std::vector<render::VertexPos>& new_vertices, const TVertexType& center, TVertexType* vertices, s32 vertex_count, f32 extrusion_width)
            {
                for (s32 i = 0; i < vertex_count; ++i)
                {
                    glm::vec3 dir = glm::normalize(vertices[i].position - center.position);

                    glm::vec3 p1 = vertices[i].position;
                    glm::vec3 p2 = p1 + (dir * extrusion_width);

                    new_vertices.push_back({ p1 });
                    new_vertices.push_back({ p2 });
                }
            }

            void extrude_indices(std::vector<render::Index>& indices, s32 original_vertex_count)
            {
                constexpr s32 vertices_per_extrusion_segment = 2;

                const s32 new_vertex_count = original_vertex_count * vertices_per_extrusion_segment;

                for (s32 i = 0; i < new_vertex_count; i += vertices_per_extrusion_segment)
                {
                    indices.push_back((i + 0) % (new_vertex_count));
                    indices.push_back((i + 2) % (new_vertex_count));
                    indices.push_back((i + 1) % (new_vertex_count));

                    indices.push_back((i + 2) % (new_vertex_count));
                    indices.push_back((i + 1) % (new_vertex_count));
                    indices.push_back((i + 3) % (new_vertex_count));
                }
            }

            std::vector<render::VertexPos>& make_ellipse_vertices(bool from_corner, f32 x, f32 y, f32 w, f32 h, s32 detail = 25)
            {
                s32 total_nr_vertices = detail + 1; // take center point into account

                _ellipse_data.vertices.clear();
                _ellipse_data.vertices.resize(total_nr_vertices);

                _ellipse_data.vertices[0].position = glm::vec3{ x, y, 0.0f };

                for (s32 t = 1; t < total_nr_vertices; ++t)
                {
                    f32 angle = (t / static_cast<f32>(detail)) * glm::two_pi<f32>();

                    f32 v_x = sin(angle) * w;
                    f32 v_y = cos(angle) * h;

                    _ellipse_data.vertices[t].position = glm::vec3(x + v_x, y + v_y, 0.0f);
                }

                if (from_corner == true)
                {
                    for (s32 t = 0; t < total_nr_vertices; ++t)
                    {
                        _ellipse_data.vertices[t].position += glm::vec3(w, h, 0.0f);
                    }
                }

                return _ellipse_data.vertices;
            }

            std::array<render::VertexPos, 4>& make_polygon_vertices(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
            {
                _polygon_data.vertices[0].position = glm::vec3(x1, y1, 0);
                _polygon_data.vertices[1].position = glm::vec3(x2, y2, 0);
                _polygon_data.vertices[2].position = glm::vec3(x3, y3, 0);
                _polygon_data.vertices[3].position = glm::vec3(x4, y4, 0);

                if (from_corner == false)
                {
                    // Find the minimum and maximum coordinates
                    glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3, x4 }), std::min({ y1, y2, y3, y4 }), 0.0f);
                    glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3, x4 }), std::max({ y1, y2, y3, y4 }), 0.0f);

                    f32 width = max_coord.x - min_coord.x;
                    f32 height = max_coord.y - min_coord.y;

                    // Center the shape
                    for (render::VertexPos& vertex : _polygon_data.vertices)
                    {
                        vertex.position.x = vertex.position.x - (width / 2.0f);
                        vertex.position.y = vertex.position.y - (height / 2.0f);
                    }
                }

                return _polygon_data.vertices;
            }

            std::array<render::VertexPos, 3>& make_triangle_vertices(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)
            {
                // Calculate vertices
                _triangle_data.vertices[0].position = glm::vec3(x1, y1, 0);
                _triangle_data.vertices[1].position = glm::vec3(x2, y2, 0);
                _triangle_data.vertices[2].position = glm::vec3(x3, y3, 0);

                if (from_corner == false)
                {
                    // Find the minimum and maximum coordinates
                    glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3 }), std::min({ y1, y2, y3 }), 0.0f);
                    glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3 }), std::max({ y1, y2, y3 }), 0.0f);

                    f32 width = max_coord.x - min_coord.x;
                    f32 height = max_coord.y - min_coord.y;

                    // Center the shape
                    for (render::VertexPos& vertex : internal::_triangle_data.vertices)
                    {
                        vertex.position.x = vertex.position.x - (width / 2.0f);
                        vertex.position.y = vertex.position.y - (height / 2.0f);
                    }
                }

                return _triangle_data.vertices;
            }

            std::array<render::VertexPos, 24>& make_box_vertices(f32 width, f32 height, f32 depth)
            {    
                size_t index = 0;

                // Front face
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, depth / 2) }; // Bottom Left
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, depth / 2) };  // Bottom Right
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, depth / 2) };   // Top Right
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, depth / 2) };  // Top Left

                // Back face
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, -depth / 2) }; // Bottom Left
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, -depth / 2) };  // Bottom Right
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, -depth / 2) };   // Top Right
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, -depth / 2) };  // Top Left

                // Left face
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, -depth / 2) }; // Bottom Back
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, depth / 2) };  // Bottom Front
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, depth / 2) };   // Top Front
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, -depth / 2) };  // Top Back

                // Right face
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, -depth / 2) };  // Bottom Back
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, depth / 2) };   // Bottom Front
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, depth / 2) };    // Top Front
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, -depth / 2) };   // Top Back

                // Top face
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, -depth / 2) };  // Back Left
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, -depth / 2) };   // Back Right
                _box_data.vertices[index++] = { glm::vec3(width / 2, height / 2, depth / 2) };    // Front Right
                _box_data.vertices[index++] = { glm::vec3(-width / 2, height / 2, depth / 2) };   // Front Left

                // Bottom face
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, -depth / 2) }; // Back Left
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, -depth / 2) };  // Back Right
                _box_data.vertices[index++] = { glm::vec3(width / 2, -height / 2, depth / 2) };   // Front Right
                _box_data.vertices[index++] = { glm::vec3(-width / 2, -height / 2, depth / 2) };  // Front Left

                return _box_data.vertices;
            }

            std::vector<render::VertexPos>& make_cylinder_vertices(f32 r, f32 h, s32 detail)
            {
                s32 total_nr_vertices = (detail + 1) * 2;

                _cylinder_data.vertices.clear();
                _cylinder_data.vertices.resize(total_nr_vertices);

                _cylinder_data.vertices[0].position = glm::vec3{ 0.0f, (+h) / 2, 0.0f};
                _cylinder_data.vertices[total_nr_vertices / 2].position = glm::vec3{ 0.0f, (-h) / 2, 0.0f};

                for (s32 t = 1; t < total_nr_vertices / 2; ++t)
                {
                    f32 angle = (t / static_cast<f32>(detail)) * glm::two_pi<f32>();

                    f32 v_x = cos(angle) * r;
                    f32 v_z = sin(angle) * r;

                    s32 top_index = t;
                    s32 bottom_index = (total_nr_vertices / 2) + t; 

                    _cylinder_data.vertices[top_index].position = glm::vec3(v_x, (+h) / 2, v_z);
                    _cylinder_data.vertices[bottom_index].position = glm::vec3(v_x, (-h) / 2, v_z);
                }

                return _cylinder_data.vertices;
            }

            std::array<render::VertexPos, 4>& make_plane_vertices(f32 width, f32 height)
            {
                _polygon_data.vertices[0].position = glm::vec3(x1, y1, 0);
                _polygon_data.vertices[1].position = glm::vec3(x2, y2, 0);
                _polygon_data.vertices[2].position = glm::vec3(x3, y3, 0);
                _polygon_data.vertices[3].position = glm::vec3(x4, y4, 0);

                return _polygon_data.vertices;
            }

            std::vector<render::Index>& make_ellipse_indices(f32 x, f32 y, f32 w, f32 h, s32 detail = 25)
            {
                s32 total_nr_indices = detail * 3;

                _ellipse_data.indices.clear();
                _ellipse_data.indices.resize(total_nr_indices);

                s32 i = 0;
                for (s32 t = 0; t < total_nr_indices; t += 3)
                {
                    _ellipse_data.indices[t + 0] = 0;
                    _ellipse_data.indices[t + 1] = i + 1;
                    _ellipse_data.indices[t + 2] = i + 2;

                    ++i;
                }

                // Loop back to the first triangle
                _ellipse_data.indices[total_nr_indices - 1] = 1;

                return _ellipse_data.indices;
            }

            std::array<render::Index, 6>& make_polygon_indices()
            {
                return _polygon_data.indices;
            }

            std::array<render::Index, 3>& make_triangle_indices()
            {
                return _triangle_data.indices;
            }
            
            std::array<render::Index, 36>& make_box_indices()
            {
                return _box_data.indices;
            }

            std::vector<render::Index>& make_cylinder_indices(f32 h, s32 detail, bool bottom_cap, bool top_cap)
            {
                s32 total_nr_indices = detail * 3;
                s32 total_indices = (top_cap ? total_nr_indices : 0) + (bottom_cap ? total_nr_indices : 0) + detail * 6;

                _cylinder_data.indices.clear();
                _cylinder_data.indices.resize(total_indices);

                s32 i = 0;
                s32 index_offset = 0;

                if (top_cap)
                {
                    for (s32 t = 0; t < total_nr_indices; t += 3)
                    {
                        _cylinder_data.indices[index_offset + 0] = 0;         // Center vertex of top cap
                        _cylinder_data.indices[index_offset + 1] = i + 1;     // First perimeter vertex
                        _cylinder_data.indices[index_offset + 2] = i + 2;     // Next perimeter vertex
                        ++i;
                        index_offset += 3;
                    }

                    // Loop back to the first triangle
                    _cylinder_data.indices[index_offset - 1] = 1;
                }

                s32 bottom_cap_start = _cylinder_data.vertices.size() / 2;
                i = bottom_cap_start;

                if (bottom_cap)
                {
                    for (s32 t = 0; t < total_nr_indices; t += 3)
                    {
                        _cylinder_data.indices[index_offset + 0] = bottom_cap_start; // Center vertex of bottom cap
                        _cylinder_data.indices[index_offset + 1] = i + 1;           // First perimeter vertex
                        _cylinder_data.indices[index_offset + 2] = i + 2;           // Next perimeter vertex
                        ++i;
                        index_offset += 3;
                    }

                    // Loop back to the first triangle of the bottom cap
                    _cylinder_data.indices[index_offset - 1] = bottom_cap_start + 1;
                }

                // Generate side indices
                s32 side_start = index_offset;
                for (s32 j = 0; j < detail - 1; ++j)
                {
                    // First triangle of quad (Top -> Bottom -> Next Top)
                    _cylinder_data.indices[side_start + 0] = j + 1;                       // Top current vertex
                    _cylinder_data.indices[side_start + 1] = bottom_cap_start + j + 1;    // Bottom current vertex
                    _cylinder_data.indices[side_start + 2] = j + 2;                       // Top next vertex

                    // Second triangle of quad (Bottom -> Next Bottom -> Next Top)
                    _cylinder_data.indices[side_start + 3] = bottom_cap_start + j + 1;    // Bottom current vertex
                    _cylinder_data.indices[side_start + 4] = bottom_cap_start + j + 2;    // Bottom next vertex
                    _cylinder_data.indices[side_start + 5] = j + 2;                       // Top next vertex

                    side_start += 6;
                }

                _cylinder_data.indices[side_start + 0] = detail;                           // Last top perimeter vertex
                _cylinder_data.indices[side_start + 1] = bottom_cap_start + detail;        // Last bottom perimeter vertex
                _cylinder_data.indices[side_start + 2] = 1;                                // First top perimeter vertex (to close the loop)

                _cylinder_data.indices[side_start + 3] = bottom_cap_start + detail;        // Last bottom perimeter vertex
                _cylinder_data.indices[side_start + 4] = bottom_cap_start + 1;             // First bottom perimeter vertex (to close the loop)
                _cylinder_data.indices[side_start + 5] = 1;                                // First top perimeter vertex (to close the loop)

                return _cylinder_data.indices;
            }

            std::array<render::Index, 6>& make_plane_indices()
            {
                return _plane_data.indices;
            }

            namespace image
            {
                std::array<render::VertexPosTex, 4>& make_quad_vertices(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4, f32 uvsx, f32 uvsy, f32 uvex, f32 uvey)
                {
                    _image_data.vertices[0].position = glm::vec3(x1, y1, 0);
                    _image_data.vertices[1].position = glm::vec3(x2, y2, 0);
                    _image_data.vertices[2].position = glm::vec3(x3, y3, 0);
                    _image_data.vertices[3].position = glm::vec3(x4, y4, 0);

                    _image_data.vertices[0].texcoord = glm::vec2(uvsx, uvey);
                    _image_data.vertices[1].texcoord = glm::vec2(uvex, uvey);
                    _image_data.vertices[2].texcoord = glm::vec2(uvex, uvsy);
                    _image_data.vertices[3].texcoord = glm::vec2(uvsx, uvsy);

                    if (from_corner == false)
                    {
                        // Find the minimum and maximum coordinates
                        glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3, x4 }), std::min({ y1, y2, y3, y4 }), 0.0f);
                        glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3, x4 }), std::max({ y1, y2, y3, y4 }), 0.0f);

                        f32 width = max_coord.x - min_coord.x;
                        f32 height = max_coord.y - min_coord.y;

                        // Center the shape
                        for (render::VertexPosTex& vertex : _image_data.vertices)
                        {
                            vertex.position.x = vertex.position.x - (width / 2.0f);
                            vertex.position.y = vertex.position.y - (height / 2.0f);
                        }
                    }

                    return _image_data.vertices;
                }

                std::array<render::VertexPosTex, 4>& make_quad_vertices(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
                {
                    return make_quad_vertices(from_corner, x1, y1, x2, y2, x3, y3, x4, y4, 0.0, 1.0, 1.0, 0.0f);
                }

                std::array<render::Index, 6>& make_image_indices()
                {
                    return _image_data.indices;
                }
            }
        }

        namespace image
        {
            render::RenderItem extrude_image(render::VertexPosTex* vertices, s32 vertex_count, f32 extrusion_width)
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
                    center += vertices[i].position;
                }
                center /= vertex_count;

                internal::extrude_vertices(internal::_image_data.extrude_vertices, { center }, vertices, vertex_count, extrusion_width);
                internal::extrude_indices(internal::_image_data.extrude_indices, vertex_count);

                render::RenderItem new_render_item;
                new_render_item.vertices = internal::_image_data.extrude_vertices.data();
                new_render_item.vertex_count = internal::_image_data.extrude_vertices.size();
                new_render_item.indices = internal::_image_data.extrude_indices.data();
                new_render_item.index_count = internal::_image_data.extrude_indices.size();

                return new_render_item;
            }

            render::ImageItem make_image(bool from_corner, f32 x, f32 y, f32 w, f32 h, s32 image_id)
            {
                auto& vertices = internal::image::make_quad_vertices(from_corner, x, y, x + w, y, x + w, y + h, x, y + h);
                auto& indices = internal::image::make_image_indices();

                render::ImageItem item;

                item.vertices = vertices.data();
                item.vertex_count = vertices.size();
                item.indices = indices.data();
                item.index_count = indices.size();
                item.image_id = image_id;

                return item;
            }
        }

        namespace font
        {
            render::ImageItem make_font(bool from_corner, f32 x, f32 y, f32 w, f32 h, f32 uv_start_x, f32 uv_start_y, f32 uv_end_x, f32 uv_end_y, s32 image_id)
            {
                auto& vertices = internal::image::make_quad_vertices(from_corner, x, y, x + w, y, x + w, y + h, x, y + h, uv_start_x, uv_start_y, uv_end_x, uv_end_y);
                auto& indices = internal::image::make_image_indices();

                render::ImageItem item;

                item.vertices = vertices.data();
                item.vertex_count = vertices.size();
                item.indices = indices.data();
                item.index_count = indices.size();
                item.image_id = image_id;

                return item;
            }
        }

        render::RenderItem extrude_point(render::VertexPos* vertices, s32 vertex_count, f32 extrusion_width)
        {
            const bool from_center = true;

            return geometry::make_ellipse(from_center, vertices[0].position.x, vertices[0].position.y, extrusion_width, extrusion_width);
        }

        render::RenderItem extrude_line(render::VertexPos* vertices, s32 vertex_count, f32 extrusion_width)
        {
            const bool from_center = true;

            const f32 stroke_x1 = vertices[0].position.x - extrusion_width; const f32 stroke_x4 = vertices[1].position.y + extrusion_width;
            const f32 stroke_y1 = vertices[0].position.y - extrusion_width; const f32 stroke_y4 = vertices[1].position.y + extrusion_width;

            const f32 stroke_x2 = vertices[1].position.y + extrusion_width; const f32 stroke_x3 = vertices[0].position.y - extrusion_width;
            const f32 stroke_y2 = vertices[1].position.y - extrusion_width; const f32 stroke_y3 = vertices[0].position.y + extrusion_width;

            return geometry::make_polygon(from_center, stroke_x1, stroke_y1, stroke_x2, stroke_y2, stroke_x3, stroke_y3, stroke_x4, stroke_y4);
        }

        render::RenderItem extrude_ellipse(render::VertexPos* vertices, s32 vertex_count, f32 extrusion_width)
        {
            constexpr s32 vertices_per_triangle = 3;
            constexpr s32 vertices_per_extrusion_segment = 2;

            internal::_ellipse_data.extrude_vertices.clear();
            internal::_ellipse_data.extrude_vertices.reserve(vertex_count * vertices_per_extrusion_segment);

            internal::_ellipse_data.extrude_indices.clear();
            internal::_ellipse_data.extrude_indices.reserve(vertex_count * vertices_per_extrusion_segment * vertices_per_triangle);

            const render::VertexPos& center = vertices[0];

            internal::extrude_vertices(internal::_ellipse_data.extrude_vertices, center, &vertices[1], vertex_count - 1, extrusion_width);
            internal::extrude_indices(internal::_ellipse_data.extrude_indices, vertex_count - 1);

            render::RenderItem new_render_item;
            new_render_item.vertices = internal::_ellipse_data.extrude_vertices.data();
            new_render_item.vertex_count = internal::_ellipse_data.extrude_vertices.size();
            new_render_item.indices = internal::_ellipse_data.extrude_indices.data();
            new_render_item.index_count = internal::_ellipse_data.extrude_indices.size();

            return new_render_item;
        }

        render::RenderItem extrude_polygon(render::VertexPos* vertices, s32 vertex_count, f32 extrusion_width)
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
                center += vertices[i].position;
            }
            center /= vertex_count;

            internal::extrude_vertices(internal::_polygon_data.extrude_vertices, { center }, vertices, vertex_count, extrusion_width);
            internal::extrude_indices(internal::_polygon_data.extrude_indices, vertex_count);

            render::RenderItem new_render_item;
            new_render_item.vertices = internal::_polygon_data.extrude_vertices.data();
            new_render_item.vertex_count = internal::_polygon_data.extrude_vertices.size();
            new_render_item.indices = internal::_polygon_data.extrude_indices.data();
            new_render_item.index_count = internal::_polygon_data.extrude_indices.size();

            return new_render_item;
        }

        render::RenderItem extrude_triangle(render::VertexPos* vertices, s32 vertex_count, f32 extrusion_width)
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
                center += vertices[i].position;
            }
            center /= vertex_count;

            internal::extrude_vertices(internal::_triangle_data.extrude_vertices, { center }, vertices, vertex_count, extrusion_width);
            internal::extrude_indices(internal::_triangle_data.extrude_indices, vertex_count);

            render::RenderItem new_render_item;
            new_render_item.vertices = internal::_triangle_data.extrude_vertices.data();
            new_render_item.vertex_count = internal::_triangle_data.extrude_vertices.size();
            new_render_item.indices = internal::_triangle_data.extrude_indices.data();
            new_render_item.index_count = internal::_triangle_data.extrude_indices.size();

            return new_render_item;
        }

        render::RenderItem make_point(f32 x, f32 y)
        {
            internal::_point_data.vertex.position.x = x;
            internal::_point_data.vertex.position.y = y;

            return render::RenderItem { &internal::_point_data.vertex, 1, &internal::_point_data.index, 1 };
        }

        render::RenderItem make_line(f32 x1, f32 y1, f32 x2, f32 y2)
        {
            internal::_line_data.vertices[0].position = glm::vec3(x1, y1, 0.0f);
            internal::_line_data.vertices[1].position = glm::vec3(x2, y2, 0.0f);

            return render::RenderItem { internal::_line_data.vertices.data(), internal::_line_data.vertices.size(), internal::_line_data.indices.data(), internal::_line_data.indices.size() };
        }

        render::RenderItem make_ellipse(bool from_corner, f32 x, f32 y, f32 w, f32 h, s32 detail)
        {
            auto& vertices = internal::make_ellipse_vertices(from_corner, x, y, w, h, detail);
            auto& indices = internal::make_ellipse_indices(x, y, w, h, detail);

            return render::RenderItem { vertices.data(), vertices.size(), indices.data(), indices.size() };
        }

        render::RenderItem make_polygon(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
        {
            auto& vertices = internal::make_polygon_vertices(from_corner, x1, y1, x2, y2, x3, y3, x4, y4);
            auto& indices = internal::make_polygon_indices();

            return render::RenderItem { vertices.data(), vertices.size(), indices.data(), indices.size() };
        }

        render::RenderItem make_triangle(bool from_corner, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)
        {
            auto& vertices = internal::make_triangle_vertices(from_corner, x1, y1, x2, y2, x3, y3);
            auto& indices = internal::make_triangle_indices();

            return render::RenderItem { vertices.data(), vertices.size(), indices.data(), indices.size() };
        }

        render::RenderItem make_box(f32 width, f32 height, f32 depth)
        {
            auto& vertices = internal::make_box_vertices(width, height, depth);
            auto& indices = internal::make_box_indices();

            return render::RenderItem{ vertices.data(), vertices.size(), indices.data(), indices.size() };
        }

        render::RenderItem make_cylinder(f32 radius, f32 height, f32 detail, bool bottom_cap, bool top_cap)
        {
            auto& vertices = internal::make_cylinder_vertices(radius, height, detail);
            auto& indices = internal::make_cylinder_indices(height, detail, bottom_cap, top_cap);

            return render::RenderItem{ vertices.data(), vertices.size(), indices.data(), indices.size() };
        }

        render::RenderItem make_plane(f32 width, f32 height)
        {
            auto& vertices = internal::make_plane_vertices(width, height);
            auto& indices = internal::make_plane_indices(width, height);

            return render::RenderItem{ vertices.data(), vertices.size(), indices.data(), indices.size() };
        }

        render::RenderItem make_sphere(f32 radius, f32 detailx, f32 detaily)
        {
            return render::RenderItem{};
        }

        render::RenderItem make_torus(f32 radius, f32 tube_radius, f32 detailx, f32 detaily)
        {
            return render::RenderItem{};
        }

        render::RenderItem make_cone(f32 radius, f32 height, f32 detailx, f32 detaily, bool cap)
        {
            return render::RenderItem{};
        }
    }
}