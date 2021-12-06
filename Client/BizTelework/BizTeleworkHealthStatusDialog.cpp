#include "stdafx.h"
#include "resource.h"
#include "BizTeleworkMainWindow.h"
#include "BizTeleworkDialogDependency.h"
#include "BizTeleworkHealthStatusDialog.h"
#include "BizTeleworkOffDutyDialog.h"
#include "BizTeleworkDayOffVerifyDialog.h"
#include "BizTeleworkColor.h"
#include "BizTeleworkDimension.h"
#include "BizTeleworkDialogUtility.h"
#include "BizTeleworkMessage.h"
#include "BizTeleworkDialogDependency.h"
#include "RequestHealthStatusType.h"
#include "AppConfig.h"
#include "afxdialogex.h"
#include <Windows.h>
#include <Language.h>
#include <iostream>
#include <memory>
#include <Log.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{

    DEFINE_LOGGER_FILE_NAME;

}  // namespace

IMPLEMENT_DYNAMIC(BizTeleworkHealthStatusDialog, CDialogEx)

BizTeleworkHealthStatusDialog::BizTeleworkHealthStatusDialog(const BizTeleworkDialogDependency& dependency, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_BIZTELEWORK_HEALTH_STATUS_DIALOG, pParent)
    , m_LanguageResource(dependency.GetLanguageResource())
{
}

BizTeleworkHealthStatusDialog::~BizTeleworkHealthStatusDialog()
{
}

BOOL BizTeleworkHealthStatusDialog::OnInitDialog()
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
void BizTeleworkHealthStatusDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_GOOD_HEALTH, m_ButtonGoodHealth);
    DDX_Control(pDX, IDC_BUTTON_NORMAL_HEALTH, m_ButtonNormalHealth);
    DDX_Control(pDX, IDC_BUTTON_BAD_HEALTH, m_ButtonBadHealth);
    DDX_Control(pDX, IDB_STATIC_IMAGE, m_StaticImage);
    DDX_Control(pDX, IDC_STATIC_FEELING_QUESTION, m_FeelingQuestion);
}

BEGIN_MESSAGE_MAP(BizTeleworkHealthStatusDialog, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_ACTIVATE()
    ON_BN_CLICKED(IDC_BUTTON_GOOD_HEALTH, &BizTeleworkHealthStatusDialog::OnBnClickedButtonGoodHealth)
    ON_BN_CLICKED(IDC_BUTTON_NORMAL_HEALTH, &BizTeleworkHealthStatusDialog::OnBnClickedButtonNormalHealth)
    ON_BN_CLICKED(IDC_BUTTON_BAD_HEALTH, &BizTeleworkHealthStatusDialog::OnBnClickedButtonBadHealth)
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_USER_CONDITION_LOGS_RESPONSIVE, &BizTeleworkHealthStatusDialog::OnMessageUserConditionLogsResponse)
END_MESSAGE_MAP()

// BizTeleworkHealthStatusDialog Message Handler
HBRUSH BizTeleworkHealthStatusDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hBrush = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
    if (pWnd->GetDlgCtrlID() == IDC_STATIC_FEELING_QUESTION)
    {
        pDC->SetTextColor(BizTeleworkColor::StaticText());
    }
    return hBrush;
}

void BizTeleworkHealthStatusDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    (void)pWndOther;   // Unused
    (void)bMinimized;  // Unused

    if (nState == WA_INACTIVE && !PostMessage(WM_CLOSE))
    {
        LOG_WARN("PostMessage(WM_CLOSE) failed.");
    }
}

