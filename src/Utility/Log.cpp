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

#include "Log.hpp"

#include <iostream>

#include "spdlog/sinks/stdout_color_sinks.h"

namespace SampleHive {

    std::shared_ptr<spdlog::logger> cLog::s_pLogger;

    void cLog::InitLogger(const std::string& logger)
    {
        spdlog::set_pattern("%^[%-T] [%-n] [%l]: %v %@%$");

        try
        {
            s_pLogger = spdlog::stdout_color_mt(logger);
            s_pLogger->set_level(spdlog::level::trace);
        }
        catch (const spdlog::spdlog_ex& ex)
        {
            std::cout << "Logger initialization failed: " << ex.what() << std::endl;
        }
    }

}
