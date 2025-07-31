#include "imgui/layers/layer_stats.h"

#include "imgui/imgui.h"
#include "imgui/imgui_icons_font_awesome.h"

#include "imgui/inspector/inspector.h"

#include "string/string_id.h"

#include "layers/layer.h"
#include "layers/layer_stack.h"

#include "sierra_layer.h"

#include <string>
#include <sstream>

namespace ppp
{
    namespace imgui
    {
        struct layer_stack_stats_context
        {
            bool show_entities_window = false;
            sierra_layer* selected_layer = nullptr;
        } g_ctx;

        //-------------------------------------------------------------------------
        void draw_layer_stack_stats(const layer_stack& stack)
        {
            ImGui::SeparatorText("Layer Stack");
            ImGui::BeginChild("##floating_layer_list", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

            int index = 0;
            for (auto& layer_ptr : stack)
            {
                bool enabled = layer_ptr->is_enabled();
                bool always_enabled = layer_ptr->is_always_enabled();

                std::string_view name = string::restore_sid(layer_ptr->name());

                ImGui::PushID(int(index));

                if (always_enabled)
                {
                    ImGui::BeginDisabled();
                }

                if (ImGui::Checkbox(name.data(), &enabled))
                {
                    std::stringstream ss;
                    if (enabled)
                    {
                        layer_ptr->enable();
                        ss << "Enable layer: " << name;
                        imgui::inspector::notify(imgui::inspector::notification_type::INFO, ss.str(), 2.0f);
                    }
                    else
                    {
                        layer_ptr->disable();
                        ss << "Disable layer: " << name;
                        imgui::inspector::notify(imgui::inspector::notification_type::INFO, ss.str(), 2.0f);
                    }
                }

                if (always_enabled)
                {
                    ImGui::EndDisabled();
                }

                ImGui::PopID();
            }
            ImGui::EndChild();
        }
    }
}