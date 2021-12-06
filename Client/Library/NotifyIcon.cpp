#include "stdafx.h"
#include "NotifyIcon.h"
#include "Log.h"

namespace
{

DEFINE_LOGGER_FILE_NAME;

}

NotifyIcon::NotifyIcon(HWND hWnd, UINT callbackMessage, UINT iconID, HICON icon, const std::wstring &tooltip)
    : m_hWnd(hWnd),
    m_CallbackMessage(callbackMessage),
    m_IconID(iconID),
    m_Icon(icon),
    m_Tooltip(tooltip)
{
}

NotifyIcon::~NotifyIcon()
{
    HideImpl();
}

bool NotifyIcon::Show()
{
    NOTIFYICONDATA nid = { 0 };
    PassNotifyIconData(&nid);

    errno_t err = wcsncpy_s(nid.szTip, sizeof(nid.szTip) / sizeof(nid.szTip[0]), m_Tooltip.c_str(), m_Tooltip.size());
    if (err)
    {
        LOG_WARN("wcsncpy_s() failed. %d", err);
        return false;
    }

    if (!Shell_NotifyIcon(NIM_ADD, &nid))
    {
        LOG_WARN("Shell_NotifyIcon() failed. 0x%08x", GetLastError());
        return false;
    }

    return true;
}

void NotifyIcon::Hide()
{
    HideImpl();
}


void NotifyIcon::PassNotifyIconData(PNOTIFYICONDATA nid)
{
    nid->cbSize = sizeof(NOTIFYICONDATA);
    nid->hWnd = m_hWnd;
    nid->uID = m_IconID;

    nid->uCallbackMessage = m_CallbackMessage;
    nid->uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid->hIcon = m_Icon;
}

void NotifyIcon::HideImpl()
{
    NOTIFYICONDATA nid = { 0 };
    PassNotifyIconData(&nid);

    if (!Shell_NotifyIcon(NIM_DELETE, &nid))
    {
        LOG_WARN("Shell_NotifyIcon() failed. 0x%08x", GetLastError());
    }
}
