#include <mutex>
#include "stdafx.h"
#include "WindowProcRunner.h"

namespace
{

int counter = 0;

}

HWND StartWindowProc(std::unique_ptr<std::thread> *messageThread, WNDPROC windowProc)
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lk(mtx);
    std::condition_variable cond;
    HWND hWnd = 0;

    *messageThread = std::make_unique<std::thread>([&]
    {
        HINSTANCE hInstance = GetModuleHandle(NULL);

        std::wstring szAppName = L"receiver" + counter++;
        WNDCLASSEX wc;

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = 0;
        wc.lpfnWndProc = windowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = NULL;
        wc.hCursor = NULL;
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = szAppName.c_str();
        wc.hIconSm = NULL;

        if (RegisterClassEx(&wc) == 0)
        {
            cond.notify_one();
            return;
        }

        hWnd = CreateWindowEx(0, szAppName.c_str(), NULL, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_MESSAGE, NULL, hInstance, NULL);
        if (hWnd == NULL)
        {
            cond.notify_one();
            return;
        }

        ShowWindow(hWnd, SW_SHOWNORMAL); // To catch MESSAGE_KEYBOARD.

        cond.notify_one(); // Succeeded to get hWnd.

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    });

    cond.wait(lk);

    return hWnd;
}