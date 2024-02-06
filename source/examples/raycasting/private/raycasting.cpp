#include "entrypoint.h"

#include "shapes.h"
#include "environment.h"
#include "color.h"
#include "rendering.h"
#include "events.h"
#include "structure.h"
#include "image.h"

#include "boundary.h"
#include "particle.h"

#include <vector>

namespace ppp
{
    std::vector<raycasting::Boundary> walls;

    raycasting::Particle particle(200, 200);

    AppParams entry()
    {
        AppParams app_params;

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

            walls.push_back(b);
        }

        walls.push_back(raycasting::Boundary(0, 1, 400, 1));
        walls.push_back(raycasting::Boundary(400, 0, 400, 400));
        walls.push_back(raycasting::Boundary(400, 400, 0, 400));
        walls.push_back(raycasting::Boundary(1, 400, 1, 0));
    }

    void draw()
    {
        for (const raycasting::Boundary& b : walls)
        {
            b.show();
        }

        particle.set_position(mouse::mouse_x(), mouse::mouse_y());
        particle.look(walls);
        particle.show();
    }
}