#pragma once

#include "util/hash.h"
#include "util/types.h"

#include "render/render_item.h"
#include "render/render_types.h"

#include <glm/glm.hpp>

#include <vector>
#include <functional>

namespace ppp
{
    namespace render
    {
        constexpr u32 DEPTH_BUFFER_BIT = 0x00000100;
        constexpr u32 STENCIL_BUFFER_BIT = 0x00000400;
        constexpr u32 COLOR_BUFFER_BIT = 0x00004000;

        enum class camera_mode
        {
            CAMERA_3D,
            CAMERA_2D,
            CAMERA_IMAGE,
            CAMERA_FONT
        };
       
        struct scissor_rect
        {
            s32 x;
            s32 y;
            s32 w;
            s32 h;
        };

        bool initialize(s32 w, s32 h, void* user_data);
        void terminate();

        void begin();
        void render();
        void end();
        
        // Drawing mode (BATCHED | INSTANCING)
        void draw_mode(render_draw_mode mode);

        render_draw_mode draw_mode();

        // Shader
        void push_active_shader(const std::string& tag, vertex_type type);

        const std::string& active_shader();

        // Geometry Builder
        void begin_geometry_builder(const std::string& tag);
        void end_geometry_builder();

        // Camera
        void push_camera(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const glm::mat4& proj);
        void push_active_camera_mode(camera_mode mode);

        // Rasterization
        void push_solid_rendering(bool enable);
        void push_wireframe_rendering(bool enable);
        void push_wireframe_linewidth(f32 line_width);
        void push_wireframe_color(const glm::vec4& color);

        // Scissor
        void push_scissor(s32 x, s32 y, s32 width, s32 height);
        void push_scissor_enable(bool enable);

        bool scissor_enabled();

        scissor_rect scissor();

        // Image Item
        u32 create_image_item(f32 width, f32 height, s32 channels, u8* data);
        u32 create_image_item(f32 width, f32 height, s32 channels, u8* data, image_filter_type filter_type, image_wrap_type wrap_type);

        void update_image_item(u32 id, f32 x, f32 y, f32 width, f32 height, s32 channels, u8* data);

        void submit_image_item(const irender_item* item);
        void submit_stroke_image_item(const irender_item* item, bool outer);

        // Font Item
        void submit_font_item(const irender_item* item);

        // 2D Render Item
        void submit_2d_render_item(topology_type topology, const irender_item* item);
        void submit_stroke_2d_render_item(topology_type topology, const irender_item* item, bool outer);

        // 3D Render Item
        void submit_3d_render_item(topology_type topology, const irender_item* item);

        // Clear 
        void clear_color(f32 r, f32 g, f32 b, f32 a);
        void clear(u32 flags);

        // Events
        void register_on_draw_begin(std::function<void()> draw_begin);
        void register_on_draw_end(std::function<void()> draw_end);
    }
}