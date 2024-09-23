#pragma once

#include <vector>

namespace ppp
{
    namespace wave_function_collapse
    {
        class Cell
        {
        public:
            Cell();
            Cell(int num_options);
            Cell(const std::vector<int>& options);

            void log() const;

            void mark_collapsed();
            void clear_collapsed();

            int pick_random_option();

        public:
            bool collapsed() const { return m_collapsed; }
            const std::vector<int>& options() const { return m_options; }

        private:
            bool m_collapsed;
            std::vector<int> m_options;
        };
    }
}