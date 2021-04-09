#pragma once

#include "Platform/Platform.h"
#include "Utilities/Types.h"
#include "Logger/Log.h"

#ifdef BRQ_DEBUG 
    #define BRQ_ASSERT_DEFAULT(x)               BRQ_FATAL("Assertion Failed: {}  Line: {}, File: {}, Function: {}", #x, __LINE__, __FILE__, __FUNCTION__);
    #define BRQ_CORE_ASSERT_DEFAULT(x)          BRQ_CORE_FATAL("Assertion Failed: {}  Line: {}, File: {}, Function: {}", #x, __LINE__, __FILE__, __FUNCTION__);

    #define BRQ_ASSERT(x)                       { if(!(x)) { BRQ_ASSERT_DEFAULT(x); __debugbreak(); } }
    #define BRQ_CORE_ASSERT(x)                  { if(!(x)) { BRQ_CORE_ASSERT_DEFAULT(x); __debugbreak(); } }
#else
    #define BRQ_ASSERT(x)
    #define BRQ_CORE_ASSERT(x)
#endif

#define BIT(x)  (1 << x)

#define BRQ_ALIGN(x) __declspec(align(x))

#define BRQ_BIND_EVENT_FN(function) [this](auto&&... args) -> decltype(auto) { return this->function(std::forward<decltype(args)>(args)...); }