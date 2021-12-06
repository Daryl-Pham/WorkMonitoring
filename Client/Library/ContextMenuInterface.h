#pragma once

/**
 * Context menu an icon on the Notificatoin  Area.
 */
class ContextMenuInterface
{
public:
    virtual bool Init(HWND hWnd) = 0;

    /**
     * Show context menu icon on the Notificatoin Area.
     * @return true if succeeded to show the context. false if failed.
     */
    virtual UINT Show() = 0;

    /**
     * Destroy context menu.
     * @return true if succeeded to destroy the context. false if failed.
     */
    virtual bool Destroy() = 0;

    virtual bool InitDeleteCredentialMenu() = 0;

    virtual ~ContextMenuInterface() = default;
};
