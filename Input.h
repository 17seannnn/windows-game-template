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

/* ====== STRUCTURES ====== */

// Static class
class Input
{
    static LPDIRECTINPUT8 m_pDInput;
    static LPDIRECTINPUTDEVICE8 m_pDIKey;
    static _DIKEYSTATE m_keyState;
public:
    static b32 StartUp(HINSTANCE hInstance, HWND hWindow);
    static void ShutDown();

    static b32 HandleEvents();
    static b32 KeyDown(s32 key) { return m_keyState[key] & 0x80; }
};

#endif // INPUT_H_
