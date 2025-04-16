#include "memory/memory_config.h"

#include "fileio/vfs.h"
#include "fileio/system_paths.h"

#include "util/log.h"

#include <ini.h>
#include <string>

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        // Structure to contain parsed data
        struct memory_config
        {
            bool loaded = false;

            s64 frame_heap_size = 1_mb;
            s64 tagged_heap_block_size = 500_kb;
            s32 tagged_heap_block_count = 10;
            s64 scratch_heap_size = 1_mb;
        };

        memory_config g_development_persistant_config;
        memory_config g_development_staging_config;
        memory_config g_development_debug_config;

        memory_config g_shipping_persistant_config;
        memory_config g_shipping_staging_config;
        memory_config g_shipping_debug_config;

        static int parse_memory_config(memory_config* pconfig, const char* in_section, const char* section, const char* name, const char* value)
        {
            #define MATCH(n) strcmp(section, in_section) == 0 && strcmp(name, n) == 0

            if (MATCH("frame_heap_size"))
            {
                std::istringstream iss(value);
                iss >> pconfig->frame_heap_size;
                pconfig->frame_heap_size *= kilo_byte_multiple;
                log::info("Read frame_heap_size ({} kb) from section - {}", pconfig->frame_heap_size / kilo_byte_multiple, section);
                return 1;
            }

            if (MATCH("tagged_heap_block_size"))
            {
                std::istringstream iss(value);
                iss >> pconfig->tagged_heap_block_size;
                pconfig->tagged_heap_block_size *= kilo_byte_multiple;
                log::info("Read tagged_heap_block_size ({} kb) from section - {}", pconfig->tagged_heap_block_size / kilo_byte_multiple, section);
                return 1;
            }

            if (MATCH("tagged_heap_block_count"))
            {
                std::istringstream iss(value);
                iss >> pconfig->tagged_heap_block_count;
                log::info("Read tagged_heap_block_count ({} blocks) from section - {}", pconfig->tagged_heap_block_count, section);
                return 1;
            }

            if (MATCH("scratch_heap_size"))
            {
                std::istringstream iss(value);
                iss >> pconfig->scratch_heap_size;
                pconfig->scratch_heap_size *= kilo_byte_multiple;
                log::info("Read scratch_heap_size ({} kb) from section - {}", pconfig->scratch_heap_size / kilo_byte_multiple, section);
                return 1;
            }

            return 0;
        }

        //-------------------------------------------------------------------------
        static int handler_development_persistant(void* user, const char* section, const char* name, const char* value)
        {
            memory_config* pconfig = (memory_config*)user;

            if (parse_memory_config(pconfig, "/memory/development/persistant", section, name, value))
            {
                pconfig->loaded = true;
                return 1;
            }

            return 0;  /* unknown section/name, error */
        }
        //-------------------------------------------------------------------------
        static int handler_development_staging(void* user, const char* section, const char* name, const char* value)
        {
            memory_config* pconfig = (memory_config*)user;

            if (parse_memory_config(pconfig, "/memory/development/staging", section, name, value))
            {
                pconfig->loaded = true;
                return 1;
            }

            return 0;  /* unknown section/name, error */
        }
        //-------------------------------------------------------------------------
        static int handler_development_debug(void* user, const char* section, const char* name, const char* value)
        {
            memory_config* pconfig = (memory_config*)user;

            if (parse_memory_config(pconfig, "/memory/development/debug", section, name, value))
            {
                pconfig->loaded = true;
                return 1;
            }

            return 0;  /* unknown section/name, error */
        }

        //-------------------------------------------------------------------------
        static int handler_shipping_persistant(void* user, const char* section, const char* name, const char* value)
        {
            memory_config* pconfig = (memory_config*)user;

            if (parse_memory_config(pconfig, "/memory/shipping/persistant", section, name, value))
            {
                pconfig->loaded = true;
                return 1;
            }

            return 0;  /* unknown section/name, error */
        }
        //-------------------------------------------------------------------------
        static int handler_shipping_staging(void* user, const char* section, const char* name, const char* value)
        {
            memory_config* pconfig = (memory_config*)user;

            if (parse_memory_config(pconfig, "/memory/shipping/staging", section, name, value))
            {
                pconfig->loaded = true;
                return 1;
            }

            return 0;  /* unknown section/name, error */
        }
        //-------------------------------------------------------------------------
        static int handler_shipping_debug(void* user, const char* section, const char* name, const char* value)
        {
            memory_config* pconfig = (memory_config*)user;

            if (parse_memory_config(pconfig, "/memory/shipping/debug", section, name, value))
            {
                pconfig->loaded = true;
                return 1;
            }

            return 0;  /* unknown section/name, error */
        }

        //-------------------------------------------------------------------------
        memory_size development_persistant_frame_heap_size()
        {
            if (g_development_persistant_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_persistant, &g_development_persistant_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    exit(EXIT_FAILURE);
                }
            }

            return g_development_persistant_config.frame_heap_size;
        }
        //-------------------------------------------------------------------------
        memory_size development_persistant_tagged_heap_block_size()
        {
            if (g_development_persistant_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_persistant, &g_development_persistant_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_development_persistant_config.tagged_heap_block_size;
        }
        //-------------------------------------------------------------------------
        s32 development_persistant_tagged_heap_block_count()
        {
            if (g_development_persistant_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_persistant, &g_development_persistant_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_development_persistant_config.tagged_heap_block_count;
        }
        //-------------------------------------------------------------------------
        memory_size development_persistant_scratch_heap_size()
        {
            if (g_development_persistant_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_persistant, &g_development_persistant_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_development_persistant_config.scratch_heap_size;
        }

        //-------------------------------------------------------------------------
        memory_size development_staging_frame_heap_size()
        {
            if (g_development_staging_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_staging, &g_development_staging_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_development_staging_config.frame_heap_size;
        }
        //-------------------------------------------------------------------------
        memory_size development_staging_tagged_heap_block_size()
        {
            if (g_development_staging_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_staging, &g_development_staging_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_development_staging_config.tagged_heap_block_size;
        }
        //-------------------------------------------------------------------------
        s32 development_staging_tagged_heap_block_count()
        {
            if (g_development_staging_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_staging, &g_development_staging_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_development_staging_config.tagged_heap_block_count;
        }
        //-------------------------------------------------------------------------
        memory_size development_staging_scratch_heap_size()
        {
            if (g_development_staging_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_staging, &g_development_staging_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_development_staging_config.scratch_heap_size;
        }

        //-------------------------------------------------------------------------
        memory_size development_debug_frame_heap_size()
        {
            if (g_development_debug_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_debug, &g_development_debug_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_development_debug_config.frame_heap_size;
        }
        //-------------------------------------------------------------------------
        memory_size development_debug_tagged_heap_block_size()
        {
            if (g_development_debug_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_debug, &g_development_debug_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_development_debug_config.tagged_heap_block_size;
        }
        //-------------------------------------------------------------------------
        s32 development_debug_tagged_heap_block_count()
        {
            if (g_development_debug_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_debug, &g_development_debug_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_development_debug_config.tagged_heap_block_count;
        }
        //-------------------------------------------------------------------------
        memory_size development_debug_scratch_heap_size()
        {
            if (g_development_debug_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_development_debug, &g_development_debug_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_development_debug_config.scratch_heap_size;
        }

        //-------------------------------------------------------------------------
        memory_size shipping_persistant_frame_heap_size()
        {
            if (g_shipping_persistant_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_persistant, &g_shipping_persistant_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_persistant_config.frame_heap_size;
        }
        //-------------------------------------------------------------------------
        memory_size shipping_persistant_tagged_heap_block_size()
        {
            if (g_shipping_persistant_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_persistant, &g_shipping_persistant_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_persistant_config.tagged_heap_block_size;
        }
        //-------------------------------------------------------------------------
        s32 shipping_persistant_tagged_heap_block_count()
        {
            if (g_shipping_persistant_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_persistant, &g_shipping_persistant_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_persistant_config.tagged_heap_block_count;
        }
        //-------------------------------------------------------------------------
        memory_size shipping_persistant_scratch_heap_size()
        {
            if (g_shipping_persistant_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_persistant, &g_shipping_persistant_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_persistant_config.scratch_heap_size;
        }

        //-------------------------------------------------------------------------
        memory_size shipping_staging_frame_heap_size()
        {
            if (g_shipping_staging_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_staging, &g_shipping_staging_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_staging_config.frame_heap_size;
        }
        //-------------------------------------------------------------------------
        memory_size shipping_staging_tagged_heap_block_size()
        {
            if (g_shipping_staging_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_staging, &g_shipping_staging_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_staging_config.tagged_heap_block_size;
        }
        //-------------------------------------------------------------------------
        s32 shipping_staging_tagged_heap_block_count()
        {
            if (g_shipping_staging_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_staging, &g_shipping_staging_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_staging_config.tagged_heap_block_count;
        }
        //-------------------------------------------------------------------------
        memory_size shipping_staging_scratch_heap_size()
        {
            if (g_shipping_staging_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_staging, &g_shipping_staging_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_staging_config.scratch_heap_size;
        }

        //-------------------------------------------------------------------------
        memory_size shipping_debug_frame_heap_size()
        {
            if (g_shipping_debug_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_debug, &g_shipping_debug_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_debug_config.frame_heap_size;
        }
        //-------------------------------------------------------------------------
        memory_size shipping_debug_tagged_heap_block_size()
        {
            if (g_shipping_debug_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_debug, &g_shipping_debug_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_debug_config.tagged_heap_block_size;
        }
        //-------------------------------------------------------------------------
        s32 shipping_debug_tagged_heap_block_count()
        {
            if (g_shipping_debug_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_debug, &g_shipping_debug_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_debug_config.tagged_heap_block_count;
        }
        //-------------------------------------------------------------------------
        memory_size shipping_debug_scratch_heap_size()
        {
            if (g_shipping_debug_config.loaded == false)
            {
                std::stringstream stream;
                stream << vfs::executable_directory();
                stream << "/config/default-engine.ini";

                auto ini_path = stream.str();
                if (ini_parse(ini_path.data(), handler_shipping_debug, &g_shipping_debug_config) < 0)
                {
                    log::error("Can't load 'default-engine.ini'");
                    return 0;
                }
            }

            return g_shipping_debug_config.scratch_heap_size;
        }
    }
}