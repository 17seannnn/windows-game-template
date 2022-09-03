/* TODO
 * - ClockModule -> ClockMgr
 */

/* ====== INCLUDES ====== */
#include "ClockModule.h"

/* ====== VARIABLES ====== */
ClockModule g_clockModule;

/* ====== METHODS ====== */
b32 ClockModule::StartUp(s32 fps)
{
    SetModuleInfo("Clock Module", DebugLogManager::CHANNEL_CLOCK);

    m_startTime = GetTickCount();
    m_msSyncDelay = (u32)(1000.0f / fps);

    AddNote(DebugLogManager::PRIORITY_NOTE, "Module started");

    return true;
}

void ClockModule::ShutDown()
{
    AddNote(DebugLogManager::PRIORITY_NOTE, "Module shut down");
}

f32 ClockModule::GetDelta()
{
    u32 curTime = GetTickCount();
    f32 dtTime = (f32)(curTime - m_startTime);
    m_startTime = curTime;

    return dtTime;
}

void ClockModule::Sync() const
{
    while (GetTickCount() - m_startTime < m_msSyncDelay)
        {}
}