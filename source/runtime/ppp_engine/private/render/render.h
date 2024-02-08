#pragma once

#include "util/hash.h"
#include "util/types.h"

#include <glm/glm.hpp>

#include <vector>

namespace ppp
{
    namespace render
    {
        using Index = u32;

        struct VertexPos
        {
            glm::vec3 position;
        };

        struct VertexPosTex
        {
            glm::vec3 position;
            glm::vec2 texcoord;
        };

        using VertexPosArr = std::vector<VertexPos>;
        using VertexPosTexArr = std::vector<VertexPosTex>;
        using Indices = std::vector<Index>;

        constexpr u32 DEPTH_BUFFER_BIT = 0x00000100;
        constexpr u32 STENCIL_BUFFER_BIT = 0x00000400;
        constexpr u32 COLOR_BUFFER_BIT = 0x00004000;

        enum class TopologyType
        {
            POINTS,
            LINES,
            TRIANGLES
        };

        struct RenderItem
        {
            VertexPos* vertices;
            size_t vertex_count;
            Index* indices;
            size_t index_count;
        };

        struct ImageItem
        {
            VertexPosTex* vertices;
            size_t vertex_count;
            Index* indices;
            size_t index_count;

            u32 image_id;
        };

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

        // Fill
        void push_fill_color(const glm::vec4& color);
        void push_fill_enable(bool enable);

        bool fill_enabled();
        glm::vec4 fill();

        // Stroke
        void push_stroke_width(f32 w);
        void push_stroke_scale(f32 s);
        void push_stroke_color(const glm::vec4& color);
        void push_stroke_enable(bool enable);

        bool stroke_enabled();
        f32 stroke_width();
        f32 stroke_scale();
        glm::vec4 stroke();

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

        void update_image_item(u32 id, f32 x, f32 y, f32 width, f32 height, s32 channels, u8* data);

        void submit_image_item(const ImageItem& item);
        void submit_stroke_image_item(const RenderItem& item);

        // Render Item
        void submit_render_item(TopologyType topology, const RenderItem& item);
        void submit_stroke_render_item(TopologyType topology, const RenderItem& item);

        // Clear 
        void clear_color(f32 r, f32 g, f32 b, f32 a);
        void clear(u32 flags);
    }
}