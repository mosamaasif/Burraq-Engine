#pragma once

#include <BRQ.h>

namespace BRQ {
    
    class ConsoleSink {

    public:
        enum class Color : U8 {

            White       = 7,
            Green       = 2,
            Yellow      = 6,
            Red         = 4,
            LightGreen  = 10,
            LightYellow = 14,
            LightRed    = 12,
        };

    private:
        HANDLE m_ConsoleErrorHandle;

    public:
        ConsoleSink();

        void SetConsoleColor(Color color = Color::White) const;
        void WriteToConsole(const U8* buffer, U64 size) const;
    };
}