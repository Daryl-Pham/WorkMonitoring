
// BizTeleworkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BizTelework.h"
#include "BizTeleworkMainWindow.h"
#include "BizTeleworkAuthenticationDialog.h"
#include "BizTeleworkOnDutyDialog.h"
#include "BizTeleworkHealthStatusDialog.h"
#include "BizTeleworkFailedCallApiDialog.h"
#include "BizTeleworkWorkDayTargetDialog.h"
#include "BizTeleworkMessage.h"
#include "ResponsiveCreator.h"
#include "ActiveApplication.h"
#include "afxdialogex.h"
#include "UserInputLog.h"
#include "Common.h"
#include "Log.h"
#include <LanguageResource.h>
#include <Language.h>
#include <iostream>
#include <fstream>
#include "AppConfig.h"
#include "ContextMenu.h"
#include <wtsapi32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    LPCTSTR WINDOW_NAME = _T("BizTelework");

    const UINT ICON_ID = 1;

    /* Reference ticket https://biztelework.atlassian.net/browse/BTC-413
     * Time get current active app, after every 5 minutes (The unit of interval is millisecond).
     */
    const int64_t GETTING_CURRENT_ACTIVE_APP_TIMER_INTERVAL = 1000 * 60 * 5;

    const DWORD GETTING_CURRENT_ACTIVE_APP_TIMER_ID = 10010;

    const int64_t DEFAULT_TIMER_INTERVAL_TO_REMIND_ON_DUTY_DIALOG = 60 * 60;  // The unit is second.

    const DWORD SHOW_ON_DUTY_DIALOG_REMIND_TIMER_ID = 10020;

    const int64_t DEFAULT_TIME_TO_SHOW_ON_DUTY_DIALOG = 60 * 60 * 9;  // The unit is second.

    const int DEFAULT_TIME_TO_SHOW_HEALTH_STATUS_DIALOG = 60 * 60 * 5;  // The unit is second.

    const wchar_t* CompanyName = L"OPTiM Corporation";

    const wchar_t* ProductName = L"Optimal Biz Telework";

    const wchar_t* SubProduct = L"Telework";

}  // namespace

BizTeleworkMainWindow::BizTeleworkMainWindow(const BizTeleworkMainWindowDependency &dependency)
    : m_WindowsHook(dependency.GetWindowsHook())
    , m_ApiUrl(dependency.GetApiUrl())
    , m_ContextMenu(dependency.GetContextMenu())
    , m_Request(dependency.GetRequest(HttpMethod::GET))
    , m_AppConfig(dependency.GetAppConfig())
    , m_AuthenHelper(dependency.GetAuthenHelper())
    , m_ToastNotificationHandler(dependency.GetToastNotificationHandler())
{
}

bool BizTeleworkMainWindow::Init(const BizTeleworkMainWindowDependency& dependency)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    CString wcn = ::AfxRegisterWndClass(NULL);
    if (!this->CreateEx(0, wcn, WINDOW_NAME, 0, 0, 0, 0, 0, HWND_MESSAGE, 0))
    {
        LOG_WARN("CreateEx() failed.");
    }

    if (!ShowNotifyIcon(dependency))
    {
        LOG_WARN("ShowNotifyIcon() failed.");
        return false;
    }

    if (!dependency.CreateProductKeyAtRegistryCurrentUser())
    {
        LOG_WARN("CreateProductKeyAtRegistryCurrentUser() failed.");
        return false;
    }

    if (!dependency.IsCopiedAuthenticationToCurrentUser())
    {
        if (!CopyAuthenticationToCurrentUser())
        {
            LOG_WARN("CopyAuthenticationToCurrentUser() failed.");
        }
    }

    if (!dependency.IsExistedTimeDisplayHealthStatusRemind())
    {
        SetTimeDisplayHealthStatusRemindToCurrentUser();
    }

    // Register hooks for keyboard and mouse.
    // Register catch session state of user
    if (dependency.IsActive())
    {
        SetKeyboardAndMouseHook();
        SetContextMenu();
        SetShowOnDutyDialogRemindTimer();
        OnRegisterSessionNotification();
        SetToastNotificationHandler();
        ShowDialog(WhereCallShowDialog::ON_INIT);
    }

    WriteCurrentActiveAppLog();
    SetGettingActiveAppTimer();

    return true;
}

BEGIN_MESSAGE_MAP(BizTeleworkMainWindow, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_KEYBOARD, &BizTeleworkMainWindow::OnMessageKeyboard)
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_MOUSE, &BizTeleworkMainWindow::OnMessageMouse)
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_NOTIFY_ICON, &BizTeleworkMainWindow::OnMessageNotifyIcon)
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_KEYBOAD_AND_MOUSE_HOOK, &BizTeleworkMainWindow::OnMessageKeyboardAndMouseHook)
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_CONTEXT_MENU, &BizTeleworkMainWindow::OnMessageContextMenu)
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_SET_SHOWING_ON_DUTY_DIALOG_REMIND_TIMER, &BizTeleworkMainWindow::OnSetShowOnDutyDialogRemindTimer)
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_REGISTER_SESSION_NOTIFICATION, &BizTeleworkMainWindow::OnRegisterSessionNotification)
    ON_MESSAGE(WM_WTSSESSION_CHANGE, &BizTeleworkMainWindow::OnWtsSessionChange)
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_TOAST_NOTIFICATION_HANDLER, &BizTeleworkMainWindow::OnMessageToastNotificationHandler)
    ON_WM_COPYDATA()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// BizTeleworkMainWindow message handlers

int BizTeleworkMainWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    (void)lpCreateStruct;  // Unused

    return 0;
}

void BizTeleworkMainWindow::OnDestroy()
{
    HideNotifyIcon();

    UnSetKeyboardAndMouseHook();

    UnRegisterSessionNotification();

    if (!KillGettingActiveAppTimer())
    {
        LOG_WARN("KillGettingActiveAppTimer() failed.");
    }

    if (!UnSetContextMenu())
    {
        LOG_WARN("UnSetContextMenu() failed.");
    }

    if (!KillShowOnDutyDialogRemindTimer())
    {
        LOG_WARN("KillShowOnDutyDialogRemindTimer() failed.");
    }
}

LRESULT BizTeleworkMainWindow::OnMessageKeyboard(WPARAM wParam, LPARAM lParam)
{
    (void)wParam;  // Unused
    (void)lParam;  // Unused

    UserInputLog keyboardLog(KeyboardLogFileName, LogTypeKeyboard);
    auto now = boost::chrono::system_clock::now();
    if (!keyboardLog.WriteLogOnTimeCondition(now))
    {
        LOG_ERROR("keyboardLog.WriteLogOnTimeCondition() failed.");
    }
    else
    {
        SendLogIfNeccessary(now, true, false);
        if (!m_LastActivityLog.WriteLastTime(Utility::ConvertTimePointToUnixTime(now)))
        {
            LOG_ERROR("m_LastActivityLog.WriteLastTime() failed.");
        }
    }

    return 0;
}

BOOL BizTeleworkMainWindow::KillGettingActiveAppTimer()
{
    BOOL resultKillTimer = FALSE;
    if (m_GettingActiveAppTimer)
    {
        resultKillTimer = KillTimer(m_GettingActiveAppTimer);
        m_GettingActiveAppTimer = 0;
    }
    return resultKillTimer;
}

LRESULT BizTeleworkMainWindow::OnMessageMouse(WPARAM wParam, LPARAM lParam)
{
    (void)wParam;  // Unused
    (void)lParam;  // Unused

    UserInputLog mouseLog(MouseLogFileName, LogTypeMouse);
    auto now = boost::chrono::system_clock::now();
    if (!mouseLog.WriteLogOnTimeCondition(now))
    {
        LOG_ERROR("mouseLog.WriteLogOnTimeCondition() failed.");
    }
    else
    {
        SendLogIfNeccessary(now, false, true);
        if (!m_LastActivityLog.WriteLastTime(Utility::ConvertTimePointToUnixTime(now)))
        {
            LOG_ERROR("lastActivityLog.WriteLastTime() failed.");
        }
    }

    return 0;
}

LRESULT BizTeleworkMainWindow::OnMessageNotifyIcon(WPARAM wParam, LPARAM lParam)
{
    if ((wParam == ICON_ID) && (lParam == WM_LBUTTONUP))
    {
        LOG_INFO("OnMessageNotifyIcon clicked.");
        CWnd* wnd = GetLastActivePopup();
        if ((!wnd) || (!wnd->IsWindowVisible()))
        {
            //khanhpqtest start
            BizTeleworkDialogDependency dependency;
            std::unique_ptr<BizTeleworkWorkDayTargetDialog> workDayTargetDialog = std::make_unique<BizTeleworkWorkDayTargetDialog>(dependency, nullptr);
            workDayTargetDialog->DoModal();
            return 0;
            //khanhpqtest end
            ShowDialog(WhereCallShowDialog::NOTIFY_ICON);
        }
        else
        {
            wnd->SetForegroundWindow();
        }
    }
    else if ((wParam == ICON_ID) && (lParam == WM_RBUTTONUP))
    {
        UINT itemSelected = 0;
        if (SetMenuDeleteCredential(&itemSelected))
        {
            ProcessMenuItemSelected(itemSelected);
        }
    }

    return 0;
}


