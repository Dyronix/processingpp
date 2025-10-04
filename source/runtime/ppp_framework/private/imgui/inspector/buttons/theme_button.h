#include "imgui/inspector/inspector_panel.h"
#include "imgui/inspector/inspector_theme.h"
#include "imgui/inspector/inspector_helpers.h"
#include "imgui/imgui.h"
#include "imgui/imgui_icons_font_awesome.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            class theme_button final : public ipanel
            {
            public:
                panel_info info() const override
                {
                    panel_info i = {};
                    i.id = "change_theme";
                    i.title = "Change Theme";
                    i.icon = ICON_FA_ADJUST;
                    i.tooltip = "Theme";
                    i.toolbar_only = true;
                    i.toolbar_order = 16;
                    return i;
                }

                void on_toolbar(panel_context& ctx) override
                {
                    (void)ctx;

                    if (ImGui::Button(ICON_FA_ADJUST))
                    {
                        s32 t = (s32)m_current_theme;
                        t = (t + 1) % g_total_themes;
                        m_current_theme = (theme)t;
                        on_theme_changed.emit(m_current_theme);
                    }
                    tooltip("Theme");
                }

                inspector_signal<theme> on_theme_changed;

            private:
                theme m_current_theme = theme::DARK;
            };
        }
    }
}