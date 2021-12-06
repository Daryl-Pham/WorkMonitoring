#pragma once

#include <windows.h>
#include <memory>
#include <thread>

HWND StartWindowProc(std::unique_ptr<std::thread> *thr, WNDPROC windowProc);
