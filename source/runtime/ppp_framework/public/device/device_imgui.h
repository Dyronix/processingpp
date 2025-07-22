#pragma once

namespace ppp
{
    namespace imgui
    {
        enum class style_theme_type
        {
            DARK,
            LIGHT,
            CLASSIC
        };

        void init_imgui_context(style_theme_type theme);
    }
}