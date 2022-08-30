#ifndef CLOCK_H_
#define CLOCK_H_

#include "Types.h"

class Clock
{
    u32 m_startTime;
    u32 m_msSyncDelay;
public:
    b32 StartUp(s32 fps);
    void ShutDown();

    f32 GetDelta();
    void Sync() const;
};

extern Clock g_clockModule;

#endif CLOCK_H_