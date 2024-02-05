#pragma once

#include <string>

namespace ppp
{
    namespace environment
    {
        void print(const std::string& message);

        float window_width();
        float window_height();

        float canvas_width();
        float canvas_height();
    }
}