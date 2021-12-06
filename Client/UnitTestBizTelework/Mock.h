#pragma once

#include <gmock/gmock.h>

#include <BizTeleworkDialogDependency.h>
#include <BizTeleworkMainWindowDependency.h>
#include <BizTeleworkUpdaterRpcClientInterface.h>
#include <LanguageResourceInterface.h>
#include <NotifyIconInterface.h>
#include <WindowsHookInterface.h>
#include <Request.h>
#include <HttpMethod.h>
#include <AuthenHelper.h>
#include <AppConfigInterface.h>
#include <ToastNotification.h>

class MockBizTeleworkDialogDependency : public BizTeleworkDialogDependency
{
public:
    MockBizTeleworkDialogDependency()
        : BizTeleworkDialogDependency() {}
    MOCK_CONST_METHOD0(GetLanguageResource, LanguageResourceInterface*(void));
};

class MockBizTeleworkMainWindowDependency : public BizTeleworkMainWindowDependency
{
public:
    MockBizTeleworkMainWindowDependency() : BizTeleworkMainWindowDependency(L"") {}
    MOCK_CONST_METHOD5(GetNotifyIcon, std::unique_ptr<NotifyIconInterface>(HWND, UINT, UINT, HICON, const std::wstring&));
    MOCK_CONST_METHOD0(GetLanguageResource, LanguageResourceInterface*(void));
    MOCK_CONST_METHOD0(GetWindowsHook, std::unique_ptr<WindowsHookInterface>(void));
    MOCK_CONST_METHOD0(GetApiUrl, const std::wstring(void));
    MOCK_CONST_METHOD0(IsActive, bool(void));
    MOCK_CONST_METHOD1(GetRequest, std::unique_ptr<Request>(const HttpMethod&));
    MOCK_CONST_METHOD0(CreateProductKeyAtRegistryCurrentUser, bool(void));
    MOCK_CONST_METHOD0(GetAppConfig, std::unique_ptr<AppConfigInterface>(void));
    MOCK_CONST_METHOD0(GetAuthenHelper, std::unique_ptr<AuthenHelper>(void));
    MOCK_CONST_METHOD0(IsCopiedAuthenticationToCurrentUser, bool(void));
    MOCK_CONST_METHOD0(GetToastNotificationHandler, std::unique_ptr<ToastNotificationHandlerInterface>(void));
    MOCK_CONST_METHOD0(IsExistedTimeDisplayHealthStatusRemind, bool(void));
};

class MockLanguageResource : public LanguageResourceInterface
{
public:
    MOCK_METHOD(bool, ReloadLanguageResource, (LanguageID lang));
    MOCK_METHOD(bool, GetString, (UINT, std::wstring *));
};

class MockNotifyIcon : public NotifyIconInterface
{
public:
    MOCK_METHOD(bool, Show, ());
    MOCK_METHOD(void, Hide, ());
};

class MockWindowsHook : public WindowsHookInterface
{
public:
    MOCK_METHOD(BOOL, SetHookForKeyboard, (HWND, UINT));
    MOCK_METHOD(BOOL, SetHookForMouse, (HWND, UINT));
    MOCK_METHOD(BOOL, UnsetHookForKeyboard, ());
    MOCK_METHOD(BOOL, UnsetHookForMouse, ());
};

class MockBizTeleworkUpdaterRpcClient : public BizTeleworkUpdaterRpcClientInterface
{
public:
    MOCK_METHOD(bool, Bind, ());
    MOCK_METHOD(void, Unbind, ());
    MOCK_METHOD(bool, IsBound, ());
    MOCK_METHOD(HRESULT, PollFetchingTask, (std::wstring*, std::wstring*));
    MOCK_METHOD(void, PushFetchingResult, (bool));
};

