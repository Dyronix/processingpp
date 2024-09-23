#include "typography.h"
#include "render/render.h"
#include "resources/font_pool.h"

#include "util/log.h"
#include "util/types.h"

#include <glad/glad.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ppp
{
    namespace typography
    {
        namespace internal
        {
            GLint _prev_unpack_alignment = 4;

            void set_pixel_storage(GLint storage)
            {
                glGetIntegerv(GL_UNPACK_ALIGNMENT, &_prev_unpack_alignment);

                // Set pixel unpack alignment to 1 byte (no row padding). 
                // Useful for tightly packed textures (e.g., font glyphs or non-multiple-of-4 widths).
                glPixelStorei(GL_UNPACK_ALIGNMENT, storage);
            }

            void restore_pixel_storage()
            {
                glPixelStorei(GL_UNPACK_ALIGNMENT, _prev_unpack_alignment);
            }
        
            font_pool::characcter_map load_characters(FT_Face face, u32 characters_to_load)
            {
                font_pool::characcter_map characters;

                for (u8 c = 0; c < characters_to_load; c++)
                {
                    // load character glyph
                    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                    {
                        log::error("Failed to load glyph: ", c);
                        continue;
                    }

                    u32 texture_id = render::create_image_item(
                        face->glyph->bitmap.width,
                        face->glyph->bitmap.rows,
                        1,
                        face->glyph->bitmap.buffer,
                        render::ImageFilterType::LINEAR,
                        render::ImageWrapType::CLAMP_TO_EDGE);

                    // store character for later use
                    font_pool::Character character = 
                    {
                        texture_id,
                        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                        static_cast<u32>(face->glyph->advance.x)
                    };

                    characters.emplace(c, character);
                }

                return characters;
            }
        }

        void text_size(float size)
        {
            
        }

        void text(const std::string &text)
        {
            
        }

        void text_font(const font_id& font)
        {
            font_pool::load_active_font(font);
        }

        font_id load_font(const std::string& path, unsigned int characters_to_load)
        {
            if (font_pool::has_font(path))
            {
                const font_pool::Font* font = font_pool::font_at_path(path);
                return font->font_id;
            }

            FT_Library ft;
            if (FT_Init_FreeType(&ft))
            {
                log::error("Could not init FreeType Library");
                return {};
            }

            FT_Face face;
            if (FT_New_Face(ft, path.c_str(), 0, &face))
            {
                log::error("Failed to load font: ", path);
                return {};
            }

            font_pool::Font font;

            font.file_path = path;
            font.font_id = std::hash<std::string>{} (path);

            // set size to load glyphs as
            FT_Set_Pixel_Sizes(face, 0, 48);

            internal::set_pixel_storage(1);

            font.characters = internal::load_characters(face, characters_to_load);

            assert(font.characters.size() == characters_to_load);

            internal::restore_pixel_storage();

            // destroy FreeType once we're finished
            FT_Done_Face(face);
            FT_Done_FreeType(ft);

            font_pool::add_new_font(font);

            return font.font_id;
        }
    }
}