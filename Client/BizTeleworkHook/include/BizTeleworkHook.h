#pragma once
#include <Windows.h>

/**
 * Set hook for keyboard. You can get keyboard event by catching keyboardMessage.
 * @param hWnd Target of PostMessage().
 * @param keyboardMessage Target of message.
 */
BOOL BthSetHookForKeyboard(HWND hWnd, UINT keyboardMessage);

/**
 * Set hook for keyboard. You can get mouse event by catching mouseMessage.
 * @param hWnd Target of PostMessage().
 * @param mouseMessage Target of message.
 */
BOOL BthSetHookForMouse(HWND hWnd, UINT mouseMessage);

/**
 * Unset keyboard event hook.
 */
BOOL BthUnsetHookForKeyboard();

/**
 * Unset mouse event hook.
 */
BOOL BthUnsetHookForMouse();
