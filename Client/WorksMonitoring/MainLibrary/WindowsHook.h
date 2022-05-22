#pragma once

#include "WindowsHookInterface.h"

class WindowsHook : public WindowsHookInterface
{
public:
	WindowsHook();
	~WindowsHook();

	BOOL SetHookForKeyboard(HWND hWnd, UINT keyboardMessage) override;
	BOOL SetHookForMouse(HWND hWnd, UINT mouseMessage) override;
	BOOL UnsetHookForKeyboard() override;
	BOOL UnsetHookForMouse() override;
};
