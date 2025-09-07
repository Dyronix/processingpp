#include "imgui/inspector/inspector_panel.h"
#include "imgui/inspector/inspector_helpers.h"
#include "imgui/imgui.h"
#include "imgui/imgui_icons_font_awesome.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            class close_button final : public ipanel
            {
            public:
                panel_info info() const override
                {
                    panel_info i = {};
                    i.id = "close_inspector";
                    i.title = "Close Inspector";
                    i.icon = ICON_FA_ADJUST;
                    i.tooltip = "Close Inspector";
                    i.toolbar_only = true;
                    i.toolbar_order = 100;
                    return i;
                }

                void on_toolbar(panel_context& ctx) override
                {
                    (void)ctx;

                    if (ImGui::Button(ICON_FA_TIMES))
                    {
                        on_close_inspector.emit();
                    }
                    tooltip("Close Inspector");
                }

                inspector_signal<> on_close_inspector;
            };
        }
    }
}