#pragma once

#include <cassert>

namespace BRQ {

    template <typename T>
    static void WriteToBuffer(U8* buffer, U64 size, T value) {

        assert(false && "Writing Invalid Type to Buffer!");
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, F32 value) {

        sprintf_s((char*)buffer, size, "%3.5f", value);
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, F64 value) {

        sprintf_s((char*)buffer, size, "%5.9f", value);
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, U64 value) {

        memset(buffer, 0, size);

        U64 i = 0;
        U64 max = size - 1;

        if (value == 0 && size > 1) {

            buffer[0] = 0 + 0x30;
            return;
        }

        while (value != 0 && i < max) {

            buffer[i] = value % 10 + 0x30;
            value /= 10;
            ++i;
        }

        _strrev((char*)buffer);
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, I64 value) {

        memset(buffer, 0, size);

        bool sign = (value < 0);

        U64 i = 0;
        U64 max = size - 1;

        U64 val = ((value ^ (value >> 63)) - (value >> 63));

        if (value == 0 && size > 1) {

            buffer[0] = 0 + 0x30;
            return;
        }

        while (val != 0 && i < max) {

            buffer[i] = val % 10 + 0x30;
            val /= 10;
            ++i;
        }

        if (sign)
            buffer[i++] = '-';

        _strrev((char*)buffer);
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, U32 value) {

        WriteToBuffer(buffer, size, (U64)value);
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, I32 value) {

       WriteToBuffer(buffer, size, (I64)value);
    }


    template <>
    static void WriteToBuffer(U8* buffer, U64 size, U16 value) {

        WriteToBuffer(buffer, size, (U64)value);
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, I16 value) {

        WriteToBuffer(buffer, size, (I16)value);
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, long value) {

        WriteToBuffer(buffer, size, (I64)value);
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, unsigned long value) {

        WriteToBuffer(buffer, size, (U64)value);
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, char value) {

        memset(buffer, 0, size);

        memcpy_s(buffer, size, (void*)&value, sizeof(char));
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, U8 value) {

        memset(buffer, 0, size);

        memcpy_s(buffer, size, (void*)&value, sizeof(U8));
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, const char* value) {

        U64 len = strlen(value);

        memcpy_s(buffer, size, value, len);
    }

    template <>
    static void WriteToBuffer(U8* buffer, U64 size, char* value) {

        U64 len = strlen(value);

        memcpy_s(buffer, size, value, len);
    }
}