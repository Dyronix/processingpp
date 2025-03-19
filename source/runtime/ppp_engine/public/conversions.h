#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <limits>
#include <cmath>

namespace ppp
{
    namespace conversions
    {
        // ------------------------------------------------------------------------
        // to_float
        // ------------------------------------------------------------------------

        // Converts a string to a float.
        inline float to_float(const std::string& str)
        {
            try
            {
                return std::stof(str);
            }
            catch (const std::exception&)
            {
                return std::numeric_limits<float>::quiet_NaN();
            }
        }

        // Overload for a vector of strings.
        inline std::vector<float> to_float(const std::vector<std::string>& vec)
        {
            std::vector<float> result;
            result.reserve(vec.size());
            for (const auto& s : vec)
            {
                result.push_back(to_float(s));
            }
            return result;
        }

        // If already a float, return it.
        inline float to_float(float value)
        {
            return value;
        }

        // ------------------------------------------------------------------------
        // to_int
        // ------------------------------------------------------------------------

        // Converts a string to an int using the specified radix.
        inline int to_int(const std::string& n, int radix = 10)
        {
            if (n == "Infinity")
            {
                return std::numeric_limits<int>::max();
            }
            if (n == "-Infinity")
            {
                return std::numeric_limits<int>::min();
            }
            try
            {
                size_t pos = 0;
                int result = std::stoi(n, &pos, radix);
                return result;
            }
            catch (const std::exception&)
            {
                throw std::invalid_argument("Invalid argument for to_int");
            }
        }

        // Converts a float to an int (truncates decimals).
        inline int to_int(float n, int radix = 10)
        {
            if (std::isinf(n))
            {
                if (n > 0)
                {
                    return std::numeric_limits<int>::max();
                }
                else
                {
                    return std::numeric_limits<int>::min();
                }
            }
            return static_cast<int>(n);
        }

