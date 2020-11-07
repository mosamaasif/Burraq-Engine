#pragma once

#include "Platform/Platform.h"
#include "Utilities/Types.h"
#include "Logger/Log.h"

#ifdef RTX_DEBUG 
    
    #define RTX_ASSERT_DEFAULT(x)               RTX_FATAL("Assertion Failed: {}  Line: {}, File: {}, Function: {}", #x, __LINE__, __FILE__, __FUNCTION__);
    #define RTX_CORE_ASSERT_DEFAULT(x)          RTX_CORE_FATAL("Assertion Failed: {}  Line: {}, File: {}, Function: {}", #x, __LINE__, __FILE__, __FUNCTION__);

    #define RTX_ASSERT(x)                       { if(!(x)) { RTX_ASSERT_DEFAULT(x); __debugbreak(); } }
    #define RTX_CORE_ASSERT(x, ...)             { if(!(x)) { RTX_CORE_ASSERT_DEFAULT(x); __debugbreak(); } }
#else
    #define RTX_ASSERT(x)
    #define RTX_ASSERT(x, ...)
    #define RTX_CORE_ASSERT(x)
    #define RTX_CORE_ASSERT(x, ...)
#endif