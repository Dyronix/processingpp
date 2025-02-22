#include "resources/font_pool.h"

#include "memory/memory_types.h"
#include "memory/memory_placement_new.h"

#include <unordered_map>

namespace ppp
{
    namespace font_pool
    {
        using fonts = global_hash_map<string::string_id, font>;

        struct context
        {
            fonts fonts = {};

            const font* active_font = nullptr;
        } g_ctx;

        bool initialize()
        {
            return true;
        }

        void terminate()
        {
            g_ctx.fonts.clear();

            g_ctx.active_font = nullptr;
        }

        bool has_font(string::string_id file_path)
        {
            return g_ctx.fonts.find(file_path) != g_ctx.fonts.cend();
        }

        bool has_font(u32 id)
        {
            auto it = std::find_if(std::cbegin(g_ctx.fonts), std::cend(g_ctx.fonts),
                [id](const auto& pair)
            {
                return pair.second.atlas.texture_id == id;
            });

            return it != g_ctx.fonts.cend();
        }

        const font* font_at_path(string::string_id file_path)
        {
            if (has_font(file_path))
            {
                return &g_ctx.fonts.find(file_path)->second;
            }

            return nullptr;
        }

        const font* font_at_id(u32 id)
        {
            if (has_font(id))
            {
                auto it = std::find_if(std::cbegin(g_ctx.fonts), std::cend(g_ctx.fonts),
                    [id](const auto& pair)
                {
                    return pair.second.atlas.texture_id == id;
                });

                return &it->second;
            }

            return nullptr;
        }

        void load_active_font(u32 id)
        {
            const font* font = font_at_id(id);

            if (font != nullptr)
            {
                g_ctx.active_font = font;
            }
        }

        void add_new_font(const font& font)
        {
            g_ctx.fonts.insert(std::make_pair(font.file_path, font));
        }

        const font* active_font()
        {
            return g_ctx.active_font;
        }
    }
}