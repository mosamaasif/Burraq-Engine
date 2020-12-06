#pragma once

#ifdef _MSC_VER
    #if (_MSC_VER < 1915)
        #error "Burraq Engine does NOT support MSC version less than 1915 (Visual Studio 2017 version 15.8)!"
    #endif
#else
    #error "Burraq Engine ONLY supports MSVC!"
#endif

#ifdef _WIN64
    #define BRQ_PLATFORM_WINDOWS
#else
    #error "Burraq Engine ONLY supports Windows-x64!"
#endif



