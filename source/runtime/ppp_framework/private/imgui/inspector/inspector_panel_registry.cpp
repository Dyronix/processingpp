#include "imgui/inspector/inspector_panel_registry.h"

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            //-------------------------------------------------------------------------
            panel_registry& panel_registry::get()
            {
                static panel_registry r;
                return r;
            }

            //-------------------------------------------------------------------------
            panel_registry::panel_array& panel_registry::slots()
            {
                return m_slots;
            }
            
            //-------------------------------------------------------------------------
            const panel_registry::panel_array& panel_registry::slots() const
            {
                return m_slots;
            }

            //-------------------------------------------------------------------------
            u64 panel_registry::count() const
            {
                return m_count;
            }
        }
    }
}