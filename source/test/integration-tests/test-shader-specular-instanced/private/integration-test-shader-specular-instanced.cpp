#include "engine.h"

#include <sstream>

namespace ppp
{
    bool _generate_new_data = false;
    bool _no_close_after_x_frames = false;
    bool _no_testing = false;

    constexpr int _window_width = 240;
    constexpr int _window_height = 160;

    constexpr int _total_shape_count = 8;

    int _shape_vis = 0;
    int _show_all = 1;

    int _interpolation = 8;

    float _specular_strength = 0.5f;
    int _specular_shininess = 32;

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
                print("_interpolation: " + std::to_string(_interpolation));
            }
            else if (key == key_code::KEY_2)
            {
                _interpolation = 8;
                print("_interpolation: " + std::to_string(_interpolation));
            }
            else if (key == key_code::KEY_3)
            {
                _interpolation = 12;
                print("_interpolation: " + std::to_string(_interpolation));
            }
            else if (key == key_code::KEY_4)
            {
                _interpolation = 24;
                print("_interpolation: " + std::to_string(_interpolation));
            }

            else if (key == key_code::KEY_W)
            {
                _specular_shininess = _specular_shininess >> 1;
                if (_specular_shininess < 2)
                {
                    _specular_shininess = 2;
                }
                print("_specular_shininess: " + std::to_string(_specular_shininess));
            }
            else if (key == key_code::KEY_A)
            {
                _specular_strength = _specular_strength += 0.1f;
                if (_specular_strength > 1.0f)
                {
                    _specular_strength = 1.0f;
                }
                print("_specular_strength: " + std::to_string(_specular_strength));
            }
            else if (key == key_code::KEY_S)
            {
                _specular_shininess = _specular_shininess << 1;
                if (_specular_shininess > 256)
                {
                    _specular_shininess = 256;
                }
                print("_specular_shininess: " + std::to_string(_specular_shininess));
            }
            else if (key == key_code::KEY_D)
            {
                _specular_strength = _specular_strength -= 0.1f;
                if (_specular_strength < 0.0f)
                {
                    _specular_strength = 0.0f;
                }
                print("_specular_strength: " + std::to_string(_specular_strength));
            }
        });
    }

    void end_draw()
    {
        if (frame_count() == 5)
        {
            std::stringstream stream;

            stream << "local:/";
            stream << TEST_PROJECT_NAME;
            stream << ".png";

            if (_generate_new_data)
            {
                load_pixels(0, 0, _window_width, _window_height);
                save_pixels(stream.str(), _window_width, _window_height);
            }

            if (!_no_testing)
            {
                auto test_frame = load(stream.str());
                auto test_frame_pixels = load_pixels(test_frame.id);

                size_t total_size = test_frame.width * test_frame.height * test_frame.channels;

                std::vector<unsigned char> active_test_frame_pixels(total_size);
                memcpy_s(
                    active_test_frame_pixels.data(),
                    total_size,
                    test_frame_pixels,
                    total_size);

                auto frame_pixels = load_pixels(0, 0, _window_width, _window_height);

                std::vector<unsigned char> active_frame_pixels(total_size);
                memcpy_s(
                    active_frame_pixels.data(),
                    total_size,
                    frame_pixels,
                    total_size);

                if (memcmp(active_test_frame_pixels.data(), active_frame_pixels.data(), total_size) != 0)
                {
                    print("[TEST FAILED][SISPEC] image buffers are not identical!");
                }
                else
                {
                    print("[TEST SUCCESS][SISPEC] image buffers are identical.");
                }
            }

            if (!_no_close_after_x_frames)
            {
                quit();
            }
        }
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
        setup_input_events();

        enable_instance_draw_mode();

        enable_wireframe_mode(false);
        enable_solid_mode(true);

        perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        set_scene_camera(20, -40, 400);

        specular_material();

        on_draw_end(end_draw);
    }

    void draw_shapes_grid()
    {
        if (_show_all)
        {
            float start_x = -120.0f; // Initial x position to start grid from
            float start_y = 40.0f;    // Initial y position for the grid row
            float x_spacing = 80.0f; // Horizontal spacing between shapes
            float y_spacing = -80.0f; // Vertical spacing between rows

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
            float start_x = -120.0f; // Initial x position to start grid from
            float start_y = 40.0f;    // Initial y position for the grid row
            float x_spacing = 80.0f; // Horizontal spacing between shapes
            float y_spacing = -80.0f; // Vertical spacing between rows

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

            // Move to next row and reset x position
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

        auto specular_program = get_shader(material::tags::lit::specular());

        specular_program.set_uniform("u_ambient_strength", 0.1f);
        specular_program.set_uniform("u_light_position", vec3{ 0.0f, 0.0f, 200.0f });
        specular_program.set_uniform("u_light_color", vec3{ 1.0f, 1.0f, 1.0f });
        specular_program.set_uniform("u_specular_color", vec3{ 1.0f, 1.0f, 1.0f });
        specular_program.set_uniform("u_specular_strength", _specular_strength);
        specular_program.set_uniform("u_specular_power", _specular_shininess);
        specular_program.set_uniform("u_view_position", active_camera_position());

        fill({ 255,0,0,255 });

        draw_shapes_grid();

        fill({0,0,0,255});
    }
}