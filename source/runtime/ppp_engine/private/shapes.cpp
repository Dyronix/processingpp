#include "shapes.h"
#include "render/render.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace ppp
{
    namespace shapes
    {
        //-------------------------------------------------------------------------
        void ellipse(float x, float y, float w, float h, int detail)
        {
            int total_nr_vertices = detail + 1; // take center point into account
            int total_nr_indices = detail * 3;

            render::VertexPosArr vertices;
            render::Indices indices;

            vertices.resize(total_nr_vertices);
            indices.resize(total_nr_indices);

            vertices[0].position = glm::vec3{ x, y, 0.0f };

            for (int t = 1; t < total_nr_vertices; ++t)
            {
                float angle = (t / static_cast<float>(detail)) * glm::two_pi<float>();

                float v_x = sin(angle) * w;
                float v_y = cos(angle) * h;

                vertices[t].position = glm::vec3(x + v_x, y + v_y, 0.0f);
            }

            int i = 0;
            for (int t = 0; t < total_nr_indices; t += 3)
            {
                indices[t + 0] = 0;    
                indices[t + 1] = i + 1;
                indices[t + 2] = i + 2;

                ++i;
            }

            // Loop back to the first triangle
            indices[total_nr_indices - 1] = 1;

            render::submit_render_item(render::TopologyType::TRIANGLES, { vertices, indices });
        }
        //-------------------------------------------------------------------------
        void circle(float x, float y, float r, int detail)
        {
            ellipse(x, y, r, r, detail);
        }
        //-------------------------------------------------------------------------
        void line(float x1, float y1, float x2, float y2)
        {
            render::VertexPosArr vertices = { render::VertexPos { glm::vec3(x1, y1, 0) }, render::VertexPos { glm::vec3(x2, y2, 0) } };

            render::submit_render_item(render::TopologyType::LINES, { vertices, {0, 1} });
        }
        //-------------------------------------------------------------------------
        void point(float x, float y)
        {
            render::VertexPosArr vertices = { render::VertexPos{ glm::vec3(x, y, 0) } };

            render::submit_render_item(render::TopologyType::POINTS, { vertices, {0} });
        }
        //-------------------------------------------------------------------------
        void polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
        {
            render::VertexPosArr vertices;
            render::Indices indices;

            vertices.resize(4);
            indices.resize(6);

            vertices[0].position = glm::vec3(x1, y1, 0);
            vertices[1].position = glm::vec3(x2, y2, 0);
            vertices[2].position = glm::vec3(x3, y3, 0);
            vertices[3].position = glm::vec3(x4, y4, 0);

            // Find the minimum and maximum coordinates
            glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3, x4 }), std::min({ y1, y2, y3, y4 }), 0.0f);
            glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3, x4 }), std::max({ y1, y2, y3, y4 }), 0.0f);

            float width = max_coord.x - min_coord.x;
            float height = max_coord.y - min_coord.y;

            // Center the shape
            for (render::VertexPos& vertex : vertices)
            {
                vertex.position.x = vertex.position.x - (width / 2.0f);
                vertex.position.y = vertex.position.y - (height / 2.0f);
            }

            // Set triangles
            indices[0] = 0;
            indices[1] = 1;
            indices[2] = 2;
            indices[3] = 1;
            indices[4] = 2;
            indices[5] = 3;

            render::submit_render_item(render::TopologyType::TRIANGLES, { vertices, indices });
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
            render::VertexPosArr vertices;
            render::Indices indices;

            vertices.resize(3);
            indices.resize(3);

            // Calculate vertices
            vertices[0].position = glm::vec3(x1, y1, 0);
            vertices[1].position = glm::vec3(x2, y2, 0);
            vertices[2].position = glm::vec3(x3, y3, 0);

            // Find the minimum and maximum coordinates
            glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3 }), std::min({ y1, y2, y3 }), 0.0f);
            glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3 }), std::max({ y1, y2, y3 }), 0.0f);

            float width = max_coord.x - min_coord.x;
            float height = max_coord.y - min_coord.y;

            // Center the shape
            for (render::VertexPos& vertex : vertices)
            {
                vertex.position.x = vertex.position.x - (width / 2.0f);
                vertex.position.y = vertex.position.y - (height / 2.0f);
            }

            // Set triangles
            indices[0] = 0;
            indices[1] = 1;
            indices[2] = 2;

            render::submit_render_item(render::TopologyType::TRIANGLES, { vertices, indices });
        }
    }
}