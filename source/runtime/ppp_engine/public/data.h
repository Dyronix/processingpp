#pragma once

#include <vector>
#include <algorithm>

#include <assert.h>

namespace ppp
{
    namespace data
    {
        template<typename T>
        void append(std::vector<T>& target, const std::vector<T>& append)
        {
            target.insert(target.end(), append.begin(), begin.end());
        }

        template<typename T>
        std::vector<T> append_copy(const std::vector<T>& target, const std::vector<T>& append)
        {
            std::vector<T> new_target = target;
            new_target.insert(new_target.end(), append.begin(), append.end());
            return new_target;
        }

        template<typename T>
        void reverse(std::vector<T>& target)
        {
            std::reverse(target.begin(), target.end());
        }

        template<typename T>
        void reverse_copy(const std::vector<T>& target)
        {
            std::vector<T> new_target = target;
            std::reverse(new_target.begin(), new_target.end());
            return new_target;
        }

        template<typename T>
        void shorten(std::vector<T>& target, int s)
        {
            if (s > target.size())
            {
                return;
            }

            target.resize(s);
        }

        template<typename T>
        void shorten_copy(const std::vector<T>& target, int s)
        {
            if (s > target.size())
            {
                return;
            }

            std::vector<T> new_target = target;
            new_target.resize(s);
            return new_target;
        }

        template<typename T>
        void shuffle(std::vector<T>& target)
        {
            std::shuffle(target.begin(), target.end());
        }

        template<typename T>
        void shuffle_copy(const std::vector<T>& target)
        {
            std::vector<T> new_target = target;
            std::shuffle(new_target.begin(), new_target.end());
            return new_target;
        }

        template<typename T>
        void sort(std::vector<T>& target)
        {
            std::sort(target.begin(), target.end());
        }

        template<typename T>
        void sort_copy(const std::vector<T>& target)
        {
            std::vector<T> new_target = target;
            std::sort(new_target.begin(), new_target.end());
            return new_target;
        }

        template<typename T>
        void insert(std::vector<T>& target, int idx, const std::vector<T>& insert)
        {
            auto it = target.begin();
            std::advance(it, idx);

            target.insert(it, insert.begin(), insert.end());
        }

        template<typename T>
        void insert_copy(const std::vector<T>& target, int idx, const std::vector<T>& insert)
        {
            std::vector<T> new_target = target;
            auto it = new_target.begin();
            std::advance(it, idx);

            new_target.insert(it, insert.begin(), insert.end());
            return new_target;
        }

        template<typename T>
        std::vector<T> subset(const std::vector<T>& target, int start, int end)
        {
            assert(end >= start);

            std::vector<T> sub;
            sub.reserve(end - start);

            auto it_start = target.begin();
            std::advance(it_start, start);

            auto it_end = target.begin();
            std::advance(it_end, start);
            std::advance(it_end, std::clamp(end, 0, target.size() - start));

            sub.insert(sub.end(), it_start, it_end);

            return sub;
        }
    }
}