#pragma once

#include "sierra_engine_context.h"

#include <flecs.h>

#include <string>

namespace ppp
{
    namespace imgui
    {
        void init_inspector(const sierra_engine_context* engine_context);
        void draw_inspector(const sierra_engine_context* engine_context);
    }
}