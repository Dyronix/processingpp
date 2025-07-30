#include "imgui/sierra_inspector.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_icons_font_awesome.h"
#include "imgui/ecs/ecs_stats.h"
#include "imgui/inspector/inspector.h"

#include "layers/layer_stack.h"
#include "layers/layer.h"

#include "string/string_id.h"

#include <sstream>

namespace
{
    //-------------------------------------------------------------------------
    static void tooltip(const char* tooltip)
    {
        if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.6f)
        {
            ImGui::BeginTooltip();
            ImGui::SetTooltip("%s", tooltip);
            ImGui::EndTooltip();
        }
    }
}

namespace ppp
{
    namespace imgui
    {
        //-------------------------------------------------------------------------
        struct sierra_inspector_context
        {
            bool show_data;
        };

        sierra_inspector_context& ctx()
        {
            static sierra_inspector_context context;

            return context;
        }

        //-------------------------------------------------------------------------
        void draw_inspector(const ecs_world_stats_t& world_stats, const std::string& active_scene_name, layer_stack& stack)
        {
            // Data
            {
                if (ImGui::Button(ICON_FA_DATABASE))
                {
                    ctx().show_data = true;
                }
                tooltip("Data");
            }

            if (ctx().show_data)
            {
                ImGui::Begin("Data", &ctx().show_data);
                ImGui::TextUnformatted("Active Scene:");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", active_scene_name.c_str());

                if (ImGui::BeginTabBar("DataTabs", ImGuiTabBarFlags_None))
                {
                    // ECS Stats Tab
                    if (ImGui::BeginTabItem("ECS Stats"))
                    {
                        ImGui::SeparatorText("World Stats");
                        imgui::draw_stats(world_stats);
                        ImGui::EndTabItem();
                    }

                    // Layers Tab
                    if (ImGui::BeginTabItem("Layers"))
                    {
                        ImGui::SeparatorText("Layer Stack");
                        ImGui::BeginChild("##LayerList", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

                        // Iterate through layers and display checkboxes
                        for (auto it = stack.begin(); it != stack.end(); ++it)
                        {
                            auto& layer_ptr = *it;
                            bool enabled = layer_ptr->is_enabled();
                            bool always_enabled = layer_ptr->is_always_enabled();

                            // Assume string_id has a to_string() method
                            std::string_view name = string::restore_sid(layer_ptr->name());

                            if (always_enabled)
                            {
                                ImGui::BeginDisabled();
                            }

                            if (ImGui::Checkbox(name.data(), &enabled))
                            {
                                if (enabled)
                                {
                                    layer_ptr->enable();
                                    
                                    std::stringstream stream;
                                    stream << "Enable layer: ";
                                    stream << name.data();

                                    imgui::inspector::notify(imgui::inspector::notification_type::INFO, stream.str(), 2.0f);
                                }
                                else
                                {
                                    layer_ptr->disable();

                                    std::stringstream stream;
                                    stream << "Disable layer: ";
                                    stream << name.data();

                                    imgui::inspector::notify(imgui::inspector::notification_type::INFO, stream.str(), 2.0f);
                                }
                            }

                            if (always_enabled)
                            {
                                ImGui::EndDisabled();
                            }
                        }

                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();
                }

                ImGui::End();
            }
        }
    }
}