/**
 * @file rendering.h
 * @brief Configure the canvas and render draw modes.
 */

#pragma once

namespace ppp
{
    /**
     * @brief Enable the scissor test and define the clipping rectangle.
     * @param x X-coordinate of the rectangle origin.
     * @param y Y-coordinate of the rectangle origin.
     * @param w Width of the clipping rectangle.
     * @param h Height of the clipping rectangle.
     */
    void create_canvas(float x, float y, float w, float h);

    /**
     * @brief Update the existing scissor test rectangle.
     * @param x New X-coordinate of the rectangle origin.
     * @param y New Y-coordinate of the rectangle origin.
     * @param w New width of the clipping rectangle.
     * @param h New height of the clipping rectangle.
     */
    void resize_canvas(float x, float y, float w, float h);

    /**
     * @brief Disable the scissor test and remove clipping.
     */
    void no_canvas();

    /**
     * @brief Set the line width for subsequent wireframe rendering.
     * @param line_width Thickness of the wireframe lines.
     */
    void wireframe_linewidth(float line_width);

    /**
     * @brief Set the RGB color for subsequent wireframe rendering.
     * @param r Red component in [0.0, 1.0].
     * @param g Green component in [0.0, 1.0].
     * @param b Blue component in [0.0, 1.0].
     */
    void wireframe_color(float r, float g, float b);

    /**
     * @brief Switch to instanced draw mode for subsequent renders.
     */
    void enable_instance_draw_mode();

    /**
     * @brief Switch to batched draw mode for subsequent renders.
     */
    void enable_batched_draw_mode();
}