#include "shapes.h"
#include "render.h"

namespace ppp
{
    namespace shapes
    {
        //-------------------------------------------------------------------------
        void ellipse(f32 x, f32 y, f32 w, f32 h, s32 detail)
        {
            render::Vertices vertices;
            render::Indices indices;

            vertices.resize(detail + 2);
            indices.resize(detail * 3);

            vertices[0] = glm::vec3(x, y, 0.0f); // Center of the fan

            // Create vertices and triangles for the fan
            for (s32 i = 0; i <= detail; i++)
            {
                f32 angle = (f32)i / detail * 360.0f;
                f32 v_x = sin(glm::radians(angle)) * w;
                f32 v_y = cos(glm::radians(angle)) * h;

                vertices[i + 1] = glm::vec3(x + v_x, y + v_y, 0.0f);

                if (i < detail)
                {
                    indices[i * 3] = 0;
                    indices[i * 3 + 1] = i + 1;
                    indices[i * 3 + 2] = i + 2;
                }
            }

            render::submit_render_item(render::TopologyType::TRIANGLES, { vertices, indices });
        }
        //-------------------------------------------------------------------------
        void circle(f32 x, f32 y, f32 r, s32 detail)
        {
            ellipse(x, y, r, r, detail);
        }
        //-------------------------------------------------------------------------
        void line(f32 x1, f32 y1, f32 x2, f32 y2)
        {
            render::Vertices vertices = { glm::vec3(x1, y1, 0), glm::vec3(x2, y2, 0) };

            render::submit_render_item(render::TopologyType::LINES, { vertices, {} });
        }
        //-------------------------------------------------------------------------
        void point(f32 x, f32 y)
        {
            render::Vertices vertices = { glm::vec3(x, y, 0) };

            render::submit_render_item(render::TopologyType::POINTS, { vertices, {} });
        }
        //-------------------------------------------------------------------------
        void polygon(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4)
        {
            render::Vertices vertices;
            render::Indices indices;

            vertices.resize(4);
            indices.resize(6);

            // Calculate vertices
            vertices[0] = glm::vec3(x1, y1, 0);
            vertices[1] = glm::vec3(x2, y2, 0);
            vertices[2] = glm::vec3(x3, y3, 0);
            vertices[3] = glm::vec3(x4, y4, 0);

            // Set triangles
            indices[0] = 0;
            indices[1] = 2;
            indices[2] = 1;

            indices[3] = 2;
            indices[4] = 3;
            indices[5] = 1;

            render::submit_render_item(render::TopologyType::TRIANGLES, { vertices, indices });
        }
        //-------------------------------------------------------------------------
        void rect(f32 x, f32 y, f32 w, f32 h)
        {
            polygon(x, y, x + w, y, x, y + h, x + w, y + h);
        }
        //-------------------------------------------------------------------------
        void square(f32 x, f32 y, f32 s)
        {
            rect(x, y, s, s);
        }
        //-------------------------------------------------------------------------
        void triangle(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3)
        {
            render::Vertices vertices;
            render::Indices indices;

            vertices.resize(3);
            indices.resize(3);

            // Calculate vertices
            vertices[0] = glm::vec3(x1, y1, 0);
            vertices[1] = glm::vec3(x2, y2, 0);
            vertices[2] = glm::vec3(x3, y3, 0);

            // Set triangles
            indices[0] = 0;
            indices[1] = 2;
            indices[2] = 1;

            render::submit_render_item(render::TopologyType::TRIANGLES, { vertices, indices });
        }
    }
}