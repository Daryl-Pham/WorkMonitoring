#include "stdafx.h"
#include "ContextMenu.h"
#include "Log.h"
#include "Language.h"
#include "LanguageResource.h"

namespace
{

    DEFINE_LOGGER_FILE_NAME;

}

ContextMenu::~ContextMenu()
{
    Destroy();
}

UINT ContextMenu::Show()
{
    POINT clickPoint;
    if (!GetCursorPos(&clickPoint))
    {
        LOG_WARN("GetCursorPos() failed. 0x%08x", GetLastError());
        return false;
    }

    if (!SetForegroundWindow(m_Hwnd))
    {
        LOG_WARN("SetForegroundWindow() failed.");
        return false;
    }

    UINT indexSelect = 0;
    indexSelect = TrackPopupMenu(m_PopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN | TPM_RETURNCMD, clickPoint.x, clickPoint.y, 0, m_Hwnd, NULL);
    if (!indexSelect)
    {
        LOG_WARN("TrackPopupMenu() failed or user don't select context menu.");
    }

    return indexSelect;
}

bool ContextMenu::Init(HWND hWnd)
{
    if (!hWnd)
    {
        LOG_WARN("CURLStaticText::GetSafeHwnd() failed.");
        return false;
    }

    m_Hwnd = hWnd;

    m_PopMenu = CreatePopupMenu();
    if (m_PopMenu == NULL)
    {
        LOG_WARN("CreatePopupMenu() failed. 0x%08x", GetLastError());
        return false;
    }

    return true;
}

bool ContextMenu::InitContextMenuItem(const std::wstring& textMenuItem, const UINT menuItemId)
{
    if (!m_PopMenu)
    {
        LOG_WARN("m_PopMenu NULL.");
        return false;
    }

    if (!InsertMenu(m_PopMenu, -1, MF_BYPOSITION | MF_STRING, menuItemId, textMenuItem.c_str()))
    {
        LOG_WARN("InsertMenu() failed. 0x%08x", GetLastError());
        return false;
    }

    return true;
}

bool ContextMenu::InitDeleteCredentialMenu()
{
    if (!DeleteAllMenuItems())
    {
        LOG_WARN("DeleteAllMenuItem() failed.");
        return false;
    }

    std::wstring textDeleteCredential(L"");
    auto lr = LanguageResource::Singleton();
    if (!lr->GetString(IDS_DELETE_CREDENTIALS, &textDeleteCredential))
    {
        LOG_WARN("lr->GetString(IDS_DELETE_CREDENTIALS) failed.");
        return false;
    }

    return InitContextMenuItem(textDeleteCredential, DeleteCredentialMenuId);
}

bool ContextMenu::DeleteAllMenuItems()
{
    if (!m_PopMenu)
    {
        LOG_WARN("m_PopMenu NULL.");
        return false;
    }

    MENUITEMINFO mii = { 0 };
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_SUBMENU;
    int indexMenu = 0;

    while (GetMenuItemInfo(m_PopMenu, indexMenu, TRUE, &mii))
    {
        if (mii.hSubMenu != NULL && !DestroyMenu(mii.hSubMenu))
        {
            LOG_WARN("DestroyMenu() failed. 0x%08x", GetLastError());
            return false;
        }
        if (!RemoveMenu(m_PopMenu, indexMenu, MF_BYPOSITION))
        {
            LOG_WARN("RemoveMenu() failed. 0x%08x", GetLastError());
            return false;
        }
    }

    return true;
}

bool ContextMenu::Destroy()
{
    if (m_PopMenu && !DestroyMenu(m_PopMenu))
    {
        LOG_WARN("DestroyMenu() failed. 0x%08x", GetLastError());
        return false;
    }

    return true;
}
