/* ====== INCLUDES ====== */
#include "Clock.h"

/* ====== VARIABLES ====== */
Clock g_clockModule;

/* ====== METHODS ====== */
b32 Clock::StartUp(s32 fps)
{
    SetModuleInfo("Clock", Log::CHANNEL_CLOCK);

    m_startTime = GetTickCount();
    m_msSyncDelay = (u32)(1000.0f / fps);

    AddNote(Log::PRIORITY_NOTE, "Hello, world");

    return true;
}

void Clock::ShutDown()
{
    g_logModule.Note(Log::CHANNEL_CLOCK, Log::PRIORITY_NOTE, "Module shut down");
}

f32 Clock::GetDelta()
{
    u32 curTime = GetTickCount();
    f32 dtTime = (f32)(curTime - m_startTime);
    m_startTime = curTime;

    return dtTime;
}

void Clock::Sync() const
{
    while (GetTickCount() - m_startTime < m_msSyncDelay)
        {}
}