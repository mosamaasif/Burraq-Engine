#include <BRQ.h>
#include "Timer.h"

namespace BRQ {

    Timer::Timer() {

        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        m_Frequency = 1.0 / freq.QuadPart;

        Reset();
    }

    void Timer::Reset() {

        QueryPerformanceCounter(&m_Start);
    }

    F32 Timer::GetTime() {

        QueryPerformanceCounter(&m_End);
        U64 cycles = m_End.QuadPart - m_Start.QuadPart;
        return (F32)(cycles * m_Frequency) * 1000.0f;
    }
}