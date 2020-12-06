#include <BRQ.h>

#include "Logger.h"

namespace BRQ {

    Logger* Logger::CreateLogger(const LoggerProperties& properties) {

        return new Logger(properties);
    }

    Logger::Logger(const LoggerProperties& properties)
        : m_LoggerName(properties.LoggerName), m_LogLevel(LogLevel::Warn),
          m_FileLogging(properties.FileLogging), m_Buffer(nullptr)  {

        m_Buffer = new U8[MAX_LOGGER_BUFFER];

        if (m_FileLogging) {
        memset(m_Buffer, 0, MAX_LOGGER_BUFFER);

            if (!properties.Filename) {

                GetTime("{}.{}.{}", &m_Buffer[0], (MAX_LOGGER_BUFFER / 2) - 1);
                GetDate("{}.{}.{}", &m_Buffer[MAX_LOGGER_BUFFER / 2], (MAX_LOGGER_BUFFER / 2) - 1);

                m_Formatter.Format("{} {}-{}.txt", m_LoggerName, (const char*)&m_Buffer[MAX_LOGGER_BUFFER / 2], (const char*)&m_Buffer[0]);
            
                m_FileSink.OpenFile((char*)m_Formatter.GetBuffer());
            }
            else {

                m_FileLogging = m_FileSink.OpenFile(properties.Filename);
            }
        }
    }

    Logger::~Logger() {

        if (m_Buffer)
            delete[] m_Buffer;
    }

    void Logger::GetTime(const char* format, U8* buffer, U64 size) {

        SYSTEMTIME time;
        GetLocalTime(&time);

        m_Formatter.Format(format, (unsigned)time.wHour, (unsigned)time.wMinute, (unsigned)time.wSecond);
        m_Formatter.GetBuffer(buffer, size);
    }

    void Logger::GetDate(const char* format, U8* buffer, U64 size) {

        SYSTEMTIME time;
        GetLocalTime(&time);

        m_Formatter.Format(format, time.wDay, time.wMonth, time.wYear);
        m_Formatter.GetBuffer(buffer, size);
    }

    void Logger::GetHeader(U8* buffer, U64 size) {

        memset(buffer, 0, size);
        GetTime("{}:{}:{}", buffer, size);

        m_Formatter.Format("[{}] {}: ", (const char*)buffer, m_LoggerName);

        m_Formatter.GetBuffer(buffer, size);
    }
}