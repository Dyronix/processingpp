#pragma once

#define FMT_HEADER_ONLY
#include <fmt/core.h>

#include <string>

namespace ppp
{
	namespace log
	{
		enum class log_level
		{
			INFO = 0,
			WARN,
			ERROR,
			CRITICAL,
		};

		namespace internal
		{
			void trace(log_level lvl, const char* name, const char* color, std::string_view txt);
		}

		constexpr auto green = "\033[32m";
		constexpr auto magenta = "\033[35m";
		constexpr auto red = "\033[31m";
		constexpr auto yellow = "\033[33m";
		constexpr auto reset = "\033[0m";

		void initialize();
		void shutdown();

		void set_log_level(log_level level);
		void set_file_output_enabled(std::string_view filepath);

		void enable_logging();
		void disable_logging();

		bool is_logging_enabled();
		bool is_log_level_enabled(log_level level);

		template<typename FormatString, typename... Args>
		void info(const FormatString& format, const Args&... args)
		{
			//if (!is_logging_enabled() || !is_log_level_enabled(log_level::INFO))
			//{
			//	return;
			//}

			//printf("[%sinfo%s] ", green, reset);
			//fmt::print(fmt::runtime(format), args...);
			//printf("\n");

			auto msg = fmt::format(fmt::runtime(format), args...);
			internal::trace(log_level::INFO, "info", green, msg);
		}

		template<typename FormatString, typename... Args>
		void warn(const FormatString& format, const Args&... args)
		{
			//if (!is_logging_enabled() || !is_log_level_enabled(log_level::WARN))
			//{
			//	return;
			//}

			//printf("[%swarn%s] ", magenta, reset);
			//fmt::print(fmt::runtime(format), args...);
			//printf("\n");

			auto msg = fmt::format(fmt::runtime(format), args...);
			internal::trace(log_level::WARN, "warn", magenta, msg);
		}

		template<typename FormatString, typename... Args>
		void error(const FormatString& format, const Args&... args)
		{	
			//if (!is_logging_enabled() || !is_log_level_enabled(log_level::ERROR))
			//{
			//	return;
			//}

			//printf("[%serror%s] ", red, reset);
			//fmt::print(fmt::runtime(format), args...);
			//printf("\n");

			auto msg = fmt::format(fmt::runtime(format), args...);
			internal::trace(log_level::ERROR, "error", red, msg);
		}

		template<typename FormatString, typename... Args>
		void critical(const FormatString& format, const Args&... args)
		{
			//if (!is_logging_enabled() || !is_log_level_enabled(log_level::CRITICAL))
			//{
			//	return;
			//}

			//printf("[%serror%s] ", red, reset);
			//fmt::print(fmt::runtime(format), args...);
			//printf("\n");

			auto msg = fmt::format(fmt::runtime(format), args...);
			internal::trace(log_level::CRITICAL, "critical", red, msg);
		}
	}
}
