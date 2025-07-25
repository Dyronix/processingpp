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

        int _total_shape_count = 8;

        int _shape_vis = 0;
        int _show_all = 1;
        int _show_floor = 1;

        int _interpolation = 24;

        int _enable_normal = 0;

        shader_program _material_lit;
        light_id _dir_light;

        // Initial light direction values.
        float _initial_dir_x = -0.2f;
        float _initial_dir_y = -1.0f;
        float _initial_dir_z = -0.3f;

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
    };

    //-------------------------------------------------------------------------
    void my_sketch::setup()
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

        update_directional_light();
    }

    //-------------------------------------------------------------------------
    void my_sketch::draw()
    {
        background(200);

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

        shader(material::tags::unlit::color());
        fill({ 255, 255, 255, 255 });

        fill({ 0,0,0,255 });
    }

    //-------------------------------------------------------------------------
    void my_sketch::setup_input_events()
    {
        set_quit_application_keycode(key_code::KEY_ESCAPE);

        add_key_pressed_callback(
            [&](key_code key)
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
                print("interpolation: %d", _interpolation);
            }
            else if (key == key_code::KEY_2)
            {
                _interpolation = 8;
                print("interpolation: %d", _interpolation);
            }
            else if (key == key_code::KEY_3)
            {
                _interpolation = 12;
                print("interpolation: %d", _interpolation);
            }
            else if (key == key_code::KEY_4)
            {
                _interpolation = 24;
                print("interpolation: %d", _interpolation);
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
        add_key_down_callback(
            [&](key_code key)
        {
            if (key == key_code::KEY_W)
            {
                // Decrease pitch to move the light downward.
                _current_pitch -= _pitch_speed * delta_time();
                // Clamp to a minimum (e.g., -89� in radians ~ -1.55334).
                _current_pitch = std::max(_current_pitch, -1.55334f);
            }
            else if (key == key_code::KEY_S)
            {
                // Increase pitch to move the light upward.
                _current_pitch += _pitch_speed * delta_time();
                // Clamp to a maximum (e.g., 89� in radians ~ 1.55334).
                _current_pitch = std::min(_current_pitch, 1.55334f);
            }
        });
    }
    //-------------------------------------------------------------------------
    void my_sketch::append_lights()
    {
        directional_light_desc directional_desc =
        {
            {_initial_dir_x, _initial_dir_y, _initial_dir_z},                                               // direction

            {static_cast<int>(0.05f * 255), static_cast<int>(0.05f * 255), static_cast<int>(0.05f * 255)},  // ambient          
            {static_cast<int>(0.8f * 255), static_cast<int>(0.8f * 255), static_cast<int>(0.8f * 255)},  // diffuse
            {static_cast<int>(1.0f * 255), static_cast<int>(1.0f * 255), static_cast<int>(1.0f * 255)},  // specular          
            false,                  // specular enabled
            true,                   // cast shadows
        };

        _dir_light = directional_light(directional_desc);
    }
    //-------------------------------------------------------------------------
    void my_sketch::update_directional_light()
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
    //-------------------------------------------------------------------------
    void my_sketch::draw_shapes_grid() const
    {
        float start_x = -120.0f; // Initial x position to start grid from
        float start_y = 40.0f;    // Initial y position for the grid row
        float x_spacing = 80.0f; // Horizontal spacing between shapes
        float y_spacing = -80.0f; // Vertical spacing between rows

        if (_show_all)
        {
            push();

            //enable_shadows();
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

            //disable_shadows();
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
    //-------------------------------------------------------------------------
    void my_sketch::draw_floor() const
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

    //-------------------------------------------------------------------------
    std::unique_ptr<sketch> make_sketch()
    {
        return std::make_unique<my_sketch>();
    }
}