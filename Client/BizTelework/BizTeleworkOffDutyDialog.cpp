#include "stdafx.h"
#include "resource.h"
#include "BizTeleworkOffDutyDialog.h"
#include "BizTeleworkWorkDayTargetDialog.h"
#include "BizTeleworkColor.h"
#include "BizTeleworkDimension.h"
#include "BizTeleworkDialogUtility.h"
#include "BizTeleworkMessage.h"
#include "RequestWorkLogsType.h"
#include "afxdialogex.h"
#include <Windows.h>
#include <Language.h>
#include <Log.h>
#include <boost/chrono/io/time_point_io.hpp>
#include <boost/timer/timer.hpp>
#include "DeviceLog.h"
#include "HttpStatusCode.h"
#include "ActiveApplication.h"
#include "GettingRequest.h"
#include "BizTeleworkDependency.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{

    DEFINE_LOGGER_FILE_NAME;
    const int TimeOut = 10000;

}  // namespace

IMPLEMENT_DYNAMIC(BizTeleworkOffDutyDialog, CDialogEx)

BizTeleworkOffDutyDialog::BizTeleworkOffDutyDialog(const BizTeleworkDialogDependency& dependency, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_BIZTELEWORK_OFF_DUTY_DIALOG, pParent)
    , m_LanguageResource(dependency.GetLanguageResource())
{
}

BizTeleworkOffDutyDialog::~BizTeleworkOffDutyDialog()
{
}

BOOL BizTeleworkOffDutyDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    ApplyLanguageResource();
    ApplyPositionDialogAndControl();
    ApplyStyle();

    if (!SetForegroundWindow())
    {
        LOG_ERROR("SetForegroundWindow() failed.");
    }

    return TRUE;
}

void BizTeleworkOffDutyDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_NOTIFICATION, m_StaticNotification);
    DDX_Control(pDX, IDC_BUTTON_START, m_ButtonStartWorking);
    DDX_Control(pDX, IDC_BUTTON_NOT_YET, m_ButtonNotYet);
    DDX_Control(pDX, IDB_STATIC_IMAGE, m_StaticImage);
}

BEGIN_MESSAGE_MAP(BizTeleworkOffDutyDialog, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_ACTIVATE()
    ON_BN_CLICKED(IDC_BUTTON_START, &BizTeleworkOffDutyDialog::OnBnClickedButtonStartWorking)
    ON_BN_CLICKED(IDC_BUTTON_NOT_YET, &BizTeleworkOffDutyDialog::OnBnClickedButtonNotYetWorking)
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_WORK_LOGS_RESPONSE, &BizTeleworkOffDutyDialog::OnMessageWorkLogsResponse)
END_MESSAGE_MAP()

// BizTeleworkOffDutyDialog Message Handler

HBRUSH BizTeleworkOffDutyDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hBrush = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd->GetDlgCtrlID() == IDC_STATIC_NOTIFICATION)
    {
        pDC->SetTextColor(BizTeleworkColor::StaticText());
    }

    return hBrush;
}

void BizTeleworkOffDutyDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    (void)pWndOther;   // Unused
    (void)bMinimized;  // Unused

    if (nState == WA_INACTIVE && !PostMessage(WM_CLOSE))
    {
        LOG_ERROR("PostMessage(WM_CLOSE) failed.");
    }
}

afx_msg LRESULT BizTeleworkOffDutyDialog::OnMessageWorkLogsResponse(WPARAM wParam, LPARAM lParam)
{
    if (!BizTeleworkDialogUtility::CheckVisibleDialog(this))
    {
        LOG_ERROR("BizTeleworkDialogUtility::CheckVisibleHandle failed.");
        return 0;
    }

    if (!BizTeleworkDialogUtility::SetCursorWait(false, this))
    {
        LOG_ERROR("BizTeleworkDialogUtility::SetCursorWait() failed.");
    }

    BOOL succeeded = static_cast<BOOL>(wParam);
    if (!succeeded)
    {
        const ErrorDialogMessageType errorDialogType = static_cast<ErrorDialogMessageType>(lParam);
        BizTeleworkDialogUtility::DisplayErrorDialog(errorDialogType);
    }
    else
    {
        std::unique_ptr<Request> request = GettingRequest::GetRequest();
        if (!request)
        {
            return 0;
        }

        SendLogs();

        // Display work day target dialog when start working successfull https://biztelework.atlassian.net/browse/BTC-904.
        BizTeleworkDialogDependency dependency;
        auto workDayTargetDialog = std::make_unique<BizTeleworkWorkDayTargetDialog>(dependency, nullptr);
        workDayTargetDialog->DoModal();
    }

    return 0;
}

