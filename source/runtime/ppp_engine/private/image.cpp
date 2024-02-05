#include "image.h"
#include "render/render.h"
#include "fileio/fileio.h"
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
            struct Image
            {
                std::string     file_path = {};

                int             image_id = -1;

                int			    width = -1;
                int			    height = -1;
                int			    channels = -1;
            };

            std::unordered_map<std::string, Image> _images;

            render::VertexPosTexArr make_quad_vertices(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
            {
                render::VertexPosTexArr vertices;

                vertices.resize(4);

                vertices[0].position = glm::vec3(x1, y1, 0);
                vertices[1].position = glm::vec3(x2, y2, 0);
                vertices[2].position = glm::vec3(x3, y3, 0);
                vertices[3].position = glm::vec3(x4, y4, 0);

                vertices[0].texcoord = glm::vec2(0.0f, 0.0f);
                vertices[1].texcoord = glm::vec2(1.0f, 0.0f);
                vertices[2].texcoord = glm::vec2(0.0f, 1.0f);
                vertices[3].texcoord = glm::vec2(1.0f, 1.0f);

                // Find the minimum and maximum coordinates
                glm::vec3 min_coord = glm::vec3(std::min({ x1, x2, x3, x4 }), std::min({ y1, y2, y3, y4 }), 0.0f);
                glm::vec3 max_coord = glm::vec3(std::max({ x1, x2, x3, x4 }), std::max({ y1, y2, y3, y4 }), 0.0f);

                float width = max_coord.x - min_coord.x;
                float height = max_coord.y - min_coord.y;

                // Center the shape
                for (render::VertexPosTex& vertex : vertices)
                {
                    vertex.position.x = vertex.position.x - (width / 2.0f);
                    vertex.position.y = vertex.position.y - (height / 2.0f);
                }

                return vertices;
            }

            render::Indices make_quad_indices()
            {
                render::Indices indices;

                indices.resize(6);

                // Set triangles
                indices[0] = 0;
                indices[1] = 1;
                indices[2] = 2;
                indices[3] = 1;
                indices[4] = 2;
                indices[5] = 3;

                return indices;
            }
        }

        image_id load(const std::string& file_path)
        {
            // Find image first
            auto it = internal::_images.find(file_path);
            if (it != std::cend(internal::_images))
            {
                return it->second.image_id;
            }

            auto buffer = fileio::read_binary_file(file_path);
            
            internal::Image img;
            img.file_path = file_path;
            unsigned char* data = stbi_load_from_memory(
                reinterpret_cast<unsigned char*>(buffer.data()),
                static_cast<int>(buffer.size()),
                &img.width,
                &img.height,
                &img.channels,
                4);

            if (data == nullptr)
            {
                log::error("Image {} could not be loaded!", file_path);
                return -1;
            }

            img.image_id = render::create_image_item(img.width, img.height, img.channels, data);

            stbi_image_free(data);

            internal::_images.insert(std::make_pair(file_path, img));

            return img.image_id;
        }

        image_id create(float width, float height, int channels, unsigned char* data)
        {
            internal::Image img;
            img.width = width;
            img.height = height;
            img.channels = channels;

            assert(data == nullptr || width * height == std::strlen((const char*)data));

            img.image_id = render::create_image_item(img.width, img.height, img.channels, data);

            internal::_images.insert(std::make_pair(std::to_string(img.image_id), img));

            return img.image_id;
        }

        void update(image_id image_id, float x, float y, float width, float height, int channels, unsigned char* data)
        {
            render::update_image_item(image_id, x, y, width, height, channels, data);
        }

        void draw(image_id image_id, float x, float y, float width, float height)
        {
            render::ImageItem item;

            item.vertices = internal::make_quad_vertices(x, y, x + width, y, x, y + height, x + width, y + height);
            item.indices = internal::make_quad_indices();
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