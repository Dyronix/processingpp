#pragma once

/**
 * @file panel_registry.hpp
 * @brief Fixed-capacity registry for manually registered panels.
 *
 * Usage:
 *     static MyPanel my_panel;                // you own lifetime
 *     auto& p = panel_registry::get().register_panel(my_panel); // returns MyPanel&
 *     p.subscribe_pause(...);                 // wire callbacks now if you want
 *
 * The panel_manager uses this registry to draw menus/windows.
 */

#include "imgui/inspector/inspector_panel.h"

#include "util/types.h"

#include <array>
#include <memory>

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            struct panel_slot
            {
                std::unique_ptr<ipanel> instance = nullptr;
                panel_info              info = {};
            };

            class panel_registry
            {
            public:
                static constexpr u64    max_panels = 64;
                static panel_registry&  get();

            public:
                using panel_array       = std::array<panel_slot, max_panels>;

                template <class T, class... Args>
                T*                      register_panel(Args&&... args);

            public:
                panel_array&            slots();
                const panel_array&      slots() const;

                u64                     count() const;

            private:
                panel_array             m_slots = {};
                u64                     m_count = 0;
            };

            template <class T, class... Args>
            T* panel_registry::register_panel(Args&&... args)
            {
                static_assert(std::is_base_of_v<ipanel, T>, "T must derive from ipanel");
                static_assert(std::is_constructible_v<T, Args...>, "Panel of type T is not constructible from the given arguments");

                if (m_count < max_panels)
                {
                    panel_slot& s = m_slots[m_count++];
                    s.instance = std::make_unique<T>(std::forward<Args>(args)...);
                    
                    panel_info meta = s.instance->info();   // panel defines its own metadata
                    s.info = meta;

                    return static_cast<T*>(s.instance.get());
                }

                return nullptr;
            }
        }
    }
}
