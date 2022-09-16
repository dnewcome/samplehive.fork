/* SampleHive
 * Copyright (C) 2021  Apoorv Singh
 * A simple, modern audio sample browser/manager for GNU/Linux.
 *
 * This file is a part of SampleHive
 *
 * SampleHive is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SampleHive is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>

#include <SampleHiveConfig.hpp>

#ifdef SH_BUILD_DEBUG
    #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
    #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#endif

#include <spdlog/spdlog.h>

namespace SampleHive {

    class cLog
    {
        public:
            static void InitLogger(const std::string& logger);

        public:
            inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_pLogger; }

        private:
            static std::shared_ptr<spdlog::logger> s_pLogger;
    };

    // Log macros
    #define SH_LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(::SampleHive::cLog::GetLogger(), __VA_ARGS__)
    #define SH_LOG_INFO(...)     SPDLOG_LOGGER_INFO(::SampleHive::cLog::GetLogger(), __VA_ARGS__)
    #define SH_LOG_WARN(...)     SPDLOG_LOGGER_WARN(::SampleHive::cLog::GetLogger(), __VA_ARGS__)
    #define SH_LOG_DEBUG(...)    SPDLOG_LOGGER_DEBUG(::SampleHive::cLog::GetLogger(), __VA_ARGS__)
    #define SH_LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(::SampleHive::cLog::GetLogger(), __VA_ARGS__)
    #define SH_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(::SampleHive::cLog::GetLogger(), __VA_ARGS__)

}
