#pragma once

#ifdef NOMINMAX
    #include <Windows.h>
#else
    #define NOMINMAX
    #include <Windows.h>
    #undef NOMINMAX
#endif

#include <set>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory.h>
#include <functional>
#include <string_view>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#include <Platform/Vulkan/VKCommon.h>

#include "Platform/Platform.h"
#include "Utilities/Types.h"

#include "Core/Base.h"

#include "Logger/Log.h"
