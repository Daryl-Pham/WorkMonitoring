#include "stdafx.h"
#include "resource.h"
#include "BizTeleworkFailedCallApiDialog.h"
#include "BizTeleworkColor.h"
#include "BizTeleworkDimension.h"
#include "BizTeleworkDialogUtility.h"
#include "afxdialogex.h"
#include <Windows.h>
#include <Language.h>
#include <Log.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{

    DEFINE_LOGGER_FILE_NAME;

}  // namespace

IMPLEMENT_DYNAMIC(BizTeleworkFailedCallApiDialog, CDialogEx)

BizTeleworkFailedCallApiDialog::BizTeleworkFailedCallApiDialog(const ErrorDialogMessageType errorType, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_BIZTELEWORK_FAILED_CALL_API_DIALOG, pParent)
    , m_MessageType(errorType)
{
}

BizTeleworkFailedCallApiDialog::BizTeleworkFailedCallApiDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_BIZTELEWORK_FAILED_CALL_API_DIALOG, pParent)
{
}

BizTeleworkFailedCallApiDialog::~BizTeleworkFailedCallApiDialog()
{
}

BOOL BizTeleworkFailedCallApiDialog::OnInitDialog()
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

void BizTeleworkFailedCallApiDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_RETURN_FAILED_CALL_API, m_ButtonRetry);
    DDX_Control(pDX, IDB_STATIC_IMAGE, m_StaticImage);
    DDX_Control(pDX, IDC_STATIC_TEXT_FAILED_CALL_API, m_StaticNotification);
}

BEGIN_MESSAGE_MAP(BizTeleworkFailedCallApiDialog, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_ACTIVATE()
    ON_BN_CLICKED(IDC_BUTTON_RETURN_FAILED_CALL_API, &BizTeleworkFailedCallApiDialog::OnBnClickedButtonReturn)
END_MESSAGE_MAP()

// ErrorMessageDialog Message Handler

HBRUSH BizTeleworkFailedCallApiDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hBrush = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd->GetDlgCtrlID() == IDC_STATIC_TEXT_FAILED_CALL_API)
    {
        pDC->SetTextColor(BizTeleworkColor::StaticText());
    }

    return hBrush;
}

void BizTeleworkFailedCallApiDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    (void)pWndOther;   // Unused
    (void)bMinimized;  // Unused

    if (nState == WA_INACTIVE)
    {
        LOG_INFO("OnActivate(WA_INACTIVE)");
        PostMessage(WM_CLOSE);
        this->PostMessageW(WM_CLOSE);
    }
}

// Private

void BizTeleworkFailedCallApiDialog::ApplyLanguageResource()
{
    std::wstring str(L"");
    if (!m_LanguageResource->GetString(IDS_BUTTON_RETURN_FAILED_CALL_API, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_BUTTON_OK_FAILED_CALL_API) failed.");
        str = L"";
    }
    m_ButtonRetry.SetWindowTextW(str.c_str());

    str = L"";
    switch (m_MessageType)
    {
    case ErrorDialogMessageType::BAD_REQUEST_STARTED_ERROR_TYPE:
        if (!m_LanguageResource->GetString(IDS_ERROR_MESSAGE_WORK_STARTED, &str))
        {
            LOG_WARN("m_LanguageResource->GetString(IDS_ERROR_MESSAGE_WORK_STARTED) failed.");
            str = L"";
        }
        break;
    case ErrorDialogMessageType::BAD_REQUEST_ENDED_ERROR_TYPE:
        if (!m_LanguageResource->GetString(IDS_ERROR_MESSAGE_WORK_ENDED, &str))
        {
            LOG_WARN("m_LanguageResource->GetString(IDS_ERROR_MESSAGE_WORK_ENDED) failed.");
            str = L"";
        }
        break;
    case ErrorDialogMessageType::UNAUTHORIZED_ERROR_TYPE:
        if (!m_LanguageResource->GetString(IDS_ERROR_MESSAGE_UNAUTHORIZED, &str))
        {
            LOG_WARN("m_LanguageResource->GetString(IDS_ERROR_MESSAGE_UNAUTHORIZED) failed.");
            str = L"";
        }
        break;
    case ErrorDialogMessageType::COMMUNICATION_ERROR_TYPE:
        if (!m_LanguageResource->GetString(IDS_ERROR_MESSAGE_COMMUNICATION, &str))
        {
            LOG_WARN("m_LanguageResource->GetString(IDS_ERROR_MESSAGE_COMMUNICATION) failed.");
            str = L"";
        }
        break;
    default:
        if (!m_LanguageResource->GetString(IDS_STATIC_TEXT_FAILED_CALL_API, &str))
        {
            LOG_WARN("m_LanguageResource->GetString(IDC_STATIC_TEXT_FAILED_CALL_API) failed.");
            str = L"";
        }
        break;
    }

    m_StaticNotification.SetWindowTextW(str.c_str());
}

