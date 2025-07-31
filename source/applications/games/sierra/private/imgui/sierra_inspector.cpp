#include "imgui/sierra_inspector.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_icons_font_awesome.h"

#include "imgui/inspector/inspector.h"

#include "imgui/ecs/ecs_stats.h"
#include "imgui/layers/layer_floating_window.h"
#include "imgui/layers/layer_stats.h"

#include "layers/layer_stack.h"
#include "layers/layer.h"

#include "string/string_id.h"

#include <sstream>

namespace
{
    static void tooltip(const char* tip)
    {
        if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.6f)
        {
            ImGui::BeginTooltip();
            ImGui::SetTooltip("%s", tip);
            ImGui::EndTooltip();
        }
    }
}

namespace ppp
{
    namespace imgui
    {
        struct sierra_inspector_context
        {
            bool show_ecs_data = false;
            bool show_layer_data = true;
        };

        static sierra_inspector_context& ctx()
        {
            static sierra_inspector_context context;
            return context;
        }

        void draw_inspector(const sierra_engine_context* engine_context)
        {
            const ecs::scene_manager& scene_manager = engine_context->scene_manager;
            const layer_stack& layer_stack = engine_context->layer_stack;

            const ecs::scene* active_scene = scene_manager.active_scene();
            const std::string& active_scene_name = scene_manager.active_scene_name();

            // Data button in toolbar
            if (ImGui::Button(ICON_FA_DATABASE))
            {
                ctx().show_ecs_data = !ctx().show_ecs_data;
            }
            tooltip("Data");

            // ECS
            {
                if (ctx().show_ecs_data)
                {
                    ImGui::Begin("Data", &ctx().show_ecs_data, ImGuiWindowFlags_NoCollapse);

                    ImGui::TextUnformatted("Active Scene:"); ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", active_scene_name.c_str());

                    if (ImGui::BeginTabBar("DataTabs", ImGuiTabBarFlags_None))
                    {
                        if (ImGui::BeginTabItem("World Stats"))
                        {
                            imgui::draw_ecs_world_stats(active_scene->world_stats());
                            ImGui::EndTabItem();
                        }

                        ImGui::EndTabBar();
                    }

                    ImGui::End();
                }
            }

            // Layer stack
            {
                imgui::draw_layer_stack_floating_window(layer_stack, &imgui::draw_layer_stack_stats, &ctx().show_layer_data);
            }
        }
    }
}
