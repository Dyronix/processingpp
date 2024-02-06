#include "image.h"
#include "render/render.h"
#include "fileio/fileio.h"
#include "resources/texture_pool.h"
#include "util/log.h"

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
            ImageMode _image_mode = ImageMode::CORNER;

            render::VertexPosTexArr _polygon_vertices;
            std::array<render::Index, 6> _polygon_indices = { 0, 1 ,2, 1, 2, 3 };

            render::VertexPosTexArr& make_quad_vertices(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
            {
                internal::_polygon_vertices.clear();
                internal::_polygon_vertices.resize(4);

                internal::_polygon_vertices[0].position = glm::vec3(x1, y1, 0);
                internal::_polygon_vertices[1].position = glm::vec3(x2, y2, 0);
                internal::_polygon_vertices[2].position = glm::vec3(x3, y3, 0);
                internal::_polygon_vertices[3].position = glm::vec3(x4, y4, 0);

                internal::_polygon_vertices[0].texcoord = glm::vec2(0.0f, 0.0f);
                internal::_polygon_vertices[1].texcoord = glm::vec2(1.0f, 0.0f);
                internal::_polygon_vertices[2].texcoord = glm::vec2(0.0f, 1.0f);
                internal::_polygon_vertices[3].texcoord = glm::vec2(1.0f, 1.0f);

                if (_image_mode == ImageMode::CENTER)
                {
                    // Find the minimum and maximum coordinates
                    glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3, x4 }), std::min({ y1, y2, y3, y4 }), 0.0f);
                    glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3, x4 }), std::max({ y1, y2, y3, y4 }), 0.0f);

                    float width = max_coord.x - min_coord.x;
                    float height = max_coord.y - min_coord.y;

                    // Center the shape
                    for (render::VertexPosTex& vertex : _polygon_vertices)
                    {
                        vertex.position.x = vertex.position.x - (width / 2.0f);
                        vertex.position.y = vertex.position.y - (height / 2.0f);
                    }
                }

                return internal::_polygon_vertices;
            }
        }

        void image_mode(ImageMode mode)
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

        unsigned char* pixels_as_u8()
        {
            return texture_pool::active_pixels();
        }

        unsigned int* pixels_as_u32()
        {
            return (unsigned int*)texture_pool::active_pixels();
        }

        image_id load(const std::string& file_path)
        {
            // Find image first
            if (texture_pool::has_image(file_path))
            {
                return texture_pool::image_at_path(file_path)->image_id;
            }

            auto buffer = fileio::read_binary_file(file_path);
            
            texture_pool::Image img;
            img.file_path = file_path;
            img.data = stbi_load_from_memory(
                reinterpret_cast<unsigned char*>(buffer.data()),
                static_cast<int>(buffer.size()),
                &img.width,
                &img.height,
                &img.channels,
                4);

            if (img.data == nullptr)
            {
                log::error("Image {} could not be loaded!", file_path);
                return -1;
            }

            img.image_id = render::create_image_item(img.width, img.height, img.channels, img.data);

            texture_pool::add_new_image(img);

            return img.image_id;
        }

        image_id create(float width, float height, int channels, unsigned char* data)
        {
            texture_pool::Image img;

            img.width = width;
            img.height = height;
            img.channels = channels;

            img.data = (u8*)malloc(width * height * channels);
            memset(img.data, 0, width * height * channels);

            if (data != nullptr)
            {
                assert(width * height * channels == std::strlen((const char*)data));

                memcpy(img.data, data, width * height * channels);
            }

            img.image_id = render::create_image_item(img.width, img.height, img.channels, data);

            texture_pool::add_new_image(img);

            return img.image_id;
        }

        void draw(image_id image_id, float x, float y, float width, float height)
        {
            auto& vertices = internal::make_quad_vertices(x, y, x + width, y, x, y + height, x + width, y + height);

            render::ImageItem item;

            item.vertices = vertices.data();
            item.vertex_count = vertices.size();
            item.indices = internal::_polygon_indices.data();
            item.index_count = internal::_polygon_indices.size();
            item.image_id = image_id;

            render::submit_image_item(item);
        }

        void no_tint() 
        {
            render::push_tint_enable(false);
        }

        void tint(int grayscale) 
        {
            render::push_tint_enable(true);
            render::push_tint_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, 1.0f));
        }

        void tint(int grayscale, int alpha)
        {
            render::push_tint_enable(true);
            render::push_tint_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, alpha / 255.0f));
        }

        void tint(int r, int g, int b, int a)
        {
            render::push_tint_enable(true);
            render::push_tint_color(glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
        }

        void tint(const color::Color& c)
        {
            render::push_tint_enable(true);
            render::push_tint_color(glm::vec4(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f, c.alpha / 255.0f));
        }
    }
}