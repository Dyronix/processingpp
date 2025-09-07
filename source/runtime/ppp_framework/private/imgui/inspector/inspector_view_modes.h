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
            enum class view_direction : std::uint8_t
            {
                PERSPECTIVE = 0,
                FRONT,
                BACK,
                LEFT,
                RIGHT,
                TOP,
                BOTTOM
            };

            class inspector_view_modes
            {
            public:
                view_direction current_view_direction() const { return m_current_view_direction; }

                ImVec2 draw(anchor_type type, const ImVec2& content_size, const ImVec2& padding = ImVec2(5, 5));

                inspector_signal<view_direction> on_view_mode_changed;

            private:
                view_direction m_current_view_direction = view_direction::PERSPECTIVE;
            };
        }
    }
}