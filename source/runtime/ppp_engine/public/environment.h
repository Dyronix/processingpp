/**
 * @file environment.h
 * @brief State of the sketch
 */
#pragma once

#include <iostream>

namespace ppp
{
    /**
     * @brief Prints a message to the console.
     * @param message The message to print.
     */
    void print(std::string_view message);

    /**
     * @brief Prints a C-string message to the console.
     * @param message The C-string to print.
     */
    void print(const char* message);

    /**
     * @brief Prints a formatted message to the console.
     * @tparam Args Types of format arguments.
     * @param fmt The printf-style format string.
     * @param args Arguments matching the format specifiers.
     */
    template<typename... Args>
    void print(const char* fmt, Args&&... args);

    /**
     * @brief Sets the application's target frame rate.
     * @param frame_rate Frames per second to target.
     */
    void frame_rate(unsigned int frame_rate);

    /**
     * @brief Gets the average frame rate over recent frames.
     * @return Average frames per second.
     */
    unsigned int average_frame_rate();

    /**
     * @brief Gets the current frame rate.
     * @return Current frames per second.
     */
    unsigned int frame_rate();

    /**
     * @brief Gets the number of frames rendered since start.
     * @return Frame count.
     */
    unsigned int frame_count();

    /**
     * @brief Gets the time elapsed since the last frame.
     * @return Delta time in seconds.
     */
    float delta_time();

    /**
     * @brief Gets the total running time of the application.
     * @return Total time in seconds.
     */
    float total_time();

    /**
     * @brief Gets the width of the application window.
     * @return Window width in pixels.
     */
    float window_width();

    /**
     * @brief Gets the height of the application window.
     * @return Window height in pixels.
     */
    float window_height();

    /**
     * @brief Gets the width of the drawing canvas.
     * @return Canvas width in pixels.
     */
    float canvas_width();

    /**
     * @brief Gets the height of the drawing canvas.
     * @return Canvas height in pixels.
     */
    float canvas_height();

    /**
     * @brief Returns the current working directory.
     * @return Path to the working directory as a string view.
     */
    std::string_view cwd();
}