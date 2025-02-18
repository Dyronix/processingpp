#include "fileio/vfs.h"
#include "fileio/file_info.h"

#include "string/string_ops.h"

#include "memory/allocators/tagged_heap_allocator.h"

#include "util/log.h"

#include <fstream>

namespace ppp
{
    namespace vfs
    {
		namespace internal
		{
			static global_hash_map<string::string_id, global_string>& wildcards()
			{
				static global_hash_map<string::string_id, global_string>* s_wildcards = memory::tagged_placement_new<global_hash_map<string::string_id, global_string>>();

				return *s_wildcards;
			}
		}

		//-------------------------------------------------------------------------
		void add_wildcard(string::string_id wildcard, const std::string_view value)
		{
			log::info("adding wildcard {} | {}", string::restore_sid(wildcard), value);

			internal::wildcards()[wildcard] = value;
		}

		//-------------------------------------------------------------------------
		temp_string resolve_path(std::string_view filename)
		{
			temp_string		resolved_path;

			file_info_view	file_info_view(filename);

			for (const auto& p : internal::wildcards())
			{
				std::string_view sv_wildcard = string::restore_sid(p.first);
				std::string_view sv_path = p.second;

				if (file_info_view.path().find(sv_wildcard) != std::string::npos)
				{
					resolved_path = string::string_replace<temp_string>(file_info_view.path(), sv_wildcard, sv_path);
				}
			}

			return file_info<memory::persistent_frame_allocator<char>>::make_full_path(resolved_path, file_info_view.filename(), file_info_view.extension());
		}

		//-------------------------------------------------------------------------
		bool exists(std::string_view filename)
		{
			const temp_string path = resolve_path(filename);

			std::ifstream f(path.c_str());
			bool good = f.good();
			f.close();

			return good;
		}
    }
}