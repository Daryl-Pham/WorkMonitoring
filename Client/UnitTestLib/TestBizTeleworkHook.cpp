#include "stdafx.h"

#include "gtest/gtest.h"
#include <BizTeleworkHook.h>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "WindowProcRunner.h"

// http://eternalwindows.jp/winbase/window/window17.html



namespace
{

const UINT MESSAGE_KEYBOARD = WM_APP;
const UINT MESSAGE_MOUSE = WM_APP + 1;
bool calledMessageKeyboard = false;
bool calledMessageMouse = false;


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case MESSAGE_KEYBOARD:
        OutputDebugString(_T("MESSAGE_KEYBOARD"));
        calledMessageKeyboard = true;
        break;
    case MESSAGE_MOUSE:
        OutputDebugString(_T("MESSAGE_MOUSE"));
        calledMessageMouse = true;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


} // namespace


TEST(testBizTeleworkHook_NotRunOnCI, Keyboard)
{
    calledMessageKeyboard = false;
    calledMessageMouse = false;
    std::unique_ptr<std::thread> messageThread;

    HWND hWnd = StartWindowProc(&messageThread, WindowProc);
    ASSERT_TRUE(hWnd != NULL);

    EXPECT_TRUE(BthSetHookForKeyboard(hWnd, MESSAGE_KEYBOARD));

    Sleep(1 * 1000);

    keybd_event('A', 0, 0, 0); // Down key A
    keybd_event('A', 0, KEYEVENTF_KEYUP, 0); // Up key A

    Sleep(1 * 1000);

    PostMessage(hWnd, WM_CLOSE, 0, 0);

    messageThread->join();
    messageThread = nullptr;
    EXPECT_TRUE(calledMessageKeyboard);
    EXPECT_FALSE(calledMessageMouse);
    EXPECT_EQ(1, BthUnsetHookForKeyboard());
}


TEST(testBizTeleworkHook, Mouse)
{
    calledMessageKeyboard = false;
    calledMessageMouse = false;
    std::unique_ptr<std::thread> messageThread;

    HWND hWnd = StartWindowProc(&messageThread, WindowProc);
    ASSERT_TRUE(hWnd != NULL);

    EXPECT_TRUE(BthSetHookForMouse(hWnd, MESSAGE_MOUSE));

    Sleep(1 * 1000);

    keybd_event('A', 0, 0, 0); // Down key A
    keybd_event('A', 0, KEYEVENTF_KEYUP, 0); // Up key A

    Sleep(1 * 1000);

    PostMessage(hWnd, WM_CLOSE, 0, 0);

    messageThread->join();
    messageThread = nullptr;
    EXPECT_FALSE(calledMessageKeyboard);
    // TODO EXPECT_TRUE(calledMessageMouse);
    EXPECT_EQ(1, BthUnsetHookForMouse());
}
