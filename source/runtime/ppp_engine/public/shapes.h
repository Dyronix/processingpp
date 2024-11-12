#pragma once

#include <functional>
#include <string>

namespace ppp
{
    namespace shapes
    {
        enum class shape_mode_type
        {
            CENTER,
            CORNER
        };

        enum class normal_mode_type
        {
            FLAT,
            SMOOTH
        };

        void ellipse_mode(shape_mode_type mode);
        void rect_mode(shape_mode_type mode);
        void triangle_mode(shape_mode_type mode);
        
        void normal_mode(normal_mode_type mode);

        void enable_wireframe_mode(bool enable);
        void enable_solid_mode(bool enable);

        void ellipse(float x, float y, float w, float h, int detail = 25);
        void circle(float x, float y, float r, int detail = 25);
        void line(float x1, float y1, float x2, float y2);
        void point(float x, float y);
        void polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
        void rect(float x, float y, float w, float h);
        void square(float x, float y, float s);
        void triangle(float x1, float y1, float x2, float y2, float x3, float y3);

        void point(float x, float y, float z);
        void box(float width, float height, float depth);
        void cylinder(float radius, float height, int detail = 24, bool bottom_cap = true, bool top_cap = true);
        void plane(float width, float height);
        void sphere(float radius, int detail = 24);
        void torus(float radius, float tube_radius, int detailx = 24, int detaily = 16);
        void cone(float radius, float height, int detail = 24, bool cap = true);
        void tetrahedron(float width, float height);
        void octahedron(float width, float height);

        void build_primitive_geometry(std::function<void()> callback);
        void build_textured_geometry(std::function<void()> callback);
        void build_custom_geometry(const std::string& tag, std::function<void()> callback);
    }
}