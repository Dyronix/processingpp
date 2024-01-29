#include "entrypoint.h"
#include "agents.h"

namespace agents
{
    void setup()
    {
        // Nothing to implement
    }

    void draw()
    {
        // Nothing to implement
    }
}

namespace ppp
{
    AppParams app_entry()
    {
        AppParams app_params;

        app_params.app_setup_func = [](){ agents::setup(); };
        app_params.app_setup_func = [](){ agents::draw(); };

        app_params.window_width = 1280;
        app_params.window_height = 720;

        return app_params;
    }
}