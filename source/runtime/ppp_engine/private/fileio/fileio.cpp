#include "fileio/fileio.h"
#include "util/string_ops.h"
#include "util/log.h"

#include <unordered_map>
#include <fstream>
#include <assert.h>

namespace ppp
{
    namespace fileio
    {
		namespace internal
		{
			std::unordered_map<std::string, std::string> _wildcards;

			std::string get_path(const std::string& filename)
			{
				std::string full_path = filename;

				for (const auto& p : _wildcards)
				{
					if (full_path.find(p.first) != std::string::npos)
					{
						full_path = string::string_replace(full_path, p.first, p.second);
					}
				}

				return full_path;
			}
		}

		void add_wildcard(const std::string& wildcard, const std::string& value)
		{
			internal::_wildcards[wildcard] = value;
		}

		bool exists(const std::string& filename)
		{
			const std::string path = internal::get_path(filename);

			std::ifstream f(path.c_str());
			bool good = f.good();
			f.close();

			return good;
		}

		blob read_binary_file(const std::string& filename)
		{
			// Load file from disk
			const std::string path = internal::get_path(filename);
			std::ifstream file(path, std::ios::binary | std::ios::ate);
			if (!file.is_open())
			{
				log::error("File {} with full path {} was not found!", filename, path);
				return {};
			}

			// Read file
			const std::streamsize size = file.tellg();
			file.seekg(0, std::ios::beg);
			blob buffer(size);
			if (file.read((char*)buffer.data(), size))
			{
				return buffer;
			}

			assert(false);
			return {};
		}

		std::string read_text_file(const std::string& filename)
		{
			// Load file from disk
			const std::string path = internal::get_path(filename);
			std::ifstream file(path);
			if (!file.is_open())
			{
				log::error("File {} with full path {} was not found!", filename, path);
				return {};
			}

			// Read file
			file.seekg(0, std::ios::end);
			const size_t size = file.tellg();
			std::string buffer(size, '\0');
			file.seekg(0);
			if (file.read(&buffer[0], size))
			{
				return buffer;
			}

			assert(false);
			return {};
		}
    }
}