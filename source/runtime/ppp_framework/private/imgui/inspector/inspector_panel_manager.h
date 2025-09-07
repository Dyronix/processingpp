#pragma once
/**
 * @file panel_manager.hpp
 * @brief Renders the Panels menu and draws all visible panels.
 *
 * Add this to your main editor frame:
 *     static panel_manager pm;
 *     pm.draw_ui(ctx);
 *
 * Notes:
 * - Uses a fixed array for sorting indices to avoid heap allocations.
 * - If you enable ImGui Docking, panels will dock automatically.
 */

#include "imgui/inspector/inspector_panel.h"
#include "imgui/inspector/inspector_panel_registry.h"

#include "util/types.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            class panel_manager
            {
            public:
                void draw(panel_context& ctx);

                // ---------- active-panel helpers ----------
                // Show this panel in the single viewport window. If id not found, no-op.
                void show_panel(const char* id);
                // Returns nullptr if nothing active yet (will be fixed on first draw).
                const char* active_id() const { return m_active_id; }
                // Optional: cycle through panels (useful for shortcuts)
                void next_panel();
                void prev_panel();

            private:
                panel_registry& reg();
                const panel_registry& reg() const;

                // Ensure we always have an active panel; pick first registered if none.
                void ensure_default_active();
                // Convenience to find slot index by id; returns -1 if not found.
                int find_index_by_id(const char* id) const;

                const char* m_active_id = nullptr; // points into panel_info.id lifetime managed by you
            };
        }
    }
}