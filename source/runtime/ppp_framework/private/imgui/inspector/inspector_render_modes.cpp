#include "imgui/inspector/inspector_render_modes.h"
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
            std::string_view to_string(render_mode direction)
            {
                static std::string unlit = "Unlit";
                static std::string lit = "Lit";
                static std::string wireframe = "Wireframe";

                switch (direction)
                {
                case render_mode::UNLIT: return unlit;
                case render_mode::LIT: return lit;
                case render_mode::WIREFRAME: return wireframe;
                default: return unlit;
                }
            }

            //-------------------------------------------------------------------------
            ImVec2 inspector_render_modes::draw(anchor_type type, const ImVec2& content_size, const ImVec2& padding)
            {
                bool true_that = true;
                s32 window_flags =
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_AlwaysAutoResize;

                ImGui::Begin("TL_RenderModes_Menu", &true_that, window_flags);
                ImGui::SetWindowPos(anchor_position(type, content_size, padding));
                
                if (ImGui::Button(to_string(m_current_render_mode).data()))
                {
                    ImGui::OpenPopup("ppp_inspector_render_modes");
                }

                if (ImGui::BeginPopup("ppp_inspector_render_modes"))
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
                    render_mode cur = m_current_render_mode;

                    // Section header
                    ImGui::BeginDisabled(true);
                    ImGui::TextUnformatted("Render Mode");
                    ImGui::EndDisabled();
                    ImGui::Separator();

                    if (radio_row("unlit", "Unlit", cur == render_mode::UNLIT))
                    {
                        m_current_render_mode = render_mode::UNLIT;
                        on_render_mode_changed.emit(render_mode::UNLIT);
                        ImGui::CloseCurrentPopup();
                    }
                    if (radio_row("lit", "Lit", cur == render_mode::LIT))
                    {
                        m_current_render_mode = render_mode::LIT;
                        on_render_mode_changed.emit(render_mode::LIT);
                        ImGui::CloseCurrentPopup();
                    }
                    if (radio_row("wireframe", "Wireframe", cur == render_mode::WIREFRAME))
                    {
                        m_current_render_mode = render_mode::WIREFRAME;
                        on_render_mode_changed.emit(render_mode::WIREFRAME);
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