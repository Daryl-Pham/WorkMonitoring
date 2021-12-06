#pragma once
#include "ContextMenuInterface.h"
#include "LanguageResourceInterface.h"
#include <vector>

const UINT DeleteCredentialMenuId = WM_USER + 1;

class ContextMenu : public ContextMenuInterface
{
public:
    ContextMenu() = default;
    ContextMenu(const ContextMenu&) = delete;  // Disable copy constructor.
    ContextMenu& operator=(const ContextMenu&) = delete;  // Disable assignment operator.
    ~ContextMenu();

    bool Init(HWND hWnd) override;
    UINT Show() override;
    bool Destroy() override;
    bool InitDeleteCredentialMenu() override;

private:
    bool InitContextMenuItem(const std::wstring& textMenuItem, const UINT menuItemId);
    bool DeleteAllMenuItems();

    std::vector<MENUITEMINFO> m_MenuItems;
    HMENU m_PopMenu = NULL;
    HWND m_Hwnd = NULL;
};
