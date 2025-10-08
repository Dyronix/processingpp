#include "resources/texture_pool.h"
#include "render/render_features.h"
#include "render/render.h"

namespace ppp
{
    namespace texture_pool
    {
        //-------------------------------------------------------------------------
        class default_texture_slot_manager
        {
        public:
            explicit default_texture_slot_manager(u32 max_textures)
            {
                _next_reserved_slot = max_textures - 1;
                _reserved_count = 0;
            }

        public:
            s32 next_reserved_slot()
            {
                ++_reserved_count;

                return _next_reserved_slot--;
            }
            s32 reserved_count() const
            {
                return _reserved_count;
            }

        private:
            s32 _next_reserved_slot{ 0 };
            s32 _reserved_count{ 0 };
        };

        struct default_texture
        {
            s32     reserved_slot{0};
            image   img{};
        };

        //-------------------------------------------------------------------------
        default_texture_slot_manager& reserved_slots_manager()
        {
            static default_texture_slot_manager slot_manager(render::max_textures());

            return slot_manager;
        }

        //-------------------------------------------------------------------------
        default_texture make_default_texture(u32 data)
        {
            constexpr u32 width = 1;
            constexpr u32 height = 1;
            constexpr u32 channels = 4;

            default_texture tex;

            tex.reserved_slot = reserved_slots_manager().next_reserved_slot();
            tex.img.image_id = render::create_image_item(width, height, channels, reinterpret_cast<const u8*>(&data), render::image_filter_type::NEAREST, render::image_wrap_type::REPEAT);
            tex.img.width = width;
            tex.img.height = height;
            tex.img.channels = channels;
            tex.img.data = static_cast<u8*>(malloc(width * height * channels));
            memset(tex.img.data, 0, width * height * channels);
            memcpy(tex.img.data, &data, width * height * channels);

            return tex;
        }

        struct context
        {
            images_hash_map images_hash_map;
            u8*             active_pixels;

            // default textures
            default_texture image_solid_white;
            default_texture image_solid_black;
        } g_ctx;

        //-------------------------------------------------------------------------
        images_hash_map& all_images()
        {
            return g_ctx.images_hash_map;
        }

        //-------------------------------------------------------------------------
        bool initialize()
        {
            constexpr u32 white_data = 0xFFFFFFFF;
            constexpr u32 black_data = 0xFF000000;

            g_ctx.image_solid_white = make_default_texture(white_data);
            g_ctx.image_solid_black = make_default_texture(black_data);

            add_new_image(g_ctx.image_solid_white.img);
            add_new_image(g_ctx.image_solid_black.img);

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
        s32 reserved_white_slot()
        {
            return g_ctx.image_solid_white.reserved_slot;
        }

        //-------------------------------------------------------------------------
        s32 reserved_black_slot()
        {
            return g_ctx.image_solid_black.reserved_slot;
        }

        //-------------------------------------------------------------------------
        s32 resvered_slots()
        {
            return reserved_slots_manager().reserved_count();
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
            assert(g_ctx.image_solid_white.img.image_id != -1);
            return &g_ctx.image_solid_white.img;
        }

        //-------------------------------------------------------------------------
        const image* image_solid_black()
        {
            assert(g_ctx.image_solid_black.img.image_id != -1);
            return &g_ctx.image_solid_black.img;
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