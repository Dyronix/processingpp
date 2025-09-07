#include "imgui/inspector/inspector_runtime.h"

#include "imgui/imgui_icons_font_awesome.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            //-------------------------------------------------------------------------
            ImVec2 inspector_runtime::draw(anchor_type type, const ImVec2& content_size, const ImVec2& padding)
            {
                bool true_that = true;
                s32 window_flags =
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_AlwaysAutoResize;

                ImGui::Begin("TC_Runtime", &true_that, window_flags);
                ImGui::SetWindowPos(anchor_position(type, content_size, padding));

                if (m_game_paused)
                {
                    if (ImGui::Button(ICON_FA_PLAY))
                    {
                        m_game_paused = false;
                        on_game_unpaused.emit(m_game_paused);
                    }
                    tooltip("Play");

                    ImGui::SameLine();
                    if (ImGui::Button(ICON_FA_FAST_FORWARD))
                    {
                        on_next_frame.emit();
                    }
                    tooltip("Next Frame");
                }
                else
                {
                    if (ImGui::Button(ICON_FA_PAUSE))
                    {
                        m_game_paused = true;
                        on_game_paused.emit(m_game_paused);
                    }
                    tooltip("Pause");
                }

                // capture size **before** End()
                ImVec2 size = ImGui::GetWindowSize();
                ImGui::End();
                return size;
            }
        }
    }
}