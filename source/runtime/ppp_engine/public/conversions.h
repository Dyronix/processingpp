/**
 * @file conversions.h
 * @brief String and numeric type conversion utilities.
 */
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <limits>
#include <cmath>

namespace ppp
{
    /**
     * @brief Convert string to float.
     * @param str Input string.
     * @return Floating-point value or NaN on error.
     */
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

    /**
     * @brief Convert vector of strings to floats.
     * @param vec Vector of strings.
     * @return Vector of floats.
     */
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

    /** @brief Identity: return float as is. */
    inline float to_float(float value)
    {
        return value;
    }

    /**
     * @brief Convert string to int with given radix.
     * @param n Input numeric string.
     * @param radix Numeric base (default 10).
     * @return Integer value or throws on invalid.
     */
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

    /** @brief Convert C-string to int with given radix. */
    inline int to_int(const char* n, int radix = 10)
    {
        return to_int(std::string(n), radix);
    }

    /** @brief Convert float to int by truncation. */
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

    /** @brief Convert double to int by truncation. */
    inline int to_int(double n, int radix = 10)
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

    /** @brief Convert bool to int (true=1,false=0). */
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

    /**
     * @brief Convert vector of values to ints.
     * @tparam T Element type convertible to int.
     * @param vec Vector of values.
     * @param radix Numeric base (default 10).
     * @return Vector of ints.
     */
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

    /** @brief Identity: return string as is. */
    inline std::string to_str(const std::string& n) { return n; }
    /** @brief Convert int to string. */
    inline std::string to_str(int n) { return std::to_string(n); }
    /** @brief Convert float to string. */
    inline std::string to_str(float n) { return std::to_string(n); }
    /** @brief Convert bool to string ("true"/"false"). */
    inline std::string to_str(bool n) { return n ? "true" : "false"; }

    /**
     * @brief Convert vector of values to strings.
     * @tparam T Element type convertible to string.
     * @param vec Vector of values.
     * @return Vector of strings.
     */
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

    /**
     * @brief Convert string to boolean.
     * @param n Input string (case-insensitive "true").
     * @return true if n == "true" ignoring case.
     */
    inline bool to_boolean(const std::string& n)
    {
        std::string lower;
        lower.resize(n.size());
        std::transform(n.begin(), n.end(), lower.begin(), ::tolower);
        return (lower == "true");
    }

    /** @brief Convert char to boolean. */
    inline bool to_boolean(const char* n) { return to_boolean(std::string(n)); }
    /** @brief Convert int to boolean. */
    inline bool to_boolean(int n) { return n != 0; }
    /** @brief Convert float to boolean. */
    inline bool to_boolean(float n) { return n != 0.0f; }

    /**
     * @brief Convert vector of values to booleans.
     * @tparam T Element type convertible to boolean.
     * @param vec Vector of values.
     * @return Vector of booleans.
     */
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

    /**
     * @brief Convert string to byte value (-128..127).
     * @param n Input string.
     * @return Byte value as int.
     */
    inline int to_byte(const std::string& n)
    {
        int nn = to_int(n, 10);
        return ((nn + 128) % 256) - 128;
    }

    /** @brief Converts a string to its byte value. */
    inline int to_byte(const char* n)
    {
        int nn = to_int(n);
        return ((nn + 128) % 256) - 128;
    }

    /** @brief Converts an int to its byte value. */
    inline int to_byte(int n)
    {
        return ((n + 128) % 256) - 128;
    }

    /** @brief Converts a float to its byte value. */
    inline int to_byte(float n)
    {
        int nn = to_int(n, 10);
        return ((nn + 128) % 256) - 128;
    }

    /** @brief Converts a bool to its byte value. */
    inline int to_byte(bool n)
    {
        int nn = to_int(n, 10);
        return ((nn + 128) % 256) - 128;
    }

    /**
     * @brief Convert vector of values to bytes.
     * @tparam T Element type convertible to byte.
     * @param vec Vector of values.
     * @return Vector of bytes.
     */
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

    /**
     * @brief Convert int to single-character string.
     * @param n Numeric code.
     * @return String of length 1.
     */
    inline std::string to_char(int n)
    {
        return std::string(1, static_cast<char>(n));
    }

    /** @brief Converts a float to a single-character string. */
    inline std::string to_char(float n)
    {
        int int_val = to_int(n, 10);
        return to_char(int_val);
    }

    /** @brief Converts a string to a single-character string by converting it to an int first. */
    inline std::string to_char(const std::string& n)
    {
        int int_val = to_int(n, 10);
        return to_char(int_val);
    }

    /**
    * @brief Convert vector of values to strings.
    * @tparam T Element type convertible to string.
    * @param vec Vector of values.
    * @return Vector of strings.
    */
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

    /**
     * @brief Convert single-character string to its integer code.
     * @param n Input string of length 1.
     * @return Integer code of character.
     * @throws std::invalid_argument if length != 1.
     */
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

    /**
    * @brief Convert vector of values to ints.
    * @tparam T Element type convertible to int.
    * @param vec Vector of values.
    * @return Vector of ints.
    */
    template <typename T>
    std::vector<int> unchar(const std::vector<T>& vec)
    {
        std::vector<int> result;
        result.reserve(vec.size());
        for (const auto& x : vec)
        {
            result.push_back(unchar(x));
        }
        return result;
    }

    /**
     * @brief Convert integer to hexadecimal string.
     * @param n Integer value.
     * @param digits Minimum number of hex digits.
     * @return Uppercase hex string.
     */
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

    /**
     * @brief Convert integer to hexadecimal string.
     * @param n Float value.
     * @param digits Minimum number of hex digits.
     * @return Uppercase hex string.
     */
    inline std::string hex_val(float n, int digits = 8)
    {
        int int_val = to_int(n, 10);
        return hex_val(int_val, digits);
    }

    /**
    * @brief Convert vector of values to hexdecimal strings.
    * @tparam T Element type convertible to hexdecimal string.
    * @param vec Vector of values.
    * @return Vector of hexdecimal strings.
    */
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

    /**
     * @brief Convert double to hexadecimal string.
     * @param n Double value.
     * @param digits Minimum number of hex digits.
     * @return Uppercase hex string.
     */
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

    /**
     * @brief Convert hexadecimal string to integer.
     * @param n Hex string (uppercase or lowercase).
     * @return Integer value.
     * @throws std::invalid_argument if invalid hex.
     */
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

    /**
     * @brief Convert vector of values to ints.
     * @tparam T Element type convertible to int.
     * @param vec Vector of values.
     * @return Vector of ints.
     */
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