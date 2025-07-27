#include "fileio/vfs.h"

#include "util/log.h"

#include <direct.h>
#include <io.h>
#include <cerrno>
#include <cstring>

namespace ppp
{
    namespace vfs
    {
		//-------------------------------------------------------------------------
		bool exists_directory(std::string_view directory)
		{
			return (_access(directory.data(), 0) == 0);
		}

		//-------------------------------------------------------------------------
		bool create_directory(std::string_view directory)
		{
			std::string full_path(directory);
			for (char& c : full_path)
			{
				if (c == '\\') c = '/';
			}

			size_t pos = 0;
			while ((pos = full_path.find('/', pos)) != std::string::npos)
			{
				std::string subdir = full_path.substr(0, pos++);
				if (subdir.empty() || exists_directory(subdir))
				{
					continue;
				}

				if (_mkdir(subdir.c_str()) != 0)
				{
					log::error("Failed to create directory '{}': {}", subdir, std::strerror(errno));
					return false;
				}
			}

			if (!exists_directory(full_path))
			{
				if (_mkdir(full_path.c_str()) != 0)
				{
					log::error("Failed to create final directory '{}': {}", full_path, std::strerror(errno));
					return false;
				}
			}

			return true;
		}
    }
}