#include "engine.h"

#include <sstream>

namespace ppp
{
    bool _generate_new_data = false;
    bool _no_close_after_x_frames = false;
    bool _no_testing = false;

    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

    constexpr int _total_shape_count = 8;

    int _shape_vis = 0;
    int _show_all = 1;
    int _show_floor = 1;

    int _interpolation = 24;

    int _enable_normal = 0;

    material::shader_program _material_lit;

    void append_lights()
    {
        //lights::directional_light_desc directional_desc =
        //{
        //    -0.2f, // directional_desc.dirx
        //    -1.0f, // directional_desc.diry
        //    -0.3f, // directional_desc.dirz
        //    0.8f, // directional_desc.r
        //    0.8f, // directional_desc.b
        //    0.8f, // directional_desc.b
        //    1.0f, // directional_desc.intensity
        //    1.0f, // directional_desc.spec_r
        //    1.0f, // directional_desc.spec_g
        //    1.0f, // directional_desc.spec_b
        //    false, // directional_desc.spec_enabled
        //    false // directional_desc.cast_shadows
        //};

        //lights::directional_light(directional_desc);

        lights::point_light_desc point_desc =
        {
            0.0f, 0.0f, 0.0f,       // position
            0.05f, 0.05f, 0.05f,    // ambient          
            0.8f, 0.8f, 0.8f,       // diffuse
            1.0f, 1.0f, 1.0f,       // specular          
            false,                  // specular enabled
            false,                  // cast shadows
            800.0f,                 // max range
            350.0f                  // falloff start
        };

        float start_x = -120.0f; // Initial x position to start grid from
        float x_spacing = 80.0f; // Horizontal spacing between shapes

        for(int i = 0; i < 4; i++)
        {
            switch (i)
            {
            case 0:
                // lights
                point_desc.x = start_x + (i * x_spacing);
                point_desc.y = 40.0f;
                point_desc.z = 100.0f;

                lights::point_light(point_desc);
                break;
            //case 1:
            //    // lights
            //    point_desc.x = start_x + (i * x_spacing);
            //    point_desc.y = 40.0f;
            //    point_desc.z = 100.0f;

            //    lights::point_light(point_desc);
            //    break;
            //case 2:
            //    // lights
            //    point_desc.x = start_x + (i * x_spacing);
            //    point_desc.y = 40.0f;
            //    point_desc.z = 100.0f;

            //    lights::point_light(point_desc);
            //    break;
            case 3:
                // lights
                point_desc.x = start_x + (i * x_spacing);
                point_desc.y = 40.0f;
                point_desc.z = 100.0f;

                lights::point_light(point_desc);
                break;
            }
        }
    }

    void draw_lights()
    {
        float start_x = -120.0f; // Initial x position to start grid from
        float start_y = 40.0f;    // Initial y position for the grid row
        float x_spacing = 80.0f; // Horizontal spacing between shapes

        transform::push();
        transform::translate(start_x, start_y, 100.0f);
        for (int i = 0; i < 4; i++)
        {
            // shapes
            shapes::box(5.0f, 5.0f, 5.0f);
            transform::translate(x_spacing, 0.0f);

        }
        transform::pop();
    }

    void draw_shapes_grid()
    {

        float start_x = -120.0f; // Initial x position to start grid from
        float start_y = 40.0f;    // Initial y position for the grid row
        float x_spacing = 80.0f; // Horizontal spacing between shapes
        float y_spacing = -80.0f; // Vertical spacing between rows

        if (_show_all)
        {
            transform::push();

            // Row 1
            transform::translate(start_x, start_y);
            shapes::box(50.0f, 50.0f, 50.0f);
            transform::translate(x_spacing, 0.0f);
            shapes::plane(50.0f, 50.0f);
            transform::translate(x_spacing, 0.0f);
            shapes::cylinder(25.0f, 50.0f, _interpolation);
            transform::translate(x_spacing, 0.0f);
            shapes::sphere(25.0f, _interpolation);

            // Move to next row and reset x position
            transform::translate(-3 * x_spacing, y_spacing);

            // Row 2
            shapes::torus(25.0f, 10.0f, _interpolation, _interpolation);
            transform::translate(x_spacing, 0.0f);
            shapes::cone(25.0f, 50.0f, _interpolation, true);
            transform::translate(x_spacing, 0.0f);
            shapes::tetrahedron(25.0f, 25.0f);
            transform::translate(x_spacing, 0.0f);
            shapes::octahedron(25.0f, 25.0f);

            transform::pop();
        }
        else
        {
            transform::push();

            // Row 1
            transform::translate(start_x, start_y);
            if (_shape_vis == 0) { shapes::box(50.0f, 50.0f, 50.0f); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 1) { shapes::plane(50.0f, 50.0f); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 2) { shapes::cylinder(25.0f, 50.0f, _interpolation); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 3) { shapes::sphere(25.0f, _interpolation); }

            // Move to next row and reset x positiond
            transform::translate(-3 * x_spacing, y_spacing);

            // Row 2
            if (_shape_vis == 4) { shapes::torus(25.0f, 10.0f, _interpolation, _interpolation); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 5) { shapes::cone(25.0f, 50.0f, _interpolation, true); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 6) { shapes::tetrahedron(25.0f, 25.0f); }
            transform::translate(x_spacing, 0.0f);
            if (_shape_vis == 7) { shapes::octahedron(25.0f, 25.0f); }

            transform::pop();
        }
    }

