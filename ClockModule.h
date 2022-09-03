#ifndef CLOCKMODULE_H_
#define CLOCKMODULE_H_

/* ====== INCLUDES ====== */
#include "EngineModule.h"

/* ====== STRUCTURES ====== */
class ClockModule : public EngineModule
{
    u32 m_startTime;
    u32 m_msSyncDelay;
public:
    b32 StartUp(s32 fps);
    void ShutDown();

    f32 GetDelta();
    void Sync() const;
};

extern ClockModule g_clockModule;

#endif CLOCKMODULE_H_