#include "imgui/inspector/inspector_overlays.h"

#include "imgui/imgui_icons_font_awesome.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            //-------------------------------------------------------------------------
            ImVec2 inspector_overlays::draw(anchor_type type, const ImVec2& content_size, const ImVec2& padding)
            {
                bool true_that = true;
                s32 window_flags =
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_NoScrollWithMouse |
                    ImGuiWindowFlags_AlwaysAutoResize;

                ImGui::Begin("TL_Overlay_Menu", &true_that, window_flags);
                ImGui::SetWindowPos(anchor_position(type, content_size, padding));

                if (ImGui::Button(ICON_FA_BARS))
                {
                    ImGui::OpenPopup("ppp_inspector_hamburger");
                }
                tooltip("Menu");

                if (ImGui::BeginPopup("ppp_inspector_hamburger"))
                {
                    bool stats = m_show_stats;
                    if (ImGui::MenuItem("Show Stats", nullptr, &stats))
                    {
                        m_show_stats = stats;
                        on_show_stats.emit(m_show_stats);
                    }
                    bool draw_calls = m_show_draw_calls;
                    if (ImGui::MenuItem("Show Draw Calls", nullptr, &draw_calls))
                    {
                        m_show_draw_calls = draw_calls;
                        on_show_drawcalls.emit(m_show_draw_calls);
                    }
                    bool loaded_textures = m_show_loaded_textures;
                    if (ImGui::MenuItem("Show Loaded Textures", nullptr, &loaded_textures))
                    {
                        m_show_loaded_textures = loaded_textures;
                        on_show_loaded_textures.emit(m_show_loaded_textures);
                    }
                    bool cwd = m_show_cwd;
                    if (ImGui::MenuItem("Show CWD", nullptr, &cwd))
                    {
                        m_show_cwd = cwd;
                        on_show_cwd.emit(m_show_cwd);
                    }
                    bool toolbar = m_show_toolbar;
                    if (ImGui::MenuItem("Show Toolbar", nullptr, &toolbar))
                    {
                        m_show_toolbar = toolbar;
                        on_show_toolbar.emit(m_show_toolbar);
                    }
                    ImGui::Separator();
                    float fov = m_fov;
                    if (ImGui::SliderFloat("Field of View", &fov, 20.0f, 120.0f, "%.0f deg"))
                    {
                        m_fov = fov;
                        on_change_camera_fov.emit(fov);
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