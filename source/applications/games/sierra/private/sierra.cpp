#include "sierra.h"

#include "sierra_main_layer.h"
#include "sierra_level_layer.h"
#include "sierra_placement_layer.h"
#include "sierra_ui_layer.h"
#include "sierra_imgui_layer.h"

#include "input/world_input_processor.h"
#include "input/ui_input_processor.h"
#include "input/placement_input_processor.h"

namespace ppp
{
    struct idle_state : public istate
    {
        //-------------------------------------------------------------------------
        idle_state(sierra_engine_context* context)
            : istate()
        {
            processors.push_back(std::make_unique<ui_input_processor>(context));
            processors.push_back(std::make_unique<world_input_processor>(context));
            processors.push_back(std::make_unique<placement_input_processor>(context));
        }
    };

    struct placement_state : public istate
    {
        //-------------------------------------------------------------------------
        placement_state(sierra_engine_context* context)
            : istate()
        {
            processors.push_back(std::make_unique<ui_input_processor>(context));
            processors.push_back(std::make_unique<placement_input_processor>(context));
        }
    };

    //-------------------------------------------------------------------------
    void sierra::setup()
    {
        set_quit_application_keycode(key_code::KEY_ESCAPE);

        enable_batched_draw_mode();
        enable_wireframe_mode(true);
        enable_solid_mode(true);

        rect_mode(shape_mode_type::CENTER);

        wireframe_color(0.0f, 0.0f, 0.0f);
        wireframe_linewidth(1.0f);

        _ctx.scene_manager.create_scene("main");
        _ctx.scene_manager.set_active_scene("main");

        {
            _ctx.layer_stack.push(std::make_unique<sierra_main_layer>(&_ctx));
            _ctx.layer_stack.push(std::make_unique<sierra_level_layer>(&_ctx));
            _ctx.layer_stack.push(std::make_unique<sierra_placement_layer>(&_ctx));
            _ctx.layer_stack.push(std::make_unique<sierra_ui_layer>(&_ctx));
            _ctx.layer_stack.push(std::make_unique<sierra_imgui_layer>(&_ctx));
        }

        _ctx.scene_manager.init();

        _ctx.input_hanlder.init();
        _ctx.input_hanlder.push_input_state("idle_state", std::make_unique<idle_state>(&_ctx));
        _ctx.input_hanlder.push_input_state("placement_state", std::make_unique<placement_state>(&_ctx));
        _ctx.input_hanlder.push_active_input_state("idle_state");

        _ctx.player_state.subscribe_start_placement([&]() { _ctx.input_hanlder.push_active_input_state("placement_state"); });
        _ctx.player_state.subscribe_stop_placement([&]() { _ctx.input_hanlder.push_active_input_state("idle_state"); });
    }

    //-------------------------------------------------------------------------
    void sierra::setup_inspector()
    {
        for (const std::unique_ptr<layer>& layer : _ctx.layer_stack)
        {
            if (layer->is_enabled())
            {
                layer->inspector_setup();
            }
        }
    }

    //-------------------------------------------------------------------------
    void sierra::begin_frame()
    {
        _ctx.scene_manager.begin_frame();

        for (const std::unique_ptr<layer>& layer : _ctx.layer_stack)
        {
            if (layer->is_enabled())
            {
                layer->begin_frame();
            }
        }        
    }

    //-------------------------------------------------------------------------
    void sierra::tick(float dt)
    {
        for (const std::unique_ptr<layer>& layer : _ctx.layer_stack)
        {
            if (layer->is_enabled())
            {
                layer->tick(dt);
            }
        }

        _ctx.scene_manager.tick(dt);
    }

    //-------------------------------------------------------------------------
    void sierra::draw()
    {
        background(200);
        
        for (const std::unique_ptr<layer>& layer : _ctx.layer_stack)
        {
            if (layer->is_enabled())
            {
                layer->draw();
            }
        }

        _ctx.scene_manager.draw();
    }

    //-------------------------------------------------------------------------
    void sierra::inspector_draw()
    {
        for (const std::unique_ptr<layer>& layer : _ctx.layer_stack)
        {
            if (layer->is_enabled())
            {
                layer->inspector_draw();
            }
        }
    }

    //-------------------------------------------------------------------------
    void sierra::end_frame()
    {
        for (const std::unique_ptr<layer>& layer : _ctx.layer_stack)
        {
            if (layer->is_enabled())
            {
                layer->end_frame();
            }
        }

        _ctx.scene_manager.end_frame();
    }

    //-------------------------------------------------------------------------
    void sierra::shutdown()
    {
        _ctx.layer_stack.clear();
    }

    //-------------------------------------------------------------------------
    std::unique_ptr<sketch> make_sketch()
    {
        return std::make_unique<sierra>();
    }
}