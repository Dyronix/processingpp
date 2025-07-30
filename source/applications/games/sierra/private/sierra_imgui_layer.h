#pragma once

#include "sierra_layer.h"

namespace ppp
{
    class layer_stack;

    class sierra_imgui_layer : public sierra_layer
    {
    public:
        sierra_imgui_layer(ecs::scene_manager* scene, layer_stack* stack);

    protected:
        void on_attached() override;
        void on_detached() override;
        
        void on_tick(f32 dt) override;
        void on_inspector_draw() override;

    private:
        layer_stack* _stack;
    };
}