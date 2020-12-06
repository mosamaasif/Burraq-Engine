#include <BRQ.h>

#include "Formatter.h"

namespace BRQ {

    Formatter::Formatter()
        : m_Buffer(nullptr), m_Index(0), m_FormatIndex(0) {

        m_Buffer = new U8[MAX_FORMAT_BUFFER];
        memset(m_Buffer, 0, MAX_FORMAT_BUFFER);
    }

    Formatter::~Formatter() {

        if (m_Buffer)
            delete[] m_Buffer;
    }

    void Formatter::GetBuffer(U8* buffer, U64 size) {

        memset(buffer, 0, size);
        memcpy_s(buffer, size, m_Buffer, m_Index);
    }

    void Formatter::PushEndLine() {

        if (m_Index < MAX_FORMAT_BUFFER - 1)
            m_Buffer[m_Index++] = '\n';
    }

    void Formatter::Flush() {

        memset(m_Buffer, 0, MAX_FORMAT_BUFFER);
        m_Index = m_FormatIndex = 0;
    }
}

