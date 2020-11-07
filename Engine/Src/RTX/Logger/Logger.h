#pragma once

#include <stdint.h>

#include "ConsoleSink.h"
#include "FileSink.h"
#include "Formatter.h"

#define MAX_LOGGER_BUFFER   512

struct LoggerProperties {

    const char* loggerName = "Logger";
    const char* filename = nullptr; // if nullptr then filename will be a time stamp
    bool        fileLogging = false;
};

class Logger {

public:
    enum class LogLevel : uint8_t {

        Info = 0,
        Trace,
        Warn,
        Error,
        Critical,
    };

private:
    Formatter   m_Formatter;
    ConsoleSink m_ConsoleSink;
    FileSink    m_FileSink;
    const char* m_LoggerName;
    LogLevel    m_LogLevel;
    bool        m_FileLogging;

    U8*       m_Buffer;

public:
    Logger() = delete;
    ~Logger();

    static Logger* CreateLogger(const LoggerProperties& properties);

    template <typename... Args>
    void Info(Args... args) {

        if (m_LogLevel <= LogLevel::Info) {

            m_ConsoleSink.SetConsoleColor(ConsoleSink::Color::White);

            Log(args...);

            m_ConsoleSink.SetConsoleColor(ConsoleSink::Color::White);
        }
    }

    template <typename... Args>
    void Trace(Args... args) {
        
        if (m_LogLevel <= LogLevel::Trace) {

            m_ConsoleSink.SetConsoleColor(ConsoleSink::Color::Green);

            Log(args...);

            m_ConsoleSink.SetConsoleColor(ConsoleSink::Color::White);
        }
    }

    template <typename... Args>
    void Warn(Args... args) {

        if (m_LogLevel <= LogLevel::Warn) {

            m_ConsoleSink.SetConsoleColor(ConsoleSink::Color::Yellow);

            Log(args...);

            m_ConsoleSink.SetConsoleColor(ConsoleSink::Color::White);
        }
    }

    template <typename... Args>
    void Error(Args... args) {

        if (m_LogLevel <= LogLevel::Error) {

            m_ConsoleSink.SetConsoleColor(ConsoleSink::Color::LightRed);

            Log(args...);

            m_ConsoleSink.SetConsoleColor(ConsoleSink::Color::White);
        }
    }

    template <typename... Args>
    void Critical(Args... args) {

        if (m_LogLevel <= LogLevel::Critical) {

            m_ConsoleSink.SetConsoleColor(ConsoleSink::Color::Red);

            Log(args...);

            m_ConsoleSink.SetConsoleColor(ConsoleSink::Color::White);
        }
    }    

    inline void SetLogLevel(LogLevel level) { m_LogLevel = level; }

private:
    Logger(const LoggerProperties& properties);

    void GetTime(const char* format, U8* buffer, U64 size);
    void GetDate(const char* format, U8* buffer, U64 size);

    void GetHeader(U8* buffer, U64 size);

    template <typename... Args>
    void Log(const char* format, Args... args) {
       
        GetHeader(m_Buffer, MAX_LOGGER_BUFFER);
        strcat_s((char*)m_Buffer, MAX_LOGGER_BUFFER, format);

        m_Formatter.Format((const char*)m_Buffer, args...);
        m_Formatter.PushEndLine();

        m_ConsoleSink.WriteToConsole((U8*)m_Formatter.GetBuffer(), m_Formatter.GetBufferLength());

        if (m_FileLogging) {

            m_FileSink.Write((U8*)m_Formatter.GetBuffer(), m_Formatter.GetBufferLength());
        }
    }
};