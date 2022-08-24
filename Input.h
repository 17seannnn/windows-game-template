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

// Static class
class Input
{
    static LPDIRECTINPUT8 m_pDInput;

    static LPDIRECTINPUTDEVICE8 m_pDIKey;
    static _DIKEYSTATE m_keyState;

    static LPDIRECTINPUTDEVICE8 m_pDIMouse;
    static DIMOUSESTATE m_mouseState;
public:
    static b32 StartUp(HINSTANCE hInstance, HWND hWindow);
    static void ShutDown();

    static b32 HandleEvents();

    // Keyboard
    static b32 KeyDown(s32 key) { return m_keyState[key] & 0x80; }

    // Mouse
    static b32 MouseDown(s32 key) { return m_mouseState.rgbButtons[key] & 0x80; }
    static s32 GetMouseRelX() { return m_mouseState.lX; }
    static s32 GetMouseRelY() { return m_mouseState.lY; }
    static s32 GetMouseRelZ() { return m_mouseState.lZ; }
};

#endif // INPUT_H_
