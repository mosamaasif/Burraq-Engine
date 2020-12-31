#include <BRQ.h>

#include "Logger.h"

namespace BRQ {

	Logger* Log::s_CoreLogger = nullptr;
	Logger* Log::s_ClientLogger = nullptr;

	void Log::Init() {
		
		LoggerProperties client, core;

		client.LoggerName = "APP";
		core.LoggerName = "Burraq";

		s_CoreLogger = Logger::CreateLogger(core);
		s_ClientLogger = Logger::CreateLogger(client);

#ifdef BRQ_DEBUG

		s_CoreLogger->SetLogLevel(Logger::LogLevel::Info);
		s_ClientLogger->SetLogLevel(Logger::LogLevel::Info);
#else	
		s_CoreLogger->SetLogLevel(Logger::LogLevel::Trace);
		s_ClientLogger->SetLogLevel(Logger::LogLevel::Trace);
#endif

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
