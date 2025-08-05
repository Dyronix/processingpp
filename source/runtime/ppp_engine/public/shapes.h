/**
 * @file shapes.h
 * @brief Draw 2D and 3D primitive shapes.
 */

#pragma once

#include <string>

namespace ppp
{
    using geometry_id = std::size_t; /** Type alias for unique identifier for goemetry. */

    /** @brief Positioning modes for 2D shapes. */
    enum class shape_mode_type : std::uint8_t
    {
        CENTER,
        CORNER
    };

    /** @brief Normal vector generation modes for 3D shapes. */
    enum class normal_mode_type : std::uint8_t
    {
        FLAT,
        SMOOTH
    };

    /**
     * @brief Enable shadow casting for subsequent shapes.
     */
    void enable_shadows();

    /**
     * @brief Disable shadow casting for subsequent shapes.
     */
    void disable_shadows();

    /**
     * @brief Set rectangle and square coordinate mode.
     * @param mode CENTER or CORNER.
     */
    void rect_mode(shape_mode_type mode);

    /**
     * @brief Set ellipse and circle coordinate mode.
     * @param mode CENTER or CORNER.
     */
    void ellipse_mode(shape_mode_type mode);

    /**
     * @brief Set triangle coordinate mode.
     * @param mode CENTER or CORNER.
     */
    void triangle_mode(shape_mode_type mode);

    /**
     * @brief Set normal generation mode for 3D shapes.
     * @param mode FLAT or SMOOTH.
     */
    void normal_mode(normal_mode_type mode);

    /**
     * @brief Toggle wireframe overlay.
     * @param enable True to enable wireframe.
     */
    void enable_wireframe_mode(bool enable);

    /**
     * @brief Toggle solid face rendering.
     * @param enable True to enable solid faces.
     */
    void enable_solid_mode(bool enable);

    /**
     * @brief Draw a circle.
     */
    geometry_id circle(float x, float y, float r, int detail = 25);

    /**
    * @brief Draw a circle.
    */
    geometry_id circle(float r, int detail = 25);

    /**
     * @brief Draw an ellipse.
     */
    geometry_id ellipse(float x, float y, float w, float h, int detail = 25);

    /**
     * @brief Draw an ellipse.
     */
    geometry_id ellipse(float w, float h, int detail = 25);

    /**
     * @brief Draw a line between two points.
     */
    geometry_id line(float x1, float y1, float x2, float y2);

    /**
     * @brief Draw a single 2D point.
     */
    geometry_id point(float x, float y);

    /**
     * @brief Draw a rectangle.
     */
    geometry_id rect(float x, float y, float w, float h);

     /**
     * @brief Draw a rectangle.
     */
    geometry_id rect(float w, float h);

    /**
     * @brief Draw a square.
     */
    geometry_id square(float x, float y, float s);

    /**
     * @brief Draw a square.
     */
    geometry_id square(float s);

    /**
     * @brief Draw a triangle.
     */
    geometry_id triangle(float x1, float y1, float x2, float y2, float x3, float y3);

    /**
     * @brief Draw a four-sided polygon.
     */
    geometry_id polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

    /**
     * @brief Draw a 3D box.
     */
    geometry_id box(float width, float height, float depth);

    /**
    * @brief Draw a 3D box.
    */
    geometry_id box(float size);

    /**
     * @brief Draw a 3D cone.
     */
    geometry_id cone(float radius, float height, int detail = 24, bool cap = true);

    /**
     * @brief Draw a 3D cylinder.
     */
    geometry_id cylinder(float radius, float height, int detail = 24, bool bottom_cap = true, bool top_cap = true);

    /**
     * @brief Draw a 3D plane.
     */
    geometry_id plane(float width, float height);

    /**
     * @brief Draw a 3D sphere.
     */
    geometry_id sphere(float radius, int detail = 24);

    /**
     * @brief Draw a 3D torus.
     */
    geometry_id torus(float radius, float tube_radius, int detailx = 24, int detaily = 16);

    /**
     * @brief Draw a single 3D point.
     */
    geometry_id point(float x, float y, float z);

    /**
     * @brief Draw a tetrahedron.
     */
    geometry_id tetrahedron(float width, float height);

    /**
     * @brief Draw an octahedron.
     */
    geometry_id octahedron(float width, float height);
}