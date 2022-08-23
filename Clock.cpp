/* ====== INCLUDES ====== */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Log.h"

#include "Clock.h"

/* ====== VARIABLES ====== */
u32 Clock::m_startTime = GetTickCount();
u32 Clock::m_msSyncDelay = 0;

/* ====== METHODS ====== */
b32 Clock::StartUp(s32 fps)
{
    m_msSyncDelay = (u32)(1000.0f / fps);

    Log::Note(Log::CHANNEL_CLOCK, Log::PRIORITY_NOTE, "Module started");

    return true;
}

void Clock::ShutDown()
{
    Log::Note(Log::CHANNEL_CLOCK, Log::PRIORITY_NOTE, "Module shut down");
}

f32 Clock::GetDelta()
{
    u32 curTime = GetTickCount();
    f32 dtTime = (f32)(curTime - m_startTime);
    m_startTime = curTime;

    return dtTime;
}

void Clock::Sync()
{
    while (GetTickCount() - m_startTime < m_msSyncDelay)
        {}
}