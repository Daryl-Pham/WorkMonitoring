#pragma once

#include "pch.h"
#include <thread>
#include <memory>

class WinAppForTest : public CWinApp
{
private:
    const std::function<std::unique_ptr<CWnd>(void)> m_OnInitInstance;
    std::unique_ptr<CWnd> m_Wnd;
    const bool m_IsDialog;

public:
    explicit WinAppForTest(const std::function<std::unique_ptr<CWnd>(void)>& onInitInstance, bool isDialog=false)
        : CWinApp(NULL)
        , m_OnInitInstance(onInitInstance)
        , m_IsDialog(isDialog)
    {}

    BOOL InitInstance();
};
