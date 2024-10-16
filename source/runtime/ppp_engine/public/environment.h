#pragma once

#include <string>

namespace ppp
{
    namespace environment
    {
        void print(const std::string& message);

        void frame_rate(unsigned int frame_rate);

        unsigned int average_frame_rate();
        unsigned int frame_rate();
        unsigned int frame_count();

        float delta_time();

        float window_width();
        float window_height();

        float canvas_width();
        float canvas_height();
    }
}