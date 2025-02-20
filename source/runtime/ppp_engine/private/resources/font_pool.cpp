#include "resources/font_pool.h"

#include "memory/memory_types.h"
#include "memory/memory_manager.h"

#include <unordered_map>

namespace ppp
{
    namespace font_pool
    {
        namespace internal
        {
            static global_hash_map<string::string_id, font>& fonts()
            {
                static auto s_fonts = memory::tagged_placement_new<global_hash_map<string::string_id, font>>();

                return *s_fonts;
            }

            const font* _active_font = nullptr;
        }

        bool initialize()
        {
            return true;
        }

        void terminate()
        {
            internal::fonts().clear();

            internal::_active_font = nullptr;
        }

        bool has_font(string::string_id file_path)
        {
            return internal::fonts().find(file_path) != internal::fonts().cend();
        }

        bool has_font(u32 id)
        {
            auto it = std::find_if(std::cbegin(internal::fonts()), std::cend(internal::fonts()),
                [id](const auto& pair)
            {
                return pair.second.atlas.texture_id == id;
            });

            return it != internal::fonts().cend();
        }

        const font* font_at_path(string::string_id file_path)
        {
            if (has_font(file_path))
            {
                return &internal::fonts().find(file_path)->second;
            }

            return nullptr;
        }

        const font* font_at_id(u32 id)
        {
            if (has_font(id))
            {
                auto it = std::find_if(std::cbegin(internal::fonts()), std::cend(internal::fonts()),
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
                internal::_active_font = font;
            }
        }

        void add_new_font(const font& font)
        {
            internal::fonts().insert(std::make_pair(font.file_path, font));
        }

        const font* active_font()
        {
            return internal::_active_font;
        }
    }
}