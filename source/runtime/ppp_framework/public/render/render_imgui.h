#pragma once

namespace ppp
{
    namespace imgui
    {
        void init_imgui_renderer();
        void shutdown_imgui_renderer();
        
        void begin_draw_imgui_renderer();
        void end_draw_imgui_renderer();
    }
}