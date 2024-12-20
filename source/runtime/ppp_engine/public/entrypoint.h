#pragma once

#include <functional>

namespace ppp
{
    struct app_params
    {
        int window_width = 1280;
        int window_height = 720;
    };

    bool has_argument(int argc, char** argv, const char* target);

    int find_argument(int argc, char** argv, const char* target);
    int find_argument_with_value(int argc, char** argv, const char* target, const char** value);

    extern app_params entry(int argc, char** argv);

    extern void setup();
    extern void draw();
}