#include "cell.h"
#include "environment.h"

#include <sstream>

namespace ppp
{
    namespace wave_function_collapse
    {
        Cell::Cell()
            :m_options()
            ,m_collapsed(false)
        {}

        Cell::Cell(int num_options)
            :m_collapsed(false)
        {
            m_options.reserve(num_options);
            for (int i = 0; i < num_options; ++i)
            {
                m_options.push_back(i);
            }
        }

        Cell::Cell(const std::vector<int>& options)
            :m_collapsed(false)
            ,m_options(options)
        {}

        void Cell::log() const
        {
            std::stringstream sstream;
            
            sstream << "options: ";

            for (int i = 0; i < m_options.size() - 1; ++i)
            {
                sstream << m_options[i] << ", ";
            }

            sstream << m_options[m_options.size() - 1];
            sstream << " collpased: " << m_collapsed;

            environment::print(sstream.str());
        }

        void Cell::mark_collapsed()
        {
            m_collapsed = true;
        }

        void Cell::clear_collapsed()
        {
            m_collapsed = false;
        }

        int Cell::pick_random_option()
        {
            if (m_options.size() == 0)
            {
                return -1;
            }

            int pick = m_options[rand() % m_options.size()];

            m_options = { pick };

            return pick;
        }
    }
}