        // Converts a bool to an int.
        inline int to_int(bool n, int radix = 10)
        {
            if (n)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

        // Overload for a vector.
        template <typename T>
        inline std::vector<int> to_int(const std::vector<T>& vec, int radix = 10)
        {
            std::vector<int> result;
            result.reserve(vec.size());
            for (const auto& x : vec)
            {
                result.push_back(to_int(x, radix));
            }
            return result;
        }

        // ------------------------------------------------------------------------
        // to_str
        // ------------------------------------------------------------------------

        // Converts a string to a string (identity).
        inline std::string to_str(const std::string& n)
        {
            return n;
        }

        // Converts an int to a string.
        inline std::string to_str(int n)
        {
            return std::to_string(n);
        }

        // Converts a float to a string.
        inline std::string to_str(float n)
        {
            return std::to_string(n);
        }

        // Converts a bool to a string.
        inline std::string to_str(bool n)
        {
            if (n)
            {
                return "true";
            }
            else
            {
                return "false";
            }
        }

        // Overload for a vector.
        template <typename T>
        inline std::vector<std::string> to_str(const std::vector<T>& vec)
        {
            std::vector<std::string> result;
            result.reserve(vec.size());
            for (const auto& x : vec)
            {
                result.push_back(to_str(x));
            }
            return result;
        }

        // ------------------------------------------------------------------------
        // to_boolean
        // ------------------------------------------------------------------------

        // Converts a string to a boolean.
        inline bool to_boolean(const std::string& n)
        {
            std::string lower;
            lower.resize(n.size());
            std::transform(n.begin(), n.end(), lower.begin(), ::tolower);
            return (lower == "true");
        }

        // Converts an int to a boolean.
        inline bool to_boolean(int n)
        {
            return (n != 0);
        }

        // Converts a float to a boolean.
        inline bool to_boolean(float n)
        {
            return (n != 0.0f);
        }

        // Converts a bool to a boolean.
        inline bool to_boolean(bool n)
        {
            return n;
        }

        // Overload for a vector.
        template <typename T>
        inline std::vector<bool> to_boolean(const std::vector<T>& vec)
        {
            std::vector<bool> result;
            result.reserve(vec.size());
            for (const auto& x : vec)
            {
                result.push_back(to_boolean(x));
            }
            return result;
        }

        // ------------------------------------------------------------------------
        // to_byte
        // ------------------------------------------------------------------------

        // Converts a string to its byte value.
        inline int to_byte(const std::string& n)
        {
            int nn = to_int(n, 10);
            return ((nn + 128) % 256) - 128;
        }

        // Converts an int to its byte value.
        inline int to_byte(int n)
        {
            return ((n + 128) % 256) - 128;
        }

        // Converts a float to its byte value.
        inline int to_byte(float n)
        {
            int nn = to_int(n, 10);
            return ((nn + 128) % 256) - 128;
        }

        // Converts a bool to its byte value.
        inline int to_byte(bool n)
        {
            int nn = to_int(n, 10);
            return ((nn + 128) % 256) - 128;
        }

        // Overload for a vector.
        template <typename T>
        inline std::vector<int> to_byte(const std::vector<T>& vec)
        {
            std::vector<int> result;
            result.reserve(vec.size());
            for (const auto& x : vec)
            {
                result.push_back(to_byte(x));
            }
            return result;
        }

        // ------------------------------------------------------------------------
        // to_char
        // ------------------------------------------------------------------------

        // Converts an int to a single-character string.
        inline std::string to_char(int n)
        {
            return std::string(1, static_cast<char>(n));
        }

        // Converts a float to a single-character string.
        inline std::string to_char(float n)
        {
            int int_val = to_int(n, 10);
            return to_char(int_val);
        }

        // Converts a string to a single-character string by converting it to an int first.
        inline std::string to_char(const std::string& n)
        {
            int int_val = to_int(n, 10);
            return to_char(int_val);
        }

        // Overload for a vector.
        template <typename T>
        inline std::vector<std::string> to_char(const std::vector<T>& vec)
        {
            std::vector<std::string> result;
            result.reserve(vec.size());
            for (const auto& x : vec)
            {
                result.push_back(to_char(x));
            }
            return result;
        }

        // ------------------------------------------------------------------------
        // unchar
        // ------------------------------------------------------------------------

        // Converts a single-character string to its integer (char code) value.
        inline int unchar(const std::string& n)
        {
            if (n.size() == 1)
            {
                return static_cast<int>(n[0]);
            }
            else
            {
                throw std::invalid_argument("unchar requires a single character string");
            }
        }

        // Overload for a vector.
        template <typename T>
        inline std::vector<int> unchar(const std::vector<T>& vec)
        {
            std::vector<int> result;
            result.reserve(vec.size());
            for (const auto& x : vec)
            {
                result.push_back(unchar(x));
            }
            return result;
        }

        // ------------------------------------------------------------------------
        // hex_val
        // ------------------------------------------------------------------------

        // Converts an int to a hexadecimal string with the specified number of digits.
        inline std::string hex_val(int n, int digits = 8)
        {
            if (n < 0)
            {
                n = 0xffffffff + n + 1;
            }
            std::stringstream ss;
            ss << std::uppercase << std::hex << n;
            std::string hex = ss.str();
            while (static_cast<int>(hex.size()) < digits)
            {
                hex = "0" + hex;
            }
            if (static_cast<int>(hex.size()) > digits)
            {
                hex = hex.substr(hex.size() - digits, digits);
            }
            return hex;
        }

        // Converts a float to a hexadecimal string.
        inline std::string hex_val(float n, int digits = 8)
        {
            int int_val = to_int(n, 10);
            return hex_val(int_val, digits);
        }

        // Overload for a vector.
        template <typename T>
        inline std::vector<std::string> hex_val(const std::vector<T>& vec, int digits = 8)
        {
            std::vector<std::string> result;
            result.reserve(vec.size());
            for (const auto& x : vec)
            {
                result.push_back(hex_val(x, digits));
            }
            return result;
        }

        // Converts a double to a hexadecimal string.
        // If the number is infinite, returns a string of repeated 'F' (or '0') digits.
        inline std::string hex_val(double n, int digits = 8)
        {
            if (std::isinf(n))
            {
                if (n > 0)
                {
                    return std::string(digits, 'F');
                }
                else
                {
                    return std::string(digits, '0');
                }
            }
            int int_val = to_int(n, 10);
            return hex_val(int_val, digits);
        }

        // ------------------------------------------------------------------------
        // unhex
        // ------------------------------------------------------------------------

        // Converts a hexadecimal string to an int.
        inline int unhex(const std::string& n)
        {
            try
            {
                return std::stoi(n, nullptr, 16);
            }
            catch (const std::exception&)
            {
                throw std::invalid_argument("Invalid hex string");
            }
        }

        // Overload for a vector.
        template <typename T>
        inline std::vector<int> unhex(const std::vector<T>& vec)
        {
            std::vector<int> result;
            result.reserve(vec.size());
            for (const auto& x : vec)
            {
                result.push_back(unhex(x));
            }
            return result;
        }
    }
}