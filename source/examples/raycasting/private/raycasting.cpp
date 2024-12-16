#include "entrypoint.h"

#include "shapes.h"
#include "environment.h"
#include "color.h"
#include "rendering.h"
#include "events.h"
#include "structure.h"
#include "image.h"
#include "typography.h"

#include "boundary.h"
#include "particle.h"

#include <vector>

namespace ppp
{
    std::vector<raycasting::Boundary> _walls;

    raycasting::Particle _particle(200, 200);

    typography::font_id _font;

    app_params entry(int argc, char** argv)
    {
        app_params app_params;

        app_params.window_width = 1280;
        app_params.window_height = 720;

        return app_params;
    }

    void setup()
    {
        keyboard::set_quit_application_keycode(keyboard::KeyCode::KEY_ESCAPE);

        rendering::create_canvas((1280 / 2) - 200, (720 / 2) - 200, 400.0f, 400.0f);

        color::background(5);

        for (int i = 0; i < 5; ++i)
        {
            int x1 = rand() % 400;
            int y1 = rand() % 400;
            int x2 = rand() % 400;
            int y2 = rand() % 400;

            raycasting::Boundary b(x1, x2, y1, y2);

            _walls.push_back(b);
        }

        _walls.push_back(raycasting::Boundary(0, 1, 400, 1));
        _walls.push_back(raycasting::Boundary(400, 0, 400, 400));
        _walls.push_back(raycasting::Boundary(400, 400, 0, 400));
        _walls.push_back(raycasting::Boundary(1, 400, 1, 0));

        environment::frame_rate(60);

        _font = typography::load_font("local:/content/fonts/PokemonGb-RAeo.ttf", 16);
        if (_font == -1)
        {
            exit(EXIT_FAILURE);
        }
        else
        {
            typography::text_font(_font);
        }
    }

    void draw()
    {
        for (const raycasting::Boundary& b : _walls)
        {
            b.show();
        }

        _particle.set_position(mouse::mouse_x(), mouse::mouse_y());
        _particle.look(_walls);
        _particle.show();

        color::fill({255,0,0,255});
        std::string str_frame_rate = std::to_string(environment::frame_rate());
        std::string str_delta_time = std::to_string(environment::delta_time());

        typography::text(str_frame_rate, 10, 10);
        typography::text(str_delta_time, 70, 10);
    }
}