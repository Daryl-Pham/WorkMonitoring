#include "stdafx.h"
#include "WindowsHook.h"
#include <BizTeleworkHook.h>

WindowsHook::WindowsHook()
{
}

WindowsHook::~WindowsHook()
{
}

BOOL WindowsHook::SetHookForKeyboard(HWND hWnd, UINT keyboardMessage)
{
    return BthSetHookForKeyboard(hWnd, keyboardMessage);
}

BOOL WindowsHook::SetHookForMouse(HWND hWnd, UINT mouseMessage)
{
    return BthSetHookForMouse(hWnd, mouseMessage);
}

BOOL WindowsHook::UnsetHookForKeyboard()
{
    return BthUnsetHookForKeyboard();
}

BOOL WindowsHook::UnsetHookForMouse()
{
    return BthUnsetHookForMouse();
}
