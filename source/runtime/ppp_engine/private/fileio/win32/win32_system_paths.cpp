#include "fileio/system_paths.h"

#include "util/log.h"

#include <windows.h>

namespace ppp
{
    namespace vfs
    {
        std::string g_executable_path = {};

        //-------------------------------------------------------------------------
        std::string_view executable_filepath()
        {
            if (g_executable_path.empty())
            {
                char buffer[MAX_PATH]{};

                DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
                if (length == 0)
                {
                    log::error("Failed to retrieve executable path.");
                    exit(EXIT_FAILURE);
                }

                g_executable_path = buffer;
            }

            return g_executable_path;
        }
        //-------------------------------------------------------------------------
        std::string_view executable_directory()
        {
            auto filepath = executable_filepath();

            size_t pos = filepath.find_last_of("\\/");
            if (pos != std::string::npos) 
            {
                return filepath.substr(0, pos);
            }

            return filepath;
        }
    }
}
