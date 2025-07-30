#pragma once

#include "sierra_layer.h"

namespace ppp
{
    class sierra_main_layer : public sierra_layer
    {
    public:
        sierra_main_layer(ecs::scene_manager* scene);

    protected:
        void on_attached() override;
        void on_detached() override;

        void on_tick(f32 dt) override;
    };
}