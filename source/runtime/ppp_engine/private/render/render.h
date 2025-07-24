#pragma once

#include "util/hash.h"
#include "util/types.h"

#include "render/render_item.h"
#include "render/render_types.h"
#include "render/render_scissor.h"

#include "string/string_id.h"

#include <glm/glm.hpp>

#include <vector>
#include <functional>

namespace ppp
{
    struct camera_context;

    namespace render
    {
        struct statistics
        {
            s32 batched_draw_calls = -1;
            s32 instanced_draw_calls = -1;

            s32 textures = 0;
        };

        constexpr u32 DEPTH_BUFFER_BIT = 0x00000100;
        constexpr u32 STENCIL_BUFFER_BIT = 0x00000400;
        constexpr u32 COLOR_BUFFER_BIT = 0x00004000;

        bool initialize(s32 w, s32 h, void* user_data);
        void terminate();

        statistics stats();

        void begin();
        void render(const camera_context* context);
        void end();
        
        // Drawing mode (BATCHED | INSTANCING)
        void draw_mode(render_draw_mode mode);

        render_draw_mode draw_mode();

        // Shadows
        void enable_shadows();
        void disable_shadows();

        bool shadows_enabled();

        // Shader
        void push_active_shader(string::string_id tag, shading_model_type shading_model, shading_blending_type shading_blending);

        string::string_id active_shader();

        // Rasterization
        void push_solid_rendering(bool enable);
        void push_wireframe_rendering(bool enable);
        void push_wireframe_linewidth(f32 line_width);
        void push_wireframe_color(const glm::vec4& color);

        // Texture
        u32 create_image_item(f32 width, f32 height, s32 channels, const u8* data);
        u32 create_image_item(f32 width, f32 height, s32 channels, const u8* data, image_filter_type filter_type, image_wrap_type wrap_type);

        void update_image_item(u32 id, f32 x, f32 y, f32 width, f32 height, s32 channels, u8* data);

        void read_pixels(s32 x, s32 y, s32 width, s32 height, u8* data);

        // Font Item
        void submit_font_item(const irender_item* item);
        void submit_render_item(topology_type topology, const irender_item* item);
        void submit_stroke_render_item(topology_type topology, const irender_item* item, bool outer);

        // Clear 
        void clear_color(f32 r, f32 g, f32 b, f32 a);
        void clear(u32 flags);
    }
}