#include "imgui/inspector/inspector_panel.h"
#include "imgui/inspector/inspector_panel_registry.h"
#include "imgui/inspector/inspector_profiler.h"
#include "imgui/imgui.h"
#include "imgui/imgui_icons_font_awesome.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            class about_panel final : public ipanel
            {
            public:
                panel_info info() const override
                {
                    panel_info i = {};
                    i.id = "about";
                    i.title = "About";
                    i.icon = ICON_FA_QUESTION_CIRCLE;
                    i.tooltip = "About";
                    i.toolbar_only = false;
                    i.toolbar_order = 90;
                    return i;
                }

                void on_draw(panel_context& ctx) override
                {
                    (void)ctx;

                    ImGui::TextWrapped("An attempt to create P5 processing in C++");
                }
            };
        }
    }
}