void BizTeleworkFailedCallApiDialog::ApplyPositionDialogAndControl()
{
    MONITORINFOEX monitorInfo;
    if (BizTeleworkDialogUtility::GetMonitorInfor(this, &monitorInfo))
    {
        // monitorInfo.rcMonitor shows rectangle of desktop without taskbar.
        LONG x = monitorInfo.rcWork.right - BizTeleworkDimension::FailedCallApiDialogWidth() - BizTeleworkDimension::FailedCallApiDialogMarginRight();
        LONG y = monitorInfo.rcWork.bottom - BizTeleworkDimension::FailedCallApiDialogHeight() - BizTeleworkDimension::FailedCallApiDialogMarginBottom();

        SetWindowPos(&CWnd::wndTopMost, x, y, BizTeleworkDimension::FailedCallApiDialogWidth(), BizTeleworkDimension::FailedCallApiDialogHeight(), 0);

        RECT rect;
        this->GetClientRect(&rect);

        x = rect.right - BizTeleworkDimension::ButtonWidth() - BizTeleworkDimension::RetryButtonMarginRight();
        y = rect.bottom - BizTeleworkDimension::ButtonHeight() - BizTeleworkDimension::RetryButtonMarginBottom();
        m_ButtonRetry.SetWindowPos(NULL, x, y, BizTeleworkDimension::ButtonWidth(), BizTeleworkDimension::ButtonHeight(), 0);

        x = rect.right - BizTeleworkDimension::ImageMarginRightDialog() - BizTeleworkDimension::ImageDimension();
        y = rect.bottom - BizTeleworkDimension::ImageMarginBottomOnFailedCallApiDialog() - BizTeleworkDimension::ImageDimension();
        m_StaticImage.SetWindowPos(NULL, x, y, BizTeleworkDimension::ImageDimension(), BizTeleworkDimension::ImageDimension(), 0);

        x = rect.right - BizTeleworkDimension::NotificationFailedCallApiDialogWidth() - BizTeleworkDimension::NotificationFailedCallApiDialogMarginRight();
        y = rect.bottom - BizTeleworkDimension::NotificationFailedCallApiDialogHeight() - BizTeleworkDimension::NotificationFailedCallApiDialogMarginBottom();
        m_StaticNotification.SetWindowPos(NULL, x, y, BizTeleworkDimension::NotificationFailedCallApiDialogWidth(),
            BizTeleworkDimension::NotificationFailedCallApiDialogHeight(), 0);
    }
}

void BizTeleworkFailedCallApiDialog::ApplyStyle()
{
    SetBackgroundColor(BizTeleworkColor::DialogBackground());
    ApplyButtonStyle(&m_ButtonRetry);
    BizTeleworkDialogUtility::ApplyWeightFont(&m_StaticNotification, FW_HEAVY, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontSize(&m_StaticNotification, 13, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(&m_StaticNotification, L"Yu Gothic UI Bold", m_NewFont);
    BizTeleworkDialogUtility::ApplyStaticImageStyle(&m_StaticImage, &m_BitMapCleanup);
}

void BizTeleworkFailedCallApiDialog::ApplyButtonStyle(CMFCButton* button)
{
    CMFCButton::EnableWindowsTheming(FALSE);
    button->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    button->m_bTransparent = FALSE;
    button->SetFaceColor(BizTeleworkColor::ButtonBackground(), true);
    button->SetTextColor(BizTeleworkColor::ButtonText());
    BizTeleworkDialogUtility::ApplyFontSize(button, 11, m_NewFont);
}

void BizTeleworkFailedCallApiDialog::OnBnClickedButtonReturn()
{
    if (!PostMessage(WM_CLOSE))
    {
        LOG_ERROR("PostMessage(WM_CLOSE) failed.");
    }
}
