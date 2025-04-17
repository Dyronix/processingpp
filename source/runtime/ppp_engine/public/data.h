/**
 * @file data.h
 * @brief Utility functions for vector manipulation.
 */
#pragma once

#include <vector>
#include <algorithm>
#include <random>

namespace ppp
{
    /**
     * @brief Append a value to the end of a vector.
     * @tparam T Type of elements.
     * @param array Vector to modify.
     * @param value Value to append.
     * @return Reference to the modified vector.
     */
    template <typename T>
    std::vector<T>& append(std::vector<T>& array, const T& value)
    {
        array.push_back(value);
        return array;
    }

    /**
     * @brief Copy a subrange from source vector into destination vector, replacing elements.
     * @tparam T Element type.
     * @param src Source vector.
     * @param src_position Starting index in source.
     * @param dst Destination vector (modified in place).
     * @param dst_position Starting index in destination to replace.
     * @param length Number of elements to copy.
     * @throws std::out_of_range if positions are invalid.
     */
    template <typename T>
    void array_copy(const std::vector<T>& src, int src_position, std::vector<T>& dst, int dst_position, int length)
    {
        if (src_position < 0 || src_position >= static_cast<int>(src.size()))
        {
            throw std::out_of_range("src_position out of range");
        }
        if (dst_position < 0 || dst_position > static_cast<int>(dst.size()))
        {
            throw std::out_of_range("dst_position out of range");
        }
        int available = static_cast<int>(src.size()) - src_position;
        int copy_length = std::min(length, available);

        int erase_end = dst_position + copy_length;
        if (erase_end > static_cast<int>(dst.size()))
        {
            erase_end = static_cast<int>(dst.size());
        }
        dst.erase(dst.begin() + dst_position, dst.begin() + erase_end);

        dst.insert(dst.begin() + dst_position, src.begin() + src_position, src.begin() + src_position + copy_length);
    }

    /**
     * @brief Replace entire destination vector with source vector.
     * @tparam T Element type.
     * @param src Source vector.
     * @param dst Destination vector to replace.
     */
    template <typename T>
    void array_copy(const std::vector<T>& src, std::vector<T>& dst)
    {
        dst.clear();
        dst.insert(dst.begin(), src.begin(), src.end());
    }

    /**
     * @brief Concatenate two vectors into a new one.
     * @tparam T Element type.
     * @param list0 First vector.
     * @param list1 Second vector.
     * @return New vector containing elements of list0 followed by list1.
     */
    template <typename T>
    std::vector<T> concat(const std::vector<T>& list0, const std::vector<T>& list1)
    {
        std::vector<T> result(list0);
        result.insert(result.end(), list1.begin(), list1.end());
        return result;
    }

    /**
     * @brief Reverse the order of elements in place.
     * @tparam T Element type.
     * @param list Vector to reverse.
     * @return Reference to the reversed vector.
     */
    template <typename T>
    std::vector<T>& reverse(std::vector<T>& list)
    {
        std::reverse(list.begin(), list.end());
        return list;
    }

    /**
     * @brief Remove the last element of the vector, if any.
     * @tparam T Element type.
     * @param list Vector to shorten.
     * @return Reference to the modified vector.
     */
    template <typename T>
    std::vector<T>& shorten(std::vector<T>& list)
    {
        if (!list.empty())
        {
            list.pop_back();
        }
        return list;
    }

    /**
     * @brief Shuffle elements of a vector.
     * @tparam T Element type.
     * @param arr Vector to shuffle.
     * @param modify If false, returns shuffled copy; if true, shuffles in place.
     * @return Shuffled vector or reference to arr.
     */
    template <typename T>
    std::vector<T> shuffle(std::vector<T>& arr, bool modify = false)
    {
        if (!modify)
        {
            std::vector<T> new_arr(arr);
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(new_arr.begin(), new_arr.end(), g);
            return new_arr;
        }
        else
        {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(arr.begin(), arr.end(), g);
            return arr;
        }
    }

    /**
     * @brief Sort elements of a vector, optionally only first count elements.
     * @tparam T Element type.
     * @param list Input vector.
     * @param count Number of initial elements to sort (negative for all).
     * @return New vector with sorted elements.
     */
    template <typename T>
    std::vector<T> sort_vector(const std::vector<T>& list, int count = -1)
    {
        std::vector<T> result;
        if (count < 0)
        {
            result = list;
            std::sort(result.begin(), result.end());
        }
        else
        {
            int sort_count = std::min(count, static_cast<int>(list.size()));
            std::vector<T> part(list.begin(), list.begin() + sort_count);
            std::sort(part.begin(), part.end());
            result = part;
            if (static_cast<int>(list.size()) > sort_count)
            {
                result.insert(result.end(), list.begin() + sort_count, list.end());
            }
        }
        return result;
    }

    /**
     * @brief Insert values into a vector at a specific index.
     * @tparam T Element type.
     * @param list Vector to modify.
     * @param values Values to insert.
     * @param index Position at which to insert.
     * @throws std::out_of_range if index is invalid.
     * @return Reference to the modified vector.
     */
    template <typename T>
    std::vector<T>& splice(std::vector<T>& list, const std::vector<T>& values, int index)
    {
        if (index < 0 || index > static_cast<int>(list.size()))
        {
            throw std::out_of_range("index out of range");
        }
        list.insert(list.begin() + index, values.begin(), values.end());
        return list;
    }

    /**
     * @brief Insert a single value into a vector at a specific index.
     * @tparam T Element type.
     * @param list Vector to modify.
     * @param value Value to insert.
     * @param index Position at which to insert.
     * @throws std::out_of_range if index is invalid.
     * @return Reference to the modified vector.
     */
    template <typename T>
    std::vector<T>& splice(std::vector<T>& list, const T& value, int index)
    {
        if (index < 0 || index > static_cast<int>(list.size()))
        {
            throw std::out_of_range("index out of range");
        }
        list.insert(list.begin() + index, value);
        return list;
    }

    /**
     * @brief Get a subset of a vector.
     * @tparam T Element type.
     * @param list Input vector.
     * @param start Starting index of subset.
     * @param count Number of elements to include (negative for all to end).
     * @return New vector containing the subset.
     * @throws std::out_of_range if range is invalid.
     */
    template <typename T>
    std::vector<T> subset(const std::vector<T>& list, int start, int count = -1)
    {
        if (start < 0 || start > static_cast<int>(list.size()))
        {
            throw std::out_of_range("start index out of range");
        }
        if (count < 0)
        {
            return std::vector<T>(list.begin() + start, list.end());
        }
        else
        {
            if (start + count > static_cast<int>(list.size()))
            {
                throw std::out_of_range("subset range out of range");
            }
            return std::vector<T>(list.begin() + start, list.begin() + start + count);
        }
    }
}