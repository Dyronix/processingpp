#pragma once

#include <string>
#include <iostream>

namespace ppp
{
    namespace environment
    {
        // Print to the console
        void print(std::string_view message);
        void print(const char* message);
        template<typename... Args>
        void print(const char* fmt, Args&&... args) 
        {
            std::array<char, 1024> buffer;

            // std::snprintf writes into buffer.data() and returns the number of characters
            // that would have been written if the buffer were large enough.
            int n = std::snprintf(buffer.data(), buffer.size(), fmt, std::forward<Args>(args)...);

            if (n < 0) 
            {
                std::cout << "Error: formatting failed." << std::endl;
                return;
            }
            
            if (static_cast<size_t>(n) >= buffer.size()) 
            {
                std::cout << "Warning: message truncated." << std::endl;
            }

            // Now pass the formatted string to your original print.
            print(buffer.data());
        }

        // Set the target frame rate
        void frame_rate(unsigned int frame_rate);

        // Get the average frame rate over x amount of frames
        unsigned int average_frame_rate();
        // Get the current frame rate
        unsigned int frame_rate();
        // Get the current frame count
        unsigned int frame_count();

        // Get the delta time
        float delta_time();
        // Get the total time of the application
        float total_time();

        // Get width of the window
        float window_width();
        // Get height of the window
        float window_height();

        // Get canvas width
        float canvas_width();
        // Get canvas height
        float canvas_height();

        // Get the current working director 
        std::string_view cwd();
    }
}