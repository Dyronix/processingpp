#include "resources/texture_pool.h"

#include <unordered_map>

namespace ppp
{
    namespace texture_pool
    {
        namespace internal
        {
            std::unordered_map<std::string, Image> _images;
        }

        void initialize()
        {
            // Nothing to implement
        }

        void terminate()
        {
            for (auto& i : internal::_images)
            {
                free(i.second.data);
            }
        }

        bool has_image(const std::string& file_path)
        {
            return internal::_images.find(file_path) != internal::_images.cend();
        }

        bool has_image(s32 id)
        {
            auto it = std::find_if(std::cbegin(internal::_images), std::cend(internal::_images),
                [id](const auto& pair)
            {
                return pair->id == id;
            });

            return it != internal::_images.cend();
        }

        const Image* image_at_path(const std::string& file_path)
        {
            if (has_image(file_path))
            {
                return &internal::_images.find(file_path)->second;
            }

            return nullptr;
        }

        const Image* image_at_id(s32 id)
        {
            if (has_image(id))
            {
                auto it = std::find_if(std::cbegin(internal::_images), std::cend(internal::_images),
                    [id](const auto& pair)
                {
                    return pair->id == id;
                });

                &it->second;
            }

            return nullptr;
        }

        void add_new_image(const Image& image)
        {
            internal::_images.insert(std::make_pair(image.file_path, image));
        }
    }
}