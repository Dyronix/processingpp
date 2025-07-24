#include "util/profiler.h"

namespace ppp
{
    namespace profiler
    {
        //-------------------------------------------------------------------------
        // Profiler Context
        profiler_context& ctx()
        {
            static profiler_context s_ctx;

            return s_ctx;
        }

        //-------------------------------------------------------------------------
        // Profiler Section
        profiler_section::profiler_section(const std::string& name) : m_name(name)
        {
            begin_section(m_name);
        }

        //-------------------------------------------------------------------------
        profiler_section::~profiler_section()
        {
            end_section(m_name);
        }

        //-------------------------------------------------------------------------
        void begin_section(const std::string& name)
        {
            ctx().times[name].start = std::chrono::high_resolution_clock::now();
        }

        //-------------------------------------------------------------------------
        void end_section(const std::string& name)
        {
            auto& e = ctx().times[name];
            e.end = std::chrono::high_resolution_clock::now();
            auto elapsed = e.end - e.start;
            e.accum += elapsed;
            e.count++;
        }
    }
}