bool BizTeleworkMainWindow::ShowNotifyIcon(const BizTeleworkMainWindowDependency &dependency)
{
    HWND hWnd = GetSafeHwnd();
    auto lr = dependency.GetLanguageResource();
    std::wstring tooltip;

    if (!hWnd)
    {
        LOG_WARN("GetSafeHwnd() failed.");
        return false;
    }
    else if (!lr)
    {
        LOG_WARN("dependency.LanguageResource() failed.");
        return false;
    }
    else if (!lr->GetString(IDS_NOTIFY_ICON_RUNNING, &tooltip))
    {
        LOG_WARN("lr->GetString(IDS_NOTIFY_ICON_RUNNING) failed.");
        return false;
    }
    else if (!(m_NotifyIcon = dependency.GetNotifyIcon(hWnd, BIZ_TELEWORK_MESSAGE_NOTIFY_ICON, ICON_ID, m_hIcon, tooltip)))
    {
        LOG_WARN("dependency.GetNotifyIcon() failed.");
        return false;
    }
    else if (!m_NotifyIcon->Show())
    {
        LOG_WARN("m_NotifyIcon->Show() failed.");
        return false;
    }

    return true;
}

void BizTeleworkMainWindow::HideNotifyIcon()
{
    if (m_NotifyIcon)
    {
        m_NotifyIcon->Hide();
    }
}

void BizTeleworkMainWindow::SendLogIfNeccessary(const boost::chrono::system_clock::time_point tp,
                                                const bool isKeyboardLog,
                                                const bool isMouseLog)
{
    if (m_LastActivityLog.IsSendLog(tp))
    {
        std::unique_ptr<Request> request = GettingRequest::GetRequest();
        if (!request)
        {
            return;
        }
        DeviceLog device;
        LogLines content;
        if (isKeyboardLog) device.ReadKeyboardLog(&content);
        if (isMouseLog) device.ReadMouseLog(&content);
        device.SendLog(content, isKeyboardLog, isMouseLog);

        ActiveApplication activeApp(ActiveAppLogFileName);
        content.clear();
        if (!activeApp.ReadActiveAppLog(&content))
        {
            return;
        }
        activeApp.SendLog(content);
    }
}

void BizTeleworkMainWindow::ShowDialog(WhereCallShowDialog whereCall)
{
    LOG_INFO("Call ShowDialog from %d", whereCall);
    auto lr = LanguageResource::Singleton();

    std::unique_ptr<CDialogEx> dialog;
    boost::timer::cpu_timer timer;
    if (!m_Request && !(m_Request = GettingRequest::GetRequest(HttpMethod::GET)))
    {
        BizTeleworkDependency dependency;
        dependency.ShowAuthenticationDialog(ActiveInformation());
        return;
    }
    if (whereCall == WhereCallShowDialog::ON_DEAUTHENTICATION)
    {
        return;
    }

    auto callContext = m_Request->CallWindowsTimeCardsToday();
    if (callContext == nullptr)
    {
        LOG_WARN("request.CallWindowsWorkLogs() failed.");
        return;
    }
    else if (!callContext->Wait())
    {
        LOG_WARN("callContext->Wait() takes %s[sec].", timer.format(2, "%w").c_str());
        LOG_WARN("callContext->Wait() failed.");
        dialog = std::make_unique<BizTeleworkFailedCallApiDialog>(this);
        if (dialog)
        {
            dialog->DoModal();
        }
        return;
    }

    if (callContext->StatusCode() == HTTP_STATUS_CODE_OK)
    {
        try
        {
            std::unique_ptr<Responsive> res = ResponsiveCreator::GetResponsive(HTTP_STATUS_CODE_OK, callContext->Body());
            TimeCardOkieResponsive& timeCardOKRes = dynamic_cast<TimeCardOkieResponsive&>(*res);
            if (!timeCardOKRes.ParseValue())
            {
                LOG_ERROR("OKRes->ParseValue() failed.");
                return;
            }
            const TimeCardOkieStatus status = timeCardOKRes.GetStatus();
            BizTeleworkDialogDependency dependency;
            if (status == TimeCardOkieStatus::ON_DUTY)
            {
                if ((whereCall == WhereCallShowDialog::SESSION_UNLOCK) || (whereCall == WhereCallShowDialog::ON_INIT))
                {
                    return;
                }

                const int64_t unixStartedTime = timeCardOKRes.GetUnixTimeStartedAt();
                const WorkDayTarget workDayTarget = timeCardOKRes.GetWorkDayTarget();
                if (workDayTarget.targetStatus != WorkDayTargetStatus::VARIABLE_STATUS)
                {
                    BizTeleworkDialogDependency dependency;
                    std::unique_ptr<BizTeleworkWorkDayTargetDialog> workDayTargetDialog = std::make_unique<BizTeleworkWorkDayTargetDialog>(dependency, nullptr);
                    workDayTargetDialog->DoModal();
                    return;
                }

                if (whereCall == WhereCallShowDialog::ON_TIMER && !IsShowOnDutyDialogRemindFromTimer(unixStartedTime))
                {
                    return;
                }

                if (whereCall == WhereCallShowDialog::ON_TIMER)
                {
                    std::wstring titleNotification = L"";
                    if (!lr->GetString(IDS_TITLE_PUSH_STOP_WORKING, &titleNotification))
                    {
                        LOG_WARN("lr->GetString(IDS_TITLE_PUSH_STOP_WORKING) failed.");
                    }
                    std::wstring contentNotification = L"";
                    if (!lr->GetString(IDS_CONTENT_PUSH_STOP_WORKING, &contentNotification))
                    {
                        LOG_WARN("lr->GetString(IDS_CONTENT_PUSH_STOP_WORKING) failed.");
                    }
                    ShowToastNotification(titleNotification, contentNotification);
                }
                else
                {
                    BringWindowToTop();
                    dialog = std::make_unique<BizTeleworkOnDutyDialog>(dependency, unixStartedTime, this);
                }
            }
            else if (status == TimeCardOkieStatus::OFF_DUTY)
            {
                if (whereCall == WhereCallShowDialog::ON_TIMER)
                {
                    return;
                }

                if ((whereCall == WhereCallShowDialog::SESSION_UNLOCK) || (whereCall == WhereCallShowDialog::ON_INIT))
                {
                    bool isShowHealthStatus = IsShowHealthStatusDialogRemindFromUnlocking();
                    if (isShowHealthStatus)
                    {
                        std::wstring titleNotification = L"";
                        if (!lr->GetString(IDS_TITLE_PUSH_START_WORKING, &titleNotification))
                        {
                            LOG_WARN("lr->GetString(IDS_TITLE_PUSH_START_WORKING) failed.");
                        }
                        std::wstring contentNotification = L"";
                        if (!lr->GetString(IDS_CONTENT_PUSH_START_WORKING, &contentNotification))
                        {
                            LOG_WARN("lr->GetString(IDS_CONTENT_PUSH_START_WORKING) failed.");
                        }
                        bool resultShowToastNotification = ShowToastNotification(titleNotification, contentNotification);

                        /* Reference ticket https://biztelework.atlassian.net/browse/BTC-901
                         * If show toast notification successful, then set TimeDisplayHealthStatusRemind to notification only display one time.
                         */
                        if ((resultShowToastNotification == true) && (m_AppConfig != nullptr))
                        {
                            m_AppConfig->SetTimeDisplayHealthStatusRemind(Utility::GetCurrentSecondInUnixTime());
                        }
                    }
                    return;
                }

                dialog = std::make_unique<BizTeleworkHealthStatusDialog>(dependency, this);
            }
        }
        catch (const std::bad_cast& e)
        {
            LOG_ERROR("Dynamic cast TimeCardOkieResponsive failed %s.", e.what());
        }
    }
    else if (callContext->StatusCode() == HTTP_STATUS_CODE_UNAUTHORIZED)
    {
        dialog = std::make_unique<BizTeleworkFailedCallApiDialog>(ErrorDialogMessageType::UNAUTHORIZED_ERROR_TYPE, this);
    }
    else
    {
        dialog = std::make_unique<BizTeleworkFailedCallApiDialog>(this);
    }

    if (dialog)
    {
        dialog->DoModal();
    }
}

