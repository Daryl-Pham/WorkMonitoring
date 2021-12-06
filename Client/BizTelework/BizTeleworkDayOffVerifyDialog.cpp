#include "stdafx.h"
#include "resource.h"
#include "BizTeleworkDayOffVerifyDialog.h"
#include "BizTeleworkOffDutyDialog.h"
#include "BizTeleworkColor.h"
#include "BizTeleworkDimension.h"
#include "BizTeleworkDialogUtility.h"
#include "BizTeleworkMessage.h"
#include "BizTeleworkDialogDependency.h"
#include "AppConfig.h"
#include "afxdialogex.h"
#include <Windows.h>
#include <Language.h>
#include <Log.h>
#include <boost/timer/timer.hpp>
#include "DeviceLog.h"
#include "HttpStatusCode.h"
#include "boost/date_time/posix_time/posix_time_duration.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{

    DEFINE_LOGGER_FILE_NAME;

}  // namespace

IMPLEMENT_DYNAMIC(BizTeleworkDayOffVerifyDialog, CDialogEx)

BizTeleworkDayOffVerifyDialog::BizTeleworkDayOffVerifyDialog(const BizTeleworkDialogDependency& dependency, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_BIZTELEWORK_DAY_OFF_VERIFICATION_DIALOG, pParent)
    , m_LanguageResource(dependency.GetLanguageResource())
{
}

BizTeleworkDayOffVerifyDialog::~BizTeleworkDayOffVerifyDialog()
{
}

BOOL BizTeleworkDayOffVerifyDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    ApplyLanguageResource();
    ApplyPositionDialogAndControl();
    ApplyStyle();

    if (!SetForegroundWindow())
    {
        LOG_WARN("SetForegroundWindow() failed.");
    }

    return TRUE;
}

void BizTeleworkDayOffVerifyDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_OFF_DAY, m_ButtonOffDay);
    DDX_Control(pDX, IDC_BUTTON_DO_WORKING, m_ButtonDoWorking);
    DDX_Control(pDX, IDB_STATIC_IMAGE, m_StaticImage);
    DDX_Control(pDX, IDC_STATIC_TAKE_BREAK_QUESTION, m_StaticTakeBreakQuestion);
}

BEGIN_MESSAGE_MAP(BizTeleworkDayOffVerifyDialog, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_ACTIVATE()
    ON_BN_CLICKED(IDC_BUTTON_OFF_DAY, &BizTeleworkDayOffVerifyDialog::OnBnClickedButtonOffDay)
    ON_BN_CLICKED(IDC_BUTTON_DO_WORKING, &BizTeleworkDayOffVerifyDialog::OnBnClickedButtonDoWorking)
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_DAY_OFF_REQUESTS_RESPONSIVE, &BizTeleworkDayOffVerifyDialog::OnMessageDayOffRequestsResponse)
    ON_WM_TIMER()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// BizTeleworkOnDutyDialog Message Handler

HBRUSH BizTeleworkDayOffVerifyDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hBrush = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
    if (pWnd->GetDlgCtrlID() == IDC_STATIC_TAKE_BREAK_QUESTION)
    {
        pDC->SetTextColor(BizTeleworkColor::StaticText());
    }
    return hBrush;
}

void BizTeleworkDayOffVerifyDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    (void)pWndOther;   // Unused
    (void)bMinimized;  // Unused

    if (nState == WA_INACTIVE && !PostMessage(WM_CLOSE))
    {
        LOG_ERROR("PostMessage(WM_CLOSE) failed.");
    }
}

void BizTeleworkDayOffVerifyDialog::ApplyLanguageResource()
{
    std::wstring str(L"");
    if (!m_LanguageResource->GetString(IDS_BUTTON_OFF_DAY, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_BUTTON_DAY_OFF) failed.");
        m_ButtonOffDay.SetWindowTextW(L"");
    }
    else
    {
        m_ButtonOffDay.SetWindowTextW(str.c_str());
    }

    str = L"";
    if (!m_LanguageResource->GetString(IDS_BUTTON_DO_WORKING, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_BUTTON_DO_WORKING) failed.");
        m_ButtonDoWorking.SetWindowTextW(L"");
    }
    else
    {
        m_ButtonDoWorking.SetWindowTextW(str.c_str());
    }

    str = L"";
    if (!m_LanguageResource->GetString(IDS_STATIC_TAKE_BREAK_QUESTION, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_STATIC_TAKE_BREAK_QUESTION) failed.");
        m_StaticTakeBreakQuestion.SetWindowTextW(L"");
    }
    else
    {
        m_StaticTakeBreakQuestion.SetWindowTextW(str.c_str());
    }
}