void BizTeleworkHealthStatusDialog::ApplyLanguageResource()
{
    std::wstring str(L"");
    if (!m_LanguageResource->GetString(IDS_BUTTON_GOOD_HEALTH, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_BUTTON_GOOD_HEALTH) failed.");
        m_ButtonGoodHealth.SetWindowTextW(L"");
    }
    else
    {
        m_ButtonGoodHealth.SetWindowTextW(str.c_str());
    }

    str = L"";
    if (!m_LanguageResource->GetString(IDS_BUTTON_NORMAL_HEALTH, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_BUTTON_NORMAL_HEALTH) failed.");
        m_ButtonNormalHealth.SetWindowTextW(L"");
    }
    else
    {
        m_ButtonNormalHealth.SetWindowTextW(str.c_str());
    }

    str = L"";
    if (!m_LanguageResource->GetString(IDS_BUTTON_BAD_HEALTH, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_BUTTON_BAD_HEALTH) failed.");
        m_ButtonBadHealth.SetWindowTextW(L"");
    }
    else
    {
        m_ButtonBadHealth.SetWindowTextW(str.c_str());
    }

    str = L"";
    if (!m_LanguageResource->GetString(IDS_STATIC_FEELING_QUESTION, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_STATIC_FEELING_QUESTION) failed.");
        m_FeelingQuestion.SetWindowTextW(L"");
    }
    else
    {
        m_FeelingQuestion.SetWindowTextW(str.c_str());
    }
}

void BizTeleworkHealthStatusDialog::ApplyPositionDialogAndControl()
{
    MONITORINFOEX monitorInfo;
    if (BizTeleworkDialogUtility::GetMonitorInfor(this, &monitorInfo))
    {
        // monitorInfo.rcMonitor shows rectangle of desktop without taskbar.
        LONG x = monitorInfo.rcWork.right - BizTeleworkDimension::HealthStatusDialogWidth() - BizTeleworkDimension::HealthStatusDialogMarginRight();
        LONG y = monitorInfo.rcWork.bottom - BizTeleworkDimension::HealthStatusDialogHeight() - BizTeleworkDimension::HealthStatusDialogMarginBottom();

        SetWindowPos(&CWnd::wndTopMost, x, y, BizTeleworkDimension::HealthStatusDialogWidth(), BizTeleworkDimension::HealthStatusDialogHeight(), 0);

        RECT rect;
        this->GetClientRect(&rect);

        x = rect.right - BizTeleworkDimension::FeelingQuestionWidth() - BizTeleworkDimension::FeelingQuestionMarginRight();
        y = rect.bottom - BizTeleworkDimension::FeelingQuestionHeight() - BizTeleworkDimension::FeelingQuestionMarginBottom();
        m_FeelingQuestion.SetWindowPos(NULL, x, y, BizTeleworkDimension::FeelingQuestionWidth(), BizTeleworkDimension::FeelingQuestionHeight(), 0);

        x = rect.right - BizTeleworkDimension::HealthStatusButtonWidth() - BizTeleworkDimension::BadStatusButtonMarginRight();
        y = rect.bottom - BizTeleworkDimension::HealthStatusButtonHeight() - BizTeleworkDimension::HealthStatusButtonMarginBottom();
        m_ButtonBadHealth.SetWindowPos(NULL, x, y, BizTeleworkDimension::HealthStatusButtonWidth(), BizTeleworkDimension::HealthStatusButtonHeight(), 0);

        x = rect.right - BizTeleworkDimension::HealthStatusButtonWidth() * 2
            - BizTeleworkDimension::SpaceBetween2Buttons()
            - BizTeleworkDimension::BadStatusButtonMarginRight();
        y = rect.bottom - BizTeleworkDimension::HealthStatusButtonHeight() - BizTeleworkDimension::HealthStatusButtonMarginBottom();
        m_ButtonNormalHealth.SetWindowPos(NULL, x, y, BizTeleworkDimension::HealthStatusButtonWidth(), BizTeleworkDimension::HealthStatusButtonHeight(), 0);

        x = rect.right - BizTeleworkDimension::HealthStatusButtonWidth() * 3
            - BizTeleworkDimension::SpaceBetween2Buttons() * 2
            - BizTeleworkDimension::BadStatusButtonMarginRight();
        y = rect.bottom - BizTeleworkDimension::HealthStatusButtonHeight() - BizTeleworkDimension::HealthStatusButtonMarginBottom();
        m_ButtonGoodHealth.SetWindowPos(NULL, x, y, BizTeleworkDimension::HealthStatusButtonWidth(), BizTeleworkDimension::HealthStatusButtonHeight(), 0);
    }
}

