#include "imgui/inspector/inspector_view_modes.h"
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
            std::string_view to_string(view_direction direction)
            {
                static std::string perspective = "Perspective";
                static std::string front = "Front";
                static std::string back = "Back";
                static std::string left = "Left";
                static std::string right = "Right";
                static std::string top = "Top";
                static std::string bottom = "Bottom";

                switch (direction)
                {
                case view_direction::PERSPECTIVE: return perspective;
                case view_direction::FRONT: return front;
                case view_direction::BACK: return back;
                case view_direction::LEFT: return left;
                case view_direction::RIGHT: return right;
                case view_direction::TOP: return top;
                case view_direction::BOTTOM: return bottom;
                default: return perspective;
                }
            }

            //-------------------------------------------------------------------------
            ImVec2 inspector_view_modes::draw(anchor_type type, const ImVec2& content_size, const ImVec2& padding)
            {
                bool true_that = true;
                s32 window_flags =
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_AlwaysAutoResize;

                ImGui::Begin("TL_ViewModes_Menu", &true_that, window_flags);
                ImGui::SetWindowPos(anchor_position(type, content_size, padding));

                if (ImGui::Button(to_string(m_current_view_direction).data()))
                {
                    ImGui::OpenPopup("ppp_inspector_view_modes");
                }

                if (ImGui::BeginPopup("ppp_inspector_view_modes"))
                {
                    // Small helper for a radio row with right-aligned shortcut hint
                    auto radio_row = [](const char* id, const char* label, bool selected) -> bool
                        {
                            ImGui::PushID(id);
                            bool changed = false;

                            // Radio dot
                            if (ImGui::RadioButton("##rb", selected))
                                changed = !selected;

                            ImGui::SameLine();
                            ImGui::TextUnformatted(label);

                            ImGui::PopID();
                            return changed;
                        };

                    // Current
                    view_direction cur = m_current_view_direction;

                    // Section header
                    ImGui::BeginDisabled(true);
                    ImGui::TextUnformatted("Perspective");
                    ImGui::EndDisabled();
                    ImGui::Separator();

                    if (radio_row("perspective", to_string(view_direction::PERSPECTIVE).data(), cur == view_direction::PERSPECTIVE))
                    {
                        m_current_view_direction = view_direction::PERSPECTIVE;
                        on_view_mode_changed.emit(view_direction::PERSPECTIVE);
                        ImGui::CloseCurrentPopup();
                    }

                    // Section header
                    ImGui::BeginDisabled(true);
                    ImGui::TextUnformatted("Orthographic");
                    ImGui::EndDisabled();
                    ImGui::Separator();

                    if (radio_row("top", to_string(view_direction::TOP).data(), cur == view_direction::TOP))
                    {
                        m_current_view_direction = view_direction::TOP;
                        on_view_mode_changed.emit(view_direction::TOP);
                        ImGui::CloseCurrentPopup();
                    }
                    if (radio_row("bottom", to_string(view_direction::BOTTOM).data(), cur == view_direction::BOTTOM))
                    {
                        m_current_view_direction = view_direction::BOTTOM;
                        on_view_mode_changed.emit(view_direction::BOTTOM);
                        ImGui::CloseCurrentPopup();
                    }
                    if (radio_row("left", to_string(view_direction::LEFT).data(), cur == view_direction::LEFT))
                    {
                        m_current_view_direction = view_direction::LEFT;
                        on_view_mode_changed.emit(view_direction::LEFT);
                        ImGui::CloseCurrentPopup();
                    }
                    if (radio_row("right", to_string(view_direction::RIGHT).data(), cur == view_direction::RIGHT))
                    {
                        m_current_view_direction = view_direction::RIGHT;
                        on_view_mode_changed.emit(view_direction::RIGHT);
                        ImGui::CloseCurrentPopup();
                    }
                    if (radio_row("front", to_string(view_direction::FRONT).data(), cur == view_direction::FRONT))
                    {
                        m_current_view_direction = view_direction::FRONT;
                        on_view_mode_changed.emit(view_direction::FRONT);
                        ImGui::CloseCurrentPopup();
                    }
                    if (radio_row("back", to_string(view_direction::BACK).data(), cur == view_direction::BACK))
                    {
                        m_current_view_direction = view_direction::BACK;
                        on_view_mode_changed.emit(view_direction::BACK);
                        ImGui::CloseCurrentPopup();
                    }

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