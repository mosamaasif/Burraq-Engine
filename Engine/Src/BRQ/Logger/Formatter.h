#pragma once

#include <memory>
#include "FormatterUtility.h"
#include "Utilities/Types.h"

#define MAX_FORMAT_BUFFER           4096

namespace BRQ {

    class Formatter {

    private:
        U8* m_Buffer;
        U64 m_Index;
        U64 m_FormatIndex;

    public:
        Formatter();
        ~Formatter();

        template <typename... Args>
        void Format(const char* format, Args... args) {
        
            Flush();
            Fmt(format, args...);

            size_t len = strlen(format);
            memcpy_s(&m_Buffer[m_Index], MAX_FORMAT_BUFFER, &format[m_FormatIndex], len - m_FormatIndex);
            m_Index += len - m_FormatIndex;
        }

        void GetBuffer(U8* buffer, U64 size);

        const U8* GetBuffer() const { return m_Buffer; }
        U64 GetBufferLength() const { return m_Index; }

        void PushEndLine();

    private:
        void Flush();

        void Fmt(const char* format) {

            memcpy_s(&m_Buffer[m_Index], MAX_FORMAT_BUFFER, format, strlen(format));
        }

        template <typename T>
        void Fmt(const char* format, T first) {

            size_t length = strlen(format);

            U8 buffer[512];
            memset(buffer, 0, 512);

            WriteToBuffer(buffer, 512, first);

            size_t argumentLength = strlen((const char*)buffer);

            while (m_FormatIndex < length) {

                if (format[m_FormatIndex] == '{' && format[m_FormatIndex + 1] == '}') {

                    memcpy_s(&m_Buffer[m_Index], MAX_FORMAT_BUFFER, &buffer[0], argumentLength);

                    m_FormatIndex += 2;
                    m_Index += argumentLength;
                    break;
                }
                else {

                    m_Buffer[m_Index] = format[m_FormatIndex];
                    m_FormatIndex++;
                    ++m_Index;
                }
            }
        }

        template <typename T, typename... Args>
        void Fmt(const char* format, T first, Args... args) {

            Fmt(format, first);
            Fmt(format, args...);
        }
    };
}