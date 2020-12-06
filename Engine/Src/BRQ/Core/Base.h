#pragma once

#include "Platform/Platform.h"
#include "Utilities/Types.h"
#include "Logger/Log.h"

// TODO Work on assertions
#ifdef BRQ_DEBUG 
    
    #define BRQ_ASSERT_DEFAULT(x)               BRQ_FATAL("Assertion Failed: {}  Line: {}, File: {}, Function: {}", #x, __LINE__, __FILE__, __FUNCTION__);
    #define BRQ_CORE_ASSERT_DEFAULT(x)          BRQ_CORE_FATAL("Assertion Failed: {}  Line: {}, File: {}, Function: {}", #x, __LINE__, __FILE__, __FUNCTION__);

    #define BRQ_ASSERT(x)                       { if(!(x)) { BRQ_ASSERT_DEFAULT(x); __debugbreak(); } }
    #define BRQ_CORE_ASSERT(x, ...)             { if(!(x)) { BRQ_CORE_ASSERT_DEFAULT(x); __debugbreak(); } }
#else
    #define BRQ_ASSERT(x)
    #define BRQ_ASSERT(x, ...)
    #define BRQ_CORE_ASSERT(x)
    #define BRQ_CORE_ASSERT(x, ...)
#endif