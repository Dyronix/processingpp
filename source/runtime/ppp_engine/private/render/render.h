#pragma once

#include "util/hash.h"
#include "util/types.h"

#include "render/render_item_components.h"
#include "render/render_types.h"

#include <glm/glm.hpp>

#include <vector>

namespace ppp
{
    namespace render
    {
        constexpr u32 DEPTH_BUFFER_BIT = 0x00000100;
        constexpr u32 STENCIL_BUFFER_BIT = 0x00000400;
        constexpr u32 COLOR_BUFFER_BIT = 0x00004000;
       
        struct ScissorRect
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

        // Camera
        void push_active_camera(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const glm::mat4& proj);

        // Rasterization
        void push_solid_rendering(bool enable);
        void push_wireframe_rendering(bool enable);
        void push_wireframe_linewidth(f32 line_width);
        void push_wireframe_color(const glm::vec4& color);

        // Fill
        void push_fill_color(const glm::vec4& color);
        void push_fill_enable(bool enable);

        bool fill_enabled();
        glm::vec4 fill();

        // Stroke
        void push_stroke_width(f32 w);
        void push_stroke_color(const glm::vec4& color);
        void push_stroke_enable(bool enable);
        void push_inner_stroke_width(f32 w);
        void push_inner_stroke_color(const glm::vec4& color);
        void push_inner_stroke_enable(bool enable);

        bool stroke_enabled();
        f32 stroke_width();
        glm::vec4 stroke();

        bool inner_stroke_enabled();
        f32 inner_stroke_width();
        glm::vec4 inner_stroke();

        // Tint
        void push_tint_color(const glm::vec4& color);
        void push_tint_enable(bool enable);

        bool tint_enabled();
        glm::vec4 tint();

        // Scissor
        void push_scissor(s32 x, s32 y, s32 width, s32 height);
        void push_scissor_enable(bool enable);

        bool scissor_enabled();
        ScissorRect scissor();

        // Image Item
        u32 create_image_item(f32 width, f32 height, s32 channels, u8* data);
        u32 create_image_item(f32 width, f32 height, s32 channels, u8* data, image_filter_type filter_type, image_wrap_type wrap_type);

        void update_image_item(u32 id, f32 x, f32 y, f32 width, f32 height, s32 channels, u8* data);

        void submit_image_item(const render_item& item);
        void submit_stroke_image_item(const render_item& item, bool outer);

        // Font Item
        void submit_font_item(const render_item& item);

        // Render Item
        void submit_render_item(topology_type topology, const render_item& item);
        void submit_stroke_render_item(topology_type topology, const render_item& item, bool outer);

        // Clear 
        void clear_color(f32 r, f32 g, f32 b, f32 a);
        void clear(u32 flags);
    }
}