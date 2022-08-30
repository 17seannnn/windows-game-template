#ifndef INPUT_H_
#define INPUT_H_

/* ====== INCLUDES ====== */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DIRECTINPUT_VERSION 0X0800
#include <dinput.h>

#include "Types.h"

/* ====== DEFINES ====== */
typedef u8 _DIKEYSTATE[256];

enum eMouseButton
{
    _DIM_LEFT   = 0,
    _DIM_RIGHT  = 1,
    _DIM_MIDDLE = 2
};

/* ====== STRUCTURES ====== */
class Input
{
    LPDIRECTINPUT8 m_pDInput;

    LPDIRECTINPUTDEVICE8 m_pDIKey;
    _DIKEYSTATE m_keyState;

    LPDIRECTINPUTDEVICE8 m_pDIMouse;
    DIMOUSESTATE m_mouseState;
public:
    b32 StartUp(HINSTANCE hInstance, HWND hWindow);
    void ShutDown();

    b32 HandleEvents();

    // Keyboard
    b32 KeyDown(s32 key) { return m_keyState[key] & 0x80; }

    // Mouse
    b32 MouseDown(s32 key) { return m_mouseState.rgbButtons[key] & 0x80; }
    s32 GetMouseRelX() { return m_mouseState.lX; }
    s32 GetMouseRelY() { return m_mouseState.lY; }
    s32 GetMouseRelZ() { return m_mouseState.lZ; }
};

extern Input g_inputModule;

#endif // INPUT_H_
