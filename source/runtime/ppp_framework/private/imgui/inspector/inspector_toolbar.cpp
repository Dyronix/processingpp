#include "imgui/inspector/inspector_toolbar.h"

#include "imgui/imgui.h"

#include "device/device.h"
#include "render/render.h"
#include "environment.h"

#include <sstream>
#include <iomanip>

namespace ppp 
{
    namespace imgui 
    {
        namespace inspector 
        {
            struct toolbar_state
            {
                bool show_toolbar = true;
                bool show_stats = true;
                bool show_draw_calls = true;
                bool show_loaded_textures = false;
                bool show_cwd = false;
            } g_toolbar_state;

            //-------------------------------------------------------------------------
            void set_toolbar_visible(bool visible)
            {
                g_toolbar_state.show_toolbar = visible;
            }

            //-------------------------------------------------------------------------
            void set_stats_visible(bool visible)
            {
                g_toolbar_state.show_stats = visible;
            }

            //-------------------------------------------------------------------------
            void set_drawcalls_visible(bool visible)
            {
                g_toolbar_state.show_draw_calls = visible;
            }

            //-------------------------------------------------------------------------
            void set_loaded_textures_visible(bool visible)
            {
                g_toolbar_state.show_loaded_textures = visible;
            }

            //-------------------------------------------------------------------------
            void set_cwd_visible(bool visible)
            {
                g_toolbar_state.show_cwd = visible;
            }

            //-------------------------------------------------------------------------
            void render_toolbar(f32 /*dt*/)
            {
                if (!g_toolbar_state.show_toolbar)
                {
                    return;
                }

                bool keep_open = true;

                ImGuiWindowFlags flags =
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_NoScrollWithMouse;

                ImGui::Begin("Stats", &keep_open, flags);

                const auto& io = ImGui::GetIO();
                ImGui::SetWindowPos({ 5, io.DisplaySize.y - ImGui::GetWindowHeight() - 5.0f });
                ImGui::SetWindowSize({ io.DisplaySize.x - 10, -1 });

                // Gather stats once
                const auto stats = render::stats();

                // Build the line conditionally
                std::ostringstream ss;
                ss.setf(std::ios::fixed);
                bool first = true;
                
                auto append_sep = [&]()
                    {
                        if (!first) ss << " | ";
                        first = false;
                    };

                if (g_toolbar_state.show_stats)
                {
                    append_sep();
                    ss << std::setprecision(3) << (1000.0f / io.Framerate)
                        << " ms | FPS: " << std::setprecision(1) << io.Framerate;
                    first = false; // we already inserted a " | " inside stats block
                }

                if (g_toolbar_state.show_draw_calls)
                {
                    append_sep();
                    ss << "batched draw calls:" << stats.batched_draw_calls;
                    append_sep();
                    ss << "instanced draw calls:" << stats.instanced_draw_calls;
                }

                if (g_toolbar_state.show_loaded_textures)
                {
                    append_sep();
                    ss << "textures:" << stats.textures;
                }

                if (g_toolbar_state.show_cwd)
                {
                    append_sep();
                    ss << "cwd:" << cwd();
                }

                float sx, sy;
                device::window_scale(&sx, &sy);

                const std::string line = ss.str();
                float text_height = ImGui::CalcTextSize(line.c_str()).y;
                float window_height = ImGui::GetWindowSize().y;
                ImGui::SetCursorPosX(10.0f * sx);
                ImGui::SetCursorPosY((window_height - text_height) * 0.5f);
                ImGui::TextUnformatted(line.c_str());

                ImGui::End();
            }

        } // namespace inspector
    } // namespace imgui
} // namespace ppp
