#include "imgui/inspector/inspector_notifications.h"

#include "imgui/imgui.h"
#include "imgui/imgui_icons_font_awesome.h"

#include <algorithm>
#include <vector>

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            //---------------------------------------------------------------------------
            struct notification_entry
            {
                s32 id = -1;
                notification_type type = notification_type::INFO;
                std::string message;
                f32 time = 0.0f;
            };

            static std::vector<notification_entry>& storage()
            {
                static std::vector<notification_entry> s_notifications;
                return s_notifications;
            }

            //---------------------------------------------------------------------------
            s32 notify(notification_type type, const std::string& message, f32 time)
            {
                static s32 next_id = 0;
                storage().push_back({ next_id, type, message, time });
                return next_id++;
            }

            //---------------------------------------------------------------------------
            void render_notifications(f32 dt)
            {
                if (storage().empty())
                    return;

                bool dummy_open = true; // ImGui requires a bool* for Begin when using flags below.
                auto& io = ImGui::GetIO();

                f32 x = 5.0f;
                const f32 toolbar_height = 75.0f;
                f32 y = io.DisplaySize.y - toolbar_height;

                for (auto& n : storage())
                {
                    ImGui::Begin(std::to_string(n.id).c_str(), &dummy_open,
                        ImGuiWindowFlags_NoScrollbar |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoScrollWithMouse);

                    ImGui::SetWindowPos({ /*io.DisplaySize.x - ImGui::GetWindowWidth() +*/ x, y });

                    // Icon + color per type
                    ImVec4 color;
                    const char* icon = "";
                    switch (n.type)
                    {
                    case notification_type::INFO:
                        color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                        icon = ICON_FA_INFO_CIRCLE;
                        break;
                    case notification_type::SUCCESS:
                        color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
                        icon = ICON_FA_CHECK_CIRCLE;
                        break;
                    case notification_type::WARNING:
                        color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
                        icon = ICON_FA_EXCLAMATION_TRIANGLE;
                        break;
                    case notification_type::ERROR:
                        color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                        icon = ICON_FA_TIMES_CIRCLE;
                        break;
                    }

                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::Button(icon);
                    ImGui::PopStyleColor(1);
                    ImGui::SameLine();
                    ImGui::Text("%s", n.message.c_str());

                    // Stack upwards
                    y -= ImGui::GetWindowHeight() + 5.0f;

                    ImGui::End();

                    n.time -= dt;
                }

                // Remove expired
                auto& vec = storage();
                vec.erase(std::remove_if(vec.begin(), vec.end(),
                    [](const notification_entry& n) 
                    {
                        return n.time <= 0.0f; 
                    }), vec.end());
            }

            //---------------------------------------------------------------------------
            void clear_notifications()
            {
                storage().clear();
            }
        }
    }
}
