#include "fileio/fileio.h"
#include "fileio/vfs.h"

#include "util/string_ops.h"
#include "util/log.h"
	
#include <fstream>

#include <assert.h>

namespace ppp
{
    namespace fileio
    {
		fileio_blob read_binary_file(std::string_view filename)
		{
			// Load file from disk
			const pool_string path = vfs::resolve_path(filename);

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
			const pool_string path = vfs::resolve_path(filename);

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