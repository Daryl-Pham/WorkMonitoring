#pragma once
#include <ToastNotification.h>

class BizTeleworkToastNotificationHandler : public ToastNotificationHandlerInterface
{
public:
    // Public interfaces
    bool ToastActivated() const override;
    bool ToastDismissed(ToastDismissalReasonEnum state) const override;
    bool ToastActivated(int actionIndex) const override;
    bool ToastFailed() const override;

    bool SetToastNotificationHandler(HWND hWnd) override;
private:
    HWND m_HwndMainWindow = NULL;
};
