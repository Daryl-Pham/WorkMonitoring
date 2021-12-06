#include "stdafx.h"
#include "gtest/gtest.h"
#include <NotifyIcon.h>
#include "resource.h"
#include "WindowProcRunner.h"

namespace
{

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

}

TEST(testNotifyIcon_NotRunOnCI, ShowHide)
{
    std::unique_ptr<std::thread> messageThread;

    HWND hWnd = StartWindowProc(&messageThread, WindowProc);
    ASSERT_TRUE(NULL != hWnd);

    UINT callbackMessage = 0;
    UINT iconID = 20;
    HICON icon = LoadIcon(0, MAKEINTRESOURCE(IDI_ICON1));
    std::wstring tooltip = L"aaa";

    NotifyIconInterface *ni = new NotifyIcon(hWnd, callbackMessage, iconID, icon, tooltip);

    EXPECT_TRUE(ni->Show());

    NOTIFYICONIDENTIFIER nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONIDENTIFIER);
    nid.hWnd = hWnd;
    nid.uID = iconID;
    RECT rect = { 0 };
    HRESULT hr = Shell_NotifyIconGetRect(&nid, &rect);

    EXPECT_TRUE(SUCCEEDED(hr));
    EXPECT_NE(0, rect.bottom);
    EXPECT_NE(0, rect.left);
    EXPECT_NE(0, rect.right);
    EXPECT_NE(0, rect.top);

    ni->Hide();

    ZeroMemory(&rect, sizeof(rect));
    hr = Shell_NotifyIconGetRect(&nid, &rect);

    EXPECT_TRUE(FAILED(hr));
    EXPECT_EQ(0, rect.bottom);
    EXPECT_EQ(0, rect.left);
    EXPECT_EQ(0, rect.right);
    EXPECT_EQ(0, rect.top);

    PostMessage(hWnd, WM_CLOSE, 0, 0);
    messageThread->join();
    messageThread = nullptr;
}
