#include "Log.hpp"

#include <iostream>

#include "spdlog/sinks/stdout_color_sinks.h"

namespace SampleHive {

    std::shared_ptr<spdlog::logger> Log::s_Logger;

    void Log::InitLogger(const std::string& logger)
    {
        spdlog::set_pattern("%^[%-T] [%-n] [%l]: %v %@%$");

        try
        {
            s_Logger = spdlog::stdout_color_mt(logger);
            s_Logger->set_level(spdlog::level::trace);
        }
        catch (const spdlog::spdlog_ex& ex)
        {
            std::cout << "Logger initialization failed: " << ex.what() << std::endl;
        }
    }

}
