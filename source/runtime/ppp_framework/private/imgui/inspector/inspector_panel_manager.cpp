#include "imgui/inspector/inspector_panel_manager.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <algorithm>

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            void panel_manager::draw(panel_context& ctx)
            {
                ensure_default_active();

                const auto& slots = reg().slots();
                const u64 n = reg().count();

                int item_idx[panel_registry::max_panels];  
                u64 item_n = 0;

                for (u64 i = 0; i < n; ++i)
                {
                    item_idx[item_n++] = static_cast<int>(i);
                }

                // Sort command buttons by toolbar_order (no heap allocations)
                std::sort(item_idx, item_idx + item_n, [&](int a, int b) {
                    return slots[a].info.toolbar_order < slots[b].info.toolbar_order;
                    });

                // --- Toolbar row ---
                ImGui::BeginGroup();

                for (u64 k = 0; k < item_n; ++k)
                {
                    const auto i = item_idx[k];
                    const bool is_active = (m_active_id && slots[i].info.id.data() && std::strcmp(m_active_id, slots[i].info.id.data()) == 0);

                    if (is_active)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
                    }

                    const char* label = slots[i].info.icon.data()
                        ? slots[i].info.icon.data()
                        : (slots[i].info.title.data() ? slots[i].info.title.data() : "Panel");

                    if (slots[item_idx[k]].info.toolbar_only)
                    {
                        if (auto* p = slots[item_idx[k]].instance.get())
                        {
                            p->on_toolbar(ctx);
                        }
                    }
                    else
                    {
                        if (ImGui::Button(label))
                        {
                            m_active_id = slots[i].info.id.data();
                        }
                    }

                    if (is_active)
                    {
                        ImGui::PopStyleColor();
                    }

                    if (k + 1 < item_n)
                    {
                        ImGui::SameLine();
                    }
                }

                ImGui::EndGroup();
                ImGui::Separator();

                // --- Body (only if a real panel is active) ---
                const int idx = find_index_by_id(m_active_id);
                if (idx >= 0)
                {
                    if (auto* p = slots[idx].instance.get())
                    {
                        p->on_draw(ctx);
                    }
                }
            }

            //-------------------------------------------------------------------------
            int panel_manager::find_index_by_id(const char* id) const
            {
                if (!id) return -1;

                const auto& slots = reg().slots();
                const u64 n = reg().count();

                for (u64 i = 0; i < n; ++i)
                {
                    if (slots[i].info.id.data() && std::strcmp(slots[i].info.id.data(), id) == 0)
                    {
                        return static_cast<int>(i);
                    }
                }

                return -1;
            }

            // ------------------------------------------------------------
            void panel_manager::ensure_default_active()
            {
                if (m_active_id) return;

                const auto& slots = reg().slots();
                const u64 n = reg().count();

                for (u64 i = 0; i < n; ++i)
                {
                    if (!slots[i].info.toolbar_only && slots[i].info.id.data())
                    {
                        m_active_id = slots[i].info.id.data(); return;
                    }
                }

                if (n > 0 && slots[0].info.id.data())
                {
                    m_active_id = slots[0].info.id.data();
                }
            }

            // ------------------------------------------------------------
            void panel_manager::show_panel(const char* id)
            {
                const int idx = find_index_by_id(id);

                if (idx >= 0)
                {
                    m_active_id = reg().slots()[idx].info.id.data();
                }
            }

            // ------------------------------------------------------------
            void panel_manager::next_panel()
            {
                ensure_default_active();

                const auto& slots = reg().slots();
                const u64 n = reg().count();

                if (n == 0) return;

                int cur = find_index_by_id(m_active_id);
                cur = (cur < 0) ? 0 : (cur + 1) % static_cast<int>(n);
                m_active_id = slots[cur].info.id.data();
            }
            // ------------------------------------------------------------
            void panel_manager::prev_panel()
            {
                ensure_default_active();

                const auto& slots = reg().slots();
                const u64 n = reg().count();

                if (n == 0) return;

                int cur = find_index_by_id(m_active_id);
                cur = (cur < 0) ? 0 : (cur - 1 + static_cast<int>(n)) % static_cast<int>(n);
                m_active_id = slots[cur].info.id.data();
            }

            //-------------------------------------------------------------------------
            panel_registry& panel_manager::reg()
            {
                return panel_registry::get();
            }

            //-------------------------------------------------------------------------
            const panel_registry& panel_manager::reg() const
            {
                return panel_registry::get();
            }
        }
    }
}