void BizTeleworkOffDutyDialog::ApplyLanguageResource()
{
    std::wstring str(L"");
    if (!m_LanguageResource->GetString(IDS_STATIC_NOTIFICATION, &str))
    {
        LOG_ERROR("m_LanguageResource->GetString(IDC_STATIC) failed.");
        m_StaticNotification.SetWindowTextW(L"");
    }
    else
    {
        m_StaticNotification.SetWindowTextW(str.c_str());
    }

    str = L"";
    if (!m_LanguageResource->GetString(IDS_BUTTON_START, &str))
    {
        LOG_ERROR("m_LanguageResource->GetString(IDC_BUTTON_START) failed.");
        m_ButtonStartWorking.SetWindowTextW(L"");
    }
    else
    {
        m_ButtonStartWorking.SetWindowTextW(str.c_str());
    }

    str = L"";
    if (!m_LanguageResource->GetString(IDS_BUTTON_NOT_YET, &str))
    {
        LOG_ERROR("m_LanguageResource->GetString(IDC_BUTTON_NOT_YET) failed.");
        m_ButtonNotYet.SetWindowTextW(L"");
    }
    else
    {
        m_ButtonNotYet.SetWindowTextW(str.c_str());
    }
}

void BizTeleworkOffDutyDialog::ApplyPositionDialogAndControl()
{
    MONITORINFOEX monitorInfo;
    if (BizTeleworkDialogUtility::GetMonitorInfor(this, &monitorInfo))
    {
        // monitorInfo.rcMonitor shows rectangle of desktop without taskbar.
        LONG x = monitorInfo.rcWork.right - BizTeleworkDimension::OffDutyDialogWidth() - BizTeleworkDimension::OffDutyDialogMarginRight();
        LONG y = monitorInfo.rcWork.bottom - BizTeleworkDimension::OffDutyDialogHeight() - BizTeleworkDimension::OffDutyDialogMarginBottom();

        SetWindowPos(&CWnd::wndTopMost, x, y, BizTeleworkDimension::OffDutyDialogWidth(), BizTeleworkDimension::OffDutyDialogHeight(), 0);

        RECT rect;
        this->GetClientRect(&rect);

        x = rect.right - BizTeleworkDimension::ButtonWidth() - BizTeleworkDimension::NotYetButtonMarginRight();
        y = rect.bottom - BizTeleworkDimension::ButtonHeight() - BizTeleworkDimension::StartButtonMarginBottom();
        m_ButtonNotYet.SetWindowPos(NULL, x, y, BizTeleworkDimension::ButtonWidth(), BizTeleworkDimension::ButtonHeight(), 0);

        x = rect.right - BizTeleworkDimension::ButtonWidth() * 2 - BizTeleworkDimension::NotYetButtonMarginRight()
            - BizTeleworkDimension::SpaceBetween2Buttons();
        y = rect.bottom - BizTeleworkDimension::ButtonHeight() - BizTeleworkDimension::StartButtonMarginBottom();
        m_ButtonStartWorking.SetWindowPos(NULL, x, y, BizTeleworkDimension::ButtonWidth(), BizTeleworkDimension::ButtonHeight(), 0);

        x = rect.right - BizTeleworkDimension::ImageMarginRightDialog() - BizTeleworkDimension::ImageDimension();
        y = rect.bottom - BizTeleworkDimension::ImageMarginBottomDialog() - BizTeleworkDimension::ImageDimension();
        m_StaticImage.SetWindowPos(NULL, x, y, BizTeleworkDimension::ImageDimension(), BizTeleworkDimension::ImageDimension(), 0);

        x = rect.right - BizTeleworkDimension::NotificationWidth() - BizTeleworkDimension::NotificationMarginRight();
        y = rect.bottom - BizTeleworkDimension::NotificationHeight() - BizTeleworkDimension::NotificationMarginBottom();
        m_StaticNotification.SetWindowPos(NULL, x, y, BizTeleworkDimension::NotificationWidth(), BizTeleworkDimension::NotificationHeight(), 0);
    }
}

