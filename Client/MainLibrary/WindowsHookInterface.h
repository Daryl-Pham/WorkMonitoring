#pragma once

#include <Windows.h>

class WindowsHookInterface
{
public:
	/**
	* Set hook for keyboard. You can get keyboard event by catching keyboardMessage.
	* @param hWnd Target of PostMessage().
	* @param keyboardMessage Target of message.
	*/
	virtual BOOL SetHookForKeyboard(HWND hWnd, UINT keyboardMessage) = 0;

	/**
	* Set hook for keyboard. You can get mouse event by catching mouseMessage.
	* @param hWnd Target of PostMessage().
	* @param mouseMessage Target of message.
	*/
	virtual BOOL SetHookForMouse(HWND hWnd, UINT mouseMessage) = 0;

	/**
	* Unset keyboard event hook.
	*/
	virtual BOOL UnsetHookForKeyboard() = 0;

	/**
	* Unset mouse event hook.
	*/
	virtual BOOL UnsetHookForMouse() = 0;

	virtual ~WindowsHookInterface() = default;
};
