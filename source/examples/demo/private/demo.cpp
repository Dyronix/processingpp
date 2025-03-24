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

    shader_program _material_lit;
    light_id _dir_light;

    // Initial light direction values.
    const float _initial_dir_x = -0.2f;
    const float _initial_dir_y = -1.0f;
    const float _initial_dir_z = -0.3f;

    // Compute the horizontal magnitude from the X and Z components.
    float _initial_magnitude = std::sqrt(_initial_dir_x * _initial_dir_x + _initial_dir_y * _initial_dir_y + _initial_dir_z * _initial_dir_z);

    // Normalize the initial direction.
    float _norm_x = _initial_dir_x / _initial_magnitude;
    float _norm_y = _initial_dir_y / _initial_magnitude;
    float _norm_z = _initial_dir_z / _initial_magnitude;

    // Calculate initial yaw (angle in the XZ plane) and pitch (angle from horizontal).
    float _initial_yaw = std::atan2(_norm_z, _norm_x);
    float _initial_pitch = std::asin(_norm_y);

    // Global variables to track the current angles.
    float _current_yaw = _initial_yaw;
    float _current_pitch = _initial_pitch;

    // Rotation speeds (radians per second).
    const float _yaw_speed = 1.0f;   // Automatic horizontal rotation speed.
    const float _pitch_speed = 0.5f; // Speed for pitch adjustment.

    void update_directional_light()
    {
        // Automatically update the yaw.
        _current_yaw += _yaw_speed * delta_time();

        // Compute the new directional vector from spherical coordinates.
        float new_dir_x = std::cos(_current_pitch) * std::cos(_current_yaw);
        float new_dir_y = std::sin(_current_pitch);
        float new_dir_z = std::cos(_current_pitch) * std::sin(_current_yaw);

        // Update the light's direction.
        light_direction(_dir_light, light_type::DIRECTIONAL, new_dir_x, new_dir_y, new_dir_z);
    }

    void append_lights()
    {
        directional_light_desc directional_desc =
        {
            _initial_dir_x, _initial_dir_y, _initial_dir_z,    // direction

            0.05f, 0.05f, 0.05f,    // ambient          
            0.8f, 0.8f, 0.8f,       // diffuse
            1.0f, 1.0f, 1.0f,       // specular          
            false,                  // specular enabled
            true,                   // cast shadows
        };

        _dir_light = directional_light(directional_desc);

        //point_light_desc point_desc =
        //{
        //    0.0f, 0.0f, 0.0f,       // position
        //    0.05f, 0.05f, 0.05f,    // ambient          
        //    0.8f, 0.8f, 0.8f,       // diffuse
        //    1.0f, 1.0f, 1.0f,       // specular          
        //    false,                  // specular enabled
        //    false,                  // cast shadows
        //    800.0f,                 // max range
        //    350.0f                  // falloff start
        //};

        //float start_x = -120.0f; // Initial x position to start grid from
        //float x_spacing = 80.0f; // Horizontal spacing between shapes

        //for(int i = 0; i < 4; i++)
        //{
        //    switch (i)
        //    {
        //    case 0:
        //        // lights
        //        point_desc.x = start_x + (i * x_spacing);
        //        point_desc.y = 40.0f;
        //        point_desc.z = 100.0f;

        //        point_light(point_desc);
        //        break;
        //    //case 1:
        //    //    // lights
        //    //    point_desc.x = start_x + (i * x_spacing);
        //    //    point_desc.y = 40.0f;
        //    //    point_desc.z = 100.0f;

        //    //    point_light(point_desc);
        //    //    break;
        //    //case 2:
        //    //    // lights
        //    //    point_desc.x = start_x + (i * x_spacing);
        //    //    point_desc.y = 40.0f;
        //    //    point_desc.z = 100.0f;

        //    //    point_light(point_desc);
        //    //    break;
        //    case 3:
        //        // lights
        //        point_desc.x = start_x + (i * x_spacing);
        //        point_desc.y = 40.0f;
        //        point_desc.z = 100.0f;

        //        point_light(point_desc);
        //        break;
        //    }
        //}
    }

    void draw_lights()
    {
        float start_x = -120.0f; // Initial x position to start grid from
        float start_y = 40.0f;    // Initial y position for the grid row
        float x_spacing = 80.0f; // Horizontal spacing between shapes

        push();
        translate(start_x, start_y, 100.0f);
        for (int i = 0; i < 4; i++)
        {
            // shapes
            box(5.0f, 5.0f, 5.0f);
            translate(x_spacing, 0.0f);

        }
        pop();
    }

    void draw_shapes_grid()
    {
        float start_x = -120.0f; // Initial x position to start grid from
        float start_y = 40.0f;    // Initial y position for the grid row
        float x_spacing = 80.0f; // Horizontal spacing between shapes
        float y_spacing = -80.0f; // Vertical spacing between rows

        if (_show_all)
        {
            push();

            // Row 1
            translate(start_x, start_y);
            box(50.0f, 50.0f, 50.0f);
            translate(x_spacing, 0.0f);
            plane(50.0f, 50.0f);
            translate(x_spacing, 0.0f);
            cylinder(25.0f, 50.0f, _interpolation);
            translate(x_spacing, 0.0f);
            sphere(25.0f, _interpolation);

            // Move to next row and reset x position
            translate(-3 * x_spacing, y_spacing);

            // Row 2
            torus(25.0f, 10.0f, _interpolation, _interpolation);
            translate(x_spacing, 0.0f);
            cone(25.0f, 50.0f, _interpolation, true);
            translate(x_spacing, 0.0f);
            tetrahedron(25.0f, 25.0f);
            translate(x_spacing, 0.0f);
            octahedron(25.0f, 25.0f);

            pop();
        }
        else
        {
            push();

            // Row 1
            translate(start_x, start_y);
            if (_shape_vis == 0) { box(50.0f, 50.0f, 50.0f); }
            translate(x_spacing, 0.0f);
            if (_shape_vis == 1) { plane(50.0f, 50.0f); }
            translate(x_spacing, 0.0f);
            if (_shape_vis == 2) { cylinder(25.0f, 50.0f, _interpolation); }
            translate(x_spacing, 0.0f);
            if (_shape_vis == 3) { sphere(25.0f, _interpolation); }

            // Move to next row and reset x positiond
            translate(-3 * x_spacing, y_spacing);

            // Row 2
            if (_shape_vis == 4) { torus(25.0f, 10.0f, _interpolation, _interpolation); }
            translate(x_spacing, 0.0f);
            if (_shape_vis == 5) { cone(25.0f, 50.0f, _interpolation, true); }
            translate(x_spacing, 0.0f);
            if (_shape_vis == 6) { tetrahedron(25.0f, 25.0f); }
            translate(x_spacing, 0.0f);
            if (_shape_vis == 7) { octahedron(25.0f, 25.0f); }

            pop();
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

            push();

            translate(start_x, start_y);
            translate(x_spacing * 2 - 40.0f, y_spacing - 40.0f);

            rotate(1.0f, 0.0f, 0.0f, -90.0f);

            plane(400.0f, 400.0f);

            pop();
        }
    }

    void setup_input_events()
    {
        set_quit_application_keycode(key_code::KEY_ESCAPE);

        add_key_pressed_callback(
            [](key_code key)
        {
            if (key == key_code::KEY_SPACE)
            {
                bool show_all_shapes = _show_all > 0;
                show_all_shapes = !show_all_shapes;
                _show_all = show_all_shapes ? 1 : 0;
            }

            else if (key == key_code::KEY_UP && _show_all == 0)
            {
                _shape_vis = (_shape_vis + 1) % _total_shape_count;
            }
            else if (key == key_code::KEY_DOWN && _show_all == 0)
            {
                _shape_vis = (_shape_vis - 1) < 0 ? _total_shape_count - 1 : _shape_vis - 1;
            }

            else if (key == key_code::KEY_1)
            {
                _interpolation = 4;
            }
            else if (key == key_code::KEY_2)
            {
                _interpolation = 8;
            }
            else if (key == key_code::KEY_3)
            {
                _interpolation = 12;
            }
            else if (key == key_code::KEY_4)
            {
                _interpolation = 24;
            }

            else if (key == key_code::KEY_F)
            {
                _show_floor = _show_floor == 1 ? 0 : 1;
                print("show floor: %d", _show_floor);
            }
            else if (key == key_code::KEY_N)
            {
                _enable_normal = _enable_normal == 1 ? 0 : 1;
                print("enable normal: %d", _enable_normal);

                if (_enable_normal)
                {
                    no_lights();
                }
                else
                {
                    append_lights();
                }
            }
        });

        // Keyboard callback for adjusting pitch (vertical movement).
        add_key_down_callback([](key_code key)
            {
                if (key == key_code::KEY_W)
                {
                    // Decrease pitch to move the light downward.
                    _current_pitch -= _pitch_speed * delta_time();
                    // Clamp to a minimum (e.g., -89° in radians ~ -1.55334).
                    if (_current_pitch < -1.55334f)
                    {
                        _current_pitch = -1.55334f;
                    }
                }
                else if (key == key_code::KEY_S)
                {
                    // Increase pitch to move the light upward.
                    _current_pitch += _pitch_speed * delta_time();
                    // Clamp to a maximum (e.g., 89° in radians ~ 1.55334).
                    if (_current_pitch > 1.55334f)
                    {
                        _current_pitch = 1.55334f;
                    }
                }
            });
    }

    app_params entry(int argc, char** argv)
    {
        print("Current working directory: %s", cwd().data());

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

        _material_lit = load_shader("lit", vs_path, ps_path, shading_model::LIT);

        setup_input_events();

        enable_batched_draw_mode();

        enable_wireframe_mode(false);
        enable_solid_mode(true);
        normal_mode(normal_mode_type::FLAT);

        perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        set_scene_camera(20, -40, 400);

        shader("lit");

        angle_mode(angle_mode_type::DEGREES);

        append_lights();
    }

    void draw()
    {
        background(200);

        orbit_control_options options;

        options.zoom_sensitivity = 200.0f;
        options.panning_sensitivity = 0.5f;
        options.rotation_sensitivity = 0.5f;
        options.min_zoom = 1.0f;
        options.max_zoom = 600.0f;

        orbit_control(options);

        if (_enable_normal)
        {
            normal_material();
        }
        else
        {
            shader("lit");
        }

        fill({ 255,0,0,255 });
        draw_shapes_grid();
        draw_floor();

        shader(material::tags::unlit_color());
        fill({ 255, 255, 255, 255 });
        //draw_lights();

        fill({ 0,0,0,255 });

        update_directional_light();
    }
}