#pragma once

namespace ppp
{
    namespace shapes
    {
        enum class ShapeMode
        {
            CENTER,
            CORNER
        };

        enum class RasterizationMode
        {
            SOLID = 1 << 0,
            WIREFRAME = 1 << 1
        };

        void ellipse_mode(ShapeMode mode);
        void rect_mode(ShapeMode mode);
        void triangle_mode(ShapeMode mode);

        void rasterization_mode(RasterizationMode mode);

        void ellipse(float x, float y, float w, float h, int detail = 25);
        void circle(float x, float y, float r, int detail = 25);
        void line(float x1, float y1, float x2, float y2);
        void point(float x, float y);
        void polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        void rect(float x, float y, float w, float h);
        void square(float x, float y, float s);
        void triangle(float x1, float y1, float x2, float y2, float x3, float y3);

        void box(float width, float height, float depth);
        void cylinder(float radius, float height, float detail = 24, bool bottom_cap = true, bool top_cap = true);
        void plane(float width, float height);
        void sphere(float radius, float detail = 24);
        void torus(float radius, float tube_radius, float detailx = 24, float detaily = 16);
        void cone(float radius, float height, float detail = 24, bool cap = true);
        void tetrahedron(float width, float height);
        void octahedron(float width, float height);
    }
}