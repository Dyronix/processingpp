#include "fileio/fileio.h"
#include "util/string_ops.h"
#include "util/log.h"
	
#include <fstream>

#include <assert.h>

namespace ppp
{
    namespace fileio
    {
		namespace internal
		{
			fileio_hash_map<pool_string, pool_string> _wildcards;

			class Path
			{
			public:
				Path(std::string_view full_path = "")
				{
					u64 dot_pos = full_path.find_last_of('.');

					assert(dot_pos != std::string::npos && "Path must have a filename and an extension");

					_path = full_path.substr(0, full_path.find_last_of(get_separator())); // Extract the path
					_filename = full_path.substr(full_path.find_last_of(get_separator()) + 1, dot_pos - (full_path.find_last_of(get_separator()) + 1)); // Extract filename without extension
					_extension = full_path.substr(dot_pos); // Extract extension

					_path = ensure_trailing_separator(_path);
				}

				void append(std::string_view component)
				{
					_path = ensure_trailing_separator(_path);
					_path += component;
				}

				// Method to get the complete path as a string, including filename with extension
				pool_string str() const
				{
					pool_string full_path = ensure_trailing_separator(_path) + _filename + _extension;;

					return string::string_replace(full_path, pool_string("\\"), pool_string("/"));
				}

			private:
				pool_string _path;	
				pool_string _filename;
				pool_string _extension;

				pool_string ensure_trailing_separator(const pool_string& path) const
				{
					char separator = get_separator();

					if (!path.empty() && path.back() != separator)
					{
						return path + separator;
					}

					return path;
				}

				char get_separator() const
				{
					return '/';
				}
			};

			pool_string get_path(std::string_view filename)
			{
				Path full_path(filename);

				for (const auto& p : _wildcards)
				{
					if (full_path.str().find(p.first) != std::string::npos)
					{
						full_path = Path(string::string_replace(full_path.str(), p.first, p.second));
					}
				}

				return full_path.str();
			}
		}

		void add_wildcard(const pool_string& wildcard, const pool_string& value)
		{
			log::info("adding wildcard {} | {}", wildcard, value);

			internal::_wildcards[wildcard] = value;
		}

		pool_string resolve_path(std::string_view filename)
		{
			return internal::get_path(filename);
		}

		bool exists(std::string_view filename)
		{
			const pool_string path = internal::get_path(filename);

			std::ifstream f(path.c_str());
			bool good = f.good();
			f.close();

			return good;
		}

		fileio_blob read_binary_file(std::string_view filename)
		{
			// Load file from disk
			const pool_string path = internal::get_path(filename);

			std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
			if (!file.is_open())
			{
				log::error("File {} with full path {} was not found!", filename, path);
				return {};
			}
			else
			{
				log::info("Binary file read at path: {}", path);
			}

			// Read file
			const std::streamsize size = file.tellg();
			file.seekg(0, std::ios::beg);
			fileio_blob buffer(size);
			if (file.read((char*)buffer.data(), size))
			{
				return buffer;
			}

			assert(false);
			return {};
		}

		fileio_string read_text_file(std::string_view filename)
		{
			// Load file from disk
			const pool_string path = internal::get_path(filename);

			std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
			if (!file.is_open())
			{
				log::error("File {} with full path {} was not found!", filename, path);
				return {};
			}

			// Read file
			file.seekg(0, std::ios::end);
			const u64 size = file.tellg();
			fileio_string buffer(size, '\0');
			file.seekg(0);
			if (file.read(&buffer[0], size))
			{
				return buffer;
			}

#ifdef _DEBUG
			if (file.eof()) 
			{
				log::error("Reached end of file unexpectedly while reading file {} with full path {}", filename, path); 
			}
			else if (file.bad()) 
			{
				log::error("A serious I/O error occurred while reading file {} with full path {}", filename, path); 
			}
			else if (file.fail()) 
			{
				log::error("Failed to read file {} with full path {} due to a formatting or conversion issue", filename, path); 
			}
#endif

			assert(false);
			return {};
		}
    }
}