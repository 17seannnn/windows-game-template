/* ====== INCLUDES ====== */
#define INITGUID
#include "Sound.h"
#undef INITGUID

/* ====== DEFINES ====== */
#define _DSBVOLUME100(VOL) ((VOL) == 0 ? DSBVOLUME_MIN : -50 * (100 - (VOL)) )

/* ====== VARIABLES ====== */
LPDIRECTSOUND Sound::m_pDSound = NULL;
LPDIRECTSOUNDBUFFER Sound::m_pDSBuffer = NULL;

// DEBUG(sean)
#include <math.h>

/* ====== METHODS ====== */
b32 Sound::StartUp(HWND hWindow)
{
    // Start up DirectSound
    if ( FAILED(DirectSoundCreate(NULL, &m_pDSound, NULL)) )
        return false;

    // Set cooperative level
    if ( FAILED(m_pDSound->SetCooperativeLevel(hWindow, DSSCL_NORMAL)) )
        return false;

    // Create audio buffer
    DSBUFFERDESC DSBufferDesc;
    WAVEFORMATEX waveFormat;

    // Set wave format
    memset(&waveFormat, 0, sizeof(waveFormat));
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 1;
    waveFormat.nSamplesPerSec = 11025; // 11 kHz
    waveFormat.nBlockAlign = 1;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nChannels;
    waveFormat.wBitsPerSample = 8;
    waveFormat.cbSize = 0;

    // Set buffer description
    memset(&DSBufferDesc, 0, sizeof(DSBufferDesc));
    DSBufferDesc.dwSize = sizeof(DSBufferDesc);
    DSBufferDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
    DSBufferDesc.dwBufferBytes = 22050; // 1kHz * 2
    DSBufferDesc.lpwfxFormat = &waveFormat;

    // Create buffer
    if ( FAILED(m_pDSound->CreateSoundBuffer(&DSBufferDesc, &m_pDSBuffer, NULL)) )
        return false;

    // DEBUG(sean)
    u8 *audioBuffer1, *audioBuffer2;
    lu32 audioLen1, audioLen2;

    u8* temp = new u8[22050];
    for (s32 i = 0; i < 22050; ++i)
        temp[i] = (u8)(127*sinf(6.28f*((f32)(i%110))/(f32)110));

    m_pDSBuffer->Lock(0, 22050,
                      (void**)&audioBuffer1, &audioLen1,
                      (void**)&audioBuffer2, &audioLen2,
                      DSBLOCK_ENTIREBUFFER);

    memcpy(audioBuffer1, temp, audioLen1);
    memcpy(audioBuffer2, temp+audioLen1, audioLen2);

    m_pDSBuffer->Unlock(audioBuffer1, audioLen1, audioBuffer2, audioLen2);

    delete[] temp;

    m_pDSBuffer->SetVolume(_DSBVOLUME100(1));
    m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);

    // Log::Note(Log::CHANNEL_SOUND, Log::PRIORITY_NOTE, "Module started");

    return true;
}

void Sound::ShutDown()
{
    if (m_pDSBuffer)
    {
        m_pDSBuffer->Stop();
        m_pDSBuffer->Release();
        m_pDSBuffer = NULL;
    }

    if (m_pDSound)
    {
        m_pDSound->Release();
        m_pDSound = NULL;
    }

    // Log::Note(Log::CHANNEL_SOUND, Log::PRIORITY_NOTE, "Module shut down");
}
