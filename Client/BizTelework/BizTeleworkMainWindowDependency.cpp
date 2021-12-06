#include "stdafx.h"
#include <LanguageResource.h>
#include <NotifyIcon.h>
#include <WindowsHook.h>
#include "BizTeleworkMainWindowDependency.h"
#include "BizTeleworkDependency.h"
#include <ContextMenu.h>
#include <Environment.h>
#include <AppConfig.h>
#include <AuthenHelper.h>
#include <GettingRequest.h>

LanguageResourceInterface *BizTeleworkMainWindowDependency::GetLanguageResource() const
{
    return LanguageResource::Singleton();
}

std::unique_ptr<NotifyIconInterface> BizTeleworkMainWindowDependency::GetNotifyIcon(HWND hWnd, UINT callbackMessage,
                                                                                    UINT iconID, HICON icon, const std::wstring &tooltip) const
{
    return std::make_unique<::NotifyIcon>(hWnd, callbackMessage, iconID, icon, tooltip);
}

std::unique_ptr<WindowsHookInterface> BizTeleworkMainWindowDependency::GetWindowsHook() const
{
    return std::make_unique<::WindowsHook>();
}

bool BizTeleworkMainWindowDependency::IsActive() const
{
    BizTeleworkDependency dependency;
    return dependency.IsActive();
}

std::unique_ptr<ContextMenuInterface> BizTeleworkMainWindowDependency::GetContextMenu() const
{
    return std::make_unique<::ContextMenu>();
}

std::unique_ptr<Request> BizTeleworkMainWindowDependency::GetRequest(const HttpMethod& method) const
{
    std::vector<BYTE> agentGUID(0);
    std::wstring companyGUID = L"";

    BizTeleworkDependency dependency;
    if (!dependency.IsActive(&companyGUID, &agentGUID))
    {
        return nullptr;
    }

    return GettingRequest::GetRequest(companyGUID, agentGUID, method);
}

bool BizTeleworkMainWindowDependency::CreateProductKeyAtRegistryCurrentUser() const
{
    CRegKey reg;
    CString subKey = Environment::GetInstance()->IsWindowsOS64() ? TeleworkAgentInfor64Reg : TeleworkAgentInfor32Reg;
    if (reg.Open(HKEY_CURRENT_USER, subKey, KEY_READ) == ERROR_SUCCESS)
    {
        return true;
    }

    LSTATUS retCode = ERROR_INVALID_FUNCTION;
    retCode = reg.Create(HKEY_CURRENT_USER, subKey);
    if (retCode != ERROR_SUCCESS)
    {
        reg.Close();
        return false;
    }

    reg.Close();
    return true;
}

std::unique_ptr<AppConfigInterface> BizTeleworkMainWindowDependency::GetAppConfig() const
{
    return std::make_unique<::AppConfig>();
}

std::unique_ptr<AuthenHelper> BizTeleworkMainWindowDependency::GetAuthenHelper() const
{
    return std::make_unique<::AuthenHelper>();
}

bool BizTeleworkMainWindowDependency::IsCopiedAuthenticationToCurrentUser() const
{
    AuthenHelper authenHelper;
    return authenHelper.IsCopiedAuthenticationToCurrentUser();
}

std::unique_ptr<ToastNotificationHandlerInterface> BizTeleworkMainWindowDependency::GetToastNotificationHandler() const
{
    return std::make_unique<BizTeleworkToastNotificationHandler>();
}

bool BizTeleworkMainWindowDependency::IsExistedTimeDisplayHealthStatusRemind() const
{
    AppConfig appConfig;
    int64_t time = 0;
    return appConfig.GetTimeDisplayHealthStatusRemind(&time);
}
