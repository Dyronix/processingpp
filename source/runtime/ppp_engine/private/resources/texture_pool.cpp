#include "resources/texture_pool.h"
#include "render/render.h"
#include "memory/memory_types.h"
#include "memory/memory_placement_new.h"
#include <unordered_map>

namespace ppp
{
    namespace texture_pool
    {
        using images_hash_map = global_hash_map<string::string_id, image>;

        struct context
        {
            images_hash_map images_hash_map;
            u8*             active_pixels;
        } g_ctx;

        //-------------------------------------------------------------------------
        bool initialize()
        {
            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            if (g_ctx.active_pixels != nullptr)
            {
                free(g_ctx.active_pixels);
            }

            for (auto& i : g_ctx.images_hash_map)
            {
                free(i.second.data);
            }
        }

        //-------------------------------------------------------------------------
        bool has_image(string::string_id file_path)
        {
            return g_ctx.images_hash_map.find(file_path) != g_ctx.images_hash_map.cend();
        }

        //-------------------------------------------------------------------------
        bool has_image(s32 id)
        {
            auto it = std::find_if(std::cbegin(g_ctx.images_hash_map), std::cend(g_ctx.images_hash_map),
                [id](const auto& pair)
            {
                return pair.second.image_id == id;
            });

            return it != g_ctx.images_hash_map.cend();
        }

        //-------------------------------------------------------------------------
        const image* image_at_path(string::string_id file_path)
        {
            if (has_image(file_path))
            {
                return &g_ctx.images_hash_map.find(file_path)->second;
            }

            return nullptr;
        }

        //-------------------------------------------------------------------------
        const image* image_at_id(s32 id)
        {
            if (has_image(id))
            {
                auto it = std::find_if(std::cbegin(g_ctx.images_hash_map), std::cend(g_ctx.images_hash_map),
                    [id](const auto& pair)
                {
                    return pair.second.image_id == id;
                });

                return &it->second;
            }

            return nullptr;
        }

        //-------------------------------------------------------------------------
        void add_new_image(const image& image)
        {
            g_ctx.images_hash_map.emplace(image.file_path, image);
        }

        //-------------------------------------------------------------------------
        u8* load_active_pixels(s32 id)
        {
            const image* img = image_at_id(id);

            if (img != nullptr)
            {
                if (g_ctx.active_pixels != nullptr)
                {
                    free(g_ctx.active_pixels);
                }

                g_ctx.active_pixels = (u8*)malloc(img->width * img->height * img->channels);

                memcpy(g_ctx.active_pixels, img->data, img->width * img->height * img->channels);
            }

            return g_ctx.active_pixels;
        }

        //-------------------------------------------------------------------------
        u8* load_active_pixels(s32 x, s32 y, s32 width, s32 height, s32 channels)
        {
            if (g_ctx.active_pixels != nullptr)
            {
                free(g_ctx.active_pixels);
            }

            g_ctx.active_pixels = (u8*)malloc((width - x) * (height - y) * channels);

            return g_ctx.active_pixels;
        }

        //-------------------------------------------------------------------------
        void update_active_pixels(s32 id)
        {
            const image* img = image_at_id(id);
            if (img != nullptr)
            {
                memcpy(img->data, g_ctx.active_pixels, img->width * img->height * img->channels);

                render::update_image_item(id, 0, 0, img->width, img->height, img->channels, img->data);
            }
        }

        //-------------------------------------------------------------------------
        u8* active_pixels()
        {
            return g_ctx.active_pixels;
        }
    }
}