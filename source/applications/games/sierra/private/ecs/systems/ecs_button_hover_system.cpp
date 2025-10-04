#include "ecs/systems/ecs_button_hover_system.h"

#include "ecs/components/ecs_hoverable_component.h"
#include "ecs/components/ecs_hover_component.h"

#include "ecs/ecs_pipeline_tags.h"

#include "camera.h"
#include "events.h"

#include "render/render.h"

namespace ppp
{
    namespace ecs
    {
        //-------------------------------------------------------------------------
        flecs::entity register_button_hover_enter_system(flecs::world& world)
        {
            return world.system<const ecs::hoverable_component, const ecs::hover_enter_component>()
                .kind<tick_pipeline>()
                .each([&](flecs::entity& e, const ecs::hoverable_component hoverable, const ecs::hover_enter_component hover)
                    {
                        hoverable.on_hover_enter();
                        e.remove<ecs::hover_enter_component>();
                    });
        }

        //-------------------------------------------------------------------------
        flecs::entity register_button_hover_exit_system(flecs::world& world)
        {
            return world.system<const ecs::hoverable_component, const ecs::hover_exit_component>()
                .kind<tick_pipeline>()
                .each([&](flecs::entity& e, const ecs::hoverable_component hoverable, const ecs::hover_exit_component hover)
                    {
                        hoverable.on_hover_exit();
                        e.remove<ecs::hover_exit_component>();
                    });
        }
    }
}