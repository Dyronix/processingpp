#include "image.h"
#include "material.h"

#include "render/render.h"

#include "fileio/fileio.h"
#include "fileio/vfs.h"

#include "resources/texture_pool.h"
#include "resources/geometry_pool.h"
#include "resources/material_pool.h"
#include "resources/shader_pool.h"

#include "geometry/geometry.h"
#include "geometry/geometry_helpers.h"
#include "geometry/2d/rectangle.h"
#include "geometry/2d/geometry_2d_helpers.h"

#include "util/log.h"
#include "util/transform_stack.h"
#include "util/brush.h"

#include <stb/stb_image_write.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include <unordered_map>
#include <algorithm>
#include <assert.h>
#include <sstream>

#include "render/render_shader_tags.h"

namespace ppp
{
    namespace internal
    {
        auto _image_mode = image_mode_type::CENTER;

        class image_item : public render::irender_item
        {
        public:
            image_item(const geometry::geometry* geom, const resources::imaterial* material)
                : m_geometry(geom)
                , m_material(material)
            {}

            ~image_item() override = default;

            bool has_smooth_normals() const override
            {
                return m_geometry->has_smooth_normals();
            }
            bool has_textures() const override
            {
                return m_material->has_textures();
            }
            bool cast_shadows() const override
            {
                return false;
            }

            u32 vertex_count() const override
            {
                return m_geometry->vertex_count();
            }
            u32 index_count() const override
            {
                return m_geometry->index_count();
            }

            const std::vector<glm::vec3>& vertex_positions() const override
            {
                return m_geometry->vertex_positions();
            }
            const std::vector<glm::vec3>& vertex_normals() const override
            {
                return m_geometry->vertex_normals();
            }
            const std::vector<glm::vec2>& vertex_uvs() const override
            {
                return m_geometry->vertex_uvs();
            }

            const std::vector<render::face>& faces() const override
            {
                return m_geometry->faces();
            }

            const u64 geometry_id() const override
            {
                return m_geometry->id();
            }
            const u64 material_id() const override
            {
                return m_material->shader_tag().value();
            }

            const resources::imaterial* material() const override
            {
                return m_material;
            }

        private:
            const geometry::geometry* m_geometry;
            const resources::imaterial* m_material;
        };

        //-------------------------------------------------------------------------
        geometry::geometry* extrude_image(const glm::mat4& world, const geometry::geometry* in_geom, f32 extrusion_width)
        {
            return geometry::extrude_rectangle(world, in_geom, extrusion_width);
        }

        //-------------------------------------------------------------------------
        geometry::geometry* make_image(u32 image_id)
        {
            std::stringstream stream;

            stream << "image|";
            stream << image_id;

            const std::string gid = stream.str();

            if (!geometry_pool::has_geometry(gid))
            {
                auto create_geom_fn = [](geometry::geometry* geom)
                {
                    geometry::compute_quad_faces(geom);

                    geometry::compute_quad_vertex_positions(geom);
                    geometry::compute_quad_vertex_uvs(geom);
                    geometry::compute_quad_vertex_normals(geom);
                };

                return geometry_pool::add_new_geometry(geometry::geometry(gid, false, create_geom_fn));
            }

            return geometry_pool::get_geometry(gid);
        }
    }

    //-------------------------------------------------------------------------
    void image_mode(image_mode_type mode)
    {
        internal::_image_mode = mode;
    }

    //-------------------------------------------------------------------------
    unsigned char* load_pixels(int x, int y, int width, int height)
    {
        u8* active_pixels = texture_pool::load_active_pixels(x, y, width, height, 4);

        render::read_pixels(x, y, width, height, texture_pool::active_pixels());

        return active_pixels;
    }

    //-------------------------------------------------------------------------
    unsigned char* load_pixels(image_id id)
    {
        return texture_pool::load_active_pixels(id);
    }

    //-------------------------------------------------------------------------
    void update_pixels(image_id id)
    {
        texture_pool::update_active_pixels(id);
    }

    //-------------------------------------------------------------------------
    void save_pixels(std::string_view output_name, unsigned char* data, int width, int height, int channels)
    {
        auto path = vfs::resolve_path(output_name);

        if (stbi_write_png(path.c_str(), width, height, channels, data, width * channels) == 0)
        {
            log::warn("failed to write image to path: {}", output_name);
        }
    }

    //-------------------------------------------------------------------------
    void save_pixels(std::string_view output_name, int width, int height)
    {
        auto path = vfs::resolve_path(output_name);

        if (stbi_write_png(path.c_str(), width, height, 4, texture_pool::active_pixels(), width * 4) == 0)
        {
            log::warn("failed to write image to path: {}", output_name);
        }
    }

