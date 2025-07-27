#include "util/log.h"

#include "fileio/file_info.h"
#include "fileio/vfs.h"

#include <fmt/ostream.h>

#include <atomic>
#include <mutex>
#include <vector>
#include <fstream>
#include <iostream>

namespace ppp
{
    namespace log
    {
        //-------------------------------------------------------------------------
        std::string append_timestamp(std::string_view filepath)
        {
            vfs::file_info_view info(filepath);

            std::time_t t = std::time(nullptr);
            std::tm tm{};
#if defined(_WIN32)
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif

            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "_%Y-%m-%d_%H-%M-%S", &tm);

            return vfs::file_info::make_full_path(info.path(), std::string(info.filename()) + buffer, info.extension());
        }
    
        struct sink
        {
            //-------------------------------------------------------------------------
            virtual ~sink() {}
            //-------------------------------------------------------------------------
            virtual void trace(const char* name,const char* color,std::string_view txt) = 0;
        };
        struct console_sink : public sink
        {
            //-------------------------------------------------------------------------
            void trace(const char* name,const char* color,std::string_view txt) override
            {
                std::cout << color << '[' << name << ']' << reset << ' ' << txt << '\n';
            }
        };
        struct file_sink : public sink
        {
            //-------------------------------------------------------------------------
            explicit file_sink(std::string_view path)
                : _file(path.data(), std::ios::trunc)
            {
                if (!_file.is_open())
                {
                    log::error("Unable to start file stream at file at path: {}", path.data());
                }
            }

            //-------------------------------------------------------------------------
            ~file_sink() override
            {
                _file.close();
            }

            //-------------------------------------------------------------------------
            void trace(const char* name, const char* /*color*/, std::string_view txt) override
            {
                if (!is_open())
                {
                    return;
                }

                _file << '[' << name << ']' << ' ' << txt << '\n';
                _file.flush();
            }

            //-------------------------------------------------------------------------
            bool is_open() const
            {
                return _file.is_open();
            }

        private:
            std::ofstream _file;
        };


        //-------------------------------------------------------------------------
        struct log_context
        {
            std::atomic<bool> enabled{ true };
            std::atomic<log_level> level{ log_level::INFO };
            std::mutex mtx;
            std::vector<std::unique_ptr<sink>> sinks;
        };

        //-------------------------------------------------------------------------
        log_context& ctx()
        {
            static log_context context;

            return context;
        }

        namespace internal
        {
            //-------------------------------------------------------------------------
            void trace(log_level lvl, const char* name, const char* color, std::string_view txt)
            {
                if (!is_logging_enabled() || !is_log_level_enabled(lvl))
                {
                    return;
                }

                for (auto& sink : ctx().sinks)
                {
                    sink->trace(name, color, txt);
                }
            }
        }

        //-------------------------------------------------------------------------
        void initialize()
        {
            std::lock_guard<std::mutex> lock(ctx().mtx);
            ctx().sinks.emplace_back(std::make_unique<console_sink>());
        }

        //-------------------------------------------------------------------------
        void shutdown()
        {
            std::lock_guard<std::mutex> lock(ctx().mtx);
            ctx().sinks.clear();
        }

        //-------------------------------------------------------------------------
        void set_log_level(log_level level)
        {
            ctx().level.store(level);
        }

        //-------------------------------------------------------------------------
        void set_file_output_enabled(std::string_view filepath, bool unique_file_naming)
        {
            std::lock_guard<std::mutex> lock(ctx().mtx);

            std::string final_path(filepath.data());
            if (unique_file_naming)
            {
                final_path = append_timestamp(final_path);
            }

            vfs::file_info_view info(final_path);
            if (!vfs::create_directory(info.path()))
            {
                log::error("Failed to ensure log directory exists: {}", info.path());
                return;
            }
            
            ctx().sinks.emplace_back(std::make_unique<file_sink>(final_path));
        }

        //-------------------------------------------------------------------------
        void enable_logging()
        {
            ctx().enabled.store(true);
        }
        
        //-------------------------------------------------------------------------
        void disable_logging()
        {
            ctx().enabled.store(false);
        }

        //-------------------------------------------------------------------------
        bool is_logging_enabled()
        {
            return ctx().enabled.load();
        }

        //-------------------------------------------------------------------------
        bool is_log_level_enabled(log_level level)
        {
            return ctx().level.load() <= level;
        }
    }
}