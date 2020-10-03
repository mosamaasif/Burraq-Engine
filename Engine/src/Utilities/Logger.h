#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#pragma warning(pop)

namespace RTX { namespace Util {
    
    class Logger {

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;

    public:

        static void Init();
        static void Shutdown();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() noexcept { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() noexcept { return s_ClientLogger; }
    };
} }

#define RTX_CORE_TRACE(...)     ::RTX::Util::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define RTX_CORE_INFO(...)      ::RTX::Util::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define RTX_CORE_WARN(...)      ::RTX::Util::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define RTX_CORE_ERROR(...)     ::RTX::Util::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define RTX_CORE_FATAL(...)     ::RTX::Util::Logger::GetCoreLogger()->critical(__VA_ARGS__)
                                                  
#define RTX_TRACE(...)          ::RTX::Util::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define RTX_INFO(...)           ::RTX::Util::Logger::GetClientLogger()->info(__VA_ARGS__)
#define RTX_WARN(...)           ::RTX::Util::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define RTX_ERROR(...)          ::RTX::Util::Logger::GetClientLogger()->error(__VA_ARGS__)
#define RTX_FATAL(...)          ::RTX::Util::Logger::GetClientLogger()->critical(__VA_ARGS__)