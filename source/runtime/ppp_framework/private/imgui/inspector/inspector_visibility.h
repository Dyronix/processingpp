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
            class inspector_visibility
            {
            public:
                bool show_static_geometry() const { return m_show_static_geometry; }
                bool show_dynamic_geometry() const { return m_show_dynamic_geometry; }
                bool show_bounding_boxes() const { return m_show_bounding_boxes; }
                bool show_grid() const { return m_show_grid; }

                ImVec2 draw(anchor_type type, const ImVec2& content_size, const ImVec2& padding = ImVec2(5, 5));

                inspector_signal<bool> on_show_static_geometry;
                inspector_signal<bool> on_show_dynamic_geometry;
                inspector_signal<bool> on_show_bounding_boxes;
                inspector_signal<bool> on_show_grid;

            private:
                bool m_show_static_geometry = true;
                bool m_show_dynamic_geometry = true;
                bool m_show_bounding_boxes = false;
                bool m_show_grid = false;
            };
        }
    }
}