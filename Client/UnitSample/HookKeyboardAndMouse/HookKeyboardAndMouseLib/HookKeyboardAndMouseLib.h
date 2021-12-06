#pragma once
#include <windows.h>
#include <functional>

LRESULT CALLBACK MyKeyboardHookProc(int, WPARAM, LPARAM);
LRESULT CALLBACK MyMouseHookProc(int, WPARAM, LPARAM);
int SetHook(HWND hWnd_, UINT keyboard, UINT mouse);
int SetHook2(std::function<void()> keyboardCallback, std::function<void()> mouseCallback);
int ResetHook();
int ResetHook2();
