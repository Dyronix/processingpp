/**
 * @file structure.h
 * @brief Structural setting of the sketch
 */
#pragma once

#include <functional>

namespace ppp
{
    /**
     * @brief Registers a callback invoked at the beginning of each draw cycle.
     * @param draw_begin Function to call before drawing begins.
     */
    void on_draw_begin(std::function<void()> draw_begin);

    /**
     * @brief Registers a callback invoked at the end of each draw cycle.
     * @param draw_end Function to call after drawing ends.
     */
    void on_draw_end(std::function<void()> draw_end);

    /**
     * @brief Requests a redraw of the display.
     */
    void redraw();

    /**
     * @brief Starts the application loop, causing continuous updates and rendering.
     */
    void loop();

    /**
     * @brief Stops the continuous application loop.
     */
    void no_loop();

    /**
     * @brief Checks if the application is running in headless mode (no graphics).
     * @return True if headless mode is enabled, false otherwise.
     */
    bool is_headless();

    /**
     * @brief Checks if the application loop is currently running.
     * @return True if looping, false otherwise.
     */
    bool is_looping();

    /**
     * @brief Switches the application into headless mode (no graphical output).
     */
    void headless();

    /**
     * @brief Requests the application to quit.
     */
    void quit();
}