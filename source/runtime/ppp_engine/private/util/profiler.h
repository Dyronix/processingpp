#pragma once

#include "util/types.h"

#include <deque>
#include <string>
#include <unordered_map>
#include <chrono>
#include <vector>

#define PPP_PROFILE_FUNCTION() ppp::profiler::profiler_section s_sect(__FUNCTION__)
#define PPP_PROFILE_SECTION(id) ppp::profiler::profiler_section s_sect(id)

namespace ppp
{
    namespace profiler
    {
		using time_t = std::chrono::time_point<std::chrono::steady_clock>;
		using span_t = std::chrono::nanoseconds;

		//-------------------------------------------------------------------------
		// Profile Entry
		struct entry
		{
			time_t start{};
			time_t end{};
			span_t accum{};
			s32 count = 0;
			f32 avg = 0.0f;
			std::deque<f32> history;
		};

		//-------------------------------------------------------------------------
		// Profiler Context
		struct profiler_context
		{
			std::unordered_map<std::string, entry> times{};

			time_t timer {};
		};

		profiler_context& ctx();

		//-------------------------------------------------------------------------
		// Profiler Section
		class profiler_section
		{
		public:
			profiler_section(const std::string& name);
			~profiler_section();
		private:
			std::string m_name;
		};

		void begin_section(const std::string& name);
		void end_section(const std::string& name);
    }
}