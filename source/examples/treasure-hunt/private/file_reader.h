#pragma once

#include <vector>
#include <stdexcept>

namespace ppp
{
    // FILE READER
    // Encapsulates reading binary data from a vector.
    class file_reader
    {
    private:
        const std::vector<std::byte>& data;
        size_t offset;

    public:
        file_reader(const std::vector<std::byte>& data)
            : data(data), offset(0)
        {
        }

        uint32_t read_uint32()
        {
            if (offset + 4 > data.size())
            {
                throw std::runtime_error("Unexpected end of data while reading uint32_t");
            }
            
            uint32_t value = (static_cast<uint32_t>(std::to_integer<uint8_t>(data[offset])))    |
                (static_cast<uint32_t>(std::to_integer<uint8_t>(data[offset + 1])) << 8)        |
                (static_cast<uint32_t>(std::to_integer<uint8_t>(data[offset + 2])) << 16)       |
                (static_cast<uint32_t>(std::to_integer<uint8_t>(data[offset + 3])) << 24);

            offset += 4;
            return value;
        }

        char read_byte()
        {
            if (offset >= data.size())
            {
                throw std::runtime_error("Unexpected end of data while reading a byte");
            }

            char c = static_cast<char>(std::to_integer<uint8_t>(data[offset]));
            
            offset++;
            
            return c;
        }

        std::string read_c_string()
        {
            std::string s;
            while (offset < data.size())
            {
                char c = read_byte();
                if (c == '\0')
                {
                    break;
                }
                s.push_back(c);
            }
            return s;
        }

        char peek_byte() const
        {
            if (offset >= data.size())
            {
                throw std::runtime_error("Unexpected end of data while peeking a byte");
            }

            return static_cast<char>(std::to_integer<uint8_t>(data[offset]));
        }

        bool eof() const
        {
            return offset >= data.size();
        }
    };
}