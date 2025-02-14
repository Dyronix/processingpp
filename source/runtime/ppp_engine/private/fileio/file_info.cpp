#include "fileio/file_info.h"

namespace ppp
{
    namespace vfs
    {
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
		template<typename TAlloc>
		void file_info<TAlloc>::parse_full_path(std::string_view full_path)
		{
			parsed_path parsed_path(full_path);

			m_path		= parsed_path.path;
			m_filename	= parsed_path.filename;
			m_extension = parsed_path.extension;
		}
    }
}