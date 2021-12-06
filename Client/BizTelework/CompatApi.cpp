#include "stdafx.h"
#include "CompatApi.h"
#include "Log.h"

// http://yamatyuu.net/computer/program/sdk/base/edit3font/index.html

typedef DPI_AWARENESS_CONTEXT(_stdcall* SetThreadDpiAwarenessContextFunc)(DPI_AWARENESS_CONTEXT);

namespace
{
    DEFINE_LOGGER_FILE_NAME;

    SetThreadDpiAwarenessContextFunc GetSetThreadDpiAwarenessContext(HMODULE *hUser32)
    {
        HMODULE mod;
        mod = LoadLibrary(L"User32.dll");
        if (!mod)
        {
            LOG_ERROR("LoadLibrary() failed. 0x%08x", GetLastError());
            FreeLibrary(mod);
            return NULL;
        }
        SetThreadDpiAwarenessContextFunc func = (SetThreadDpiAwarenessContextFunc)GetProcAddress(mod, "SetThreadDpiAwarenessContext");
        if (!func)
        {
            LOG_ERROR("GetProcAddress() failed. 0x%08x", GetLastError());
            FreeLibrary(mod);
            return NULL;
        }

        *hUser32 = mod;
        return func;
    }
}  // namespace

BOOL CompatApi::HasSetThreadDpiAwarenessContext()
{
    HMODULE hUser32 = NULL;
    SetThreadDpiAwarenessContextFunc func = GetSetThreadDpiAwarenessContext(&hUser32);
    if (!func)
    {
        FreeLibrary(hUser32);
        return FALSE;
    }
    FreeLibrary(hUser32);
    return TRUE;
}

DPI_AWARENESS_CONTEXT CompatApi::CallSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT dpiContext)
{
    HMODULE hUser32 = NULL;
    SetThreadDpiAwarenessContextFunc func = GetSetThreadDpiAwarenessContext(&hUser32);
    if (!func)
    {
        return NULL;
    }
    DPI_AWARENESS_CONTEXT r = func(dpiContext);
    FreeLibrary(hUser32);
    return r;
}
