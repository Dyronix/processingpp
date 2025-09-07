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
            class reload_content_panel final : public ipanel
            {
            public:
                panel_info info() const override
                {
                    panel_info i = {};
                    i.id = "reload_content";
                    i.title = "Reload Content";
                    i.icon = ICON_FA_IMAGES;
                    i.tooltip = "Reload Content";
                    i.toolbar_only = false;
                    i.toolbar_order = 15;
                    return i;
                }

                void on_draw(panel_context& ctx) override
                {
                    (void)ctx;

                    if (ImGui::Button(ICON_FA_IMAGES "  Reload Art"))
                    {
                        on_reload_images.emit();
                    }

                    if (ImGui::Button(ICON_FA_CODE "  Reload Shaders"))
                    {
                        on_reload_shaders.emit();
                    }
                }

                inspector_signal<> on_reload_images;
                inspector_signal<> on_reload_shaders;
            };
        }
    }
}