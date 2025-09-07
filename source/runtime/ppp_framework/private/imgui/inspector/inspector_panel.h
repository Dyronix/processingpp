#pragma once

/**
 * @file panel.hpp
 * @brief Minimal interface for pluggable editor panels.
 *
 * How to use:
 * 1) Derive from ipanel and implement info() and draw().
 * 2) In your .cpp, add REGISTER_PANEL(YourPanelType).
 * 3) PanelManager will auto-discover and render your panel.
 *
 * Design goals:
 * - No dynamic allocations at runtime (static panel instances).
 * - Stable ids for persistence.
 * - Optional lifecycle and persistence hooks.
 */

#include "util/types.h"

#include "imgui/inspector/inspector_signal.h"

#include <string_view>
#include <functional>

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            struct panel_context
            {
                f32 delta_time = 0.0f;
                // Add services here (selection, scene, renderer, etc.)
            };

            struct panel_info
            {
                std::string_view id;                  ///< unique id, e.g. "profiler"
                std::string_view title;               ///< window title
                std::string_view icon;                ///< FontAwesome text, e.g. ICON_FA_CHART_BAR
                std::string_view tooltip;             ///< shown on toolbar hover
                bool toolbar_only = false;            ///< no separate window, only toolbar button
                int  toolbar_order = 100;             ///< toolbar/menu ordering
            };

            class ipanel
            {
            public:
                virtual ~ipanel() = default;

                /// @brief Static metadata for the panel (id/title/icon/order/visibility).
                virtual panel_info info() const = 0;

                /// @brief Called once the first time the panel becomes visible.
                virtual void on_open(panel_context&) { /* Nothing to implement */ }

                /// @brief Called when the panel is hidden (closed by the user).
                virtual void on_close(panel_context&) { /* Nothing to implement */ }

                /// @brief draw toolbar controls.
                virtual void on_toolbar(panel_context& ctx) { /* Nothing to implement */ };

                /// @brief Draw the panel contents.
                virtual void on_draw(panel_context& ctx) { /* Nothing to implement */ };

                /// @brief Optional: write panel-specific state to your config system.
                virtual void save_state() { /* Nothing to implement */ }

                /// @brief Optional: read panel-specific state from your config system.
                virtual void load_state() { /* Nothing to implement */ }
            };
        }
    }
}