std::wstring BizTeleworkMainWindow::GetApiUrl() const
{
    return m_ApiUrl;
}

void BizTeleworkMainWindow::SetGettingActiveAppTimer()
{
    DWORD timerGetActiveApp;
    AppConfig config;
    if (!config.GetTimerIntervalGetActiveApplication(&timerGetActiveApp))
    {
        LOG_WARN("appConfig.GetTimerIntervalGetActiveApplication() failed.");
        timerGetActiveApp = GETTING_CURRENT_ACTIVE_APP_TIMER_INTERVAL;
    }
    else if (timerGetActiveApp == 0)
    {
        LOG_INFO("The timerGetActiveApp is zero and set default value.");
        timerGetActiveApp = GETTING_CURRENT_ACTIVE_APP_TIMER_INTERVAL;
    }
    m_GettingActiveAppTimer = this->SetTimer(GETTING_CURRENT_ACTIVE_APP_TIMER_ID, timerGetActiveApp, NULL);
}

void BizTeleworkMainWindow::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent)
    {
    case GETTING_CURRENT_ACTIVE_APP_TIMER_ID:
        WriteCurrentActiveAppLog();
        break;
    case SHOW_ON_DUTY_DIALOG_REMIND_TIMER_ID:
        ShowDialog(WhereCallShowDialog::ON_TIMER);
        break;
    default:
        break;
    }

    CWnd::OnTimer(nIDEvent);
}

void BizTeleworkMainWindow::WriteCurrentActiveAppLog()
{
    AuthenHelper authen;
    AgentCredential agentCredential;
    std::vector<BYTE> agentGUID = authen.GetAgentGUIDFromRegistry();
    std::wstring companyGUID = authen.GetCompanyGUIDFromRegistry();

    if (agentGUID.empty() || companyGUID.empty())
    {
        LOG_INFO("BizTelework is not active.");
        return;
    }

    if (!authen.CheckAuthenTelework(companyGUID, agentGUID, &agentCredential))
    {
        LOG_WARN("authen.CheckAuthenTelework() failed.");
        return;
    }

    ActiveApplication activeApp(ActiveAppLogFileName);
    activeApp.GetCurrentProcessName();
    if (!activeApp.WriteLog(boost::chrono::system_clock::now(), activeApp.GetProcessName()))
    {
        LOG_ERROR("ActiveApplication.WriteLog() failed.");
        return;
    }
}

