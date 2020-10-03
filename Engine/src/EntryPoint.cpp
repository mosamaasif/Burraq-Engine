#include <iostream>

#include "Platform/Platform.h"

#include "Utilities/Logger.h"

int main() {

	RTX::Util::Logger::Init();

	RTX_CORE_FATAL("BREH");
	RTX_FATAL("BREH");

	RTX::Util::Logger::Shutdown();

	return 0;
}