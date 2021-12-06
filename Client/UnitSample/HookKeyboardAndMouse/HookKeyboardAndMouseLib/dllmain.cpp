// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include <tchar.h>

#include "HookKeyboardAndMouseLib.h"

// 参考 http://www.kumei.ne.jp/c_lang/sdk2/sdk_161.htm


#pragma data_seg("MY_DATA")
HHOOK hMyKeyboardHook = 0;
HHOOK hMyMouseHook = 0;
HWND hWnd = 0;
UINT msgKeyboard = 0;
UINT msgMouse = 0;

HHOOK hMyKeyboardHook2 = 0;
HHOOK hMyMouseHook2 = 0;
std::function<void()> keyboardCallback = nullptr;
std::function<void()> mouseCallback = nullptr;
HINSTANCE hInst;
#pragma data_seg()


LRESULT CALLBACK MyKeyboardHookProc(int nCode, WPARAM wp, LPARAM lp);
LRESULT CALLBACK MyMouseHookProc(int nCode, WPARAM wp, LPARAM lp);
LRESULT CALLBACK MyKeyboardHookProc2(int nCode, WPARAM wp, LPARAM lp);
LRESULT CALLBACK MyMouseHookProc2(int nCode, WPARAM wp, LPARAM lp);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hInst = hModule;
        OutputDebugString(_T("アタッチしました"));
        break;
    case DLL_PROCESS_DETACH:
        OutputDebugString(_T("デタッチしました"));
        break;
    }
    return TRUE;
}

int SetHook(HWND hWnd_, UINT keyboard, UINT mouse)
{
    hWnd = hWnd_;
    msgKeyboard = keyboard;
    msgMouse = mouse;

    hMyKeyboardHook = SetWindowsHookEx(WH_KEYBOARD, MyKeyboardHookProc, hInst, 0);
    if (hMyKeyboardHook == NULL)
    {
        OutputDebugString(_T("キーボードフック失敗"));
    }
    else
    {
        OutputDebugString(_T("キーボードフック成功"));
    }

    hMyMouseHook = SetWindowsHookEx(WH_MOUSE, MyMouseHookProc, hInst, 0);
    if (hMyMouseHook == NULL)
    {
        OutputDebugString(_T("マウスフック失敗"));
    }
    else
    {
        OutputDebugString(_T("マウスフック成功"));
    }

    return 0;
}

int SetHook2(std::function<void()> keyboardCallback_, std::function<void()> mouseCallback_)
{

    keyboardCallback = keyboardCallback_;
    mouseCallback = mouseCallback_;
    keyboardCallback();
    mouseCallback();
    hMyKeyboardHook2 = SetWindowsHookEx(WH_KEYBOARD, MyKeyboardHookProc2, hInst, 0);
    if (hMyKeyboardHook2 == NULL)
    {
        OutputDebugString(_T("キーボードフック失敗"));
    }
    else
    {
        OutputDebugString(_T("キーボードフック成功"));
    }

    hMyMouseHook2 = SetWindowsHookEx(WH_MOUSE, MyMouseHookProc2, hInst, 0);
    if (hMyMouseHook2 == NULL)
    {
        OutputDebugString(_T("マウスフック失敗"));
    }
    else
    {
        OutputDebugString(_T("マウスフック成功"));
    }


    return 0;
}

int ResetHook()
{
    hWnd = 0;
    msgKeyboard = 0;
    msgMouse = 0;

    if (UnhookWindowsHookEx(hMyKeyboardHook) != 0)
    {
        OutputDebugString(_T("キーボードフック解除成功"));
    }
    else
    {
        OutputDebugString(_T("キーボードフック解除失敗"));
    }

    if (UnhookWindowsHookEx(hMyMouseHook) != 0)
    {
        OutputDebugString(_T("マウスフック解除成功"));
    }
    else
    {
        OutputDebugString(_T("マウスフック解除失敗"));
    }
    return 0;
}

int ResetHook2()
{
    keyboardCallback = nullptr;
    mouseCallback = nullptr;

    if (UnhookWindowsHookEx(hMyKeyboardHook2) != 0)
    {
        OutputDebugString(_T("キーボードフック解除成功"));
    }
    else
    {
        OutputDebugString(_T("キーボードフック解除失敗"));
    }

    if (UnhookWindowsHookEx(hMyMouseHook2) != 0)
    {
        OutputDebugString(_T("マウスフック解除成功"));
    }
    else
    {
        OutputDebugString(_T("マウスフック解除失敗"));
    }
    return 0;
}

LRESULT CALLBACK MyKeyboardHookProc(int nCode, WPARAM wp, LPARAM lp)
{
    if (nCode >= 0)
    {
        OutputDebugString(_T("MyKeyboardHookProc() called.\n"));
        if (hWnd && msgKeyboard)
        {
            PostMessage(hWnd, msgKeyboard, wp, lp);
        }
    }
    return CallNextHookEx(hMyKeyboardHook, nCode, wp, lp);
}

LRESULT CALLBACK MyMouseHookProc(int nCode, WPARAM wp, LPARAM lp)
{
    if (nCode >= 0)
    {
        wchar_t str[256] = { 0 };
        LPMOUSEHOOKSTRUCT m;
        m = reinterpret_cast<LPMOUSEHOOKSTRUCT>(lp);
        wsprintf(str, _T("MyMouseHookProc(%ld, %ld)\n"), m->pt.x, m->pt.y);
        OutputDebugString(str);

        if (hWnd && msgMouse)
        {
            PostMessage(hWnd, msgMouse, wp, lp);
        }
    }
    return CallNextHookEx(hMyMouseHook, nCode, wp, lp);
}

LRESULT CALLBACK MyKeyboardHookProc2(int nCode, WPARAM wp, LPARAM lp)
{
    if (nCode >= 0)
    {
        OutputDebugString(_T("MyKeyboardHookProc2() called.\n"));

        if (keyboardCallback)
        {
            OutputDebugString(_T("MyKeyboardHookProc2(2) called.\n"));
            keyboardCallback();
        }
    }
    return CallNextHookEx(hMyKeyboardHook, nCode, wp, lp);
}

LRESULT CALLBACK MyMouseHookProc2(int nCode, WPARAM wp, LPARAM lp)
{
    if (nCode >= 0)
    {
        wchar_t str[256] = { 0 };
        LPMOUSEHOOKSTRUCT m;
        m = reinterpret_cast<LPMOUSEHOOKSTRUCT>(lp);
        wsprintf(str, _T("MyMouseHookProc2(%ld, %ld)\n"), m->pt.x, m->pt.y);
        OutputDebugString(str);

        if (mouseCallback)
        {
            mouseCallback();
        }
    }
    return CallNextHookEx(hMyMouseHook, nCode, wp, lp);
}