    //-------------------------------------------------------------------------
    void save_pixels(std::string_view output_name, image_id id)
    {
        auto path = vfs::resolve_path(output_name);

        const texture_pool::image* img = texture_pool::image_at_id(id);

        if (stbi_write_png(path.c_str(), img->width, img->height, img->channels, img->data, img->width * img->channels) == 0)
        {
            log::warn("failed to write image to path: {}", output_name);
        }
    }

    //-------------------------------------------------------------------------
    pixels_u8_ptr pixels_as_u8()
    {
        return texture_pool::active_pixels();
    }

    //-------------------------------------------------------------------------
    pixels_s32_ptr pixels_as_u32()
    {
        return (pixels_s32_ptr)texture_pool::active_pixels();
    }

    //-------------------------------------------------------------------------
    image load(std::string_view file_path)
    {
        auto sid = string::store_sid(file_path);

        // Find image first
        if (texture_pool::has_image(sid))
        {
            const texture_pool::image* img = texture_pool::image_at_path(sid);
            return { (image_id)img->image_id, img->width, img->height, img->channels };
        }

        auto buffer = fileio::read_binary_file(file_path);

        texture_pool::image img;
        img.file_path = sid;
        img.data = stbi_load_from_memory(
            reinterpret_cast<pixels_u8_ptr>(buffer.data()),
            static_cast<int>(buffer.size()),
            &img.width,
            &img.height,
            &img.channels,
            0);

        if (img.data == nullptr)
        {
            log::error("Image {} could not be loaded!", file_path);
            return {};
        }

        img.image_id = render::create_image_item(img.width, img.height, img.channels, img.data);

        texture_pool::add_new_image(img);

        return { (image_id)img.image_id, img.width, img.height, img.channels };
    }

    //-------------------------------------------------------------------------
    image create(float width, float height, int channels, pixels_u8_ptr data)
    {
        texture_pool::image img;

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

    //-------------------------------------------------------------------------
    void draw(image_id image_id, float x, float y, float width, float height)
    {
        auto prev_shader = render::active_shader();
        auto curr_shader = render::unlit::tags::texture::batched();

        shader(string::restore_sid(curr_shader));

        reset_textures();
        texture(image_id);

        geometry::geometry* image_geom = internal::make_image(image_id);
        resources::imaterial* mat_unlit_tex = material_pool::get_or_create_material_instance(render::active_shader());

        internal::image_item image_render_item = internal::image_item(image_geom, mat_unlit_tex);

        transform_stack::push();
        transform_stack::translate(glm::vec2(x, y));

        if (internal::_image_mode == image_mode_type::CORNER)
        {
            glm::vec2 center = geometry::rectanglular_center_translation(x, y, width, height);

            transform_stack::translate(glm::vec2(center.x, center.y));
        }

        transform_stack::scale(glm::vec2(width, height));

        render::submit_render_item(render::topology_type::TRIANGLES, &image_render_item);

        glm::mat4 world = transform_stack::active_world();

        transform_stack::pop();

        curr_shader = render::unlit::tags::color::batched();

        shader(string::restore_sid(curr_shader));

        resources::imaterial* mat_unlit_col = material_pool::get_or_create_material_instance(render::active_shader());

        if (render::brush::stroke_enabled())
        {
            constexpr bool outer_stroke = true;

            geometry::geometry* stroke_geom = internal::extrude_image(world, image_geom, render::brush::stroke_width());
            internal::image_item stroke_item = internal::image_item(stroke_geom, mat_unlit_col);

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_item, outer_stroke);
        }

        if (render::brush::inner_stroke_enabled())
        {
            constexpr bool outer_stroke = false;

            geometry::geometry* stroke_geom = internal::extrude_image(world, image_geom, -render::brush::stroke_width());
            internal::image_item stroke_item = internal::image_item(stroke_geom, mat_unlit_col);

            render::submit_stroke_render_item(render::topology_type::TRIANGLES, &stroke_item, outer_stroke);
        }

        shader(string::restore_sid(prev_shader));
    }

    //-------------------------------------------------------------------------
    void no_tint()
    {
        render::brush::push_tint_enable(false);
    }

    //-------------------------------------------------------------------------
    void tint(int grayscale)
    {
        render::brush::push_tint_enable(true);
        render::brush::push_tint_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, 1.0f));
    }

    //-------------------------------------------------------------------------
    void tint(int grayscale, int alpha)
    {
        render::brush::push_tint_enable(true);
        render::brush::push_tint_color(glm::vec4(grayscale / 255.0f, grayscale / 255.0f, grayscale / 255.0f, alpha / 255.0f));
    }

    //-------------------------------------------------------------------------
    void tint(int r, int g, int b, int a)
    {
        render::brush::push_tint_enable(true);
        render::brush::push_tint_color(glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
    }

    //-------------------------------------------------------------------------
    void tint(const color& c)
    {
        render::brush::push_tint_enable(true);
        render::brush::push_tint_color(glm::vec4(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f, c.alpha / 255.0f));
    }

    //-------------------------------------------------------------------------
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

    //-------------------------------------------------------------------------
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
