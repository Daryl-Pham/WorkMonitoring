#pragma once
#include "stdafx.h"
#include <ToastNotification.h>
#include "BizTeleworkToastNotificationHandler.h"
#include "BizTeleworkMessage.h"
#include <Log.h>

namespace
{

    DEFINE_LOGGER_FILE_NAME;

}

// Public interfaces
bool BizTeleworkToastNotificationHandler::ToastActivated() const
{
    if (m_HwndMainWindow)
    {
        if (!::PostMessage(m_HwndMainWindow,
            BIZ_TELEWORK_MESSAGE_TOAST_NOTIFICATION_HANDLER,
            (WPARAM)ToastNotificationHandlerInterface::TypeEventHandlerEnum::Actived,
            NULL))
        {
            LOG_ERROR("PostMessage() notification handler failed.");
            return false;
        }
        return true;
    }
    return false;
}

bool BizTeleworkToastNotificationHandler::ToastActivated(int actionIndex) const
{
    return true;
}

bool BizTeleworkToastNotificationHandler::ToastDismissed(ToastDismissalReasonEnum state) const
{
    return true;
}

bool BizTeleworkToastNotificationHandler::ToastFailed() const
{
    return true;
}

bool BizTeleworkToastNotificationHandler::SetToastNotificationHandler(HWND hWnd)
{
    if (hWnd)
    {
        m_HwndMainWindow = hWnd;
        return true;
    }
    return false;
}
