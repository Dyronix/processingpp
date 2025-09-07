#pragma once

#include "imgui/imgui.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            enum class anchor_type
            {
                TOP_LEFT,
                TOP_RIGHT,
                TOP_CENTER,
                BOTTOM_LEFT,
                BOTTOM_RIGHT,
                BOTTOM_CENTER
            };

            void tooltip(const char* tooltip);

            ImVec2 anchor_position(anchor_type anchor, const ImVec2& content_size = ImVec2(0, 0), const ImVec2& padding = ImVec2(5, 5));

            ImVec2 top_left(const ImVec2& content_size, const ImVec2& padding = ImVec2(5, 5));
            ImVec2 top_right(const ImVec2& content_size, const ImVec2& padding = ImVec2(5, 5));
            ImVec2 top_center(const ImVec2& content_size, const ImVec2& padding = ImVec2(5, 5));
            ImVec2 bottom_left(const ImVec2& content_size, const ImVec2& padding = ImVec2(5, 5));
            ImVec2 bottom_right(const ImVec2& content_size, const ImVec2& padding = ImVec2(5, 5));
            ImVec2 bottom_center(const ImVec2& content_size, const ImVec2& padding = ImVec2(5, 5));
        }
    }
}