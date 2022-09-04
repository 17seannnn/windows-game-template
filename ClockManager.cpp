/* ====== INCLUDES ====== */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "ClockManager.h"

/* ====== VARIABLES ====== */
ClockManager g_clockMgr;

/* ====== METHODS ====== */
b32 ClockManager::StartUp(s32 fps)
{
    m_startTime = GetTickCount();
    m_msSyncDelay = (u32)(1000.0f / fps);

    return true;
}

void ClockManager::ShutDown()
{
}

f32 ClockManager::GetDelta()
{
    u32 curTime = GetTickCount();
    f32 dtTime = (f32)(curTime - m_startTime);
    m_startTime = curTime;

    return dtTime;
}

void ClockManager::Sync() const
{
    while (GetTickCount() - m_startTime < m_msSyncDelay)
        {}
}