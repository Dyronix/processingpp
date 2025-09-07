#pragma once

#include "util/types.h"

namespace ppp
{
    namespace imgui
    {
        void init_imgui_context(f32 ui_scale);
        void shutdown_imgui_context();

        void step_imgui_context();
    }
}