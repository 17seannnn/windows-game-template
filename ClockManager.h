#ifndef CLOCKMANAGER_H_
#define CLOCKMANAGER_H_

/* ====== INCLUDES ====== */
#include "Types.h"

/* ====== STRUCTURES ====== */
class ClockManager
{
    u32 m_startTime;
    u32 m_msSyncDelay;
public:
    b32 StartUp(s32 fps);
    void ShutDown();

    f32 GetDelta();
    void Sync() const;
};

extern ClockManager g_clockMgr;

#endif // CLOCKMANAGER_H_