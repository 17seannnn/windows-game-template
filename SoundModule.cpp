/* ====== TODO ======
 * - Maybe give classes instead of id's and classes will call Sound's functions
 * - Check if id == -1 and state == STATE_NULL
 * - GetCaps()
 * - GetStatus()
 */

/* ====== INCLUDES ====== */
#include <direct.h> // _getcwd()

#define INITGUID
#include "SoundModule.h"
#undef INITGUID

/* ====== DEFINES ====== */
#define MULTI_TO_WIDE(W, M) MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, M, -1, W, _MAX_PATH)

/* ====== VARIABLES ====== */
SoundModule g_soundModule;

/* ====== METHODS ====== */
b32 SoundModule::StartUp(HWND hWindow)
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
        m_aSounds[i].state = STATE_NULL;
    }

    // Inititialize DirectMusic
    if ( FAILED(CoInitialize(NULL)) )
    {
        // TODO(sean)
        return false;
    }

    // COM init
    if ( FAILED(CoCreateInstance(CLSID_DirectMusicPerformance,
                                 NULL,
                                 CLSCTX_INPROC,
                                 IID_IDirectMusicPerformance,
                                 (void**)&m_pDMPerf)) )
    {
        // TODO(sean)
        return false;
    }

    // Init DirectMusic Perfomance
    if ( FAILED(m_pDMPerf->Init(NULL, m_pDSound, hWindow)) )
    {
        // TODO(sean)
        return false;
    }

    // Add default port
    if ( FAILED(m_pDMPerf->AddPort(NULL)) )
    {
        // TODO(sean)
        return false;
    }

    // Initialize DirectMusic Loader
    if ( FAILED(CoCreateInstance(CLSID_DirectMusicLoader,
                                 NULL,
                                 CLSCTX_INPROC,
                                 IID_IDirectMusicLoader,
                                 (void**)&m_pDMLoader)) )
    {
        // TODO(sean)
        return false;
    }

    // Initialize array of midi
    for (s32 id = 0; id < MAX_MIDI; ++id)
    {
        m_aMIDI[id].pDMSeg = NULL;
        m_aMIDI[id].pDMSegState = NULL;
        m_aMIDI[id].state = STATE_NULL;
    }

    // g_logModule.Note(Log::CHANNEL_SOUND, Log::PRIORITY_NOTE, "Module started");

    return true;
}

void SoundModule::ShutDown()
{
    s32 i;

    // DirectMusic
    if (m_pDMPerf)
        m_pDMPerf->Stop(NULL, NULL, 0, 0);

    for (i = 0; i < MAX_MIDI; ++i)
    {
        if (m_aMIDI[i].state != STATE_NULL && m_aMIDI[i].pDMSeg)
        {
            m_aMIDI[i].pDMSeg->SetParam(GUID_Unload, -1, 0, 0, (void*)m_pDMPerf);
            m_aMIDI[i].pDMSeg->Release();
            m_aMIDI[i].pDMSeg = NULL;
        }
    }

    if (m_pDMLoader)
    {
        m_pDMLoader->Release();
        m_pDMLoader = NULL;
    }

    if (m_pDMPerf)
    {
        m_pDMPerf->CloseDown();
        m_pDMPerf->Release();
        m_pDMPerf = NULL;
    }

    CoUninitialize();

    // DirectSound
    for (i = 0; i < MAX_SOUNDS; ++i)
    {
        if (m_aSounds[i].state != STATE_NULL && m_aSounds[i].pDSBuffer)
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

    // g_logModule.Note(Log::CHANNEL_SOUND, Log::PRIORITY_NOTE, "Module shut down");
}

s32 SoundModule::LoadWAV(const char *fileName)
{
    // Find free sound id
    s32 soundID = -1;
    for (s32 i = 0; i < MAX_SOUNDS; ++i)
    {
        if (m_aSounds[i].state == STATE_NULL)
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
    m_aSounds[soundID].state = STATE_LOADED;

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

void SoundModule::UnloadBuffer(s32 id)
{
    m_aSounds[id].state = STATE_NULL;
    if (m_aSounds[id].pDSBuffer)
    {
        m_aSounds[id].pDSBuffer->Stop();
        m_aSounds[id].pDSBuffer->Release();
        m_aSounds[id].pDSBuffer = NULL;
    }
}

b32 SoundModule::PlayBuffer(s32 id, b32 loop)
{
    m_aSounds[id].state = STATE_PLAYING;
    return SUCCEEDED(m_aSounds[id].pDSBuffer->Play(0, 0, loop ? DSBPLAY_LOOPING : 0));
}

b32 SoundModule::StopBuffer(s32 id)
{
    m_aSounds[id].state = STATE_STOPPED;
    return SUCCEEDED(m_aSounds[id].pDSBuffer->Stop());
}

s32 SoundModule::LoadMIDI(const char* fileName)
{
    // Find free slot
    s32 id = -1;
    for (s32 i = 0; i < MAX_MIDI; ++i)
    {
        if (m_aMIDI[i].state == STATE_NULL)
        {
            id = i;
            break;
        }
    }

    if (id == -1)
        return -1;

    // Set loader's search directory
    char dirName[_MAX_PATH];
    WCHAR wDirName[_MAX_PATH];

    if (_getcwd(dirName, _MAX_PATH) == NULL)
    {
        // TODO(sean)
        return -1;
    }
    MULTI_TO_WIDE(wDirName, dirName);

    if ( FAILED(m_pDMLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wDirName, FALSE)) )
    {
        // TODO(sean)
        return -1;
    }

    // Convert file name
    WCHAR wFileName[_MAX_PATH];
    MULTI_TO_WIDE(wFileName, fileName);

    // Set object description
    DMUS_OBJECTDESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);

    desc.guidClass = CLSID_DirectMusicSegment;
    wcscpy(desc.wszFileName, wFileName);
    desc.dwValidData = DMUS_OBJ_CLASS|DMUS_OBJ_FILENAME;
    
    // Get segment
    IDirectMusicSegment* pSeg;
    if ( FAILED(m_pDMLoader->GetObject(&desc,
                                       IID_IDirectMusicSegment,
                                       (void**)&pSeg)) )
    {
        // TODO(sean)
        return -1;
    }

    // Segment is MIDI file
    if ( FAILED(pSeg->SetParam(GUID_StandardMIDIFile,
                               -1, 0, 0,
                               (void*)m_pDMPerf)) )
    {
        // TODO(sean)
        return -1;
    }

    // Set instruments
    if ( FAILED(pSeg->SetParam(GUID_Download,
                               -1, 0, 0,
                               (void*)m_pDMPerf)) )
    {
        // TODO(sean)
        return -1;
    }

    // Finally...
    m_aMIDI[id].pDMSeg = pSeg;
    m_aMIDI[id].pDMSegState = NULL;
    m_aMIDI[id].state = STATE_LOADED;

    return id;
}

void SoundModule::UnloadMIDI(s32 id) {}

b32 SoundModule::PlayMIDI(s32 id)
{
    m_aMIDI[id].state = STATE_PLAYING;
    return SUCCEEDED(m_pDMPerf->PlaySegment(m_aMIDI[id].pDMSeg, 0, 0, &m_aMIDI[id].pDMSegState));
}

b32 SoundModule::StopMIDI(s32 id)
{
    m_aMIDI[id].state = STATE_STOPPED;
    return SUCCEEDED(m_pDMPerf->Stop(m_aMIDI[id].pDMSeg, NULL, 0, 0));
}