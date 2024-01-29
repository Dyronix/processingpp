#pragma once

#include <vector>

namespace ppp
{
    namespace render
    {
        using Index = int;

        using Vertices = std::vector<glm::vec3>;
        using Indices = std::vector<Index>;

        enum class TopologyType
        {
            POINTS,
            LINES,
            TRIANGLES
        };

        struct RenderItem
        {
            Vertices        vertices;
            Indices         indices;

            TopologyType    topology_type;
        };

        void initialize();
        void terminate();

        void submit_render_item(const RenderItem& item);
    }
}