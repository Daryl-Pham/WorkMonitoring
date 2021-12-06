#include "stdafx.h"
#include "resource.h"
#include "BizTeleworkOnDutyDialog.h"
#include "BizTeleworkColor.h"
#include "BizTeleworkDimension.h"
#include "BizTeleworkDialogUtility.h"
#include "BizTeleworkMessage.h"
#include "RequestWorkLogsType.h"
#include "BizTeleworkDialogDependency.h"
#include "afxdialogex.h"
#include <Windows.h>
#include <shellapi.h>
#include <AppConfig.h>
#include <AuthenHelper.h>
#include <Language.h>
#include <Log.h>
#include <cpprest/uri_builder.h>
#include <boost/timer/timer.hpp>
#include "DeviceLog.h"
#include "HttpStatusCode.h"
#include "boost/date_time/posix_time/posix_time_duration.hpp"
#include "ActiveApplication.h"
#include "ResponsiveCreator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    const DWORD CALCULATE_WORKING_TIME_TIMER_ID = 10000;

    const DWORD CALCULATE_WORKING_TIME_TIMER_INTERVAL = 1000;

    const int64_t TWENTY_FOR_HOURS_IN_SECONDS = 86400;

    /* Reference ticket https://biztelework.atlassian.net/browse/BTC-377
     * Time of some devices are slower than the system time. So, we will reduce the start time by 5 seconds.
     */
    const int64_t DELAY_TIME_IN_SECONDS = 5;

}  // namespace

IMPLEMENT_DYNAMIC(BizTeleworkOnDutyDialog, CDialogEx)

BizTeleworkOnDutyDialog::BizTeleworkOnDutyDialog(const BizTeleworkDialogDependency& dependency,
    const int64_t unixStartedTime,
    CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_BIZTELEWORK_ON_DUTY_DIALOG, pParent)
    , m_LanguageResource(dependency.GetLanguageResource())
    , m_UnixStartTime(unixStartedTime - DELAY_TIME_IN_SECONDS)
{
}

BizTeleworkOnDutyDialog::~BizTeleworkOnDutyDialog()
{
}

BOOL BizTeleworkOnDutyDialog::KillCalculateTimer()
{
    BOOL resultKillTimer = FALSE;
    if (m_CalculateWorkingTimeTimer)
    {
        resultKillTimer = KillTimer(m_CalculateWorkingTimeTimer);
        m_CalculateWorkingTimeTimer = 0;
    }
    return resultKillTimer;
}

BOOL BizTeleworkOnDutyDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    ApplyLanguageResource();
    ApplyPositionDialogAndControl();
    SetOperatingTimeContent();
    ApplyStyle();
    SetTimersDialog();

    if (!SetForegroundWindow())
    {
        LOG_WARN("SetForegroundWindow() failed.");
    }

    return TRUE;
}

void BizTeleworkOnDutyDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_END_WORK, m_ButtonEndWork);
    DDX_Control(pDX, IDB_STATIC_IMAGE, m_StaticImage);
    DDX_Control(pDX, IDC_STATIC_OPERATING_TIME_LABEL, m_OperatingTimeLabel);
    DDX_Control(pDX, IDC_STATIC_OPERATING_TIME_CONTENT, m_OperatingTimeContent);
    DDX_Control(pDX, IDC_STATIC_NOTIFICATION, m_NotificationTerminalTimeIncorrect);
}

BEGIN_MESSAGE_MAP(BizTeleworkOnDutyDialog, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_ACTIVATE()
    ON_BN_CLICKED(IDC_BUTTON_END_WORK, &BizTeleworkOnDutyDialog::OnBnClickedButtonFinishWorking)
    ON_WM_TIMER()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// BizTeleworkOnDutyDialog Message Handler

HBRUSH BizTeleworkOnDutyDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hBrush = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
    if (pWnd->GetDlgCtrlID() == IDC_STATIC_OPERATING_TIME_LABEL ||
        pWnd->GetDlgCtrlID() == IDC_STATIC_OPERATING_TIME_CONTENT ||
        pWnd->GetDlgCtrlID() == IDC_STATIC_NOTIFICATION)
    {
        pDC->SetTextColor(BizTeleworkColor::StaticText());
    }
    return hBrush;
}

void BizTeleworkOnDutyDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    (void)pWndOther;   // Unused
    (void)bMinimized;  // Unused

    if (nState == WA_INACTIVE && !PostMessage(WM_CLOSE))
    {
        LOG_WARN("PostMessage(WM_CLOSE) failed.");
    }
}

void BizTeleworkOnDutyDialog::ApplyLanguageResource()
{
    std::wstring str(L"");
    if (!m_LanguageResource->GetString(IDS_BUTTON_END_WORK, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_BUTTON_END_WORK) failed.");
        m_ButtonEndWork.SetWindowTextW(L"");
    }
    else
    {
        m_ButtonEndWork.SetWindowTextW(str.c_str());
    }

    str = L"";
    if (!m_LanguageResource->GetString(IDS_STATIC_OPERATING_TIME_LABEL, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_STATIC_OPERATING_TIME_LABEL) failed.");
        m_OperatingTimeLabel.SetWindowTextW(L"");
    }
    else
    {
        m_OperatingTimeLabel.SetWindowTextW(str.c_str());
    }
}

void BizTeleworkOnDutyDialog::ApplyPositionDialogAndControl()
{
    MONITORINFOEX monitorInfo;
    if (BizTeleworkDialogUtility::GetMonitorInfor(this, &monitorInfo))
    {
        // monitorInfo.rcMonitor shows rectangle of desktop without taskbar.
        LONG x = monitorInfo.rcWork.right - BizTeleworkDimension::OnDutyDialogWidth() - BizTeleworkDimension::OnDutyDialogMarginRight();
        LONG y = monitorInfo.rcWork.bottom - BizTeleworkDimension::OnDutyDialogHeight() - BizTeleworkDimension::OnDutyDialogMarginBottom();

        SetWindowPos(&CWnd::wndTopMost, x, y, BizTeleworkDimension::OnDutyDialogWidth(), BizTeleworkDimension::OnDutyDialogHeight(), 0);

        RECT rect;
        this->GetClientRect(&rect);

        x = rect.right - BizTeleworkDimension::ButtonWidth() - BizTeleworkDimension::EndButtonMarginRight();
        y = rect.bottom - BizTeleworkDimension::ButtonHeight() - BizTeleworkDimension::StartButtonMarginBottom();
        m_ButtonEndWork.SetWindowPos(NULL, x, y, BizTeleworkDimension::ButtonWidth(), BizTeleworkDimension::ButtonHeight(), 0);

        x = rect.right - BizTeleworkDimension::ImageMarginRightDialog() - BizTeleworkDimension::ImageDimension();
        y = rect.bottom - BizTeleworkDimension::ImageMarginBottomOnDutyDialog() - BizTeleworkDimension::ImageDimension();
        m_StaticImage.SetWindowPos(NULL, x, y, BizTeleworkDimension::ImageDimension(), BizTeleworkDimension::ImageDimension(), 0);

        x = rect.right - BizTeleworkDimension::OperationLabelWidth() - BizTeleworkDimension::OperationLabelMarginRight();
        y = rect.bottom - BizTeleworkDimension::OperationLabelHeight() - BizTeleworkDimension::OperationLabelMarginBottom();
        m_OperatingTimeLabel.SetWindowPos(NULL, x, y, BizTeleworkDimension::OperationLabelWidth(), BizTeleworkDimension::OperationLabelHeight(), 0);

        x = rect.right - BizTeleworkDimension::OperationContentWidth() - BizTeleworkDimension::OperationContentMarginRight();
        y = rect.bottom - BizTeleworkDimension::OperationContentHeight() - BizTeleworkDimension::OperationContentMarginBottom();
        m_OperatingTimeContent.SetWindowPos(NULL, x, y, BizTeleworkDimension::OperationContentWidth(), BizTeleworkDimension::OperationContentHeight(), 0);

        x = rect.right - BizTeleworkDimension::NotificationTerminalTimeWidth() - BizTeleworkDimension::NotificationTerminalTimeMarginRight();
        y = rect.bottom - BizTeleworkDimension::NotificationTerminalTimeHeight() - BizTeleworkDimension::NotificationTerminalTimeMarginBottom();
        m_NotificationTerminalTimeIncorrect.SetWindowPos(NULL, x, y, BizTeleworkDimension::NotificationTerminalTimeWidth(),
            BizTeleworkDimension::NotificationTerminalTimeHeight(), 0);
    }
}

