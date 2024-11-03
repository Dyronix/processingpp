#include "image.h"
#include "render/render.h"
#include "render/render_brush.h"
#include "fileio/fileio.h"
#include "resources/texture_pool.h"
#include "util/log.h"
#include "geometry/2d/geometry_2d.h"

#include <stb/stb_image.h>

#include <unordered_map>
#include <algorithm>
#include <assert.h>

namespace ppp
{
    namespace image
    {
        namespace internal
        {
            auto _image_mode = image_mode_type::CENTER;
        }

        void image_mode(image_mode_type mode)
        {
            internal::_image_mode = mode;
        }

        void load_pixels(image_id id)
        {
            texture_pool::load_active_pixels(id);
        }

        void update_pixels(image_id id)
        {
            texture_pool::update_active_pixels(id);
        }

        pixels_u8_ptr pixels_as_u8()
        {
            return texture_pool::active_pixels();
        }

        pixels_s32_ptr pixels_as_u32()
        {
            return (pixels_s32_ptr)texture_pool::active_pixels();
        }

        image load(const std::string& file_path)
        {
            // Find image first
            if (texture_pool::has_image(file_path))
            {
                const texture_pool::Image* img = texture_pool::image_at_path(file_path);
                return { (image_id)img->image_id, img->width, img->height, img->channels };
            }

            auto buffer = fileio::read_binary_file(file_path);
            
            texture_pool::Image img;
            img.file_path = file_path;
            img.data = stbi_load_from_memory(
                reinterpret_cast<pixels_u8_ptr>(buffer.data()),
                static_cast<int>(buffer.size()),
                &img.width,
                &img.height,
                &img.channels,
                4);

            if (img.data == nullptr)
            {
                log::error("Image {} could not be loaded!", file_path);
                return {};
            }

            img.image_id = render::create_image_item(img.width, img.height, img.channels, img.data);

            texture_pool::add_new_image(img);

            return { (image_id)img.image_id, img.width, img.height, img.channels };
        }

        image create(float width, float height, int channels, pixels_u8_ptr data)
        {
            texture_pool::Image img;

            img.width = width;
            img.height = height;
            img.channels = channels;

            img.data = (u8*)malloc(width * height * channels);
            memset(img.data, 0, width * height * channels);

            if (data != nullptr)
            {
                //assert(width * height * channels == std::strlen((const char*)data));

                memcpy(img.data, data, width * height * channels);
            }

            img.image_id = render::create_image_item(img.width, img.height, img.channels, data);

            texture_pool::add_new_image(img);

            return { (image_id)img.image_id, img.width, img.height, img.channels };
        }

        void draw(image_id image_id, float x, float y, float width, float height)
        {
            render::render_item item = geometry::image::make_image(internal::_image_mode == image_mode_type::CORNER, x, y, width, height, image_id);

            render::submit_image_item(item);

            if (render::brush::stroke_enabled())
            {
                auto vert_comp = item.get_component<render::vertex_component>();

                assert(vert_comp != nullptr);

                constexpr bool outer_stroke = true;

                auto vertex_positions = vert_comp->get_attribute_data<glm::vec3>(render::vertex_attribute_type::POSITION);

                render::render_item stroke_item = geometry::image::extrude_image(vertex_positions, vert_comp->vertex_count(), render::brush::stroke_width());

                render::submit_stroke_image_item(stroke_item, outer_stroke);
            }

            if (render::brush::inner_stroke_enabled())
            {
                auto vert_comp = item.get_component<render::vertex_component>();

                assert(vert_comp != nullptr);

                constexpr bool outer_stroke = false;

                auto vertex_positions = vert_comp->get_attribute_data<glm::vec3>(render::vertex_attribute_type::POSITION);

                render::render_item stroke_item = geometry::image::extrude_image(vertex_positions, vert_comp->vertex_count(), -render::brush::inner_stroke_width());

                render::submit_stroke_image_item(stroke_item, outer_stroke);
            }
        }

        void no_tint() 
        {
            render::brush::push_tint_enable(false);
        }

        void tint(int grayscale) 
        {
            render::brush::push_tint_enable(true);
            render::brush::push_tint_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, 1.0f));
        }

        void tint(int grayscale, int alpha)
        {
            render::brush::push_tint_enable(true);
            render::brush::push_tint_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, alpha / 255.0f));
        }

        void tint(int r, int g, int b, int a)
        {
            render::brush::push_tint_enable(true);
            render::brush::push_tint_color(glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
        }

        void tint(const color::Color& c)
        {
            render::brush::push_tint_enable(true);
            render::brush::push_tint_color(glm::vec4(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f, c.alpha / 255.0f));
        }

        std::vector<pixels_s32> rotate_clockwise(const pixels_s32_ptr arr, int width, int height, int num_rotations)
        {
            std::vector<pixels_s32> rotated_arr(arr, arr + (width * height));

            for (int r = 0; r < num_rotations; ++r)
            {
                std::vector<pixels_s32> temp_arr(rotated_arr);

                for (int i = 0; i < height; ++i)
                {
                    for (int j = 0; j < width; ++j)
                    {
                        // Calculate new index after rotation
                        int new_i = j;
                        int new_j = height - 1 - i;
                        int new_index = new_i * height + new_j;

                        // Copy value to the rotated array
                        rotated_arr[new_index] = temp_arr[i * width + j];
                    }
                }
            }

            return rotated_arr;
        }

        std::vector<pixels_s32> rotate_counterclockwise(const pixels_s32_ptr arr, int width, int height, int num_rotations)
        {
            std::vector<pixels_s32> rotated_arr(arr, arr + (width * height));

            for (int r = 0; r < num_rotations; ++r)
            {
                std::vector<pixels_s32> temp_arr(rotated_arr);

                for (int i = 0; i < height; ++i)
                {
                    for (int j = 0; j < width; ++j)
                    {
                        // Calculate new index after rotation
                        int new_i = width - 1 - j;
                        int new_j = i;
                        int new_index = new_i * height + new_j;

                        // Copy value to the rotated array
                        rotated_arr[new_index] = temp_arr[i * width + j];
                    }
                }
            }

            return rotated_arr;
        }
    }
}