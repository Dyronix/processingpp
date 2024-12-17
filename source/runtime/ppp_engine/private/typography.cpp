#include "typography.h"
#include "render/render.h"
#include "resources/font_pool.h"
#include "resources/geometry_pool.h"
#include "fileio/fileio.h"
#include "geometry/geometry.h"
#include "geometry/geometry_helpers.h"
#include "geometry/2d/rectangle.h"
#include "geometry/2d/geometry_2d_helpers.h"

#include "util/log.h"
#include "util/types.h"
#include "util/transform_stack.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <sstream>

namespace ppp
{
    namespace typography
    {
        namespace internal
        {
            auto _text_mode = text_mode_type::CENTER;

            class font_character_item : public render::irender_item
            {
            public:
                font_character_item(const geometry::geometry* geom, const std::vector<render::texture_id>& ids = {})
                    :m_geometry(geom)
                    ,m_texture_ids(ids)
                {}

                bool has_texture_id(render::texture_id id) const override
                {
                    return std::find_if(std::cbegin(texture_ids()), std::cend(texture_ids()),
                        [id](const render::texture_id other)
                    {
                        return id == other;
                    }) != std::cend(texture_ids());
                }
                bool has_smooth_normals() const override
                {
                    return m_geometry->has_smooth_normals();
                }

                u64 vertex_count() const override
                {
                    return m_geometry->vertex_count();
                }
                u64 index_count() const override
                {
                    return m_geometry->index_count();
                }
                u64 texture_count() const override
                {
                    return texture_ids().size();
                }

                const std::vector<glm::vec3>& vertex_positions() const override
                {
                    return m_geometry->vertex_positions();
                }
                const std::vector<glm::vec3>& vertex_normals() const override
                {
                    return m_geometry->vertex_normals();
                }
                const std::vector<glm::vec2>& vertex_uvs() const override
                {
                    return m_geometry->vertex_uvs();
                }

                const std::vector<render::face>& faces() const override
                {
                    return m_geometry->faces();
                }
                const std::vector<render::texture_id>& texture_ids() const override
                {
                    return m_texture_ids;
                }

                const u64 id() const override
                {
                    return m_geometry->id();
                }

            private:
                const geometry::geometry* m_geometry;
                const std::vector<render::texture_id> m_texture_ids;
            };

            font_character_item make_font_character(s32 character, render::texture_id texture_id, f32 uv_start_x, f32 uv_start_y, f32 uv_end_x, f32 uv_end_y)
            {
                std::stringstream stream;

                stream << "font_character_item|";
                stream << character << "|";
                stream << texture_id;

                const std::string gid = stream.str();

                if (!geometry_pool::has_geometry(gid))
                {
                    auto create_geom_fn = [uv_start_x, uv_start_y, uv_end_x, uv_end_y](geometry::geometry* geom)
                    {
                        compute_quad_faces(geom);

                        compute_quad_vertex_positions(geom);
                        compute_quad_vertex_normals(geom);

                        geom->vertex_uvs().assign(4, glm::vec2(0.0f, 0.0f));

                        s32 index = 0;

                        geom->vertex_uvs()[index++] = glm::vec2(uv_start_x, uv_end_y);
                        geom->vertex_uvs()[index++] = glm::vec2(uv_end_x, uv_end_y);
                        geom->vertex_uvs()[index++] = glm::vec2(uv_end_x, uv_start_y);
                        geom->vertex_uvs()[index++] = glm::vec2(uv_start_x, uv_start_y);
                    };

                    geometry::geometry* geom = geometry_pool::add_new_geometry(geometry::geometry(gid, false, create_geom_fn));

                    return font_character_item(geom, { texture_id });
                }

                geometry::geometry* geom = geometry_pool::get_geometry(gid);

                return font_character_item(geom, { texture_id });
            }
        }

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

                internal::font_character_item font_character_render_item = internal::make_font_character((s32)*it, active_font->atlas.texture_id, ch.uv_start.x, ch.uv_start.y, ch.uv_end.x, ch.uv_end.y);

                glm::vec2 center = geometry::rectanglular_center_translation(xpos, ypos, w, h);

                transform_stack::push();
                transform_stack::translate(glm::vec2(xpos, ypos));
                if (internal::_text_mode == text_mode_type::CORNER)
                {
                    transform_stack::translate(glm::vec2(center.x, center.y));
                }
                transform_stack::scale(glm::vec2(w, h));

                render::submit_font_item(&font_character_render_item);

                transform_stack::pop();

                // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
            }
        }

        void text_font(const font_id& font)
        {
            font_pool::load_active_font(font);
        }

        void text_mode(text_mode_type mode)
        {
            internal::_text_mode = mode;
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