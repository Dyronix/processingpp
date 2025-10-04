#include "imgui/inspector/inspector_panel.h"
#include "imgui/inspector/inspector_panel_registry.h"
#include "imgui/imgui.h"
#include "imgui/imgui_icons_font_awesome.h"

#include "imgui/layers/layer_stats.h"

namespace ppp
{
    namespace imgui
    {
        class layer_stack_panel final : public inspector::ipanel
        {
        public:
            //-------------------------------------------------------------------------
            layer_stack_panel(const layer_stack* layer_stack)
                : m_layer_stack(layer_stack)
            {

            }

            //-------------------------------------------------------------------------
            inspector::panel_info info() const override
            {
                inspector::panel_info i = {};
                i.id = "layer_stack";
                i.title = "Layer Stack";
                i.icon = ICON_FA_LAYER_GROUP;
                i.tooltip = "Layer Stack";
                i.toolbar_only = false;
                i.toolbar_order = 1;
                return i;
            }

            //-------------------------------------------------------------------------
            void on_draw(inspector::panel_context& ctx) override
            {
                (void)ctx;

                draw_layer_stack_stats(*m_layer_stack);
            }

        private:
            const layer_stack* m_layer_stack = nullptr;
        };
    }
}