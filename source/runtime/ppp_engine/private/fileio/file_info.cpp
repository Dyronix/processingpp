#include "fileio/file_info.h"

#include "util/string_ops.h"

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
			pool_string ensure_trailing_separator(const pool_string& path)
			{
				char separator = get_separator();

				if (!path.empty() && path.back() != separator)
				{
					return path + separator;
				}

				return path;
			}
		}

		//-------------------------------------------------------------------------
		file_info::file_info(std::string_view full_path)
		{
			u64 dot_pos = full_path.find_last_of('.');

			assert(dot_pos != std::string::npos && "file_info must have a filename and an extension");

			_path = full_path.substr(0, full_path.find_last_of(internal::get_separator())); // Extract the path
			_filename = full_path.substr(full_path.find_last_of(internal::get_separator()) + 1, dot_pos - (full_path.find_last_of(internal::get_separator()) + 1)); // Extract filename without extension
			_extension = full_path.substr(dot_pos); // Extract extension

			_path = internal::ensure_trailing_separator(_path);
		}

		//-------------------------------------------------------------------------
		void file_info::append(std::string_view component)
		{
			_path = internal::ensure_trailing_separator(_path);
			_path += component;
		}

		//-------------------------------------------------------------------------
		pool_string file_info::str() const
		{
			pool_string full_path = internal::ensure_trailing_separator(_path) + _filename + _extension;;

			return string::string_replace(full_path, pool_string("\\"), pool_string("/"));
		}
    }
}