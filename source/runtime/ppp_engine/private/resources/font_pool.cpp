#include "resources/font_pool.h"

#include <unordered_map>

namespace ppp
{
    namespace font_pool
    {
        namespace internal
        {
            std::unordered_map<std::string, Font> _fonts;

            const Font* _active_font = nullptr;
        }

        bool initialize()
        {
            return true;
        }

        void terminate()
        {
            
        }

        bool has_font(const std::string& file_path)
        {
            return internal::_fonts.find(file_path) != internal::_fonts.cend();
        }

        bool has_font(u32 id)
        {
            auto it = std::find_if(std::cbegin(internal::_fonts), std::cend(internal::_fonts),
                [id](const auto& pair)
            {
                return pair.second.atlas.texture_id == id;
            });

            return it != internal::_fonts.cend();
        }

        const Font* font_at_path(const std::string& file_path)
        {
            if (has_font(file_path))
            {
                return &internal::_fonts.find(file_path)->second;
            }

            return nullptr;
        }

        const Font* font_at_id(u32 id)
        {
            if (has_font(id))
            {
                auto it = std::find_if(std::cbegin(internal::_fonts), std::cend(internal::_fonts),
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
            const Font* font = font_at_id(id);

            if (font != nullptr)
            {
                internal::_active_font = font;
            }
        }

        void add_new_font(const Font& font)
        {
            internal::_fonts.insert(std::make_pair(font.file_path, font));
        }

        const Font* active_font()
        {
            return internal::_active_font;
        }
    }
}