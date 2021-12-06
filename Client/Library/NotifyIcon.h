#pragma once

#include "NotifyIconInterface.h"
#include <Windows.h>
#include <string>

class NotifyIcon : public NotifyIconInterface
{
public:
    NotifyIcon(HWND hWnd, UINT callbackMessage, UINT iconID, HICON icon, const std::wstring &tooltip);
    NotifyIcon(const NotifyIcon&) = delete;  // Disable copy constructor.
    NotifyIcon& operator=(const NotifyIcon&) = delete;  // Disable assignment operator.
    ~NotifyIcon();

    bool Show() override;
    void Hide() override;

private:
    void PassNotifyIconData(PNOTIFYICONDATA nid);
    void HideImpl();  // This method is for calling Hide() on the destructor.

    HWND m_hWnd;
    UINT m_CallbackMessage;
    UINT m_IconID;
    HICON m_Icon;
    const std::wstring m_Tooltip;
};

