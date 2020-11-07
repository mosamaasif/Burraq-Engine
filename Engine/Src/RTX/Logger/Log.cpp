#include <RTX.h>

#include "Logger.h"

namespace RTX {

	Logger* Log::s_CoreLogger = nullptr;
	Logger* Log::s_ClientLogger = nullptr;

	void Log::Init() {
		
		LoggerProperties client, core;

		client.loggerName = "APP";
		core.loggerName = "RTX";

		s_CoreLogger = Logger::CreateLogger(core);
		s_ClientLogger = Logger::CreateLogger(client);

		RTX_CORE_INFO("Logger Initialized!");
		RTX_INFO("Logger Initialized!");
	}

	void Log::Shutdown() {
		
		if (s_CoreLogger)
			delete s_CoreLogger;

		if (s_ClientLogger)
			delete s_ClientLogger;
	}
}
