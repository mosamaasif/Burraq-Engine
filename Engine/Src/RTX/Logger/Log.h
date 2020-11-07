#pragma once

#include <Logger/Logger.h>

namespace RTX {
    
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

#define RTX_CORE_TRACE(...)     ::RTX::Log::GetCoreLogger()->Trace(__VA_ARGS__)
#define RTX_CORE_INFO(...)      ::RTX::Log::GetCoreLogger()->Info(__VA_ARGS__)
#define RTX_CORE_WARN(...)      ::RTX::Log::GetCoreLogger()->Warn(__VA_ARGS__)
#define RTX_CORE_ERROR(...)     ::RTX::Log::GetCoreLogger()->Error(__VA_ARGS__)
#define RTX_CORE_FATAL(...)     ::RTX::Log::GetCoreLogger()->Critical(__VA_ARGS__)
                                                  
#define RTX_TRACE(...)          ::RTX::Log::GetClientLogger()->Trace(__VA_ARGS__)
#define RTX_INFO(...)           ::RTX::Log::GetClientLogger()->Info(__VA_ARGS__)
#define RTX_WARN(...)           ::RTX::Log::GetClientLogger()->Warn(__VA_ARGS__)
#define RTX_ERROR(...)          ::RTX::Log::GetClientLogger()->Error(__VA_ARGS__)
#define RTX_FATAL(...)          ::RTX::Log::GetClientLogger()->Critical(__VA_ARGS__)