BOOL BizTeleworkMainWindow::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
    CopyData* cp = reinterpret_cast<CopyData*>(pCopyDataStruct->lpData);

    if (pCopyDataStruct->dwData != COPYDATA_TYPE_1)
    {
        LOG_ERROR("Unknow copy data.");
        return FALSE;
    }

    if (pCopyDataStruct->cbData < sizeof(GUID))
    {
        LOG_ERROR("Unknow copy data.");
        return FALSE;
    }

    if (!memcmp(&Signature, &cp->signature, sizeof(GUID)) == 0)
    {
        LOG_ERROR("Unknow copy data.");
        return FALSE;
    }

    BizTeleworkDependency dependency;
    if (!dependency.IsActive())
    {
        ActiveInformation activeInformation;
        activeInformation = cp->infor;
        dependency.ShowAuthenticationDialog(activeInformation);
    }
    else
    {
        ShowDialog(WhereCallShowDialog::ON_COPY_DATA);
    }

    return TRUE;
}

LRESULT BizTeleworkMainWindow::OnMessageKeyboardAndMouseHook(WPARAM wParam, LPARAM lParam)
{
    (void)lParam;  // Unused

    BOOL isSetHook = static_cast<BOOL>(wParam);

    if (isSetHook)
    {
        return SetKeyboardAndMouseHook();
    }
    else
    {
        return UnSetKeyboardAndMouseHook();
    }
}

BOOL BizTeleworkMainWindow::UnSetKeyboardAndMouseHook()
{
    if (!m_WindowsHook)
    {
        return FALSE;
    }

    if (!m_WindowsHook->UnsetHookForKeyboard())
    {
        LOG_ERROR("m_WindowsHook->UnsetHookForKeyboard() failed.");
    }

    if (!m_WindowsHook->UnsetHookForMouse())
    {
        LOG_ERROR("m_WindowsHook->UnsetHookForMouse() failed.");
    }
    return TRUE;
}

LRESULT BizTeleworkMainWindow::OnMessageContextMenu(WPARAM wParam, LPARAM lParam)
{
    (void)lParam;  // Unused

    BOOL isSetContextMenu = static_cast<BOOL>(wParam);

    if (isSetContextMenu)
    {
        return SetContextMenu();
    }
    else
    {
        return UnSetContextMenu();
    }
}

bool BizTeleworkMainWindow::SetContextMenu()
{
    HWND hWnd = GetSafeHwnd();
    if (hWnd == NULL)
    {
        LOG_WARN("GetSafeHwnd() failed.");
        return FALSE;
    }

    return m_ContextMenu->Init(hWnd);
}
bool BizTeleworkMainWindow::UnSetContextMenu()
{
    return m_ContextMenu->Destroy();
}

bool BizTeleworkMainWindow::SetMenuDeleteCredential(UINT* itemSelected)
{
    if (!m_ContextMenu->InitDeleteCredentialMenu())
    {
        LOG_WARN("m_ContextMenu->InitDeleteCredentialMenu() failed.");
        return false;
    }

    *itemSelected = m_ContextMenu->Show();

    if (!(*itemSelected))
    {
        return false;
    }
    else
    {
        return true;
    }
}

BOOL BizTeleworkMainWindow::SetKeyboardAndMouseHook()
{
    HWND hWnd = GetSafeHwnd();
    if (hWnd == NULL)
    {
        LOG_WARN("GetSafeHwnd() failed.");
        return FALSE;
    }

    if (!m_WindowsHook->SetHookForKeyboard(hWnd, BIZ_TELEWORK_MESSAGE_KEYBOARD))
    {
        LOG_ERROR("m_WindowsHook->SetHookForKeyboard() failed.");
    }

    if (!m_WindowsHook->SetHookForMouse(hWnd, BIZ_TELEWORK_MESSAGE_MOUSE))
    {
        LOG_ERROR("m_WindowsHook->SetHookForKeyboard() failed.");
    }

    return TRUE;
}

void BizTeleworkMainWindow::ProcessMenuItemSelected(const UINT itemSelected)
{
    switch (itemSelected)
    {
    case DeleteCredentialMenuId:
        DeleteCredential();
        break;
    default:
        break;
    }
}

