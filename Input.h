#ifndef INPUT_H_
#define INPUT_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DIRECTINPUT_VERSION 0X0800
#include <dinput.h>

#include "Types.h"

// Static class
class Input
{
    static LPDIRECTINPUT8 m_pDInput;
public:
    static b32 StartUp(HINSTANCE hInstance);
    static void ShutDown();
};

#endif // INPUT_H_
