#pragma once

#include <functional>
#include <string>

namespace ppp
{
    // Utilities
    // // Calculates the position and size of the smallest box that contains the geometry.
    // void calculate_bounding_box(std::size_t geometry_id);
    // // Calculate the normal vector for each vertex on the geometry
    // void calculate_normals(std::size_t geometry_id);

    // // An array with the geometry's vertices.
    // std::vector<vector3> vertex_positions();
    // // An array that lists the texture coordinates for each of the geometry's vertices.
    // std::vector<vector2> vertex_uvs();
    // // An array with the vectors that are normal to the geometry's vertices.
    // std::vector<vector3> vertex_normals();

    // Attributes
    // The drawing mode on how a particular shape is drawn
    enum class shape_mode_type
    {
        CENTER,
        CORNER
    };
    // The shading type on how a particular shape is drawn
    enum class normal_mode_type
    {
        FLAT,
        SMOOTH
    };

    // Changes where ellipses and circles are drawn
    void ellipse_mode(shape_mode_type mode);
    // Changes where rectangles and squares are drawn
    void rect_mode(shape_mode_type mode);
    // Changes where triangles are drawn
    void triangle_mode(shape_mode_type mode);
    // Change how shapes are lit, using unique or duplicated normals
    void normal_mode(normal_mode_type mode);

    // Should we draw the wireframe of a mesh
    void enable_wireframe_mode(bool enable);
    // Should we draw the solid of a mesh
    void enable_solid_mode(bool enable);

    // 2D Shapes
    // Draws a circle
    std::size_t circle(float x, float y, float r, int detail = 25);
    // Draws an ellipse (oval)
    std::size_t ellipse(float x, float y, float w, float h, int detail = 25);
    // Draws a straight line between two points
    std::size_t line(float x1, float y1, float x2, float y2);
    // Draws a single point in 2D space
    std::size_t point(float x, float y);
    // Draws a rectangle
    std::size_t rect(float x, float y, float w, float h);
    // Draws a square
    std::size_t square(float x, float y, float s);
    // Draws a triangle
    std::size_t triangle(float x1, float y1, float x2, float y2, float x3, float y3);
    // Draws a polygon with 4 corners
    std::size_t polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

    // 3D Shapes
    // Draws a box (rectangular prism)
    std::size_t box(float width, float height, float depth);
    // Draws a cone
    std::size_t cone(float radius, float height, int detail = 24, bool cap = true);
    // Draws a cyclinder
    std::size_t cylinder(float radius, float height, int detail = 24, bool bottom_cap = true, bool top_cap = true);
    // Draws a plane
    std::size_t plane(float width, float height);
    // Draws a sphere
    std::size_t sphere(float radius, int detail = 24);
    // Dras a torus
    std::size_t torus(float radius, float tube_radius, int detailx = 24, int detaily = 16);
    // Draws a single point in 3D space
    std::size_t point(float x, float y, float z);
    // Draws a tetrahedron
    std::size_t tetrahedron(float width, float height);
    // Draws an octahedron
    std::size_t octahedron(float width, float height);
}