void BizTeleworkMainWindow::DeleteCredential()
{
    LanguageResourceInterface* language(LanguageResource::Singleton());
    std::wstring titleMessageConfirm(L"Optimal Biz Telework");
    std::wstring contentMessageConfirm(L"");

    if (!language->GetString(IDS_DELETE_CREDENTIALS_MESSAGE_BOX_CONTENT, &contentMessageConfirm))
    {
        contentMessageConfirm = L"";
    }

    const int result = MessageBox(contentMessageConfirm.c_str() ,
                                  titleMessageConfirm.c_str(),
                                  MB_OKCANCEL | MB_ICONQUESTION);
    switch (result)
    {
    case IDOK:
    {
        AuthenHelper authen;
        if (authen.DeleteAuthentication())
        {
            if (!UnSetKeyboardAndMouseHook())
            {
                LOG_ERROR("UnSetKeyboardAndMouseHook() failed.");
            }
            if (!UnSetContextMenu())
            {
                LOG_ERROR("UnSetContextMenu() failed.");
            }

            Logging mouseLog(Utility::GetDefaultDataDirectory(), MouseLogFileName);
            if (!mouseLog.DeleteLog())
            {
                LOG_ERROR("mouseLog.DeleteLog() failed.");
            }

            Logging keyboardLog(Utility::GetDefaultDataDirectory(), KeyboardLogFileName);
            if (!keyboardLog.DeleteLog())
            {
                LOG_ERROR("keyboardLog.DeleteLog() failed.");
            }

            Logging activeAppLog(Utility::GetDefaultDataDirectory(), ActiveAppLogFileName);
            if (!activeAppLog.DeleteLog())
            {
                LOG_ERROR("activeAppLog.DeleteLog() failed.");
            }

            /* Reference ticket https://biztelework.atlassian.net/browse/BTC-858
            * Then condition screen is displayed when clear authentication information.
            */
            if (m_Request)
            {
                m_Request.reset();
            }
            if (!m_Request && !GettingRequest::GetRequest())
            {
                ShowDialog(WhereCallShowDialog::ON_DEAUTHENTICATION);
            }
        }
    }
        break;
    default:
        break;
    }
}

BOOL BizTeleworkMainWindow::SetShowOnDutyDialogRemindTimer()
{
    int64_t timerIntervalToRemidOnDutyDialog = 0;
    AppConfig appConfig;
    if (!appConfig.GetTimerIntervalToRemidOnDutyDialog(&timerIntervalToRemidOnDutyDialog))
    {
        LOG_WARN("appConfig.GetTimerIntervalToRemidOnDutyDialog() failed.");
        timerIntervalToRemidOnDutyDialog = DEFAULT_TIMER_INTERVAL_TO_REMIND_ON_DUTY_DIALOG;
    }

    DWORD timerSetShowOnDutyDialog = timerIntervalToRemidOnDutyDialog * 1000;
    if (!m_ShowOnDutyDialogRemindTimer)
    {
        m_ShowOnDutyDialogRemindTimer = this->SetTimer(SHOW_ON_DUTY_DIALOG_REMIND_TIMER_ID, timerSetShowOnDutyDialog, NULL);
        return (m_ShowOnDutyDialogRemindTimer != 0);
    }
    return TRUE;
}

BOOL BizTeleworkMainWindow::KillShowOnDutyDialogRemindTimer()
{
    BOOL resultKillTimer = FALSE;
    if (m_ShowOnDutyDialogRemindTimer)
    {
        resultKillTimer = KillTimer(m_ShowOnDutyDialogRemindTimer);
        m_ShowOnDutyDialogRemindTimer = 0;
    }

    return resultKillTimer;
}

bool BizTeleworkMainWindow::IsShowOnDutyDialogRemindFromTimer(const int64_t unixStartedTime)
{
    int64_t timeToShowOnDutyDialog = 0;
    AppConfig appConfig;
    if (!appConfig.GetTimeToShowOnDutyDialog(&timeToShowOnDutyDialog))
    {
        LOG_WARN("appConfig.GetTimeToShowOnDutyDialog() failed.");
        timeToShowOnDutyDialog = DEFAULT_TIME_TO_SHOW_ON_DUTY_DIALOG;
    }
    return ((Utility::GetCurrentSecondInUnixTime() - unixStartedTime) > timeToShowOnDutyDialog);
}

LRESULT BizTeleworkMainWindow::OnSetShowOnDutyDialogRemindTimer(WPARAM wParam, LPARAM lParam)
{
    (void)wParam;  // Unused
    (void)lParam;  // Unused

    HWND hWnd = GetSafeHwnd();
    if (hWnd == NULL)
    {
        LOG_WARN("GetSafeHwnd() failed.");
        return FALSE;
    }

    if (SetShowOnDutyDialogRemindTimer())
    {
        LOG_WARN("SetShowOnDutyDialogRemindTimer() failed.");
    }

    return TRUE;
}

LRESULT BizTeleworkMainWindow::OnRegisterSessionNotification(WPARAM wParam, LPARAM lParam)
{
    (void)wParam;  // Unused
    (void)lParam;  // Unused

    HWND hWnd = GetSafeHwnd();
    if (hWnd == NULL)
    {
        LOG_WARN("GetSafeHwnd() failed.");
        return FALSE;
    }

    if (!WTSRegisterSessionNotification(hWnd, NOTIFY_FOR_THIS_SESSION))
    {
        LOG_WARN("WTSRegisterSessionNotification() failed. 0x%08x", GetLastError());
        return FALSE;
    }

    return TRUE;
}

