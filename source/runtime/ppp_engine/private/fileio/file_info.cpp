#include "fileio/file_info.h"

namespace ppp
{
    namespace vfs
    {
		namespace internal
		{
			//-------------------------------------------------------------------------
			char get_separator()
			{
				return '/';
			}

			//-------------------------------------------------------------------------
			std::string ensure_trailing_separator(std::string_view path)
			{
				std::string new_path(path.data(), path.size());

				char separator = get_separator();

				if (!path.empty() && path.back() != separator)
				{
					return new_path + separator;
				}

				return new_path;
			}
		}

		//-------------------------------------------------------------------------
		// FILE INFO VIEW
		file_info_view::file_info_view(std::string_view full_path)
		{
			parse_full_path(full_path);
		}

		//-------------------------------------------------------------------------
		void file_info_view::parse_full_path(std::string_view full_path)
		{
			parsed_path parsed_path(full_path);

			m_path		= parsed_path.path;
			m_filename	= parsed_path.filename;
			m_extension = parsed_path.extension;
		}

		//-------------------------------------------------------------------------
		// FILE INFO
		void file_info::parse_full_path(std::string_view full_path)
		{
			parsed_path parsed_path(full_path);

			m_path		= parsed_path.path;
			m_filename	= parsed_path.filename;
			m_extension = parsed_path.extension;
		}
    }
}