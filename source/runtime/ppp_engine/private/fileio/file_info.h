#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include "string/string_ops.h"

#include <string>

namespace ppp
{
    namespace vfs
    {
		namespace internal
		{
			//-------------------------------------------------------------------------
			inline char get_separator()
			{
				return '/';
			}

			//-------------------------------------------------------------------------
			template<typename TAlloc>
			std::basic_string<char, std::char_traits<char>, TAlloc> ensure_trailing_separator(std::string_view path)
			{
				std::basic_string<char, std::char_traits<char>, TAlloc> new_path = path;

				char separator = get_separator();

				if (!path.empty() && path.back() != separator)
				{
					return new_path + separator;
				}

				return new_path;
			}
		}

		struct parsed_path
		{
			parsed_path(std::string_view full_path)
			{
				char sep = internal::get_separator();

				auto last_sep = full_path.find_last_of(sep);
				auto dot_pos = full_path.find_last_of('.');

				assert(dot_pos != std::string::npos && "file_info_view must have a filename and an extension");

				path = (last_sep != std::string_view::npos) ? full_path.substr(0, last_sep) : "";
				filename = (last_sep != std::string_view::npos)
					? full_path.substr(last_sep + 1, dot_pos - last_sep - 1)
					: full_path.substr(0, dot_pos);
				extension = full_path.substr(dot_pos);
			}

			std::string_view path;
			std::string_view filename;
			std::string_view extension;
		};

		class file_info_view
		{
		public:
			file_info_view(std::string_view full_path);

			std::string_view path() const { return m_path; }
			std::string_view filename() const { return m_filename; }
			std::string_view extension() const { return m_extension; }

		private:
			void parse_full_path(std::string_view full_path);

		private:
			std::string_view m_path;
			std::string_view m_filename;
			std::string_view m_extension;
		};

		template<typename TAlloc = std::allocator<char>>
		class file_info
		{
		public:
			using string_type = std::basic_string<char, std::char_traits<char>, TAlloc>;

		public:
			//-------------------------------------------------------------------------
			static string_type make_full_path(std::string_view full_path)
			{
				parsed_path parsed_path(full_path);

				auto path	   = parsed_path.path;
				auto filename  = parsed_path.filename;
				auto extension = parsed_path.extension;

				string_type full_path = internal::ensure_trailing_separator(path) + filename + extension;

				return string::string_replace<string_type>(full_path, "\\", "/");
			}

		public:
			//-------------------------------------------------------------------------
			file_info(std::string_view full_path)
			{
				parse_full_path(full_path);
			}

			//-------------------------------------------------------------------------
			string_type full_path() const
			{
				string_type full_path = internal::ensure_trailing_separator(m_path) + m_filename + m_extension;

				return string::string_replace<string_type>(full_path, "\\", "/");
			}

		private:
			void parse_full_path(std::string_view full_path);

		private:
			string_type m_path;
			string_type m_filename;
			string_type m_extension;
		};
    }
}