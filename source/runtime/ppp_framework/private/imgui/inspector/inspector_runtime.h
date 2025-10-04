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
            class inspector_runtime
            {
            public:
                bool is_paused() const { return m_game_paused; }

                ImVec2 draw(anchor_type type, const ImVec2& content_size, const ImVec2& padding = ImVec2(5, 5));

                inspector_signal<bool> on_game_paused;
                inspector_signal<bool> on_game_unpaused;
                inspector_signal<> on_next_frame;

            private:
                bool m_game_paused = false;
            };
        }
    }
}