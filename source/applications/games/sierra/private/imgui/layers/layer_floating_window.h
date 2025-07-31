#pragma once

#include <functional>

namespace ppp
{
    class layer_stack;

    namespace imgui
    {
        using layer_stat_drawer = std::function<void(const layer_stack&)>;

        void draw_layer_stack_floating_window(const layer_stack& stack, layer_stat_drawer drawer, bool* p_open = nullptr);
    }
}