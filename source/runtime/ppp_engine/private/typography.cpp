#include "typography.h"
#include "util/log.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ppp
{
    namespace typography
    {
        void text_size(float size)
        {
        }

        void text(const std::string &text)
        {
        }

        Font load_font(const std::string &font)
        {
            // FreeType
            // --------
            FT_Library ft;

            // All functions return a value different than 0 whenever an error occurred
            if (FT_Init_FreeType(&ft))
            {
                log::error("Could not init FreeType Library");
                return {};
            }

            FT_Face face;
            if (FT_New_Face(ft, font.c_str(), 0, &face))
            {
                log::error("Failed to load font: ", font);
                return {};
            }

            // // set size to load glyphs as
            // FT_Set_Pixel_Sizes(face, 0, 48);

            // // disable byte-alignment restriction
            // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // // load first 128 characters of ASCII set
            // for (unsigned char c = 0; c < 128; c++)
            // {
            //     // Load character glyph
            //     if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            //     {
            //         log::error("Failed to load Glyph: ", c);
            //         continue;
            //     }

            //     // generate texture
            //     unsigned int texture;
            //     glGenTextures(1, &texture);
            //     glBindTexture(GL_TEXTURE_2D, texture);
            //     glTexImage2D(
            //         GL_TEXTURE_2D,
            //         0,
            //         GL_RED,
            //         face->glyph->bitmap.width,
            //         face->glyph->bitmap.rows,
            //         0,
            //         GL_RED,
            //         GL_UNSIGNED_BYTE,
            //         face->glyph->bitmap.buffer);
            //     // set texture options
            //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //     // now store character for later use
            //     Character character = {
            //         texture,
            //         glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            //         glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            //         static_cast<unsigned int>(face->glyph->advance.x)};
            //     Characters.insert(std::pair<char, Character>(c, character));
            // }

            // glBindTexture(GL_TEXTURE_2D, 0);

            // destroy FreeType once we're finished
            FT_Done_Face(face);
            FT_Done_FreeType(ft);
        }
    }
}