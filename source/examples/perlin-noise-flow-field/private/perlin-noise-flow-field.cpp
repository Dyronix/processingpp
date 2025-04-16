#include "entrypoint.h"

#include "shapes.h"
#include "color.h"
#include "rendering.h"
#include "events.h"
#include "structure.h"
#include "transform.h"
#include "environment.h"
#include "image.h"
#include "mathematics.h"
#include "trigonometry.h"
#include "constants.h"

#include "particle.h"

#include <vector>
#include <algorithm>

namespace ppp
{
    constexpr int _window_width = 1280;
    constexpr int _window_height = 720;
    constexpr int _canvas_width = 600;
    constexpr int _canvas_height = 600;

    constexpr int _nr_particles = 1000;

    constexpr float _max_particle_speed = 4.0f;
    constexpr int _max_previous_positions = 200;

    float _chaos_multiplier = 1.0f;

    int _rows = 0;
    int _cols = 0;

    float _inc_x = 0.25f;
    float _inc_y = 0.25f;
    float _inc_z = 0.1f;

    float _scl = 12.0f;

    float _z_off = 0.0f;
    bool _z_off_enabled = true;

    bool _lines_enabled = false;

    std::vector<flow_field::Particle> _particles;
    std::vector<vec2> _flow_field;

    app_params entry(int argc, char** argv)
    {
        app_params app_params;

        app_params.window_width = _window_width;
        app_params.window_height = _window_height;

        return app_params;
    }

    void setup()
    {
        angle_mode(angle_mode_type::RADIANS);

        set_quit_application_keycode(key_code::KEY_ESCAPE);

        add_key_pressed_callback(
            [&](key_code key)
        {
            switch(key)
            {
            case key_code::KEY_UP:
                _chaos_multiplier = std::clamp(_chaos_multiplier + 0.1f, 1.0f, 10.0f);
                print("_chaos_multiplier: ", _chaos_multiplier);
                break;
            case key_code::KEY_DOWN:
                _chaos_multiplier = std::clamp(_chaos_multiplier - 0.1f, 1.0f, 10.0f);
                print("_chaos_multiplier: ", _chaos_multiplier);
                break;
            case key_code::KEY_SPACE:
                _z_off_enabled = !_z_off_enabled;
                print("_z_off_enabled: ", _z_off_enabled);
                break;
            case key_code::KEY_F1:
                _lines_enabled = !_lines_enabled;
                print("_lines_enabled: ", _lines_enabled);
                break;
            }
        });

        create_canvas((_window_width / 2) - (_canvas_width / 2), (_window_height / 2) - (_canvas_height / 2), _canvas_width, _canvas_height);
        background(0);

        _cols = math::floor(_canvas_width / _scl);
        _rows = math::floor(_canvas_height / _scl);

        for (int i = 0; i < _nr_particles; ++i)
        {
            float x = (rand() % _canvas_width);
            float y = (rand() % _canvas_height);
            _particles.push_back(flow_field::Particle(x, y, _max_particle_speed, _max_previous_positions));
        }

        _flow_field.reserve(_cols * _rows);
    }

    void draw()
    {
        _flow_field.clear();

        rect_mode(shape_mode_type::CORNER);

        float z_off = 0;
        float y_off = 0;

        for (int y = 0; y < _rows; ++y)
        {
            float x_off = 0;
            for (int x = 0; x < _cols; ++x)
            {
                int index = x + (y * _cols);
                
                float angle = math::noise(x_off, y_off, _z_off) * two_pi() * _chaos_multiplier;

                vec2 v = from_angle(angle);
                vec2 clamped_v = v.set_mag(0.1f);

                auto it = _flow_field.begin();
                std::advance(it, index);
                _flow_field.insert(it, clamped_v);

                if (_lines_enabled)
                {
                    push();
                    {
                        translate(x * _scl, y * _scl);
                        rotate(_flow_field[index].heading());

                        fill(255, 255, 255, 50);
                        line(0, 0, _scl, 0);
                    }
                    pop();
                }

                x_off += _inc_x;
            }

            y_off += _inc_y;
        }

        if (_z_off_enabled)
        {
            _z_off += _inc_z;
        }

        for (flow_field::Particle& p : _particles)
        {
            p.follow(_flow_field, _rows, _cols, _scl);
            p.update();
            p.show();
            p.loop_around_edges();
        }
    }
}