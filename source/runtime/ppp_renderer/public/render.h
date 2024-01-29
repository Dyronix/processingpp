#pragma once

#include "hash.h"
#include "types.h"

#include <glm/glm.hpp>

#include <vector>

namespace ppp
{
    namespace render
    {
        using Index = s32;

        using Vertices = std::vector<glm::vec3>;
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
            Vertices vertices;
            Indices indices;
        };

        bool initialize(s32 w, s32 h, void* user_data);
        void terminate();

        void begin();
        void render();
        void end();

        void submit_render_item(TopologyType topology, const RenderItem& item);

        void viewport(s32 x, s32 y, s32 width, s32 height);
        void clear_color(f32 r, f32 g, f32 b, f32 a);
        void clear(u32 flags);
    }
}