#include "resources/texture_pool.h"
#include "render/render.h"
#include <unordered_map>

namespace ppp
{
    namespace texture_pool
    {
        namespace internal
        {
            std::unordered_map<std::string, Image> _images;

            unsigned char* _active_pixels = nullptr;
        }

        bool initialize()
        {
            return true;
        }

        void terminate()
        {
            if (internal::_active_pixels != nullptr)
            {
                free(internal::_active_pixels);
            }

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
                return pair.second.image_id == id;
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
                    return pair.second.image_id == id;
                });

                return &it->second;
            }

            return nullptr;
        }

        void add_new_image(const Image& image)
        {
            internal::_images.insert(std::make_pair(image.file_path, image));
        }

        u8* load_active_pixels(s32 id)
        {
            const Image* img = image_at_id(id);

            if (img != nullptr)
            {
                if (internal::_active_pixels != nullptr)
                {
                    free(internal::_active_pixels);
                }

                internal::_active_pixels = (u8*)malloc(img->width * img->height * img->channels);

                memcpy(internal::_active_pixels, img->data, img->width * img->height * img->channels);
            }

            return internal::_active_pixels;
        }

        u8* load_active_pixels(s32 x, s32 y, s32 width, s32 height, s32 channels)
        {
            if (internal::_active_pixels != nullptr)
            {
                free(internal::_active_pixels);
            }

            internal::_active_pixels = (u8*)malloc((width - x) * (height - y) * channels);

            return internal::_active_pixels;
        }

        void update_active_pixels(s32 id)
        {
            const Image* img = image_at_id(id);
            if (img != nullptr)
            {
                memcpy(img->data, internal::_active_pixels, img->width * img->height * img->channels);

                render::update_image_item(id, 0, 0, img->width, img->height, img->channels, img->data);
            }
        }

        unsigned char* active_pixels()
        {
            return internal::_active_pixels;
        }
    }
}