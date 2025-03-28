#include "engine.h"

#include <sstream>

namespace ppp
{
    bool _generate_new_data = false;
    bool _no_close_after_x_frames = false;
    bool _no_testing = false;

    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;

    model::model_id _model_id;
    image::image _image;

    void setup_canvas()
    {
        color::background(15);
    }

    void setup_input_events()
    {
        keyboard::set_quit_application_keycode(keyboard::key_code::KEY_ESCAPE);
    }

    void end_draw()
    {
        if (environment::frame_count() == 5)
        {
            std::stringstream stream;

            stream << "local:/";
            stream << TEST_PROJECT_NAME;
            stream << ".png";

            if (_generate_new_data)
            {
                image::load_pixels(0, 0, _window_width, _window_height);
                image::save_pixels(stream.str(), _window_width, _window_height);
            }

            if (!_no_testing)
            {
                auto test_frame = image::load(stream.str());
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
                    environment::print("[TEST FAILED][MDLC] image buffers are not identical!");
                }
                else
                {
                    environment::print("[TEST SUCCESS][MDLC] image buffers are identical.");
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
        environment::print("Current working directory: %s", environment::cwd().data());

        app_params app_params;

        app_params.window_width = 1280;
        app_params.window_height = 720;

        _generate_new_data = has_argument(argc, argv, "--generate-new-data");
        _no_close_after_x_frames = has_argument(argc, argv, "--no-close");
        _no_testing = has_argument(argc, argv, "--no-testing");

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

        _image = image::load("local:content/t_crystal.png");

        std::string crystal_string = 
            #include "crystal.h"

        _model_id = model::create_model(crystal_string, model::model_file_type::OBJ);

        structure::on_draw_end(end_draw);

        material::shader(material::tags::unlit_texture());
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
        material::texture(_image.id);

        transform::push();
        transform::translate(00.0f, -45.0f, 0.0f);
        transform::scale(40.0f, 40.0f, 40.0f);
        model::draw(_model_id);
        transform::pop();
    }
}