#ifndef SOUND_H_
#define SOUND_H_

/* ====== INCLUDES ====== */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#include <dsound.h>
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>

#include "Types.h"

/* ====== DEFINES====== */
#define MAX_SOUNDS 256
#define MAX_MIDI 64

/* ====== STRUCTURES ====== */
class Sound
{
private:
    enum eState
    {
        STATE_NULL = 0,
        STATE_LOADED,
        STATE_PLAYING,
        STATE_STOPPED
    };

    struct Buffer
    {
        LPDIRECTSOUNDBUFFER pDSBuffer;
        eState state;
        s32 rate;
        s32 size;
    };

    struct Midi
    {
        IDirectMusicSegment* pDMSeg;
        IDirectMusicSegmentState* pDMSegState;
        eState state;
    };

    LPDIRECTSOUND m_pDSound;
    Buffer m_aSounds[MAX_SOUNDS];

    IDirectMusicPerformance* m_pDMPerf;
    IDirectMusicLoader* m_pDMLoader;
    Midi m_aMIDI[MAX_MIDI];

public:
    b32 StartUp(HWND hWindow);
    void ShutDown();

    /* Sound */
    // Returns buffer's ID, -1 on error
    s32 LoadWAV(const char* fileName);
    void UnloadBuffer(s32 id);

    b32 PlayBuffer(s32 id, b32 loop = false);
    b32 StopBuffer(s32 id);

    b32 SetBufferVolume(s32 id, s32 volume) // volume = [0..100]
        { return m_aSounds[id].pDSBuffer->SetVolume(volume > 0 ? -50 * (100-volume) : DSBVOLUME_MIN); }
    b32 SetBufferFrequency(s32 id, s32 frequency) // frequency = [100..100000]
        { return m_aSounds[id].pDSBuffer->SetFrequency(frequency); }
    b32 SetBufferPan(s32 id, s32 pan) // pan = [-10000..10000]
        { return m_aSounds[id].pDSBuffer->SetPan(pan); }

    /* Midi */
    // -1 on error
    s32 LoadMIDI(const char* fileName);
    void UnloadMIDI(s32 id);

    b32 PlayMIDI(s32 id);
    b32 StopMIDI(s32 id);
};

extern Sound g_soundModule;

#endif // SOUND_H_