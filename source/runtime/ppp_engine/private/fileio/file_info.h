#pragma once

#include "util/types.h"

#include "memory/memory_types.h"

#include <string>

namespace ppp
{
    namespace vfs
    {
		class file_info
		{
		public:
			file_info(std::string_view full_path = "");

			void append(std::string_view component);

			// Method to get the complete path as a string, including filename with extension
			pool_string str() const;

		private:
			pool_string _path;
			pool_string _filename;
			pool_string _extension;
		};
    }
}