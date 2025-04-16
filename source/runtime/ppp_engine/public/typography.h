#pragma once

#include <string>
#include <map>

namespace ppp
{
    // A unique identifier to describe each font
    using font_id = unsigned int;

    // Attributes
    // The drawing mode on how a particular font character is drawn
    enum class text_mode_type
    {
        CENTER,
        CORNER
    };

    // Sets the font size when text() is called
    void text_size(unsigned int size);
    // Changes where font characters are drawn
    void text_mode(text_mode_type mode);

    // Load & Display
    // Draws text to the canvas.
    void text(std::string_view text, float x, float y);
    // Sets the font used by the text() function.
    void text_font(const font_id& font);

    // Loads a font and creates a Font object
    font_id load_font(std::string_view path, unsigned int size = 16, unsigned int characters_to_load = 128);
}