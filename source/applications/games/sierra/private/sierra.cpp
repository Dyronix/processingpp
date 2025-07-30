#include "framework.h"

#include "sierra_main_layer.h"
#include "sierra_imgui_layer.h"

#include "layers/layer_stack.h"

#include "ecs/ecs_scene_manager.h"

namespace ppp
{
    class sierra : public sketch
    {
    public:
        sierra() = default;
        ~sierra() override = default;

    public:
        void setup() override;
        void begin_frame() override;
        void tick(float dt) override;
        void draw() override;
        void inspector_draw() override;
        void end_frame() override;
        void shutdown() override;

    private:
        ecs::scene_manager _scene_manager;

        layer_stack _layer_stack;
    };

    //-------------------------------------------------------------------------
    void sierra::setup()
    {
        set_quit_application_keycode(key_code::KEY_ESCAPE);

        shader(material::tags::unlit::color());

        enable_batched_draw_mode();
        enable_wireframe_mode(true);
        enable_solid_mode(true);

        wireframe_color(0.0f, 0.0f, 0.0f);
        wireframe_linewidth(2.0f);

        _scene_manager.create_scene("main");
        _scene_manager.set_active_scene("main");

        _layer_stack.push(std::make_unique<sierra_main_layer>(&_scene_manager));
        _layer_stack.push(std::make_unique<sierra_imgui_layer>(&_scene_manager, &_layer_stack));

        _scene_manager.init();
    }

    //-------------------------------------------------------------------------
    void sierra::begin_frame()
    {
        _scene_manager.begin_frame();

        for (const std::unique_ptr<layer>& layer : _layer_stack)
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
        for (const std::unique_ptr<layer>& layer : _layer_stack)
        {
            if (layer->is_enabled())
            {
                layer->tick(dt);
            }
        }

        _scene_manager.tick(dt);
    }

    //-------------------------------------------------------------------------
    void sierra::draw()
    {
        background(200);
        
        for (const std::unique_ptr<layer>& layer : _layer_stack)
        {
            if (layer->is_enabled())
            {
                layer->draw();
            }
        }

        _scene_manager.draw();
    }

    //-------------------------------------------------------------------------
    void sierra::inspector_draw()
    {
        for (const std::unique_ptr<layer>& layer : _layer_stack)
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
        for (const std::unique_ptr<layer>& layer : _layer_stack)
        {
            if (layer->is_enabled())
            {
                layer->end_frame();
            }
        }

        _scene_manager.end_frame();
    }

    //-------------------------------------------------------------------------
    void sierra::shutdown()
    {
        _layer_stack.clear();
    }

    //-------------------------------------------------------------------------
    std::unique_ptr<sketch> make_sketch()
    {
        return std::make_unique<sierra>();
    }
}