#pragma once

#include "imgui/inspector/inspector_signal.h"
#include "imgui/inspector/inspector_helpers.h"

#include "util/types.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            enum class render_mode : std::uint8_t
            {
                UNLIT = 0,
                LIT,
                WIREFRAME
            };

            class inspector_render_modes
            {
            public:
                render_mode current_render_mode() const { return m_current_render_mode; }

                ImVec2 draw(anchor_type type, const ImVec2& content_size, const ImVec2& padding = ImVec2(5, 5));

                inspector_signal<render_mode> on_render_mode_changed;

            private:
                render_mode m_current_render_mode = render_mode::UNLIT;
            };
        }
    }
}