/* ====== INCLUDES ====== */
#include "Log.h"

#define INITGUID
#include <objbase.h>
#include "Input.h"
#undef INITGUID

/* ====== VARIABLES ====== */
LPDIRECTINPUT8 Input::m_pDInput = NULL;
LPDIRECTINPUTDEVICE8 Input::m_pDIKey = NULL;
_DIKEYSTATE Input::m_keyState;

/* ====== METHODS ====== */
b32 Input::StartUp(HINSTANCE hInstance, HWND hWindow)
{
    // Init DirectInput
    if ( FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDInput, NULL)) )
        return false;

    // Create keyboard device
    if ( FAILED(m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pDIKey, NULL)) )
        return false;

    // Set cooperative level
    if ( FAILED(m_pDIKey->SetCooperativeLevel(hWindow, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)) )
        return false;

    // Set default data format for keyboard
    if ( FAILED(m_pDIKey->SetDataFormat(&c_dfDIKeyboard)) )
        return false;

    // Capture device
    if ( FAILED(m_pDIKey->Acquire()) )
        return false;

    // Get keyboard state
    if ( FAILED(m_pDIKey->GetDeviceState(sizeof(m_keyState), (LPVOID)m_keyState)) )
        return false;

    //Log::Note(Log::CHANNEL_INPUT, Log::PRIORITY_NOTE, "Module started");

    return true;
}

void Input::ShutDown()
{  
    if (m_pDIKey)
    {
        m_pDIKey->Unacquire();
        m_pDIKey->Release();
        m_pDIKey = NULL;
    }

    if (m_pDInput)
    {
        m_pDInput->Release();
        m_pDInput = NULL;
    }

    //Log::Note(Log::CHANNEL_INPUT, Log::PRIORITY_NOTE, "Module shut down");
}

b32 Input::HandleEvents()
{
    return SUCCEEDED(m_pDIKey->GetDeviceState(sizeof(m_keyState), (LPVOID)m_keyState));
}