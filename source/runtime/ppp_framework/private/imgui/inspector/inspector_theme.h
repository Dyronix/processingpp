#pragma once

#include "util/types.h"

#include "imgui/imgui.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            enum class theme
            {
                DARK,
                LIGHT,
                GRAY
            };

            static const s32 g_total_themes = 3;

            // Apply the currently selected theme
            void apply_theme(theme t);

            // Push menu style overrides for the current theme
            void push_menu_theme(theme t);

            // Pop menu style overrides for the current theme
            void pop_menu_theme(theme t);

            // Explicit theme application
            void embrace_the_darkness();  // Dark theme
            void follow_the_light();      // Light theme
            void go_gray();               // Gray theme
        }
    }
}