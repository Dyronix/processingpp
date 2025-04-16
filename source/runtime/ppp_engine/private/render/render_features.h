#pragma once

#include "render/render_types.h"

#include "util/types.h"

namespace ppp
{
    namespace render
    {
        bool has_debugging_capabilities();

        u32 max_vertices(topology_type type);
        u32 max_indices(topology_type type);
        u32 max_textures();
    }
}