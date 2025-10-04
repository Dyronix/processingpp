#pragma once

#include "sierra_layer.h"

namespace ppp
{
    class layer_stack;

    class sierra_imgui_layer : public sierra_layer
    {
    public:
        sierra_imgui_layer(sierra_engine_context* ctx);

    protected:
        void on_attached() override;
        void on_detached() override;
        
        void on_inspector_setup() override;
        void on_inspector_draw() override;
    };
}