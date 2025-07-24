#include "resources/texture_pool.h"
#include "render/render.h"
#include <unordered_map>

namespace ppp
{
    namespace texture_pool
    {
        struct context
        {
            images_hash_map images_hash_map;
            u8*             active_pixels;

            // default textures
            image           image_solid_white;
            image           image_solid_black;
        } g_ctx;

        //-------------------------------------------------------------------------
        images_hash_map& all_images()
        {
            return g_ctx.images_hash_map;
        }

        //-------------------------------------------------------------------------
        bool initialize()
        {
            image& image_solid_white = g_ctx.image_solid_white;
            image& image_solid_black = g_ctx.image_solid_black;

            constexpr u32 width = 1;
            constexpr u32 height = 1;
            constexpr u32 channels = 4;

            constexpr u32 white_data = 0xFFFFFFFF;
            constexpr u32 black_data = 0xFF000000;

            image_solid_white.image_id = render::create_image_item(width, height, channels, reinterpret_cast<const u8*>(&white_data), render::image_filter_type::NEAREST, render::image_wrap_type::REPEAT);
            image_solid_white.width = width;
            image_solid_white.height = height;
            image_solid_white.channels = channels;
            image_solid_white.data = static_cast<u8*>(malloc(width * height * channels));
            memset(image_solid_white.data, 0, width * height * channels);
            memcpy(image_solid_white.data, &white_data, width * height * channels);
            
            image_solid_black.image_id = render::create_image_item(width, height, channels, reinterpret_cast<const u8*>(&black_data), render::image_filter_type::NEAREST, render::image_wrap_type::REPEAT);
            image_solid_black.width = width;
            image_solid_black.height = height;
            image_solid_black.channels = channels;
            image_solid_black.data = static_cast<u8*>(malloc(width * height * channels));
            memset(image_solid_black.data, 0, width * height * channels);
            memcpy(image_solid_black.data, &black_data, width * height * channels);

            add_new_image(image_solid_white);
            add_new_image(image_solid_black);

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            if (g_ctx.active_pixels != nullptr)
            {
                free(g_ctx.active_pixels);
            }

            for (const auto& i : g_ctx.images_hash_map)
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
        const image* image_solid_white()
        {
            assert(g_ctx.image_solid_white.image_id != -1);
            return &g_ctx.image_solid_white;
        }

        //-------------------------------------------------------------------------
        const image* image_solid_black()
        {
            assert(g_ctx.image_solid_black.image_id != -1);
            return &g_ctx.image_solid_black;
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