#include "resources/texture_reloader.h"
#include "resources/texture_pool.h"

#include "fileio/fileio.h"
#include "fileio/vfs.h"

#include "render/render.h"

#include "image.h"

#include <stb/stb_image.h>

namespace ppp
{
    namespace texture_reloader
    {
        s32 reload()
        {
            s32 count = 0;

            texture_pool::images_hash_map& all_images = texture_pool::all_images();
            for (auto& pair : all_images)
            {
                if (pair.first.is_none())
                {
                    continue;
                }

                auto file_path = string::restore_sid(pair.first);
                if (file_path.empty())
                {
                    log::error("Invalid file path for image found.");
                    continue;
                }

                auto& img = pair.second;

                auto buffer = fileio::read_binary_file(vfs::resolve_path(file_path));
                if (buffer.empty())
                {
                    log::error("Unable to read binary from disk.");
                    continue;
                }

                s32 width, height, channels;
                u8* data = stbi_load_from_memory(
                    reinterpret_cast<pixels_u8_ptr>(buffer.data()),
                    static_cast<s32>(buffer.size()),
                    &width,
                    &height,
                    &channels,
                    0);

                if (data == nullptr)
                {
                    log::error("Image {} could not be reloaded!", file_path);
                    continue;
                }

                free(img.data);

                img.data = data;
                img.width = width;
                img.height = height;
                img.channels = channels;

                render::update_image_item(img.image_id, 0, 0, img.width, img.height, img.channels, img.data);

                ++count;
            }

            return count;
        }
    }
}