#include <RTX.h>

#include "FileSink.h"


FileSink::~FileSink() {

    if (m_FileHandle)
        fclose(m_FileHandle);
}

bool FileSink::OpenFile(const std::string_view& filename) {

    if (fopen_s(&m_FileHandle, filename.data(), "w")) {

        return true;
    }
    return false;
}

void FileSink::Write(const U8* buffer, U64 size) {

    fwrite((const char*)buffer, sizeof(char), size, m_FileHandle);
    fflush(m_FileHandle);
}