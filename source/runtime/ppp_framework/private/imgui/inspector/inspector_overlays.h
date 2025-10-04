#pragma once

#include "imgui/inspector/inspector_signal.h"
#include "imgui/inspector/inspector_helpers.h"

#include "util/types.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            class inspector_overlays
            {
            public:
                bool show_stats() const { return m_show_stats; }
                bool show_draw_calls() const { return m_show_draw_calls; }
                bool show_loaded_textures() const { return m_show_loaded_textures; }
                bool show_cwd() const { return m_show_cwd; }
                bool show_toolbar() const { return m_show_toolbar; }

                f32 camera_fov() const { return m_fov; }

                ImVec2 draw(anchor_type type, const ImVec2& content_size, const ImVec2& padding = ImVec2(5, 5));

                inspector_signal<bool> on_show_stats;
                inspector_signal<bool> on_show_drawcalls;
                inspector_signal<bool> on_show_loaded_textures;
                inspector_signal<bool> on_show_cwd;
                inspector_signal<bool> on_show_toolbar;
                inspector_signal<f32> on_change_camera_fov;

            private:
                bool m_show_stats = true;
                bool m_show_draw_calls = true;
                bool m_show_loaded_textures = false;
                bool m_show_cwd = false;
                bool m_show_toolbar = true;

                f32 m_fov = 90.0f;
            };
        } 
    } 
} 