#pragma once

#include <BRQ.h>

namespace BRQ {

    class Timer {
    
    private:
        LARGE_INTEGER   m_Start;
        LARGE_INTEGER   m_End;
        F64             m_Frequency;

    public:
        Timer();
        ~Timer() = default;

        void Reset();

        F32 GetTime();
    };
}