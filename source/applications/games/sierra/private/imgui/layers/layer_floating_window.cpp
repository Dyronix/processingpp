#include "imgui/layers/layer_floating_window.h"

#include "imgui/imgui.h"

namespace ppp
{
    namespace imgui
    {
        void draw_layer_stack_floating_window(const layer_stack& stack, layer_stat_drawer drawer, bool* p_open)
        {
            ImGui::Begin("Layers", p_open,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::SetWindowPos({ 5,40 });
            ImGui::SetWindowSize({ 230,150 });

            drawer(stack);

            ImGui::End();
        }
    }
}