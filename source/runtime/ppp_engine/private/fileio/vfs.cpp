#include "fileio/vfs.h"
#include "fileio/file_info.h"

#include "string/string_ops.h"

#include "util/log.h"

#include <fstream>

namespace ppp
{
    namespace vfs
    {
		struct context
		{
			std::unordered_map<string::string_id, std::string> wildcards;
		} g_ctx;

		//-------------------------------------------------------------------------
		void add_wildcard(string::string_id wildcard, const std::string_view value)
		{
			log::info("Adding wildcard: [{} | {}]", string::restore_sid(wildcard), value);

			g_ctx.wildcards[wildcard] = value;
		}

		//-------------------------------------------------------------------------
		std::string resolve_path(std::string_view filename)
		{
			std::string	resolved_path;

			// No wildcards available return empty string
			if (g_ctx.wildcards.empty())
			{
				log::warn("No wildcards available to resolve path");
				return resolved_path;
			}

			file_info_view file_info_view(filename);

			for (const auto& p : g_ctx.wildcards)
			{
				std::string_view sv_wildcard = string::restore_sid(p.first);
				std::string_view sv_path = p.second;

				if (file_info_view.path().find(sv_wildcard) != std::string::npos)
				{
					resolved_path = string::string_replace<std::string>(file_info_view.path(), sv_wildcard, sv_path);
				}
			}

			return file_info::make_full_path(resolved_path, file_info_view.filename(), file_info_view.extension());
		}

		//-------------------------------------------------------------------------
		bool exists(std::string_view filename)
		{
			const std::string path = resolve_path(filename);

			std::ifstream f(path.c_str());
			bool good = f.good();
			f.close();

			return good;
		}
    }
}