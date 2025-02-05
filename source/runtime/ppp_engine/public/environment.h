#pragma once

#include <string>
#include <iostream>

namespace ppp
{
    namespace environment
    {
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

        void frame_rate(unsigned int frame_rate);

        unsigned int average_frame_rate();
        unsigned int frame_rate();
        unsigned int frame_count();

        float delta_time();
        float total_time();

        float window_width();
        float window_height();

        float canvas_width();
        float canvas_height();

        std::string_view cwd();
    }
}