#pragma once

/**
 * Notify an icon on the Notificatoin  Area.
 */
class NotifyIconInterface
{
public:
    /**
     * Show notification icon on the Notificatoin Area.
     * @return true if succeeded to show the notification icon. false if failed.
     */
    virtual bool Show() = 0;

    /**
     * Hide notification icon.
     */
    virtual void Hide() = 0;

    virtual ~NotifyIconInterface() = default;
};
