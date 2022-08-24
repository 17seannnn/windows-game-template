/* ====== INCLUDES ====== */
#define INITGUID
#include "Sound.h"
#undef INITGUID

/* ====== VARIABLES ====== */
LPDIRECTSOUND Sound::m_pDSound = NULL;

/* ====== METHODS ====== */
b32 Sound::StartUp(HWND hWindow)
{
    // Start up DirectSound
    if ( FAILED(DirectSoundCreate(NULL, &m_pDSound, NULL)) )
        return false;

    // Set cooperative level
    if ( FAILED(m_pDSound->SetCooperativeLevel(hWindow, DSSCL_NORMAL)) )
        return false;

    // Log::Note(Log::CHANNEL_SOUND, Log::PRIORITY_NOTE, "Module started");

    return true;
}

void Sound::ShutDown()
{
    if (m_pDSound)
    {
        m_pDSound->Release();
        m_pDSound = NULL;
    }

    // Log::Note(Log::CHANNEL_SOUND, Log::PRIORITY_NOTE, "Module shut down");
}
