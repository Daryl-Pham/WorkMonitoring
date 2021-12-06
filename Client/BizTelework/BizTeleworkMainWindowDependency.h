#pragma once

#include <memory>
#include <LanguageResourceInterface.h>
#include <NotifyIconInterface.h>
#include <WindowsHookInterface.h>
#include <ContextMenuInterface.h>
#include "HttpMethod.h"
#include "Request.h"
#include <AppConfigInterface.h>
#include <AuthenHelper.h>
#include "BizTeleworkToastNotificationHandler.h"

class BizTeleworkMainWindowDependency
{
public:
    explicit BizTeleworkMainWindowDependency(const std::wstring &apiUrl)
        : m_ApiUrl(apiUrl)
    {}
    virtual ~BizTeleworkMainWindowDependency() = default;

    virtual LanguageResourceInterface *GetLanguageResource() const;
    virtual std::unique_ptr<NotifyIconInterface> GetNotifyIcon(HWND hWnd, UINT callbackMessage, UINT iconID, HICON icon, const std::wstring &tooltip) const;
    virtual std::unique_ptr<WindowsHookInterface> GetWindowsHook() const;
    virtual const std::wstring GetApiUrl() const { return m_ApiUrl; }
    virtual bool IsActive() const;
    virtual std::unique_ptr<ContextMenuInterface> GetContextMenu() const;
    virtual std::unique_ptr<Request> GetRequest(const HttpMethod& method = HttpMethod::POST) const;
    virtual std::unique_ptr<AppConfigInterface> GetAppConfig() const;
    virtual std::unique_ptr<AuthenHelper> GetAuthenHelper() const;
    virtual std::unique_ptr<ToastNotificationHandlerInterface> GetToastNotificationHandler() const;

    virtual bool CreateProductKeyAtRegistryCurrentUser() const;
    virtual bool IsCopiedAuthenticationToCurrentUser() const;
    virtual bool IsExistedTimeDisplayHealthStatusRemind() const;

private:
    const std::wstring m_ApiUrl;
};
