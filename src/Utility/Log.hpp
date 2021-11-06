#pragma once

#include <memory>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>

namespace SampleHive {

    class Log
    {
        public:
            static void InitLogger(const std::string& logger);

        public:
            inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

        private:
            static std::shared_ptr<spdlog::logger> s_Logger;
    };

}

// Log macros
#define SH_LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(::SampleHive::Log::GetLogger(), __VA_ARGS__)
#define SH_LOG_INFO(...)     SPDLOG_LOGGER_INFO(::SampleHive::Log::GetLogger(), __VA_ARGS__)
#define SH_LOG_WARN(...)     SPDLOG_LOGGER_WARN(::SampleHive::Log::GetLogger(), __VA_ARGS__)
#define SH_LOG_DEBUG(...)    SPDLOG_LOGGER_DEBUG(::SampleHive::Log::GetLogger(), __VA_ARGS__)
#define SH_LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(::SampleHive::Log::GetLogger(), __VA_ARGS__)
#define SH_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(::SampleHive::Log::GetLogger(), __VA_ARGS__)