bool BizTeleworkMainWindow::UnRegisterSessionNotification()
{
    HWND hWnd = GetSafeHwnd();
    if (hWnd == NULL)
    {
        LOG_WARN("GetSafeHwnd() failed.");
        return false;
    }

    if (!WTSUnRegisterSessionNotification(hWnd))
    {
        LOG_WARN("WTSUnRegisterSessionNotification() failed. 0x%08x", GetLastError());
        return false;
    }

    return true;
}

LRESULT BizTeleworkMainWindow::OnWtsSessionChange(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case WTS_SESSION_UNLOCK:
        ShowDialog(WhereCallShowDialog::SESSION_UNLOCK);
        break;

    default:
        break;
    }
    return TRUE;
}

bool BizTeleworkMainWindow::IsShowHealthStatusDialogRemindFromUnlocking()
{
    time_t now = time(0);
    struct tm timeCurrentStruct;

    errno_t e;
    if (e = localtime_s(&timeCurrentStruct, &now))
    {
        LOG_ERROR("localtime_s() failed. %d", e);
        return false;
    }

    int64_t timeToShowHealthStatusDialog = 0;
    AppConfig appConfig;
    if (!appConfig.GetTimeToShowHealthStatusDialog(&timeToShowHealthStatusDialog))
    {
        LOG_WARN("appConfig.GetTimeToShowHealthStatusDialog() failed.");
        timeToShowHealthStatusDialog = DEFAULT_TIME_TO_SHOW_HEALTH_STATUS_DIALOG;
    }

    int sec = timeToShowHealthStatusDialog % 60;
    int min = (timeToShowHealthStatusDialog / 60) % 60;
    int hour = timeToShowHealthStatusDialog / 3600;

    struct tm timePointCanBeDisplayRemindStruct;
    timePointCanBeDisplayRemindStruct = timeCurrentStruct;
    timePointCanBeDisplayRemindStruct.tm_hour = hour;
    timePointCanBeDisplayRemindStruct.tm_min = min;
    timePointCanBeDisplayRemindStruct.tm_sec = sec;

    double diffCurrentAndTimePointCanBeDisplay = mktime(&timeCurrentStruct) - mktime(&timePointCanBeDisplayRemindStruct);
    if (diffCurrentAndTimePointCanBeDisplay < 0)
    {
        LOG_INFO("Current time early %d h : %d m : %d s", hour, min, sec);
        return false;
    }

    int64_t timeDisplayRemind = 0;
    if (!appConfig.GetTimeDisplayHealthStatusRemind(&timeDisplayRemind))
    {
        // Reach here Windows registry items has some problems.
        LOG_ERROR("appConfig.GetTimeDisplayHealthStatusRemind() failed.");
        return false;
    }
    time_t timeTDisplay = timeDisplayRemind;
    struct tm localTimeDisplayRemind;
    if (e = localtime_s(&localTimeDisplayRemind, &timeTDisplay))
    {
        LOG_ERROR("localtime_s() failed. %d", e);
        return false;
    }

    struct tm timePointStartDayStruct;
    timePointStartDayStruct = timeCurrentStruct;
    timePointStartDayStruct.tm_hour = 0;
    timePointStartDayStruct.tm_min = 0;
    timePointStartDayStruct.tm_sec = 0;

    double diffTimePointStartDayAndTimeDisplayRemind = mktime(&timePointStartDayStruct) - mktime(&localTimeDisplayRemind);
    if (diffTimePointStartDayAndTimeDisplayRemind < 0)
    {
        LOG_INFO("Displayed remind.");
        return false;
    }

    return true;
}

bool BizTeleworkMainWindow::BringWindowToTop()
{
    HWND hCurWnd = NULL;
    if (!(hCurWnd = ::GetForegroundWindow()))
    {
        LOG_WARN("GetForegroundWindow() failed.");
        return false;
    }

    DWORD dwMyID = 0;
    dwMyID = ::GetCurrentThreadId();
    if (dwMyID == 0)
    {
        LOG_ERROR("GetCurrentThreadId() failed.");
        return false;
    }

    DWORD dwCurID = 0;
    dwCurID = ::GetWindowThreadProcessId(hCurWnd, NULL);
    if (dwCurID == 0)
    {
        LOG_ERROR("GetCurrentThreadId() failed.");
        return false;
    }

    if (!::AttachThreadInput(dwCurID, dwMyID, TRUE))
    {
        LOG_ERROR("AttachThreadInput() failed. %lu", GetLastError());
        return false;
    }

    if (!::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE))
    {
        LOG_ERROR("SetWindowPos() failed. %lu", GetLastError());
        return false;
    }

    if (!::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE))
    {
        LOG_ERROR("SetWindowPos() failed. %lu", GetLastError());
        return false;
    }

    if (!::SetForegroundWindow(m_hWnd))
    {
        LOG_ERROR("SetForegroundWindow() failed.");
        return false;
    }

    if (!::SetFocus(m_hWnd))
    {
        LOG_ERROR("SetFocus() failed. %lu", GetLastError());
        return false;
    }

    if (!::SetActiveWindow(m_hWnd))
    {
        LOG_ERROR("SetActiveWindow() failed. %lu", GetLastError());
        return false;
    }

    if (!::AttachThreadInput(dwCurID, dwMyID, FALSE))
    {
        LOG_ERROR("AttachThreadInput() failed. %lu", GetLastError());
        return false;
    }

    return true;
}

