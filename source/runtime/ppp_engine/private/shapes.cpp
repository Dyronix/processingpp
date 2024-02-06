#include "shapes.h"
#include "render/render.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>

namespace ppp
{
    namespace shapes
    {
        namespace internal
        {
            ShapeMode _rect_mode = ShapeMode::CENTER;
            ShapeMode _ellipse_mode = ShapeMode::CENTER;

            render::VertexPosArr _ellipse_vertices;
            render::Indices _ellipse_indices;

            std::array<render::VertexPos, 4> _polygon_vertices;
            std::array<render::Index, 6> _polygon_indices = { 0, 1 ,2, 1, 2, 3 };

            std::array<render::VertexPos, 3> _triangle_vertices;
            std::array<render::Index, 3> _triangle_indices = {0, 1, 2};

            std::array<render::VertexPos, 2> _line_vertices;
            std::array<render::Index, 2> _line_indices = { 0, 1 };

            render::VertexPos _point_vertex;
            render::Index _point_index = { 0 };
        }

        //-------------------------------------------------------------------------
        void rect_mode(ShapeMode mode)
        {
            internal::_rect_mode = mode;
        }

        //-------------------------------------------------------------------------
        void ellipse_mode(ShapeMode mode)
        {
            internal::_ellipse_mode = mode;
        }

        //-------------------------------------------------------------------------
        void point_width(float pw)
        {
            render::push_stroke_enable(true);
            render::push_stroke_width(pw);
        }

        //-------------------------------------------------------------------------
        void line_width(float lw)
        {
            render::push_stroke_enable(true);
            render::push_stroke_width(lw);
        }

        //-------------------------------------------------------------------------
        void ellipse(float x, float y, float w, float h, int detail)
        {
            int total_nr_vertices = detail + 1; // take center point into account
            int total_nr_indices = detail * 3;

            internal::_ellipse_vertices.clear();
            internal::_ellipse_indices.clear();

            internal::_ellipse_vertices.resize(total_nr_vertices);
            internal::_ellipse_indices.resize(total_nr_indices);

            internal::_ellipse_vertices[0].position = glm::vec3{ x, y, 0.0f };

            for (int t = 1; t < total_nr_vertices; ++t)
            {
                float angle = (t / static_cast<float>(detail)) * glm::two_pi<float>();

                float v_x = sin(angle) * w;
                float v_y = cos(angle) * h;

                internal::_ellipse_vertices[t].position = glm::vec3(x + v_x, y + v_y, 0.0f);
            }

            if (internal::_ellipse_mode == ShapeMode::CORNER)
            {
                for (int t = 0; t < total_nr_vertices; ++t)
                {
                    internal::_ellipse_vertices[t].position += glm::vec3(w, h, 0.0f);
                }
            }

            int i = 0;
            for (int t = 0; t < total_nr_indices; t += 3)
            {
                internal::_ellipse_indices[t + 0] = 0;    
                internal::_ellipse_indices[t + 1] = i + 1;
                internal::_ellipse_indices[t + 2] = i + 2;

                ++i;
            }

            // Loop back to the first triangle
            internal::_ellipse_indices[total_nr_indices - 1] = 1;

            render::submit_render_item(render::TopologyType::TRIANGLES, { internal::_ellipse_vertices.data(), internal::_ellipse_vertices.size(), internal::_ellipse_indices.data(), internal::_ellipse_indices.size() });
        }
        //-------------------------------------------------------------------------
        void circle(float x, float y, float r, int detail)
        {
            ellipse(x, y, r, r, detail);
        }
        //-------------------------------------------------------------------------
        void line(float x1, float y1, float x2, float y2)
        {
            internal::_line_vertices[0].position = glm::vec3(x1, y1, 0.0f);
            internal::_line_vertices[1].position = glm::vec3(x2, y2, 0.0f);

            render::submit_render_item(render::TopologyType::LINES, { internal::_line_vertices.data(), internal::_line_vertices.size(), internal::_line_indices.data(), internal::_line_indices.size() });
        }
        //-------------------------------------------------------------------------
        void point(float x, float y)
        {
            internal::_point_vertex.position.x = x;
            internal::_point_vertex.position.y = y;

            render::submit_render_item(render::TopologyType::POINTS, { &internal::_point_vertex, 1, &internal::_point_index, 1 });
        }
        //-------------------------------------------------------------------------
        void polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
        {
            internal::_polygon_vertices[0].position = glm::vec3(x1, y1, 0);
            internal::_polygon_vertices[1].position = glm::vec3(x2, y2, 0);
            internal::_polygon_vertices[2].position = glm::vec3(x3, y3, 0);
            internal::_polygon_vertices[3].position = glm::vec3(x4, y4, 0);

            if (internal::_rect_mode == ShapeMode::CENTER)
            {
                // Find the minimum and maximum coordinates
                glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3, x4 }), std::min({ y1, y2, y3, y4 }), 0.0f);
                glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3, x4 }), std::max({ y1, y2, y3, y4 }), 0.0f);

                float width = max_coord.x - min_coord.x;
                float height = max_coord.y - min_coord.y;

                // Center the shape
                for (render::VertexPos& vertex : internal::_polygon_vertices)
                {
                    vertex.position.x = vertex.position.x - (width / 2.0f);
                    vertex.position.y = vertex.position.y - (height / 2.0f);
                }
            }

            render::submit_render_item(render::TopologyType::TRIANGLES, { internal::_polygon_vertices.data(), internal::_polygon_vertices.size(), internal::_polygon_indices.data(), internal::_polygon_indices.size() });
        }
        //-------------------------------------------------------------------------
        void rect(float x, float y, float w, float h)
        {
            polygon(x, y, x + w, y, x, y + h, x + w, y + h);
        }
        //-------------------------------------------------------------------------
        void square(float x, float y, float s)
        {
            rect(x, y, s, s);
        }
        //-------------------------------------------------------------------------
        void triangle(float x1, float y1, float x2, float y2, float x3, float y3)
        {
            // Calculate vertices
            internal::_triangle_vertices[0].position = glm::vec3(x1, y1, 0);
            internal::_triangle_vertices[1].position = glm::vec3(x2, y2, 0);
            internal::_triangle_vertices[2].position = glm::vec3(x3, y3, 0);

            // Find the minimum and maximum coordinates
            glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3 }), std::min({ y1, y2, y3 }), 0.0f);
            glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3 }), std::max({ y1, y2, y3 }), 0.0f);

            float width = max_coord.x - min_coord.x;
            float height = max_coord.y - min_coord.y;

            // Center the shape
            for (render::VertexPos& vertex : internal::_triangle_vertices)
            {
                vertex.position.x = vertex.position.x - (width / 2.0f);
                vertex.position.y = vertex.position.y - (height / 2.0f);
            }

            render::submit_render_item(render::TopologyType::TRIANGLES, { internal::_triangle_vertices.data(), internal::_triangle_vertices.size(), internal::_triangle_indices.data(), internal::_triangle_indices.size() });
        }
    }
}