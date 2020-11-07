#pragma once

#include <RTX.h>

class FileSink {

private:
    FILE* m_FileHandle;

public:
    FileSink() = default;
    ~FileSink();

    bool OpenFile(const std::string_view& filename);
    void Write(const U8* buffer, U64 size);
};