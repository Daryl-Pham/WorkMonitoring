#pragma once

#include <Windows.h>

class CompatApi
{
public:
    static BOOL HasSetThreadDpiAwarenessContext();
    static DPI_AWARENESS_CONTEXT CallSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT dpiContext);
};
