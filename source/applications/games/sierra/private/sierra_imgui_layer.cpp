#include "sierra_imgui_layer.h"

#include "imgui/sierra_inspector.h"

namespace ppp
{
    //-------------------------------------------------------------------------
    sierra_imgui_layer::sierra_imgui_layer(ecs::scene_manager* scene_manager, layer_stack* stack)
        :sierra_layer(scene_manager, "imgui"_sid, 1, true)
        ,_stack(stack)
    {}

    //-------------------------------------------------------------------------
    void sierra_imgui_layer::on_attached()
    {
        // Nothing to implement
    }
    //-------------------------------------------------------------------------
    void sierra_imgui_layer::on_detached()
    {
        // Nothing to implement
    }

    //-------------------------------------------------------------------------
    void sierra_imgui_layer::on_tick(f32 dt)
    {

    }

    //-------------------------------------------------------------------------
    void sierra_imgui_layer::on_inspector_draw()
    {
        ecs::scene_manager* _scene_manager = scene_manager();
        ecs::scene* _active_scene = _scene_manager->active_scene();
        const std::string& _active_scene_name = _scene_manager->active_scene_name();

        imgui::draw_inspector(_active_scene->world_stats(), _active_scene_name, *_stack);
    }
}