    void draw_floor()
    {
        if (_show_floor)
        {
            float start_x = -120.0f; // Initial x position to start grid from
            float start_y = 40.0f;    // Initial y position for the grid row
            float x_spacing = 80.0f; // Horizontal spacing between shapes
            float y_spacing = -80.0f; // Vertical spacing between rows

            transform::push();

            transform::translate(start_x, start_y);
            transform::translate(x_spacing * 2 - 40.0f, y_spacing - 40.0f);

            transform::rotate(1.0f, 0.0f, 0.0f, -90.0f);

            shapes::plane(400.0f, 400.0f);

            transform::pop();
        }
    }

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::key_code::KEY_ESCAPE);

        keyboard::add_key_pressed_callback(
            [](keyboard::key_code key)
        {
            if (key == keyboard::key_code::KEY_SPACE)
            {
                bool show_all_shapes = _show_all > 0;
                show_all_shapes = !show_all_shapes;
                _show_all = show_all_shapes ? 1 : 0;
            }

            else if (key == keyboard::key_code::KEY_UP && _show_all == 0)
            {
                _shape_vis = (_shape_vis + 1) % _total_shape_count;
            }
            else if (key == keyboard::key_code::KEY_DOWN && _show_all == 0)
            {
                _shape_vis = (_shape_vis - 1) < 0 ? _total_shape_count - 1 : _shape_vis - 1;
            }

            else if (key == keyboard::key_code::KEY_1)
            {
                _interpolation = 4;
            }
            else if (key == keyboard::key_code::KEY_2)
            {
                _interpolation = 8;
            }
            else if (key == keyboard::key_code::KEY_3)
            {
                _interpolation = 12;
            }
            else if (key == keyboard::key_code::KEY_4)
            {
                _interpolation = 24;
            }

            else if (key == keyboard::key_code::KEY_F)
            {
                _show_floor = _show_floor == 1 ? 0 : 1;
                environment::print("show floor: %d", _show_floor);
            }
            else if (key == keyboard::key_code::KEY_N)
            {
                _enable_normal = _enable_normal == 1 ? 0 : 1;
                environment::print("enable normal: %d", _enable_normal);

                if (_enable_normal)
                {
                    lights::no_lights();
                }
                else
                {
                    append_lights();
                }
            }
        });
    }

    app_params entry(int argc, char** argv)
    {
        environment::print("Current working directory: %s", environment::cwd().data());

        app_params app_params;

        app_params.window_width = _window_width;
        app_params.window_height = _window_height;

        _generate_new_data = has_argument(argc, argv, "--generate-new-data");
        _no_close_after_x_frames = has_argument(argc, argv, "--no-close");
        _no_testing = has_argument(argc, argv, "--no-testing");

        return app_params;
    }

    void setup()
    {
        std::string vs_path = "local:/content/shaders/lit.vs";
        std::string ps_path = "local:/content/shaders/lit.fs";

        _material_lit = material::load_shader("lit", vs_path, ps_path, material::shading_model::LIT);

        setup_input_events();

        rendering::enable_batched_draw_mode();

        shapes::enable_wireframe_mode(false);
        shapes::enable_solid_mode(true);
        shapes::normal_mode(shapes::normal_mode_type::FLAT);

        camera::perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        camera::set_scene_camera(20, -40, 400);

        material::shader("lit");

        trigonometry::angle_mode(trigonometry::angle_mode_type::DEGREES);

        append_lights();
    }

    void draw()
    {
        color::background(200);

        camera::orbit_scene_camera_options options;

        options.zoom_sensitivity = 200.0f;
        options.panning_sensitivity = 0.5f;
        options.rotation_sensitivity = 0.5f;
        options.min_zoom = 1.0f;
        options.max_zoom = 600.0f;

        camera::orbit_control(options);

        if (_enable_normal)
        {
            material::normal_material();
        }
        else
        {
            material::shader("lit");
        }
        color::fill({ 255,0,0,255 });
        draw_shapes_grid();
        draw_floor();

        material::shader(material::tags::unlit_color());
        color::fill({ 255, 255, 255, 255 });
        draw_lights();

        color::fill({ 0,0,0,255 });
    }
}