#include <RTX.h>

#include "ConsoleSink.h"


ConsoleSink::ConsoleSink() 
    : m_ConsoleErrorHandle(nullptr) {

    m_ConsoleErrorHandle = GetStdHandle(STD_ERROR_HANDLE);
}

void ConsoleSink::SetConsoleColor(Color color) const {

    SetConsoleTextAttribute(m_ConsoleErrorHandle, (WORD)color);
}

void ConsoleSink::WriteToConsole(const U8* buffer, size_t size) const {

    DWORD written;

    WriteConsoleA(m_ConsoleErrorHandle, (const void*)buffer, (WORD)size, &written, nullptr);
}