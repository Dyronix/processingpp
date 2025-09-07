#pragma once

#include "util/types.h"

namespace ppp 
{
    namespace imgui 
    {
        namespace inspector 
        {
            void set_toolbar_visible(bool visible);
            void set_stats_visible(bool visible);
            void set_drawcalls_visible(bool visible);
            void set_loaded_textures_visible(bool visible);
            void set_cwd_visible(bool visible);

            void render_toolbar(f32 dt);

        } // namespace inspector
    } // namespace imgui
} // namespace ppp