void BizTeleworkOnDutyDialog::ApplyStyle()
{
    SetBackgroundColor(BizTeleworkColor::DialogBackground());
    ApplyButtonStyle(&m_ButtonEndWork);

    BizTeleworkDialogUtility::ApplyStaticImageStyle(&m_StaticImage, &m_BitMapCleanup);

    BizTeleworkDialogUtility::ApplyWeightFont(&m_OperatingTimeLabel, FW_NORMAL, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontSize(&m_OperatingTimeLabel, 10, m_NewFont);

    BizTeleworkDialogUtility::ApplyWeightFont(&m_OperatingTimeContent, FW_BOLD, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(&m_OperatingTimeContent, L"Yu Gothic UI Semibold", m_NewFont);
    BizTeleworkDialogUtility::ApplyFontSize(&m_OperatingTimeContent, 19, m_NewFont);

    BizTeleworkDialogUtility::ApplyWeightFont(&m_NotificationTerminalTimeIncorrect, FW_MEDIUM, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(&m_NotificationTerminalTimeIncorrect, L"Yu Gothic UI Semibold", m_NewFont);
    BizTeleworkDialogUtility::ApplyFontSize(&m_NotificationTerminalTimeIncorrect, 14, m_NewFont);
}

void BizTeleworkOnDutyDialog::ApplyButtonStyle(CMFCButton* button)
{
    CMFCButton::EnableWindowsTheming(FALSE);
    button->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    button->m_bTransparent = FALSE;
    button->SetFaceColor(BizTeleworkColor::ButtonBackground(), true);
    button->SetTextColor(BizTeleworkColor::ButtonText());
    BizTeleworkDialogUtility::ApplyFontSize(button, 11, m_NewFont);
}

void BizTeleworkOnDutyDialog::OnBnClickedButtonFinishWorking()
{
    m_ButtonEndWork.EnableWindow(FALSE);
    auto thr = std::thread([this]()
        {
            std::unique_ptr<Request> request = GettingRequest::GetRequest();
            if (request)
            {
                ProcessDeviceLog(request.get());
                ProcessActiveAppLog(request.get());
                ProcessWorkLog(request.get());
            }

        });
    thr.detach();
}

void BizTeleworkOnDutyDialog::SetTimersDialog()
{
    m_CalculateWorkingTimeTimer = this->SetTimer(CALCULATE_WORKING_TIME_TIMER_ID, CALCULATE_WORKING_TIME_TIMER_INTERVAL, NULL);
}

void BizTeleworkOnDutyDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == CALCULATE_WORKING_TIME_TIMER_ID)
    {
        SetOperatingTimeContent();
    }

    CDialogEx::OnTimer(nIDEvent);
}

TypeOperatingContent BizTeleworkOnDutyDialog::GetOperatingTimeContentType(int64_t secondWorkingTime)
{
    TypeOperatingContent type = TypeOperatingContent::NORMAL_TYPE;
    if (secondWorkingTime < 0)
    {
        type = TypeOperatingContent::PAST_TYPE;
    }
    else if (secondWorkingTime >= TWENTY_FOR_HOURS_IN_SECONDS)
    {
        type = TypeOperatingContent::TWENTY_FOUR_TYPE;
    }

    return type;
}

void BizTeleworkOnDutyDialog::SetOperatingTimeContent()
{
    int64_t secondWorkingTime = Utility::ConvertTimePointToUnixTime(boost::chrono::system_clock::now()) - m_UnixStartTime;
    TypeOperatingContent type = GetOperatingTimeContentType(secondWorkingTime);
    std::wstring str = L"";
    GetOperatingTimeContent(type, secondWorkingTime, &str);
    m_OperatingTimeContent.SetWindowTextW(str.c_str());
    if (type == TypeOperatingContent::PAST_TYPE)
    {
        if (!m_LanguageResource->GetString(IDS_TERMINAL_TIME_IN_PAST, &str))
        {
            str = L"";
        }
    }
    else
    {
        str = L"";
    }
    m_NotificationTerminalTimeIncorrect.SetWindowTextW(str.c_str());
}

void BizTeleworkOnDutyDialog::OpenEmployeeDashboard(const std::wstring& url) const
{
    int result = reinterpret_cast<int>(ShellExecute(NULL, L"open",url.c_str() , NULL, NULL, SW_SHOW));
    if (result <= 32)
    {
        LOG_WARN("ShellExecute() failed. %d", result);
    }
}

bool BizTeleworkOnDutyDialog::GetOperatingTimeContent(const TypeOperatingContent type,
                                                      const int64_t secondWorkingTime,
                                                      std::wstring* operatingContent)
{
    std::wstring str = L"";
    std::wstringstream stream;
    bool result = true;
    switch (type)
    {
    case TypeOperatingContent::NORMAL_TYPE:
        if (!m_LanguageResource->GetString(IDS_HOUR_LABEL, &str))
        {
            str = L"";
        }
        stream << std::to_wstring(secondWorkingTime / 3600) << str;

        str = L"";
        if (!m_LanguageResource->GetString(IDS_MINUTE_LABEL, &str))
        {
            str = L"";
        }
        stream << std::to_wstring((secondWorkingTime % 3600) / 60) << str;

        str = L"";
        str = stream.str();
        break;
    case TypeOperatingContent::PAST_TYPE:
        if (!m_LanguageResource->GetString(IDS_HOUR_LABEL, &str))
        {
            str = L"";
        }
        stream << L"0" << str;

        str = L"";
        if (!m_LanguageResource->GetString(IDS_MINUTE_LABEL, &str))
        {
            str = L"";
        }
        stream << L"0" << str;

        str = L"";
        str = stream.str();
        break;
    case TypeOperatingContent::TWENTY_FOUR_TYPE:
        if (!m_LanguageResource->GetString(IDS_TWENTY_FOUR_HOURS_OR_MORE, &str))
        {
            str = L"";
        }
        break;
    default:
        result = false;
        break;
    }

    *operatingContent = str;
    return result;
}

void BizTeleworkOnDutyDialog::OnClose()
{
    if (!KillCalculateTimer())
    {
        LOG_WARN("KillCalculateTimer() failed.");
    }
    CDialogEx::OnClose();
}

int64_t BizTeleworkOnDutyDialog::GetUnixStartTime() const
{
    return m_UnixStartTime;
}

BOOL BizTeleworkOnDutyDialog::ProcessDeviceLog(Request* request)
{
    DeviceLog deviceLog;
    bool keyboardLogFileExists = false;
    bool mouseLogFileExists = false;

    std::wstring jsonStrigify;
    LogLines logLines;
    deviceLog.ReadDeviceLog(&logLines, &keyboardLogFileExists, &mouseLogFileExists);
    if (!Logging::DeviceLogStringifyAsJsonBody(logLines, &jsonStrigify))
    {
        LOG_ERROR("LogFile::StringifyAsJsonBody() failed.");
        return FALSE;
    }

    boost::timer::cpu_timer timer;  // To know duration of post. For debug Internet connectivity condition.

    std::unique_ptr<CallContext> callContext = request->CallWindowsDeviceLogs(jsonStrigify);
    if (CheckCallContext(callContext.get()))
    {
        // "%w" means wall clock time.
        LOG_INFO("callContext->Wait() takes %s[sec].", timer.format(2 ,"%w").c_str());
        LOG_INFO("Succeeded to upload an event log.");
        deviceLog.DeleteDeviceLog(keyboardLogFileExists ,mouseLogFileExists);
        return TRUE;
    }
    else
    {
        LOG_WARN("Requesting jsonStrigify = %S", jsonStrigify.c_str());
    }
    return FALSE;
}
BOOL BizTeleworkOnDutyDialog::ProcessActiveAppLog(Request* request)
{
    std::wstring jsonStrigify;
    LogLines logLines;
    ActiveApplication activeApp(ActiveAppLogFileName);
    if (!activeApp.ReadActiveAppLog(&logLines))
    {
        LOG_ERROR("ProcessActiveAppLog Log is Null");
        return FALSE;
    }

    if (!Logging::ActiveAppLogStringifyAsJsonBody(logLines, &jsonStrigify))
    {
        LOG_ERROR("LogFile::ActiveAppLogStringifyAsJsonBody() failed.");
        return FALSE;
    }

    boost::timer::cpu_timer timer;  // To know duration of post. For debug Internet connectivity condition.

    std::unique_ptr<CallContext> callContext = request->CallWindowsApplicationLogs(jsonStrigify);
    if (CheckCallContext(callContext.get()))
    {
        // "%w" means wall clock time.
        LOG_INFO("callContext->Wait() takes %s[sec].", timer.format(2 ,"%w").c_str());
        LOG_INFO("Succeeded to upload an application log.");
        Logging applicationLogFile = Logging(Utility::GetDefaultDataDirectory(), ActiveAppLogFileName);
        if (!applicationLogFile.DeleteLog())
        {
            LOG_WARN("applicationLogFile.DeleteLog() failed.");
        }
        return TRUE;
    }
    else
    {
        LOG_WARN("Requesting jsonStrigify = %S", jsonStrigify.c_str());
    }
    return FALSE;
}

BOOL BizTeleworkOnDutyDialog::CheckCallContext(CallContext* callContext) const
{
    boost::timer::cpu_timer timer;  // To know duration of post. For debug Internet connectivity condition.
    if (callContext == nullptr)
    {
        LOG_WARN("Call API failed.");
        return FALSE;
    }
    else if (!callContext->Wait())
    {
        LOG_INFO("callContext->Wait() takes %s[sec].", timer.format(2, "%w").c_str());
        LOG_WARN("callContext->Wait() failed.");
        return FALSE;
    }
    else if (callContext->StatusCode() != HTTP_STATUS_CODE_CREATED)
    {
        LOG_WARN("The request is failed. StatusCode: %d", callContext->StatusCode());
        return FALSE;
    }
    return TRUE;
}

BOOL BizTeleworkOnDutyDialog::ProcessWorkLog(Request* request)
{
    std::unique_ptr<CallContext> callContext = request->CallWindowsWorkLogs(REQUEST_WORK_LOGS_TYPE_END);

    if (!BizTeleworkDialogUtility::CheckVisibleDialog(this))
    {
        LOG_WARN("BizTeleworkDialogUtility::CheckVisibleHandle failed.");
        return 0;
    }

    if (!BizTeleworkDialogUtility::SetCursorWait(false ,this))
    {
        LOG_WARN("BizTeleworkDialogUtility::SetCursorWait() failed.");
    }

    if (CheckCallContext(callContext.get()))
    {
        try
        {
            std::unique_ptr<Responsive> res = ResponsiveCreator::GetResponsive(HTTP_STATUS_CODE_OK, callContext->Body());
            WorkLogOkieResponsive& workLogResponsive = dynamic_cast<WorkLogOkieResponsive&>(*res);
            if (!workLogResponsive.ParseValue())
            {
                LOG_ERROR("authenRes.ParseValue() failed.");
                return FALSE;
            }
            std::wstring url = workLogResponsive.GetSelfManagementUrl();
            OpenEmployeeDashboard(url);
        }
        catch (const std::bad_cast& e)
        {
            LOG_ERROR("Dynamic cast failed %s.", e.what());
            return FALSE;
        }
    }
    else
    {
        ErrorDialogMessageType errorType = BizTeleworkDialogUtility::GetErrorDialogType(callContext->StatusCode(), callContext->Body());
        const ErrorDialogMessageType errorDialogType = static_cast<ErrorDialogMessageType>(errorType);
        BizTeleworkDialogUtility::DisplayErrorDialog(errorDialogType);
        LOG_WARN("Requesting workLogsType = %d", REQUEST_WORK_LOGS_TYPE_END);
        return FALSE;
    }

    return TRUE;
}
