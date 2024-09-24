#include "typography.h"
#include "render/render.h"
#include "resources/font_pool.h"
#include "fileio/fileio.h"

#include "util/log.h"
#include "util/types.h"
#include "util/geometry.h"

#include <glad/glad.h>

#include <algorithm>

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
        
            font_pool::characcter_map load_characters_to_atlas(FT_Face face, u32 characters_to_load)
            {
                font_pool::characcter_map characters;

                int total_area = 0;

                for (u32 c = 0; c < characters_to_load; c++)
                {
                    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                    {
                        log::error("Failed to load glyph: ", c);
                        continue;
                    }

                    FT_GlyphSlot g = face->glyph;

                    // Calculate the area of the glyph in pixels
                    int glyph_area = g->bitmap.width * g->bitmap.rows;
                    total_area += glyph_area;
                }

                int atlas_side_length = std::ceil(std::sqrt(total_area));

                auto next_power_of_two = [](int x)
                {
                    int power = 1;
                    while (power < x) power *= 2;
                    return power;
                };

                int atlas_width = next_power_of_two(atlas_side_length);
                int atlas_height = next_power_of_two(atlas_side_length);

                // Now pack the glyphs and adjust atlas size if necessary
                bool success = false;
                while (!success)
                {
                    success = true;
                    int pen_x = 0, pen_y = 0, row_height = 0;

                    for (u32 c = 0; c < characters_to_load; c++)
                    {
                        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                        {
                            log::error("Failed to load glyph: ", c);
                            continue;
                        }

                        FT_GlyphSlot g = face->glyph;

                        if (pen_x + g->bitmap.width >= atlas_width)
                        {
                            pen_x = 0;
                            pen_y += row_height;
                            row_height = 0;
                        }

                        if (pen_y + g->bitmap.rows >= atlas_height)
                        {
                            atlas_width *= 2;
                            atlas_height *= 2;
                            success = false;
                            break;
                        }

                        pen_x += g->bitmap.width;
                        row_height = std::max(row_height, (int)g->bitmap.rows);
                    }
                }

                // Create an empty image buffer for the atlas
                std::vector<unsigned char> atlas_buffer(atlas_width * atlas_height, 0);

                int pen_x = 0;
                int pen_y = 0;
                int row_height = 0;

                for (u32 c = 0; c < characters_to_load; c++)
                {
                    // Load character glyph
                    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                    {
                        log::error("Failed to load glyph: ", c);
                        continue;
                    }

                    FT_GlyphSlot g = face->glyph;

                    // If the glyph will not fit in the current row, move to the next row
                    if (pen_x + g->bitmap.width >= atlas_width)
                    {
                        pen_x = 0;
                        pen_y += row_height;
                        row_height = 0;
                    }

                    // Check if we've run out of space in the atlas
                    if (pen_y + g->bitmap.rows >= atlas_height)
                    {
                        log::error("Texture atlas is full.");
                        break;
                    }

                    // Copy the glyph's bitmap into the atlas using memcpy
                    for (int row = 0; row < g->bitmap.rows; ++row)
                    {
                        // The start position in the atlas
                        unsigned char* dest = &atlas_buffer[(pen_x)+(pen_y + (g->bitmap.rows - 1 - row)) * atlas_width];

                        // The source row in the glyph's bitmap
                        unsigned char* src = &g->bitmap.buffer[row * g->bitmap.pitch];

                        // Copy the whole row at once
                        memcpy(dest, src, g->bitmap.width);
                    }

                    // Update row height
                    row_height = std::max(row_height, (int)g->bitmap.rows);

                    // Store character info (atlas position and size)
                    font_pool::Character character =
                    {
                        0,                                              // atlas texture ID
                        glm::ivec2(g->bitmap.width, g->bitmap.rows),    // size
                        glm::ivec2(g->bitmap_left, g->bitmap_top),      // bearing
                        static_cast<u32>(g->advance.x),                 // advance
                        glm::vec2(static_cast<float>(pen_x) / atlas_width, static_cast<float>(pen_y) / atlas_height),  // tex coords
                        glm::vec2(static_cast<float>(g->bitmap.width) / atlas_width, static_cast<float>(g->bitmap.rows) / atlas_height) // tex coords size
                    };

                    characters.emplace(c, character);

                    // Advance the pen position
                    pen_x += g->bitmap.width;
                }

                // Create OpenGL texture for the atlas
                u32 texture_id;
                glGenTextures(1, &texture_id);
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_width, atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, atlas_buffer.data());

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                return characters;
            }

        }

        void text_size(float size)
        {
            
        }

        void text(const std::string &text, float x, float y)
        {
            //float scale = 1.0f;
            //const font_pool::Font* active_font = font_pool::active_font();

            //std::string::const_iterator it;
            //for(it = std::cbegin(text); it != std::cend(text); ++it)
            //{
            //    const font_pool::Character& ch = active_font->characters.at(*it);

            //    float xpos = x + ch.bearing.x * scale;
            //    float ypos = y - (ch.size.y - ch.bearing.y) * scale;

            //    float w = ch.size.x * scale;
            //    float h = ch.size.y * scale;

            //    render::ImageItem item = geometry::image::make_image(true, xpos, ypos, w, h, ch.texture_id);

            //    render::submit_image_item(item);

            //    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            //    x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
            //}
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

            auto buffer = fileio::read_binary_file(path);

            FT_Face face;
            if (FT_New_Memory_Face(ft, reinterpret_cast<const FT_Byte*>(buffer.data()), buffer.size(), 0, &face))
            {
                log::error("Failed to load font: ", path);
                return {};
            }

            font_pool::Font font;

            font.file_path = path;
            font.font_id = std::hash<std::string>{} (path);

            // set size to load glyphs as
            FT_Set_Pixel_Sizes(face, 0, 16);

            //internal::set_pixel_storage(1);

            //font.characters = internal::load_characters(face, characters_to_load);
            font.characters = internal::load_characters_to_atlas(face, characters_to_load);

            assert(font.characters.size() == characters_to_load);

            //internal::restore_pixel_storage();

            // destroy FreeType once we're finished
            FT_Done_Face(face);
            FT_Done_FreeType(ft);

            font_pool::add_new_font(font);

            return font.font_id;
        }
    }
}