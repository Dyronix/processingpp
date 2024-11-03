#include "typography.h"
#include "render/render.h"
#include "resources/font_pool.h"
#include "fileio/fileio.h"

#include "util/log.h"
#include "util/types.h"
#include "geometry/2d/geometry_2d.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ppp
{
    namespace typography
    {
        void text_size(unsigned int size)
        {
            const font_pool::Font* active_font = font_pool::active_font();

            if (active_font->size * 0.5f > size || active_font->size * 2.0f < size)
            {
                // Append new size to font atlas
            }
            else
            {
                // Change font scale
            }
        }

        void text(const std::string &text, float x, float y)
        {           
            const font_pool::Font* active_font = font_pool::active_font();
            if (active_font == nullptr)
            {
                log::warn("No font specified as active font, please call typography::text_font");
                return;
            }

            f32 scale = 1.0f;

            std::string::const_iterator it;
            for(it = std::cbegin(text); it != std::cend(text); ++it)
            {
                const FontCharacter& ch = active_font->atlas.characters.at(*it);

                f32 xpos = x + ch.bearing.x * scale;
                f32 ypos = y - (ch.size.y - ch.bearing.y) * scale;

                f32 w = ch.size.x * scale;
                f32 h = ch.size.y * scale;

                render::render_item item = geometry::font::make_font(true, xpos, ypos, w, h, ch.uv_start.x, ch.uv_start.y, ch.uv_end.x, ch.uv_end.y, active_font->atlas.texture_id);

                render::submit_font_item(item);

                // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
            }
        }

        void text_font(const font_id& font)
        {
            font_pool::load_active_font(font);
        }

        font_id load_font(const std::string& path, unsigned int size, unsigned int characters_to_load)
        {
            if (font_pool::has_font(path))
            {
                const font_pool::Font* font = font_pool::font_at_path(path);
                return font->atlas.texture_id;
            }

            FT_Library ft;
            if (FT_Init_FreeType(&ft))
            {
                log::error("Could not init FreeType Library");
                return -1;
            }

            auto buffer = fileio::read_binary_file(path);

            FT_Face face;
            if (FT_New_Memory_Face(ft, reinterpret_cast<const FT_Byte*>(buffer.data()), buffer.size(), 0, &face))
            {
                log::error("Failed to load font: {}", path);
                return -1;
            }

            // set size to load glyphs as
            FT_Set_Pixel_Sizes(face, 0, size);

            font_pool::Font font;

            font.file_path = path;
            font.size = size;
            font.atlas = make_font_atlas(face, characters_to_load);

            assert(font.atlas.characters.size() == characters_to_load);

            // destroy FreeType once we're finished
            FT_Done_Face(face);
            FT_Done_FreeType(ft);

            font_pool::add_new_font(font);

            return font.atlas.texture_id;
        }
    }
}