class MockRequest : public Request
{
public:
    MOCK_METHOD(std::unique_ptr<CallContextForWindowsDeviceLogs>, CallWindowsDeviceLogs, (std::wstring&));
    MOCK_METHOD(std::unique_ptr<CallContextForWindowsWorkLogs>, CallWindowsWorkLogs, (RequestWorkLogsType));
    MOCK_METHOD(std::unique_ptr<CallContextForTimeCardsToday>, CallWindowsTimeCardsToday, ());
    MOCK_METHOD(std::unique_ptr<CallContextForWindowsUserConditionLogs>, CallWindowsUserConditionLogs, (RequestHealthStatusType));
    MOCK_METHOD(std::unique_ptr<CallContextForWindowsDayOffRequests>, CallWindowsDayOffRequests, ());
    MOCK_METHOD(std::unique_ptr<CallContextForWindowsApplicationLogs>, CallWindowsApplicationLogs, (const std::wstring&));
    MOCK_METHOD(std::unique_ptr<CallContextForWindowsAuthenticate>, CallWindowsAuthenticate, (const std::wstring&, const std::wstring&));
    MOCK_METHOD(std::unique_ptr<CallContextForWindowsWorkTarget>, CallWindowsWorkTarget, (const std::wstring&));
};

class MockCallContextForTimeCardsToday : public CallContextForTimeCardsToday
{
public:
    MOCK_METHOD(bool, Wait, ());
    MOCK_METHOD(void, Cancel, ());
    MOCK_METHOD(int, StatusCode, ());
    MOCK_METHOD(std::wstring, Body, ());
    MOCK_CONST_METHOD0(GetTypeCallContext, TypeCallContext(void));
};

class MockAppConfig : public AppConfigInterface
{
public:
    MOCK_METHOD(bool, GetApiUrl, (std::wstring*));
    MOCK_METHOD(bool, GetDataDir, (std::wstring*));
    MOCK_METHOD(bool, GetUpdateUrl, (std::wstring*));
    MOCK_METHOD(bool, GetUpgradeCode, (std::wstring*));
    MOCK_METHOD(bool, GetVersion, (std::wstring*));
    MOCK_METHOD(bool, GetTimerIntervalGetActiveApplication, (DWORD*));
    MOCK_METHOD(bool, GetTimerIntervalCheckAuthen, (DWORD*));
    MOCK_METHOD(bool, GetSentryDSN, (std::string*));
    MOCK_METHOD(bool, GetReleaseEnvironment, (std::string*));
    MOCK_METHOD(bool, GetBizTeleworkUpdaterRPCEndpoint, (std::wstring*));
    MOCK_METHOD(bool, GetExecutedBizTelework, (bool*));
    MOCK_METHOD(bool, SetExecutedBizTelework, (bool));
    MOCK_METHOD(bool, GetTimeDisplayHealthStatusRemind, (int64_t*));
    MOCK_METHOD(bool, SetTimeDisplayHealthStatusRemind, (const int64_t));
    MOCK_METHOD(bool, GetTimerIntervalToRemidOnDutyDialog, (int64_t*));
    MOCK_METHOD(bool, GetTimeToShowOnDutyDialog, (int64_t*));
    MOCK_METHOD(bool, GetTimeToShowHealthStatusDialog, (int64_t*));
    MOCK_METHOD(bool, SetCopiedAuthentication, (bool));
    MOCK_METHOD(bool, GetCopiedAuthentication, (bool*));
    MOCK_METHOD(bool, DeleteTimeDisplayHealthStatusRemind, ());
};

class MockAuthenHelper : public AuthenHelper
{
public:
    MOCK_CONST_METHOD0(IsCopiedAuthenticationToCurrentUser, bool(void));
    MOCK_CONST_METHOD1(SetCompanyGuid, bool(const std::wstring&));
    MOCK_CONST_METHOD1(SetAgentGuid, bool(const std::vector<BYTE>&));
    MOCK_CONST_METHOD3(GetAuthenInforFromLocalMachine, bool(std::wstring*, std::vector<BYTE>*, std::wstring*));
    MOCK_CONST_METHOD1(SetApiUrl, bool(const std::wstring&));
    MOCK_CONST_METHOD0(DeleteAuthenticationFromLocalMachine, bool(void));
};

class MockToastNotificationHandler : public ToastNotificationHandlerInterface
{
public:
    MOCK_CONST_METHOD0(ToastActivated, bool(void));
    MOCK_CONST_METHOD1(ToastActivated, bool(int));
    MOCK_CONST_METHOD1(ToastDismissed, bool(ToastDismissalReasonEnum));
    MOCK_CONST_METHOD0(ToastFailed, bool(void));
    MOCK_METHOD(bool, SetToastNotificationHandler, (HWND));
};
