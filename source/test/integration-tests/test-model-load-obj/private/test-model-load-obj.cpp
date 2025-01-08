#include "engine.h"

// Demo of shapes and images with stroke and inner stroke
namespace ppp
{
    bool _generate_new_data = false;
    bool _no_close_after_x_frames = false;
    bool _no_testing = false;

    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;

    model::model_id _model_ambulance;
    model::model_id _model_police;
    model::model_id _model_firetruck;

    image::image _image_color_map;

    void setup_canvas()
    {
        color::background(15);
    }

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::KeyCode::KEY_ESCAPE);
    }

    void end_draw()
    {
        if (environment::frame_count() == 5)
        {
            if (_generate_new_data)
            {
                image::load_pixels(0, 0, _window_width, _window_height);
                image::save_pixels("local:/test-model-load.png", _window_width, _window_height);
            }

            if (!_no_testing)
            {
                auto test_frame = image::load("local:/test-model-load.png");
                auto test_frame_pixels = image::load_pixels(test_frame.id);

                size_t total_size = test_frame.width * test_frame.height * test_frame.channels;

                std::vector<unsigned char> active_test_frame_pixels(total_size);
                memcpy_s(
                    active_test_frame_pixels.data(),
                    total_size,
                    test_frame_pixels,
                    total_size);

                auto frame_pixels = image::load_pixels(0, 0, _window_width, _window_height);

                std::vector<unsigned char> active_frame_pixels(total_size);
                memcpy_s(
                    active_frame_pixels.data(),
                    total_size,
                    frame_pixels,
                    total_size);

                if (memcmp(active_test_frame_pixels.data(), active_frame_pixels.data(), total_size) != 0)
                {
                    environment::print("[TEST FAILED][MDLL] image buffers are not identical!");
                }
                else
                {
                    environment::print("[TEST SUCCESS][MDLL] image buffers are identical.");
                }
            }

            if (!_no_close_after_x_frames)
            {
                structure::quit();
            }
        }
    }

    app_params entry(int argc, char** argv)
    {
        environment::print("current working dir: " + environment::cwd());

        app_params app_params;

        app_params.window_width = 1280;
        app_params.window_height = 720;

        _generate_new_data = has_argument(argc, argv, "--generate-new-data");
        _no_close_after_x_frames = has_argument(argc, argv, "--no_close");
        _no_testing = has_argument(argc, argv, "--no_testing");

        return app_params;
    }

    void setup()
    {
        setup_canvas();
        setup_input_events();

        shapes::enable_wireframe_mode(false);
        shapes::enable_solid_mode(true);

        camera::perspective(55.0f, _window_width / _window_height, 0.1f, 2000.0f);
        camera::set_scene_camera(20, -40, 400);

        _image_color_map = image::load("local:content/t_colormap.png");
        _model_ambulance = model::load_model("local:content/models/ambulance.obj");
        _model_police = model::load_model("local:content/models/police.obj");
        _model_firetruck = model::load_model("local:content/models/firetruck.obj");

        structure::on_draw_end(end_draw);
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

        color::fill({ 255,0,0,255 });

        material::reset_textures();
        material::texture(_image_color_map.id);

        transform::push();
        transform::translate(-105.0f, -25.0f, 0.0f);
        transform::scale(50.0f, 50.0f, 50.0f);
        model::draw(_model_police);
        transform::pop();

        transform::push();
        transform::translate(0.0f, -25.0f, 0.0f);
        transform::scale(50.0f, 50.0f, 50.0f);
        model::draw(_model_ambulance);
        transform::pop();

        transform::push();
        transform::translate(115.0f, -25.0f, 0.0f);
        transform::scale(50.0f, 50.0f, 50.0f);
        model::draw(_model_firetruck);
        transform::pop();
    }
}