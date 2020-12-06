#pragma once

#include <Logger/Logger.h>

namespace BRQ {
    
    class Log {

    private:
        static Logger* s_CoreLogger;
        static Logger* s_ClientLogger;

    public:

        static void Init();
        static void Shutdown();

        static void SetCoreVerbosity(Logger::LogLevel level) { s_CoreLogger->SetLogLevel(level); };
        static void SetClientVerbosity(Logger::LogLevel level) { s_ClientLogger->SetLogLevel(level); };

        inline static Logger* GetCoreLogger() noexcept { return s_CoreLogger; }
        inline static Logger* GetClientLogger() noexcept { return s_ClientLogger; }
    };
}

#define BRQ_CORE_TRACE(...)     ::BRQ::Log::GetCoreLogger()->Trace(__VA_ARGS__)
#define BRQ_CORE_INFO(...)      ::BRQ::Log::GetCoreLogger()->Info(__VA_ARGS__)
#define BRQ_CORE_WARN(...)      ::BRQ::Log::GetCoreLogger()->Warn(__VA_ARGS__)
#define BRQ_CORE_ERROR(...)     ::BRQ::Log::GetCoreLogger()->Error(__VA_ARGS__)
#define BRQ_CORE_FATAL(...)     ::BRQ::Log::GetCoreLogger()->Critical(__VA_ARGS__)
                                        
#define BRQ_TRACE(...)          ::BRQ::Log::GetClientLogger()->Trace(__VA_ARGS__)
#define BRQ_INFO(...)           ::BRQ::Log::GetClientLogger()->Info(__VA_ARGS__)
#define BRQ_WARN(...)           ::BRQ::Log::GetClientLogger()->Warn(__VA_ARGS__)
#define BRQ_ERROR(...)          ::BRQ::Log::GetClientLogger()->Error(__VA_ARGS__)
#define BRQ_FATAL(...)          ::BRQ::Log::GetClientLogger()->Critical(__VA_ARGS__)