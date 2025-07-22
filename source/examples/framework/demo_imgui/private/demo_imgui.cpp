#include "framework.h"

#include <algorithm>
#include <memory>

namespace
{
    //-------------------------------------------------------------------------
    static void draw_cube()
    {
        ppp::push();

        ppp::box(50.0f, 50.0f, 50.0f);

        ppp::pop();
    }
    //-------------------------------------------------------------------------
    static void draw_floor()
    {
        float start_x = -120.0f; // Initial x position to start grid from
        float start_y = 40.0f;    // Initial y position for the grid row
        float x_spacing = 80.0f; // Horizontal spacing between shapes
        float y_spacing = -80.0f; // Vertical spacing between rows

        ppp::push();

        ppp::translate(start_x, start_y);
        ppp::translate(x_spacing * 2 - 40.0f, y_spacing - 40.0f);

        ppp::rotate(1.0f, 0.0f, 0.0f, -90.0f);

        ppp::plane(400.0f, 400.0f);

        ppp::pop();
    }
}

namespace ppp
{
    class my_sketch : public sketch
    {
    public:
        my_sketch() = default;
        ~my_sketch() override = default;

    public:
        void setup() override;
        void draw() override;
        void debug_draw() override;

    private:
        void setup_directional_light();

    private:
        int _window_width = 1280;
        int _window_height = 720;

        // Initial light direction values.
        float _initial_dir_x = -0.2f;
        float _initial_dir_y = -1.0f;
        float _initial_dir_z = -0.3f;
    };

    //-------------------------------------------------------------------------
    void my_sketch::setup()
    {
        set_quit_application_keycode(key_code::KEY_ESCAPE);

        std::string vs_path = "local:/content/shaders/lit.vs";
        std::string ps_path = "local:/content/shaders/lit.fs";

        load_shader("lit", vs_path, ps_path, shading_model::LIT);

        enable_batched_draw_mode();

        enable_wireframe_mode(false);
        enable_solid_mode(true);
        normal_mode(normal_mode_type::FLAT);

        perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        set_scene_camera(20, -40, 400);

        shader("lit");

        angle_mode(angle_mode_type::DEGREES);

        setup_directional_light();
    }

    //-------------------------------------------------------------------------
    void my_sketch::draw()
    {
        background(200);

        orbit_control_options options;

        options.zoom_sensitivity = 200.0f;
        options.panning_sensitivity = 0.5f;
        options.rotation_sensitivity = 0.5f;
        options.min_zoom = 1.0f;
        options.max_zoom = 600.0f;

        orbit_control(options);

        fill({ 255,0,0,255 });

        draw_cube();
        draw_floor();
    }

    //-------------------------------------------------------------------------
    void my_sketch::debug_draw()
    {
        
    }

    //-------------------------------------------------------------------------
    void my_sketch::setup_directional_light()
    {
        directional_light_desc directional_desc =
        {
            {_initial_dir_x, _initial_dir_y, _initial_dir_z},                                               // direction

            {static_cast<int>(0.05f * 255), static_cast<int>(0.05f * 255), static_cast<int>(0.05f * 255)},  // ambient          
            {static_cast<int>(0.8f * 255), static_cast<int>(0.8f * 255), static_cast<int>(0.8f * 255)},     // diffuse
            {static_cast<int>(1.0f * 255), static_cast<int>(1.0f * 255), static_cast<int>(1.0f * 255)},     // specular          
            false,                                                                                          // specular enabled
            true,                                                                                           // cast shadows
        };

        directional_light(directional_desc);
    }

    //-------------------------------------------------------------------------
    std::unique_ptr<sketch> make_sketch()
    {
        return std::make_unique<my_sketch>();
    }
}