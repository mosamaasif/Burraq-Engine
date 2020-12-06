#pragma once

#ifdef NOMINMAX
    #include <Windows.h>
#else
    #define NOMINMAX
    #include <Windows.h>
    #undef NOMINMAX
#endif

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory.h>
#include <functional>
#include <string_view>

#include "Platform/Platform.h"
#include "Utilities/Types.h"

#include "Core/Base.h"

#include "Logger/Log.h"
