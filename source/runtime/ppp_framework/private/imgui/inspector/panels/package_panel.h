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
            enum class configuration_type
            {
                DEBUG = 0,
                RELEASE,
                SHIPPING
            };

            struct package_info
            {
                bool include_symbols;
                bool build_config;
                configuration_type configuration;
            };

            class package_panel final : public ipanel
            {
            public:
                panel_info info() const override
                {
                    panel_info i = {};
                    i.id = "package";
                    i.title = "Packaging";
                    i.icon = ICON_FA_BOX_OPEN;
                    i.tooltip = "Packaging";
                    i.toolbar_only = false;
                    i.toolbar_order = 95;
                    return i;
                }

                void on_draw(panel_context& ctx) override
                {
                    (void)ctx;

                    ImGui::TextUnformatted("Packaging");
                    ImGui::Separator();

                    static bool include_symbols = false;
                    static bool compress_output = true;
                    static int  build_config = 1; // 0=Debug 1=Release 2=Shipping

                    if (build_config == 0)
                    {
                        ImGui::Checkbox("Include Symbols", &include_symbols);
                    }
                    ImGui::Checkbox("Compress Output", &compress_output);
                    ImGui::Combo("Configuration", &build_config, "Debug\0Release\0Shipping\0");

                    ImGui::Separator();

                    if (ImGui::Button(ICON_FA_CUBE "  Cook & Package"))
                    {
                        on_cook_and_package.emit({ include_symbols, compress_output, static_cast<configuration_type>(build_config) });
                    }
                }

                inspector_signal<package_info> on_cook_and_package;
            };
        }
    }
}