bool BizTeleworkMainWindow::CopyAuthenticationToCurrentUser()
{
    std::wstring companyGuid;
    std::vector<BYTE> agentGuid{ 0 };
    std::wstring apiUrl;
    if (!m_AuthenHelper->GetAuthenInforFromLocalMachine(&companyGuid, &agentGuid, &apiUrl))
    {
        LOG_INFO("The application has not been authenticated.");
        return true;
    }

    if (!m_AuthenHelper->SetCompanyGuid(companyGuid))
    {
        LOG_ERROR("Copy company GUID to current user failed.");
        return false;
    }

    if (!m_AuthenHelper->SetAgentGuid(agentGuid))
    {
        LOG_ERROR("Copy agent GUID to current user failed.");
        return false;
    }

    if (!m_AuthenHelper->SetApiUrl(apiUrl))
    {
        LOG_ERROR("Copy api Url to current user failed.");
        return false;
    }

    if (!m_AppConfig->SetCopiedAuthentication(true))
    {
        LOG_ERROR("SetCopiedAuthentication() failed.");
        return false;
    }

    if (!m_AuthenHelper->DeleteAuthenticationFromLocalMachine())
    {
        LOG_ERROR("Delete authentication from local machine failed.");
        return false;
    }
    return true;
}

void BizTeleworkMainWindow::SetTimeDisplayHealthStatusRemindToCurrentUser()
{
    if (!m_AppConfig->DeleteTimeDisplayHealthStatusRemind())
    {
        LOG_WARN("Delete TimeDisplayHealthStatusRemind from local machine failed.");
    }

    if (!m_AppConfig->SetTimeDisplayHealthStatusRemind(0))
    {
        LOG_WARN("Set TimeDisplayHealthStatusRemind LOCAL USER failed.");
    }
}

bool BizTeleworkMainWindow::ShowToastNotification(const std::wstring& title, const std::wstring& content)
{
    std::wstring version = L"";
    m_AppConfig->GetVersion(&version);
    TeleworkToastNotification::GetInstance()->SetAppName(ProductName);
    TeleworkToastNotification::GetInstance()->SetAppUserModelId(TeleworkToastNotification::BuildAUMI(CompanyName,
                                                                                                      ProductName,
                                                                                                      SubProduct,
                                                                                                      version));

    TeleworkToastNotification::ToastError error;
    if (!TeleworkToastNotification::GetInstance()->Initialize(&error))
    {
        LOG_ERROR("Initialize toast notification failed.");
        return false;
    }

    ToastNotificationTemplate::ToastNotificationTemplateType type = ToastNotificationTemplate::ToastNotificationTemplateType::ImageAndText02;
    ToastNotificationTemplate templ(type);

    wchar_t processPath[MAX_PATH];
    if (::GetModuleFileName(NULL, processPath, MAX_PATH) != 0)
    {
        boost::filesystem::path path(processPath);
        std::wstring pathImage = path.parent_path().append(L"BizTelework.ico").wstring();
        templ.SetImagePath(pathImage);
    }
    templ.SetTextField(title, ToastNotificationTemplate::TextField::FirstLine);
    templ.SetTextField(content, ToastNotificationTemplate::TextField::SecondLine);

    TeleworkToastNotification::GetInstance()->Initialize();

    INT64 resultShowToast = TeleworkToastNotification::GetInstance()->ShowToast(templ, m_ToastNotificationHandler.get());

    return (resultShowToast != -1);
}

LRESULT BizTeleworkMainWindow::OnMessageToastNotificationHandler(WPARAM wParam, LPARAM lParam)
{
    (void)lParam;  // Unused

    ToastNotificationHandlerInterface::TypeEventHandlerEnum typeHandler = (ToastNotificationHandlerInterface::TypeEventHandlerEnum)wParam;

    if (typeHandler == ToastNotificationHandlerInterface::TypeEventHandlerEnum::Actived)
    {
        ShowDialog(WhereCallShowDialog::ON_TOAST_NOTIFICATION_HANDLER);
    }

    return 0;
}

bool BizTeleworkMainWindow::SetToastNotificationHandler()
{
    HWND hWnd = GetSafeHwnd();
    if (hWnd == NULL)
    {
        LOG_WARN("GetSafeHwnd() failed.");
        return false;
    }

    m_ToastNotificationHandler->SetToastNotificationHandler(hWnd);
    return true;
}
