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

LPDIRECTINPUTDEVICE8 Input::m_pDIMouse = NULL;
DIMOUSESTATE Input::m_mouseState;

/* ====== METHODS ====== */
b32 Input::StartUp(HINSTANCE hInstance, HWND hWindow)
{
    // Init DirectInput
    if ( FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDInput, NULL)) )
        return false;

    { // Create keyboard device
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
    }

    { // Create mouse device
        if ( FAILED(m_pDInput->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL)) )
            return false;

        // Set cooperative level
        if ( FAILED(m_pDIMouse->SetCooperativeLevel(hWindow, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)) )
            return false;

        // Set data format
        if ( FAILED(m_pDIMouse->SetDataFormat(&c_dfDIMouse)) )
            return false;

        // Capture device
        if ( FAILED(m_pDIMouse->Acquire()) )
            return false;

        // Get mouse state
        if ( FAILED(m_pDIMouse->GetDeviceState(sizeof(m_mouseState), (LPVOID)&m_mouseState)) )
            return false;
    }

    Log::Note(Log::CHANNEL_INPUT, Log::PRIORITY_NOTE, "Module started");

    return true;
}

void Input::ShutDown()
{
    if (m_pDIMouse)
    {
        m_pDIMouse->Unacquire();
        m_pDIMouse->Release();
        m_pDIMouse = NULL;
    }

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

    Log::Note(Log::CHANNEL_INPUT, Log::PRIORITY_NOTE, "Module shut down");
}

b32 Input::HandleEvents()
{
    // Try to get keyboard state
    HRESULT hRes;
    while ( DIERR_INPUTLOST == (hRes = m_pDIKey->GetDeviceState(sizeof(m_keyState),
                                                                (LPVOID)m_keyState)) )
        if ( FAILED(m_pDIKey->Acquire()) )
            return false;

    // If we got different from INPUTLOST error
    if ( FAILED(hRes) )
        return false;

    // Mouse state
    while ( DIERR_INPUTLOST == (hRes = m_pDIMouse->GetDeviceState(sizeof(m_mouseState),
                                                                  (LPVOID)&m_mouseState)) )
        if ( FAILED(m_pDIMouse->Acquire()) )
            return false;

    if ( FAILED(hRes) )
        return false;

    return true;
}