void BizTeleworkOffDutyDialog::ApplyStyle()
{
    SetBackgroundColor(BizTeleworkColor::DialogBackground());
    ApplyButtonStyle(&m_ButtonStartWorking);
    ApplyButtonStyle(&m_ButtonNotYet);
    BizTeleworkDialogUtility::ApplyWeightFont(&m_StaticNotification, FW_HEAVY, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontSize(&m_StaticNotification, 13, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(&m_StaticNotification, L"Yu Gothic UI Bold", m_NewFont);
    BizTeleworkDialogUtility::ApplyStaticImageStyle(&m_StaticImage, &m_BitMapCleanup);
}

void BizTeleworkOffDutyDialog::ApplyButtonStyle(CMFCButton* button)
{
    CMFCButton::EnableWindowsTheming(FALSE);
    button->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    button->m_bTransparent = FALSE;
    button->SetFaceColor(BizTeleworkColor::ButtonBackground(), true);
    button->SetTextColor(BizTeleworkColor::ButtonText());
    BizTeleworkDialogUtility::ApplyFontSize(button, 11, m_NewFont);
}

void BizTeleworkOffDutyDialog::OnBnClickedButtonStartWorking()
{
    if (!BizTeleworkDialogUtility::StartRequestWorkLogsAndControlItems(&m_ButtonStartWorking, REQUEST_WORK_LOGS_TYPE_START, this))
    {
        LOG_ERROR("BizTeleworkDialogUtility::StartRequestAndControlItems(START) failed.");
    }
}

void BizTeleworkOffDutyDialog::OnBnClickedButtonNotYetWorking()
{
    if (!PostMessage(WM_CLOSE))
    {
        LOG_ERROR("PostMessage(WM_CLOSE) failed.");
    }
}

bool BizTeleworkOffDutyDialog::SendLogs()
{
    LogLines logLines;
    bool keyboardLogFileExists = false;
    bool mouseLogFileExists = false;
    DeviceLog deviceLog;

    bool resultReadLog = deviceLog.ReadFullDeviceLog(&logLines, &keyboardLogFileExists, &mouseLogFileExists);

    // Function ReadDeviceLog() get content log from log files.
    // But, maybe at that time, the log files are held by another action (of BizTeleworkInterval, of BizTeleworkHook).
    // So we will wait to log files aren't held by another action with timeout is 10s
    int delay = 0;
    while (!resultReadLog)
    {
        Sleep(1000);
        if (delay < TimeOut)
        {
            delay += 1000;
            resultReadLog = deviceLog.ReadFullDeviceLog(&logLines, &keyboardLogFileExists, &mouseLogFileExists);
        }
        else
        {
            break;
        }
    }

    if (logLines.size() == 0)
    {
        const wchar_t TimeFormat[] = L"%Y-%m-%dT%H:%M:%SZ";
        auto now = boost::chrono::system_clock::now();
        std::wstringstream stream;
        stream << boost::chrono::time_fmt(boost::chrono::timezone::utc, TimeFormat) << now;
        logLines.push_back(std::make_pair(stream.str(), L"mouse_operation"));
    }

    deviceLog.SendLog(logLines, keyboardLogFileExists, mouseLogFileExists);

    ActiveApplication activeApp(ActiveAppLogFileName);
    logLines.clear();
    if (!activeApp.ReadActiveAppLog(&logLines))
    {
        return false;
    }
    activeApp.SendLog(logLines);

    return true;
}
