#pragma once

#include <BRQ.h>

#ifdef BRQ_DEBUG
    #define VK_CHECK(function) { BRQ_CORE_ASSERT(function == VK_SUCCESS) }
#else
    #define VK_CHECK(function) function
#endif