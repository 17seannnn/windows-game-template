#ifndef SOUND_H_
#define SOUND_H_

/* ====== INCLUDES ====== */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <mmsystem.h>
#include <dsound.h>

#include "Types.h"

/* ====== DEFINES====== */
#define MAX_SOUNDS 256

/* ====== STRUCTURES ====== */
// Static class
class Sound
{
    struct Buffer
    {
        enum eState
        {
            STATE_NULL = 0,
            STATE_LOADED,
            STATE_PLAYING,
            STATE_STOPPED
        };

        LPDIRECTSOUNDBUFFER pDSBuffer;
        eState state;
        s32 rate;
        s32 size;
    };

    static LPDIRECTSOUND m_pDSound;
    static Buffer m_aSounds[MAX_SOUNDS];

public:
    static b32 StartUp(HWND hWindow);
    static void ShutDown();

    // Returns buffer's ID, -1 on error
    static s32 LoadWAV(const char* fileName);
    static void Unload(s32 id);

    static b32 Play(s32 id, b32 loop = false);
    static b32 Stop(s32 id);

    static b32 SetVolume(s32 id, s32 volume) // volume = [0..100]
        { return m_aSounds[id].pDSBuffer->SetVolume(volume > 0 ? -50 * (100-volume) : DSBVOLUME_MIN); }
    static b32 SetFrequency(s32 id, s32 frequency) // frequency = [100..100000]
        { return m_aSounds[id].pDSBuffer->SetFrequency(frequency); }
    static b32 SetPan(s32 id, s32 pan) // pan = [-10000..10000]
        { return m_aSounds[id].pDSBuffer->SetPan(pan); }
};

#endif // SOUND_H_