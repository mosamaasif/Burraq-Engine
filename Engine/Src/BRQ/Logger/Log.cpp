#include <BRQ.h>

#include "Logger.h"

namespace BRQ {

	Logger* Log::s_CoreLogger = nullptr;
	Logger* Log::s_ClientLogger = nullptr;

	void Log::Init() {
		
		LoggerProperties client, core;

		client.LoggerName = "APP";
		core.LoggerName = "RTX";

		s_CoreLogger = Logger::CreateLogger(core);
		s_ClientLogger = Logger::CreateLogger(client);

		BRQ_CORE_INFO("Logger Initialized!");
		BRQ_INFO("Logger Initialized!");
	}

	void Log::Shutdown() {
		
		if (s_CoreLogger)
			delete s_CoreLogger;

		if (s_ClientLogger)
			delete s_ClientLogger;
	}
}
