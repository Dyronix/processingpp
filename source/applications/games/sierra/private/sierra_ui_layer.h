#pragma once

#include "sierra_layer.h"

#include "ecs/components/ecs_components.h"
#include "ecs/systems/ecs_systems.h"

#include <flecs.h>

#include <vector>
#include <memory>

namespace ppp
{
    class sierra_ui_layer : public sierra_layer
    {
    public:
        sierra_ui_layer(sierra_engine_context* ctx);

    protected:
        void on_attached() override;
        void on_enable() override;

    private:
        void create_button(const char* tag, const glm::vec2& position, const glm::vec4& color);
    };
}