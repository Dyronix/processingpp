#include "resources/texture_manager.h"

#include "render/helpers/render_texture_registry.h"
#include "render/render_features.h"

namespace ppp
{
    namespace texture_manager
    {
        namespace internal
        {
            std::unordered_map<std::string, std::vector<render::texture_id>> _registered_images;
        }

        void add_image(const std::string& shader_tag, render::texture_id image)
        {
            auto it = internal::_registered_images.find(shader_tag);
            if (it == std::cend(internal::_registered_images))
            {
                internal::_registered_images[shader_tag] = { image };
            }
            else
            {
                auto image_it = std::find(std::cbegin(it->second), std::cend(it->second), image);
                if (image_it == std::cend(it->second))
                {
                    it->second.push_back(image);
                }
            }
        }

        const std::vector<render::texture_id>& images(const std::string& shader_tag)
        {
            auto it = internal::_registered_images.find(shader_tag);
            if (it != std::cend(internal::_registered_images))
            {
                return it->second;
            }

            static std::vector<render::texture_id> empty_ids;
            return empty_ids;
        }

        void reset_images(const std::string& shader_tag)
        {
            auto it = internal::_registered_images.find(shader_tag);
            if (it != std::cend(internal::_registered_images))
            {
                internal::_registered_images.at(shader_tag).clear();
            }
        }
    }
}