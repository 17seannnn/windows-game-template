#ifndef SOUND_H_
#define SOUND_H_

/* ====== INCLUDES ====== */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <mmsystem.h>
#include <dsound.h>

#include "Types.h"

/* ====== STRUCTURES ====== */
// Static class
class Sound
{
    static LPDIRECTSOUND m_pDSound;
public:
    static b32 StartUp(HWND hWindow);
    static void ShutDown();
};

#endif // SOUND_H_