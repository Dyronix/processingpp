#include "ecs/systems/ecs_button_click_system.h"

#include "ecs/components/ecs_clickable_component.h"
#include "ecs/components/ecs_click_component.h"

#include "ecs/ecs_pipeline_tags.h"

#include "camera.h"
#include "events.h"

#include "render/render.h"

namespace ppp
{
    namespace ecs
    {
        //-------------------------------------------------------------------------
        flecs::entity register_button_click_system(flecs::world& world)
        {
            return world.system<const ecs::clickable_component, const ecs::click_component>()
                .kind<tick_pipeline>()
                .each([](flecs::entity& e, const ecs::clickable_component clickable, const ecs::click_component click)
                    {
                        clickable.on_click();

                        e.remove<ecs::click_component>();
                    });

            return {};
        }
    }
}