#include "fileio/vfs.h"
#include "fileio/file_info.h"

#include "string/string_ops.h"

#include "util/log.h"

#include <fstream>

namespace ppp
{
    namespace vfs
    {
		namespace internal
		{
			pool_hash_map<pool_string, pool_string> _wildcards;
		}

		//-------------------------------------------------------------------------
		void add_wildcard(const pool_string& wildcard, const pool_string& value)
		{
			log::info("adding wildcard {} | {}", wildcard, value);

			internal::_wildcards[wildcard] = value;
		}

		//-------------------------------------------------------------------------
		pool_string resolve_path(std::string_view filename)
		{
			file_info full_path(filename);

			for (const auto& p : internal::_wildcards)
			{
				if (full_path.str().find(p.first) != std::string::npos)
				{
					full_path = file_info(string::string_replace(full_path.str(), p.first, p.second));
				}
			}

			return full_path.str();
		}

		//-------------------------------------------------------------------------
		bool exists(std::string_view filename)
		{
			const pool_string path = resolve_path(filename);

			std::ifstream f(path.c_str());
			bool good = f.good();
			f.close();

			return good;
		}
    }
}