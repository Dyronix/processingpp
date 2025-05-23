#include "resources/font_atlas.h"
#include "render/render.h"

#include "util/log.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <optional>

namespace ppp
{
        struct glyph_bitmap
        {
            u32 width;
            u32 height;

            s32 left;
            s32 top;

            std::vector<u8> blob;
        };

        struct glyph_info
        {
            u32 character;

            s32 pitch;           

            slong advance_x;
            slong advance_y;

            glyph_bitmap bitmap;
        };

        struct font_atlas_dimensions
        {
            u32 width;
            u32 height;
        };

        glyph_info copy_glyph_info(u32 character, FT_GlyphSlot glyph)
        {
            glyph_info glyph_info;

            glyph_info.character = character;

            glyph_info.pitch = glyph->bitmap.pitch;

            glyph_info.advance_x = glyph->advance.x;
            glyph_info.advance_y = glyph->advance.y;
            
            glyph_info.bitmap.width = glyph->bitmap.width;
            glyph_info.bitmap.height = glyph->bitmap.rows;
            glyph_info.bitmap.left = glyph->bitmap_left;
            glyph_info.bitmap.top = glyph->bitmap_top;

            // Copy the bitmap buffer (make sure it is correctly sized)
            glyph_info.bitmap.blob.resize(glyph_info.bitmap.width * glyph_info.bitmap.height);
            memcpy(glyph_info.bitmap.blob.data(), glyph->bitmap.buffer, glyph_info.bitmap.width * glyph_info.bitmap.height);

            return glyph_info;
        }

        std::vector<glyph_info> load_glyph_info(FT_FaceRec_* face, u32 characters_to_load)
        {
            std::vector<glyph_info> glyph_info;

            glyph_info.reserve(characters_to_load);

            for (u32 c = 0; c < characters_to_load; c++)
            {
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    log::error("Failed to load glyph: ", c);
                    continue;
                }

                glyph_info.push_back(copy_glyph_info(c, face->glyph));
            }

            return glyph_info;
        }

        std::optional<font_atlas_dimensions> try_packing_glyphs(const std::vector<glyph_info>& info, font_atlas_dimensions dimensions)
        {
            u32 pen_x = 0;
            u32 pen_y = 0;
            u32 row_height = 0;

            for (const auto& i : info)
            {
                // If the glyph does not fit in the current row, move to the next row
                if (pen_x + i.bitmap.width >= dimensions.width)
                {
                    pen_x = 0;
                    pen_y += row_height;
                    row_height = 0;
                }

                // If the glyph does not fit in the atlas, return false to retry with a larger atlas
                if (pen_y + i.bitmap.height >= dimensions.height)
                {
                    return std::nullopt;
                }

                // Advance the pen position and update row height
                pen_x += i.bitmap.width;
                row_height = std::max(row_height, i.bitmap.height);
            }

            return dimensions;
        }

        font_atlas_dimensions pack_glyphs(const std::vector<glyph_info>& info, font_atlas_dimensions dimensions)
        {
            // Try packing glyphs into the current atlas dimensions
            auto result = try_packing_glyphs(info, dimensions);

            // If packing failed, double the size and recurse
            if (!result)
            {
                dimensions.width *= 2;
                dimensions.height *= 2;

                return pack_glyphs(info, dimensions);
            }

            return *result;
        }

        font_character build_font_character(const glyph_info& info, u32 pen_x, u32 pen_y, u32 atlas_width, u32 atlas_height)
        {
            font_character c;

            c.size = glm::ivec2(info.bitmap.width, info.bitmap.height);
            c.bearing = glm::ivec2(info.bitmap.left, info.bitmap.top);
            c.advance = static_cast<u32>(info.advance_x);
            c.uv_start = glm::vec2(static_cast<f32>(pen_x) / atlas_width, static_cast<f32>(pen_y) / atlas_height);
            c.uv_end = glm::vec2((static_cast<f32>(pen_x) + static_cast<f32>(info.bitmap.width)) / atlas_width, (static_cast<f32>(pen_y) + static_cast<f32>(info.bitmap.height)) / atlas_height);

            return c;
        }

        font_atlas build_atlas(const std::vector<glyph_info>& info)
        {
            // Start with an initial atlas size (e.g., 128x128)
            font_atlas_dimensions atlas_dimensions = pack_glyphs(info, { 128, 128 });

            font_atlas atlas;

            atlas.texture_id = -1;
            atlas.texture_width = atlas_dimensions.width;
            atlas.texture_height = atlas_dimensions.height;
            atlas.characters = {};

            std::vector<u8> atlas_buffer(atlas.texture_width * atlas.texture_height, 0);

            u32 pen_x = 0;
            u32 pen_y = 0;
            u32 row_height = 0;

            for (const auto& i : info)
            {
                // one-pixel transparent border around each glyph
                constexpr u32 PADDING = 1;

                // Place glyph into atlas, accounting for padding
                if (pen_x + i.bitmap.width + 2 * PADDING >= atlas.texture_width)
                {
                    pen_x = 0;
                    // move down by row height + top & bottom padding
                    pen_y += row_height + 2 * PADDING;
                    row_height = 0;
                }

                // ensure glyph + padding fits in height
                assert(pen_y + i.bitmap.height + 2 * PADDING < atlas.texture_height && "Texture atlas is full.");

                // Copy glyph bitmap into atlas
                for (s32 row = 0; row < i.bitmap.height; ++row)
                {
                    // Copy into atlas at (pen_x+PADDING, pen_y+PADDING)
                    u8 * dest = &atlas_buffer[ (pen_x + PADDING) + (pen_y + PADDING + row) * atlas.texture_width ];
                    const u8 * src = &i.bitmap.blob[row * i.pitch];
                    memcpy(dest, src, i.bitmap.width);
                }

                // Store glyph UVs shifted by PADDING
                atlas.characters.emplace(i.character, build_font_character(i, pen_x + PADDING, pen_y + PADDING, atlas.texture_width, atlas.texture_height));

                // Advance pen by glyph width + left+right padding
                pen_x += i.bitmap.width + 2 * PADDING;
                row_height = std::max(row_height, i.bitmap.height + 2 * PADDING);
            }

            atlas.texture_id = render::create_image_item(
                atlas.texture_width,
                atlas.texture_height,
                1,
                atlas_buffer.data(),
                render::image_filter_type::NEAREST,
                render::image_wrap_type::CLAMP_TO_EDGE);

            return atlas;
        }

        font_atlas make_font_atlas(FT_FaceRec_* face, u32 characters_to_load)
        {
            auto glyph_info = load_glyph_info(face, characters_to_load);

            return build_atlas(glyph_info);
        }
}