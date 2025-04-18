/**
 * @file typography.h
 * @brief Load fonts and render text to the canvas.
 */

#pragma once

#include <string>

namespace ppp
{
    using font_id = unsigned int; /**Type alias for unique identifier for fonts. */

    /**
     * @brief Text positioning modes.
     */
    enum class text_mode_type : std::uint8_t
    {
        CENTER, 
        CORNER 
    };

    /**
     * @brief Set the font size for subsequent text operations.
     * @param size Font size in pixels.
     */
    void text_size(unsigned int size);

    /**
     * @brief Set the text positioning mode (CENTER or CORNER).
     * @param mode Text mode.
     */
    void text_mode(text_mode_type mode);

    /**
     * @brief Render a string at the specified coordinates.
     * @param str String to draw.
     * @param x X-coordinate for text placement.
     * @param y Y-coordinate for text placement.
     */
    void text(std::string_view str, float x, float y);

    /**
     * @brief Set the active font for text rendering.
     * @param font Identifier of the loaded font.
     */
    void text_font(const font_id& font);

    /**
     * @brief Load a font and generate its glyph atlas.
     * @param path Filesystem path to font file.
     * @param size Pixel size of glyphs (default: 16).
     * @param count Number of characters to include (default: 128).
     * @return Identifier for the newly loaded font.
     */
    font_id load_font(std::string_view path, unsigned int size = 16, unsigned int count = 128);
}