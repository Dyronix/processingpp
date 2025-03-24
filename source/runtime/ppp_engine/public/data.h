#pragma once

#include <vector>
#include <algorithm>

#include <assert.h>

namespace ppp
{
    // Appends a value to the end of the vector.
    template <typename T>
    std::vector<T>& append(std::vector<T>& array, const T& value)
    {
        array.push_back(value);
        return array;
    }

    // Copies a subrange from the source vector into the destination vector.
    // This version takes five parameters: src, src_position, dst, dst_position, length.
    // It removes 'length' elements from dst (starting at dst_position) and replaces them
    // with the elements taken from src (starting at src_position).
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

    // Simplified version: copies entire src into dst.
    // Equivalent to array_copy(src, 0, dst, 0, src.size())
    template <typename T>
    void array_copy(const std::vector<T>& src, std::vector<T>& dst)
    {
        dst.clear();
        dst.insert(dst.begin(), src.begin(), src.end());
    }

    // Concatenates two vectors and returns the result.
    template <typename T>
    std::vector<T> concat(const std::vector<T>& list0, const std::vector<T>& list1)
    {
        std::vector<T> result(list0);
        result.insert(result.end(), list1.begin(), list1.end());
        return result;
    }

    // Reverses the order of the vector in place and returns it.
    template <typename T>
    std::vector<T>& reverse(std::vector<T>& list)
    {
        std::reverse(list.begin(), list.end());
        return list;
    }

    // Removes the last element of the vector (if any) and returns the vector.
    template <typename T>
    std::vector<T>& shorten(std::vector<T>& list)
    {
        if (!list.empty())
        {
            list.pop_back();
        }
        return list;
    }

    // Shuffles the elements of the vector.
    // If modify is false then a shuffled copy is returned (leaving the original unchanged);
    // if modify is true then the original vector is shuffled in place and returned.
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

    // Sorts the vector.
    // If count is not provided (or negative) the entire vector is sorted;
    // if count is provided, only the first 'count' elements are sorted and then
    // concatenated with the rest of the original vector.
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

    // Inserts a vector of values into an existing vector at the given index.
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

    // Overload for inserting a single value into the vector.
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

    // Returns a subset of the vector, starting at 'start'.
    // If count is provided (non-negative) then count elements are returned;
    // otherwise, all elements from start to the end are returned.
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