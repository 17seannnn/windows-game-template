/* ====== INCLUDES ====== */
#include "Log.h"

#define INITGUID
#include "Input.h"
#undef INITGUID

/* ====== VARIABLES ====== */
LPDIRECTINPUT8 Input::m_pDInput = NULL;

/* ====== METHODS ====== */
b32 Input::StartUp(HINSTANCE hInstance)
{
    if ( FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDInput, NULL)) )
        return false;

    //Log::Note(Log::CHANNEL_INPUT, Log::PRIORITY_NOTE, "Module started");

    return true;
}

void Input::ShutDown()
{
    if (m_pDInput)
    {
        m_pDInput->Release();
        m_pDInput = NULL;
    }

    //Log::Note(Log::CHANNEL_INPUT, Log::PRIORITY_NOTE, "Module shut down");
}