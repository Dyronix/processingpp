/**
 * @file entrypoint.h
 * @brief Entrypoint of the sketch
 */
#pragma once

namespace ppp
{
    /**
     * @brief C-style function pointer type for engine event callbacks.
     */
    using engine_delegates = void(*)();

    /**
     * @brief Subscription functions for engine events
     */
    void subscribe_begin_frame(engine_delegates callback);
    void subscribe_pre_render(engine_delegates callback);
    void subscribe_post_render(engine_delegates callback);
    void subscribe_end_frame(engine_delegates callback);
    void subscribe_step(engine_delegates callback);

    /**
     * @brief Holds parameters for application initialization.
     */
    struct app_params
    {
        /** Window width in pixels (default: 1280). */
        int window_width = 1280;
        /** Window height in pixels (default: 720). */
        int window_height = 720;
    };

    /**
     * @brief Determines whether a specific command-line argument is present.
     * @param argc Argument count.
     * @param argv Argument values.
     * @param target The argument to look for.
     * @return True if the argument is found, false otherwise.
     */
    bool has_argument(int argc, char** argv, const char* target);

    /**
     * @brief Finds the index of a specific command-line argument.
     * @param argc Argument count.
     * @param argv Argument values.
     * @param target The argument to find.
     * @return Index of the argument if found, or -1 if not found.
     */
    int find_argument(int argc, char** argv, const char* target);

    /**
     * @brief Finds the index of a specific command-line argument and retrieves its value.
     * @param argc Argument count.
     * @param argv Argument values.
     * @param target The argument to find.
     * @param value Out parameter that receives the argument's value.
     * @return Index of the argument if found (value set), or -1 if not found.
     */
    int find_argument_with_value(int argc, char** argv, const char* target, const char** value);

    /**
     * @brief Entry point for application initialization.
     * @param argc Argument count.
     * @param argv Argument values.
     * @return Configured application parameters.
     */
    extern app_params entry(int argc, char** argv);

    /**
     * @brief User-defined setup function, called once after initialization.
     */
    extern void setup();

    /**
     * @brief User-defined draw function, called each frame.
     */
    extern void draw();
}