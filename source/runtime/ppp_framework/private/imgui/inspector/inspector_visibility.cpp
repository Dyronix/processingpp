#include "imgui/inspector/inspector_visibility.h"
#include "imgui/inspector/inspector_helpers.h"

#include "imgui/imgui_icons_font_awesome.h"

#include <string>

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            //-------------------------------------------------------------------------
            ImVec2 inspector_visibility::draw(anchor_type type, const ImVec2& content_size, const ImVec2& padding)
            {
                bool true_that = true;
                s32 window_flags =
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_AlwaysAutoResize;

                ImGui::Begin("TL_Visibility_Menu", &true_that, window_flags);
                ImGui::SetWindowPos(anchor_position(type, content_size, padding));

                if (ImGui::Button("Show"))
                {
                    ImGui::OpenPopup("ppp_inspector_visibility");
                }

                if (ImGui::BeginPopup("ppp_inspector_visibility"))
                {
                    auto checkbox_row = [](const char* id, const char* label, bool* value, auto&& on_change) 
                        {
                            ImGui::PushID(id);
                            bool changed = ImGui::MenuItem(label, nullptr, value);
                            if (changed)
                            {
                                on_change(*value);
                            }
                            ImGui::PopID();
                        };

                    // Section header
                    ImGui::BeginDisabled(true);
                    ImGui::TextUnformatted("Common Show Flags");
                    ImGui::EndDisabled();
                    ImGui::Separator();

                    checkbox_row("static", "Static Geometry", &m_show_static_geometry,
                        [&](bool v) {
                            on_show_static_geometry.emit(v);
                        });

                    checkbox_row("dynamic", "Dynamic Geometry", &m_show_dynamic_geometry,
                        [&](bool v) {
                            on_show_dynamic_geometry.emit(v);
                        });

                    checkbox_row("bbox", "Bounding Boxes", &m_show_bounding_boxes,
                        [&](bool v) {
                            on_show_bounding_boxes.emit(v);
                        });

                    checkbox_row("grid", "Grid", &m_show_grid,
                        [&](bool v) {
                            on_show_grid.emit(v);
                        });

                    ImGui::EndPopup();
                }

                // capture size **before** End()
                ImVec2 size = ImGui::GetWindowSize();
                ImGui::End();
                return size;
            }
        }
    }
}