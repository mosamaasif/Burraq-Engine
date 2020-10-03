#include "Logger.h"


namespace RTX { namespace Util {

	std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

	void Logger::Init() {
		
		spdlog::set_pattern("[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("RTX");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);

		RTX_CORE_INFO("Logger Initialized!");
		RTX_INFO("Logger Initialized!");
	}

	void Logger::Shutdown() {
	
		RTX_CORE_INFO("Shutting Down Logger!");
		RTX_INFO("Shutting Down Logger!");
	}
} }
