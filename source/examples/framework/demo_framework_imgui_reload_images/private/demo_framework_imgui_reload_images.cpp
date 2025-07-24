#include "framework.h"

#include <algorithm>
#include <memory>

namespace ppp
{
    class my_sketch : public sketch
    {
    public:
        my_sketch() = default;
        ~my_sketch() override = default;

    public:
        void setup() override;
        void tick(float dt) override;
        void draw() override;

    private:
        void setup_input_events();
        void append_lights();

        void update_directional_light();
        
        void draw_shapes_grid() const;
        void draw_floor() const;

    private:
        int _window_width = 1280;
        int _window_height = 720;

        image _image_container;
        image _image_wall;
    };

    //-------------------------------------------------------------------------
    void my_sketch::setup()
    {
        setup_input_events();

        enable_batched_draw_mode();
        enable_wireframe_mode(false);
        enable_solid_mode(true);

        perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        set_scene_camera(20, -40, 400);
        shader(material::tags::unlit::texture());

        _image_container = load("local:content/t_container.jpg");
        _image_wall = load("local:content/t_wall.jpg");
    }

    //-------------------------------------------------------------------------
    void my_sketch::tick(float dt)
    {
        orbit_control_options options;

        options.zoom_sensitivity = 200.0f;
        options.panning_sensitivity = 0.5f;
        options.rotation_sensitivity = 0.5f;
        options.min_zoom = 1.0f;
        options.max_zoom = 600.0f;

        orbit_control(options);
    }

    //-------------------------------------------------------------------------
    void my_sketch::draw()
    {
        background(200);

        int grid_width = 5;
        int grid_height = 3;
        int grid_depth = 4;

        float cube_x_offset = 60.0f;
        float cube_y_offset = 60.0f;
        float cube_z_offset = 60.0f;

        float start_x = (-1.0f * cube_x_offset) * (grid_width / 2);
        float start_y = (-1.0f * cube_y_offset) * (grid_height / 2);
        float start_z = (-1.0f * cube_z_offset) * (grid_depth / 2);

        for (int layer = 0; layer < grid_depth; ++layer)
        {
            for (int row = 0; row < grid_height; ++row)
            {
                for (int col = 0; col < grid_width; ++col)
                {
                    push();

                    reset_textures();
                    texture((layer + row + col) % 2 ? _image_wall.id : _image_container.id);

                    translate(
                        start_x + col * cube_x_offset,
                        start_y + row * cube_y_offset,
                        start_z + layer * cube_z_offset
                    );

                    box(50.0f, 50.0f, 50.0f);
                    pop();
                }
            }
        }
    }

    //-------------------------------------------------------------------------
    void my_sketch::setup_input_events()
    {
        set_quit_application_keycode(key_code::KEY_ESCAPE);
    }

    //-------------------------------------------------------------------------
    std::unique_ptr<sketch> make_sketch()
    {
        return std::make_unique<my_sketch>();
    }
}