/* ====== TODO ======
 * - My structure for sound buffers
 * - GetCaps()
 * - GetStatus()
 */

/* ====== INCLUDES ====== */
#define INITGUID
#include "Sound.h"
#undef INITGUID

/* ====== DEFINES ====== */
#define _DSBVOLUME100(VOL) ((VOL) == 0 ? DSBVOLUME_MIN : -50 * (100 - (VOL)) )

/* ====== VARIABLES ====== */
LPDIRECTSOUND Sound::m_pDSound = NULL;
Sound::Buffer Sound::m_aSounds[MAX_SOUNDS];

/* ====== METHODS ====== */
b32 Sound::StartUp(HWND hWindow)
{
    // Start up DirectSound
    if ( FAILED(DirectSoundCreate(NULL, &m_pDSound, NULL)) )
        return false;

    // Set cooperative level
    if ( FAILED(m_pDSound->SetCooperativeLevel(hWindow, DSSCL_NORMAL)) )
        return false;

    // Initialize buffers
    for (s32 i = 0; i < MAX_SOUNDS; ++i)
    {
        m_aSounds[i].pDSBuffer = NULL;
        m_aSounds[i].state = Buffer::STATE_NULL;
    }

    // Log::Note(Log::CHANNEL_SOUND, Log::PRIORITY_NOTE, "Module started");

    return true;
}

void Sound::ShutDown()
{
    for (s32 i = 0; i < MAX_SOUNDS; ++i)
    {
        if (m_aSounds[i].pDSBuffer)
        {
            m_aSounds[i].pDSBuffer->Stop();
            m_aSounds[i].pDSBuffer->Release();
            m_aSounds[i].pDSBuffer = NULL;
        }
    }

    if (m_pDSound)
    {
        m_pDSound->Release();
        m_pDSound = NULL;
    }

    // Log::Note(Log::CHANNEL_SOUND, Log::PRIORITY_NOTE, "Module shut down");
}

s32 Sound::LoadWAV(const char *fileName)
{
    // Find free sound id
    s32 soundID = -1;
    for (s32 i = 0; i < MAX_SOUNDS; ++i)
    {
        if (m_aSounds[i].state == Buffer::STATE_NULL)
        {
            soundID = i;
            break;
        }
    }

    if (soundID == -1)
        return -1;

    // Set parent, child structures
    MMCKINFO parent, child;

    parent.ckid = (FOURCC)0;
    parent.cksize = 0;
    parent.fccType = (FOURCC)0;
    parent.dwFlags = 0;
    child = parent;

    // Open WAV
    HMMIO hWAV = mmioOpen((char*)fileName, NULL, MMIO_READ | MMIO_ALLOCBUF);
    if (!hWAV)
    {
        // TODO(sean) log
        return -1;
    }

    // RIFF
    parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(hWAV, &parent, NULL, MMIO_FINDRIFF))
    {
        mmioClose(hWAV, 0);
        // TODO(sean) log
        return -1;
    }

    // FMT
    WAVEFORMATEX waveFormat;

    child.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(hWAV, &child, &parent, 0))
    {
        mmioClose(hWAV, 0);
        // TODO(sean) log
        return -1;
    }

    if (sizeof(waveFormat) != mmioRead(hWAV, (s8*)&waveFormat, sizeof(waveFormat)))
    {
        mmioClose(hWAV, 0);
        // TODO(sean) log
        return -1;
    }

    if (waveFormat.wFormatTag != WAVE_FORMAT_PCM)
    {
        mmioClose(hWAV, 0);
        // TODO(sean) log
        return -1;
    }

    // Data section
    if (mmioAscend(hWAV, &child, 0))
    {
        mmioClose(hWAV, 0);
        // TODO(sean) log
        return -1;
    }

    child.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(hWAV, &child, &parent, MMIO_FINDCHUNK))
    {
        mmioClose(hWAV, 0);
        // TODO(sean) log
        return -1;
    }

    // Allocate buffer for wave data
    u8* buffer = new u8[child.cksize];

    // Read wave data
    mmioRead(hWAV, (s8*)buffer, child.cksize);
    mmioClose(hWAV, 0);

    m_aSounds[soundID].rate = waveFormat.nSamplesPerSec;
    m_aSounds[soundID].size = child.cksize;
    m_aSounds[soundID].state = Buffer::STATE_LOADED;

    // Set wave format
    memset(&waveFormat, 0, sizeof(waveFormat));
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 1;
    waveFormat.nSamplesPerSec =  11025;
    waveFormat.nBlockAlign = 1;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.wBitsPerSample = 8;
    waveFormat.cbSize = 0;

    // Set buffer description
    DSBUFFERDESC DSBufferDesc;
    memset(&DSBufferDesc, 0, sizeof(DSBufferDesc));

    DSBufferDesc.dwSize = sizeof(DSBufferDesc);
    DSBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY |
                           DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
    DSBufferDesc.dwBufferBytes = child.cksize;
    DSBufferDesc.lpwfxFormat = &waveFormat;

    // Create sound buffer
    if ( FAILED(m_pDSound->CreateSoundBuffer(&DSBufferDesc, &m_aSounds[soundID].pDSBuffer, NULL)) )
    {
        delete[] buffer;
        // TODO(sean) log
        return -1;
    }

    // Copy wave data in sound buffer
    u8 *audioBuffer1, *audioBuffer2;
    lu32 audioLen1, audioLen2;

    if ( FAILED(m_aSounds[soundID].pDSBuffer->Lock(0, child.cksize,
                                                   (void**)&audioBuffer1, &audioLen1,
                                                   (void**)&audioBuffer2, &audioLen2,
                                                   DSBLOCK_FROMWRITECURSOR)) )
    {
        delete[] buffer;
        // TODO(sean) log
        return -1;
    }

    memcpy(audioBuffer1, buffer, audioLen1);
    memcpy(audioBuffer2, buffer+audioLen1, audioLen2);

    m_aSounds[soundID].pDSBuffer->Unlock(audioBuffer1, audioLen1, audioBuffer2, audioLen2);

    // Free buffer
    delete[] buffer;

    return soundID;
}

void Sound::Unload(s32 id)
{
    m_aSounds[id].state = Buffer::STATE_NULL;
    if (m_aSounds[id].pDSBuffer)
    {
        m_aSounds[id].pDSBuffer->Stop();
        m_aSounds[id].pDSBuffer->Release();
        m_aSounds[id].pDSBuffer = NULL;
    }
}