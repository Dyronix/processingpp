#include "resources/texture_pool.h"
#include "render/render.h"
#include "memory/memory_types.h"
#include "memory/memory_placement_new.h"
#include <unordered_map>

namespace ppp
{
    namespace texture_pool
    {
        namespace internal
        {
            static global_hash_map<string::string_id, image>& images()
            {
                static auto s_images = memory::tagged_placement_new<global_hash_map<string::string_id, image>>();

                return *s_images;
            }

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

            for (auto& i : internal::images())
            {
                free(i.second.data);
            }
        }

        bool has_image(string::string_id file_path)
        {
            return internal::images().find(file_path) != internal::images().cend();
        }

        bool has_image(s32 id)
        {
            auto it = std::find_if(std::cbegin(internal::images()), std::cend(internal::images()),
                [id](const auto& pair)
            {
                return pair.second.image_id == id;
            });

            return it != internal::images().cend();
        }

        const image* image_at_path(string::string_id file_path)
        {
            if (has_image(file_path))
            {
                return &internal::images().find(file_path)->second;
            }

            return nullptr;
        }

        const image* image_at_id(s32 id)
        {
            if (has_image(id))
            {
                auto it = std::find_if(std::cbegin(internal::images()), std::cend(internal::images()),
                    [id](const auto& pair)
                {
                    return pair.second.image_id == id;
                });

                return &it->second;
            }

            return nullptr;
        }

        void add_new_image(const image& image)
        {
            internal::images().emplace(image.file_path, image);
        }

        u8* load_active_pixels(s32 id)
        {
            const image* img = image_at_id(id);

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
            const image* img = image_at_id(id);
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