void BizTeleworkHealthStatusDialog::ApplyStyle()
{
    SetBackgroundColor(BizTeleworkColor::DialogBackground());
    CMFCButton::EnableWindowsTheming(FALSE);
    ApplyButtonStyle(&m_ButtonGoodHealth);
    ApplyButtonStyle(&m_ButtonNormalHealth);
    ApplyButtonStyle(&m_ButtonBadHealth);
    BizTeleworkDialogUtility::ApplyStaticImageStyle(&m_StaticImage, &m_BitMapCleanup);
    BizTeleworkDialogUtility::ApplyWeightFont(&m_FeelingQuestion, FW_BOLD, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(&m_FeelingQuestion, L"Yu Gothic UI Bold", m_NewFont);
    BizTeleworkDialogUtility::ApplyFontSize(&m_FeelingQuestion, 13, m_NewFont);
}

void BizTeleworkHealthStatusDialog::ApplyButtonStyle(CMFCButton* button)
{
    button->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    button->m_bTransparent = FALSE;
    button->SetFaceColor(BizTeleworkColor::ButtonBackground(), true);
    button->SetTextColor(BizTeleworkColor::ButtonText());
    BizTeleworkDialogUtility::ApplyFontSize(button, 11, m_NewFont);
}

void BizTeleworkHealthStatusDialog::OnBnClickedButtonGoodHealth()
{
    if (!BizTeleworkDialogUtility::StartRequestUserConditionLogsAndControlItems(&m_ButtonGoodHealth,
                                                                                RequestHealthStatusType::REQUEST_HEALTH_STATUS_TYPE_GOOD,
                                                                                this))
    {
        LOG_WARN("StartRequestUserConditionLogsAndControlItems(GOOD) failed.");
    }
}

void BizTeleworkHealthStatusDialog::OnBnClickedButtonNormalHealth()
{
    if (!BizTeleworkDialogUtility::StartRequestUserConditionLogsAndControlItems(&m_ButtonNormalHealth,
                                                                                RequestHealthStatusType::REQUEST_HEALTH_STATUS_TYPE_NORMAL,
                                                                                this))
    {
        LOG_WARN("StartRequestUserConditionLogsAndControlItems(NORMAL) failed.");
    }
}

void BizTeleworkHealthStatusDialog::OnBnClickedButtonBadHealth()
{
    if (!BizTeleworkDialogUtility::StartRequestUserConditionLogsAndControlItems(&m_ButtonBadHealth,
                                                                                RequestHealthStatusType::REQUEST_HEALTH_STATUS_TYPE_BAD, this))
    {
        LOG_WARN("StartRequestUserConditionLogsAndControlItems(BAD) failed.");
    }
}

afx_msg LRESULT BizTeleworkHealthStatusDialog::OnMessageUserConditionLogsResponse(WPARAM wParam, LPARAM lParam)
{
    if (!BizTeleworkDialogUtility::CheckVisibleDialog(this))
    {
        LOG_WARN("BizTeleworkDialogUtility::CheckVisibleHandle failed.");
        return 0;
    }

    if (!BizTeleworkDialogUtility::SetCursorWait(false, this))
    {
        LOG_WARN("BizTeleworkDialogUtility::SetCursorWait() failed.");
    }

    BOOL succeeded = static_cast<BOOL>(wParam);
    if (!succeeded)
    {
        const ErrorDialogMessageType errorDialogType = static_cast<ErrorDialogMessageType>(lParam);
        BizTeleworkDialogUtility::DisplayErrorDialog(errorDialogType);
    }
    else
    {
        const RequestHealthStatusType requestHealthStatusType = static_cast<RequestHealthStatusType>(lParam);
        if (requestHealthStatusType == RequestHealthStatusType::REQUEST_HEALTH_STATUS_TYPE_BAD)
        {
            BizTeleworkDialogDependency dependency;
            std::unique_ptr<BizTeleworkDayOffVerifyDialog> dayOffDialog = std::make_unique<BizTeleworkDayOffVerifyDialog>(dependency, nullptr);
            dayOffDialog->DoModal();
        }
        else
        {
            BizTeleworkDialogDependency dependency;
            std::unique_ptr<BizTeleworkOffDutyDialog> offDutyDialog = std::make_unique<BizTeleworkOffDutyDialog>(dependency, nullptr);
            offDutyDialog->DoModal();
        }
    }

    return 0;
}
