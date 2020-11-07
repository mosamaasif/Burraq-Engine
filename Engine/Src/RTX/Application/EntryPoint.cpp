#include <RTX.h>

#include "Platform/Platform.h"
#include "Logger/Log.h"

int main() {

	RTX::Log::Init();

	RTX_CORE_INFO("INFO: {}", __LINE__);
	RTX_CORE_TRACE("TRACE: {}", __LINE__);
	RTX_CORE_WARN("WARN: {}", __LINE__);
	RTX_CORE_ERROR("ERROR: {}", __LINE__);
	RTX_CORE_FATAL("FATAL: {}", __LINE__);

	RTX_INFO("INFO: {}", __LINE__);
	RTX_TRACE("TRACE: {}", __LINE__);
	RTX_WARN("WARN: {}", __LINE__);
	RTX_ERROR("ERROR: {}", __LINE__);
	RTX_FATAL("FATAL: {}", __LINE__);

	std::cout << "----------------------------------" << std::endl;

	RTX::Log::SetClientVerbosity(Logger::LogLevel::Critical);
	RTX::Log::SetCoreVerbosity(Logger::LogLevel::Critical);

	RTX_CORE_INFO("INFO: {}", __LINE__);
	RTX_CORE_TRACE("TRACE: {}", __LINE__);
	RTX_CORE_WARN("WARN: {}", __LINE__);
	RTX_CORE_ERROR("ERROR: {}", __LINE__);
	RTX_CORE_FATAL("FATAL: {}", __LINE__);

	RTX_INFO("INFO: {}", __LINE__);
	RTX_TRACE("TRACE: {}", __LINE__);
	RTX_WARN("WARN: {}", __LINE__);
	RTX_ERROR("ERROR: {}", __LINE__);
	RTX_FATAL("FATAL: {}", __LINE__);

	std::cout << "----------------------------------" << std::endl;

	RTX::Log::SetClientVerbosity(Logger::LogLevel::Warn);
	RTX::Log::SetCoreVerbosity(Logger::LogLevel::Warn);

	RTX_CORE_INFO("INFO: {}", __LINE__);
	RTX_CORE_TRACE("TRACE: {}", __LINE__);
	RTX_CORE_WARN("WARN: {}", __LINE__);
	RTX_CORE_ERROR("ERROR: {}", __LINE__);
	RTX_CORE_FATAL("FATAL: {}", __LINE__);

	RTX_INFO("INFO: {}", __LINE__);
	RTX_TRACE("TRACE: {}", __LINE__);
	RTX_WARN("WARN: {}", __LINE__);
	RTX_ERROR("ERROR: {}", __LINE__);
	RTX_FATAL("FATAL: {}", __LINE__);

	std::cout << "----------------------------------" << std::endl;

	RTX::Log::SetClientVerbosity(Logger::LogLevel::Info);
	RTX::Log::SetCoreVerbosity(Logger::LogLevel::Info);

	RTX_CORE_INFO("INFO: {}", __LINE__);
	RTX_CORE_TRACE("TRACE: {}", __LINE__);
	RTX_CORE_WARN("WARN: {}", __LINE__);
	RTX_CORE_ERROR("ERROR: {}", __LINE__);
	RTX_CORE_FATAL("FATAL: {}", __LINE__);

	RTX_INFO("INFO: {}", __LINE__);
	RTX_TRACE("TRACE: {}", __LINE__);
	RTX_WARN("WARN: {}", __LINE__);
	RTX_ERROR("ERROR: {}", __LINE__);
	RTX_FATAL("FATAL: {}", __LINE__);

	RTX::Log::Shutdown();

	return 0;
}