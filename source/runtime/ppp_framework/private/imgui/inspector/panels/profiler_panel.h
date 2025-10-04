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
            class profiler_panel final : public ipanel
            {
            public:
                panel_info info() const override
                {
                    panel_info i = {};
                    i.id = "profiler";
                    i.title = "Profiler";
                    i.icon = ICON_FA_CHART_BAR;
                    i.tooltip = "Profiler";
                    i.toolbar_only = false;
                    i.toolbar_order = 10;
                    return i;
                }

                void on_draw(panel_context& ctx) override
                {
                    (void)ctx;
                    bool dummy = true;
                    inspect_profiler(dummy);
                }
            };
        }
    }
}