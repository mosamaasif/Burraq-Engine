#pragma once

#include "Platform/Platform.h"
#include "Utilities/Logger.h"

#ifdef RTX_DEBUG 
    #define RTX_ASSERT(x, ...) { if(!(x)) { RTX_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
    #define RTX_CORE_ASSERT(x, ...) { if(!(x)) { RTX_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
    #define RTX_ASSERT(x, ...)
    #define RTX_CORE_ASSERT(x, ...)
#endif
