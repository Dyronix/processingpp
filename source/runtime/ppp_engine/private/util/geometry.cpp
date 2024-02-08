#include "util/geometry.h"

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
                std::vector<render::Index> extrude_indices;;
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

            template<typename TVertexType>
            void extrude_vertices(std::vector<render::VertexPos>& new_vertices, const TVertexType& center, TVertexType* vertices, int vertex_count, float extrusion_width)
            {
                for (int i = 0; i < vertex_count; ++i)
                {
                    glm::vec3 dir = glm::normalize(vertices[i].position - center.position);

                    glm::vec3 p1 = vertices[i].position;
                    glm::vec3 p2 = p1 + (dir * extrusion_width);

                    new_vertices.push_back({ p1 });
                    new_vertices.push_back({ p2 });
                }
            }
            void extrude_indices(std::vector<render::Index>& indices, int original_vertex_count)
            {
                constexpr int vertices_per_extrusion_segment = 2;

                const int new_vertex_count = original_vertex_count * vertices_per_extrusion_segment;

                for (int i = 0; i < new_vertex_count; i += vertices_per_extrusion_segment)
                {
                    indices.push_back((i + 0) % (new_vertex_count));
                    indices.push_back((i + 2) % (new_vertex_count));
                    indices.push_back((i + 1) % (new_vertex_count));

                    indices.push_back((i + 2) % (new_vertex_count));
                    indices.push_back((i + 1) % (new_vertex_count));
                    indices.push_back((i + 3) % (new_vertex_count));
                }
            }

            std::vector<render::VertexPos>& make_ellipse_vertices(bool from_corner, float x, float y, float w, float h, int detail = 25)
            {
                int total_nr_vertices = detail + 1; // take center point into account

                _ellipse_data.vertices.clear();
                _ellipse_data.vertices.resize(total_nr_vertices);

                _ellipse_data.vertices[0].position = glm::vec3{ x, y, 0.0f };

                for (int t = 1; t < total_nr_vertices; ++t)
                {
                    float angle = (t / static_cast<float>(detail)) * glm::two_pi<float>();

                    float v_x = sin(angle) * w;
                    float v_y = cos(angle) * h;

                    _ellipse_data.vertices[t].position = glm::vec3(x + v_x, y + v_y, 0.0f);
                }

                if (from_corner == true)
                {
                    for (int t = 0; t < total_nr_vertices; ++t)
                    {
                        _ellipse_data.vertices[t].position += glm::vec3(w, h, 0.0f);
                    }
                }

                return _ellipse_data.vertices;
            }
            std::array<render::VertexPos, 4>& make_polygon_vertices(bool from_corner, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
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

                    float width = max_coord.x - min_coord.x;
                    float height = max_coord.y - min_coord.y;

                    // Center the shape
                    for (render::VertexPos& vertex : _polygon_data.vertices)
                    {
                        vertex.position.x = vertex.position.x - (width / 2.0f);
                        vertex.position.y = vertex.position.y - (height / 2.0f);
                    }
                }

                return _polygon_data.vertices;
            }
            std::array<render::VertexPos, 3>& make_triangle_vertices(bool from_corner, float x1, float y1, float x2, float y2, float x3, float y3)
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

                    float width = max_coord.x - min_coord.x;
                    float height = max_coord.y - min_coord.y;

                    // Center the shape
                    for (render::VertexPos& vertex : internal::_triangle_data.vertices)
                    {
                        vertex.position.x = vertex.position.x - (width / 2.0f);
                        vertex.position.y = vertex.position.y - (height / 2.0f);
                    }
                }

                return _triangle_data.vertices;
            }

            std::vector<render::Index>& make_ellipse_indices(float x, float y, float w, float h, int detail = 25)
            {
                int total_nr_indices = detail * 3;

                _ellipse_data.indices.clear();
                _ellipse_data.indices.resize(total_nr_indices);

                int i = 0;
                for (int t = 0; t < total_nr_indices; t += 3)
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

            namespace image
            {
                std::array<render::VertexPosTex, 4>& make_quad_vertices(bool from_corner, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
                {
                    _image_data.vertices[0].position = glm::vec3(x1, y1, 0);
                    _image_data.vertices[1].position = glm::vec3(x2, y2, 0);
                    _image_data.vertices[2].position = glm::vec3(x3, y3, 0);
                    _image_data.vertices[3].position = glm::vec3(x4, y4, 0);

                    _image_data.vertices[0].texcoord = glm::vec2(0.0f, 0.0f);
                    _image_data.vertices[1].texcoord = glm::vec2(1.0f, 0.0f);
                    _image_data.vertices[2].texcoord = glm::vec2(0.0f, 1.0f);
                    _image_data.vertices[3].texcoord = glm::vec2(1.0f, 1.0f);

                    if (from_corner == false)
                    {
                        // Find the minimum and maximum coordinates
                        glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3, x4 }), std::min({ y1, y2, y3, y4 }), 0.0f);
                        glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3, x4 }), std::max({ y1, y2, y3, y4 }), 0.0f);

                        float width = max_coord.x - min_coord.x;
                        float height = max_coord.y - min_coord.y;

                        // Center the shape
                        for (render::VertexPosTex& vertex : _image_data.vertices)
                        {
                            vertex.position.x = vertex.position.x - (width / 2.0f);
                            vertex.position.y = vertex.position.y - (height / 2.0f);
                        }
                    }

                    return _image_data.vertices;
                }

                std::array<render::Index, 6>& make_image_indices()
                {
                    return _image_data.indices;
                }
            }
        }

        namespace image
        {
            render::RenderItem extrude_image(render::VertexPosTex* vertices, int vertex_count, float extrusion_width)
            {
                constexpr int vertices_per_triangle = 4;
                constexpr int vertices_per_extrusion_segment = 2;

                internal::_image_data.extrude_vertices.clear();
                internal::_image_data.extrude_vertices.reserve(vertex_count * vertices_per_extrusion_segment);

                internal::_image_data.extrude_indices.clear();
                internal::_image_data.extrude_indices.reserve(vertex_count * vertices_per_extrusion_segment * vertices_per_triangle);

                glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
                for (int i = 0; i < vertex_count; ++i)
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

            render::ImageItem make_image(bool from_corner, float x, float y, float w, float h, s32 image_id)
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

        render::RenderItem extrude_point(render::VertexPos* vertices, int vertex_count, float extrusion_width)
        {
            const bool from_center = true;

            return geometry::make_ellipse(from_center, vertices[0].position.x, vertices[0].position.y, extrusion_width, extrusion_width);
        }

        render::RenderItem extrude_line(render::VertexPos* vertices, int vertex_count, float extrusion_width)
        {
            const bool from_center = true;

            const float stroke_x1 = vertices[0].position.x - extrusion_width; const float stroke_x4 = vertices[1].position.y + extrusion_width;
            const float stroke_y1 = vertices[0].position.y - extrusion_width; const float stroke_y4 = vertices[1].position.y + extrusion_width;

            const float stroke_x2 = vertices[1].position.y + extrusion_width; const float stroke_x3 = vertices[0].position.y - extrusion_width;
            const float stroke_y2 = vertices[1].position.y - extrusion_width; const float stroke_y3 = vertices[0].position.y + extrusion_width;

            return geometry::make_polygon(from_center, stroke_x1, stroke_y1, stroke_x2, stroke_y2, stroke_x3, stroke_y3, stroke_x4, stroke_y4);
        }

        render::RenderItem extrude_ellipse(render::VertexPos* vertices, int vertex_count, float extrusion_width)
        {
            constexpr int vertices_per_triangle = 3;
            constexpr int vertices_per_extrusion_segment = 2;

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

        render::RenderItem extrude_polygon(render::VertexPos* vertices, int vertex_count, float extrusion_width)
        {
            constexpr int vertices_per_triangle = 4;
            constexpr int vertices_per_extrusion_segment = 2;

            internal::_polygon_data.extrude_vertices.clear();
            internal::_polygon_data.extrude_vertices.reserve(vertex_count * vertices_per_extrusion_segment);

            internal::_polygon_data.extrude_indices.clear();
            internal::_polygon_data.extrude_indices.reserve(vertex_count * vertices_per_extrusion_segment * vertices_per_triangle);

            glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
            for (int i = 0; i < vertex_count; ++i)
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

        render::RenderItem extrude_triangle(render::VertexPos* vertices, int vertex_count, float extrusion_width)
        {
            constexpr int vertices_per_triangle = 4;
            constexpr int vertices_per_extrusion_segment = 2;

            internal::_triangle_data.extrude_vertices.clear();
            internal::_triangle_data.extrude_vertices.reserve(vertex_count * vertices_per_extrusion_segment);

            internal::_triangle_data.extrude_indices.clear();
            internal::_triangle_data.extrude_indices.reserve(vertex_count * vertices_per_extrusion_segment * vertices_per_triangle);

            glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
            for (int i = 0; i < vertex_count; ++i)
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

        render::RenderItem make_point(float x, float y)
        {
            internal::_point_data.vertex.position.x = x;
            internal::_point_data.vertex.position.y = y;

            return render::RenderItem { &internal::_point_data.vertex, 1, &internal::_point_data.index, 1 };
        }

        render::RenderItem make_line(float x1, float y1, float x2, float y2)
        {
            internal::_line_data.vertices[0].position = glm::vec3(x1, y1, 0.0f);
            internal::_line_data.vertices[1].position = glm::vec3(x2, y2, 0.0f);

            return render::RenderItem { internal::_line_data.vertices.data(), internal::_line_data.vertices.size(), internal::_line_data.indices.data(), internal::_line_data.indices.size() };
        }

        render::RenderItem make_ellipse(bool from_corner, float x, float y, float w, float h, int detail)
        {
            auto& vertices = internal::make_ellipse_vertices(from_corner, x, y, w, h, detail);
            auto& indices = internal::make_ellipse_indices(x, y, w, h, detail);

            return render::RenderItem { vertices.data(), vertices.size(), indices.data(), indices.size() };
        }

        render::RenderItem make_polygon(bool from_corner, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
        {
            auto& vertices = internal::make_polygon_vertices(from_corner, x1, y1, x2, y2, x3, y3, x4, y4);
            auto& indices = internal::make_polygon_indices();

            return render::RenderItem { vertices.data(), vertices.size(), indices.data(), indices.size() };
        }

        render::RenderItem make_triangle(bool from_corner, float x1, float y1, float x2, float y2, float x3, float y3)
        {
            auto& vertices = internal::make_triangle_vertices(from_corner, x1, y1, x2, y2, x3, y3);
            auto& indices = internal::make_triangle_indices();

            return render::RenderItem { vertices.data(), vertices.size(), indices.data(), indices.size() };
        }
    }
}