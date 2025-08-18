#include "input/ui_input_processor.h"

#include "ecs/components/ecs_rect_transform_component.h"
#include "ecs/components/ecs_bounding_box_2d_component.h"
#include "ecs/components/ecs_sorting_layer_component.h"
#include "ecs/components/ecs_pickable_component.h"

#include "sierra_layer.h"

namespace ppp
{
    ui_input_processor::ui_input_processor(sierra_layer* layer)
    {
        _picking_query = layer->create_query<
            const ecs::rect_transform_component,
            const ecs::bounding_box_2d_component,
            const ecs::sorting_layer_component,
            const ecs::pickable_component>();
    }

    bool on_input(const input_event& evt)
    {

    }
}