void BizTeleworkDayOffVerifyDialog::ApplyPositionDialogAndControl()
{
    MONITORINFOEX monitorInfo;
    if (BizTeleworkDialogUtility::GetMonitorInfor(this, &monitorInfo))
    {
        // monitorInfo.rcMonitor shows rectangle of desktop without taskbar.
        LONG x = monitorInfo.rcWork.right - BizTeleworkDimension::DayOffVerifyDialogWidth() - BizTeleworkDimension::DayOffVerifyDialogMarginRight();
        LONG y = monitorInfo.rcWork.bottom - BizTeleworkDimension::DayOffVerifyDialogHeight() - BizTeleworkDimension::DayOffVerifyDialogMarginBottom();

        SetWindowPos(&CWnd::wndTopMost, x, y, BizTeleworkDimension::DayOffVerifyDialogWidth(), BizTeleworkDimension::DayOffVerifyDialogHeight(), 0);

        RECT rect;
        this->GetClientRect(&rect);

        x = rect.right - BizTeleworkDimension::ButtonWidth() - BizTeleworkDimension::ButtonDoWorkingMarginRight();
        y = rect.bottom - BizTeleworkDimension::ButtonHeight() - BizTeleworkDimension::ButtonOffDayMarginBottom();
        m_ButtonDoWorking.SetWindowPos(NULL, x, y, BizTeleworkDimension::ButtonWidth(), BizTeleworkDimension::ButtonHeight(), 0);

        x = rect.right - BizTeleworkDimension::ButtonWidth() * 2
            - BizTeleworkDimension::SpaceBetween2Buttons()
            - BizTeleworkDimension::ButtonDoWorkingMarginRight();
        y = rect.bottom - BizTeleworkDimension::ButtonHeight() - BizTeleworkDimension::ButtonOffDayMarginBottom();
        m_ButtonOffDay.SetWindowPos(NULL, x, y, BizTeleworkDimension::ButtonWidth(), BizTeleworkDimension::ButtonHeight(), 0);

        x = rect.right - BizTeleworkDimension::TakeBreakQuestionWidth() - BizTeleworkDimension::TakeBreakQuestionMarginRight();
        y = rect.bottom - BizTeleworkDimension::TakeBreakQuestionHeight() - BizTeleworkDimension::TakeBreakQuestionMarginBottom();
        m_StaticTakeBreakQuestion.SetWindowPos(NULL, x, y, BizTeleworkDimension::TakeBreakQuestionWidth(), BizTeleworkDimension::TakeBreakQuestionHeight(), 0);
    }
}

void BizTeleworkDayOffVerifyDialog::ApplyStyle()
{
    SetBackgroundColor(BizTeleworkColor::DialogBackground());
    CMFCButton::EnableWindowsTheming(FALSE);
    ApplyButtonStyle(&m_ButtonOffDay);
    ApplyButtonStyle(&m_ButtonDoWorking);
    BizTeleworkDialogUtility::ApplyStaticImageStyle(&m_StaticImage, &m_BitMapCleanup);
    BizTeleworkDialogUtility::ApplyWeightFont(&m_StaticTakeBreakQuestion, FW_BOLD, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(&m_StaticTakeBreakQuestion, L"Yu Gothic UI Bold", m_NewFont);
    BizTeleworkDialogUtility::ApplyFontSize(&m_StaticTakeBreakQuestion, 14, m_NewFont);
}

void BizTeleworkDayOffVerifyDialog::ApplyButtonStyle(CMFCButton* button)
{
    button->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    button->m_bTransparent = FALSE;
    button->SetFaceColor(BizTeleworkColor::ButtonBackground(), true);
    button->SetTextColor(BizTeleworkColor::ButtonText());
    BizTeleworkDialogUtility::ApplyFontSize(button, 11, m_NewFont);
}

void BizTeleworkDayOffVerifyDialog::OnBnClickedButtonOffDay()
{
    if (!BizTeleworkDialogUtility::StartRequestDayOffRequestsAndControlItems(&m_ButtonOffDay, this))
    {
        LOG_WARN("StartRequestDayOffRequestsAndControlItems() failed.");
    }
}

void BizTeleworkDayOffVerifyDialog::OnBnClickedButtonDoWorking()
{
    AppConfig appConfig;
    std::wstring apiURL;
    if (appConfig.GetApiUrl(&apiURL))
    {
        BizTeleworkDialogDependency dependency;
        std::unique_ptr<BizTeleworkOffDutyDialog> offDutyDialog = std::make_unique<BizTeleworkOffDutyDialog>(dependency, nullptr);
        offDutyDialog->DoModal();
    }
}

afx_msg LRESULT BizTeleworkDayOffVerifyDialog::OnMessageDayOffRequestsResponse(WPARAM wParam, LPARAM lParam)
{
    if (!BizTeleworkDialogUtility::CheckVisibleDialog(this))
    {
        LOG_WARN("BizTeleworkDialogUtility::CheckVisibleHandle failed.");
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

    if (!PostMessage(WM_CLOSE))
    {
        LOG_WARN("PostMessage(WM_CLOSE) failed.");
    }

    return 0;
}
