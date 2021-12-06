#include "stdafx.h"
#include "resource.h"
#include "BizTeleworkWorkDayTargetDialog.h"
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

    struct
    {
        std::wstring workDayTarget;
    } TemporaryWorkDayTarget;

    const unsigned int LIMT_CHARACTER_WORK_TARGET = 256;

}  // namespace

int PaintRichEditWorkTarget(HWND hwnd)
{
    HDC hdc = GetWindowDC(hwnd);
    RECT rc;
    GetClientRect(hwnd, &rc);
    rc.right += 4;
    rc.bottom += 4;

    auto hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HPEN hpen = CreatePen(PS_SOLID, 3, BizTeleworkColor::InputBorder());
    auto oldbrush = (HBRUSH)SelectObject(hdc, hbrush);
    auto oldpen = (HPEN)SelectObject(hdc, hpen);
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
    SelectObject(hdc, oldpen);
    SelectObject(hdc, oldbrush);
    DeleteObject(hpen);
    DeleteObject(hbrush);

    ReleaseDC(hwnd, hdc);
    return 0;
}

LRESULT CALLBACK RichEditProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR, DWORD_PTR)
{
    static int borderThickness = 2;
    switch (msg)
    {
    case WM_NCPAINT:
        PaintRichEditWorkTarget(hwnd);
        break;

    case WM_NCCALCSIZE:
        if (lp)
        {
            auto sz = reinterpret_cast<NCCALCSIZE_PARAMS*>(lp);
            InflateRect(&sz->rgrc[0], -borderThickness, -borderThickness);
            return 0;
        }
        break;

    case WM_NCDESTROY:
        RemoveWindowSubclass(hwnd, RichEditProc, 0);
        break;

    default:
        break;
    }

    return DefSubclassProc(hwnd, msg, wp, lp);
}

IMPLEMENT_DYNAMIC(BizTeleworkWorkDayTargetDialog, CDialogEx)

BizTeleworkWorkDayTargetDialog::BizTeleworkWorkDayTargetDialog(const BizTeleworkDialogDependency& dependency, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_BIZTELEWORK_WORK_DAY_TARGET_DIALOG, pParent)
    , m_LanguageResource(dependency.GetLanguageResource())
{
}

BOOL BizTeleworkWorkDayTargetDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetWindowSubclass(m_EditWorkDayTarget.GetSafeHwnd(), RichEditProc, 0, 0);
    ApplyLanguageResource();
    ApplyPositionDialogAndControl();
    ApplyStyle();
    InitTargetText();

    if (!SetForegroundWindow())
    {
        LOG_WARN("SetForegroundWindow() failed.");
    }

    return TRUE;
}

void BizTeleworkWorkDayTargetDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_DO_WHAT_TODAY, m_StaticDoWhat);
    DDX_Control(pDX, IDC_BUTTON_TARGET_START, m_ButtonSendWorkTarget);
    DDX_Control(pDX, IDB_STATIC_IMAGE, m_StaticImage);
    DDX_Control(pDX, IDC_EDIT_WORK_DAY_TARGET, m_EditWorkDayTarget);
}

BEGIN_MESSAGE_MAP(BizTeleworkWorkDayTargetDialog, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_ACTIVATE()
    ON_BN_CLICKED(IDC_BUTTON_TARGET_START, &BizTeleworkWorkDayTargetDialog::OnBnClickedButtonSendWorkTarget)
    ON_MESSAGE(BIZ_TELEWORK_MESSAGE_WORK_TARGET, &BizTeleworkWorkDayTargetDialog::OnMessageWorkTargetResponse)
END_MESSAGE_MAP()

// BizTeleworkWorkDayTargetDialog Message Handler

HBRUSH BizTeleworkWorkDayTargetDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hBrush = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd->GetDlgCtrlID() == IDC_STATIC_DO_WHAT_TODAY)
    {
        pDC->SetTextColor(BizTeleworkColor::StaticText());
    }

    if (pWnd->GetDlgCtrlID() == IDC_EDIT_WORK_DAY_TARGET)
    {
        pDC->SetTextColor(BizTeleworkColor::InputText());
        pDC->SetBkColor(BizTeleworkColor::InputWorkDayTargetBackground());
        hBrush = m_BrBack;
    }

    return hBrush;
}

void BizTeleworkWorkDayTargetDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    (void)pWndOther;   // Unused
    (void)bMinimized;  // Unused

    if (nState == WA_INACTIVE && SaveTargetToTemporaryMemory()&& !PostMessage(WM_CLOSE))
    {
        LOG_WARN("PostMessage(WM_CLOSE) failed.");
    }
}

void BizTeleworkWorkDayTargetDialog::ApplyLanguageResource()
{
    std::wstring str(L"");
    if (!m_LanguageResource->GetString(IDS_STATIC_DO_WHAT_TODAY, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_STATIC_DO_WHAT_TODAY) failed.");
        m_StaticDoWhat.SetWindowTextW(L"");
    }
    else
    {
        m_StaticDoWhat.SetWindowTextW(str.c_str());
    }

    str = L"";
    if (!m_LanguageResource->GetString(IDS_BUTTON_TARGET_START, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDC_BUTTON_START) failed.");
        m_ButtonSendWorkTarget.SetWindowTextW(L"");
    }
    else
    {
        m_ButtonSendWorkTarget.SetWindowTextW(str.c_str());
    }
}

void BizTeleworkWorkDayTargetDialog::ApplyPositionDialogAndControl()
{
    if (MONITORINFOEX monitorInfo; BizTeleworkDialogUtility::GetMonitorInfor(this, &monitorInfo))
    {
        // monitorInfo.rcMonitor shows rectangle of desktop without taskbar.
        LONG x = monitorInfo.rcWork.right - BizTeleworkDimension::WorkDayTargetDialogWidth() - BizTeleworkDimension::WorkDayTargetMarginRight();
        LONG y = monitorInfo.rcWork.bottom - BizTeleworkDimension::WorkDayTargetDialogHeight() - BizTeleworkDimension::WorkDayTargetMarginBottom();

        SetWindowPos(&CWnd::wndTopMost, x, y, BizTeleworkDimension::WorkDayTargetDialogWidth(), BizTeleworkDimension::WorkDayTargetDialogHeight(), 0);

        RECT rect;
        this->GetClientRect(&rect);

        x = rect.right - BizTeleworkDimension::ButtonWidth() - BizTeleworkDimension::StartWorkingButtonMarginRight();
        y = rect.bottom - BizTeleworkDimension::ButtonHeight() - BizTeleworkDimension::StartWorkingButtonMarginBottom();
        m_ButtonSendWorkTarget.SetWindowPos(nullptr, x, y, BizTeleworkDimension::ButtonWidth(), BizTeleworkDimension::ButtonHeight(), 0);

        x = rect.right - BizTeleworkDimension::ImageMarginRightTargetDialog() - BizTeleworkDimension::ImageDimension();
        y = rect.bottom - BizTeleworkDimension::ImageMarginBottomTargetDialog() - BizTeleworkDimension::ImageDimension();
        m_StaticImage.SetWindowPos(nullptr, x, y, BizTeleworkDimension::ImageDimension(), BizTeleworkDimension::ImageDimension(), 0);

        x = rect.right - BizTeleworkDimension::StaticDoWhatWidth() - BizTeleworkDimension::StaticDoWhatMarginRight();
        y = rect.bottom - BizTeleworkDimension::StaticDoWhatHeight() - BizTeleworkDimension::StaticDoWhatMarginBottom();
        m_StaticDoWhat.SetWindowPos(nullptr, x, y, BizTeleworkDimension::StaticDoWhatWidth(), BizTeleworkDimension::StaticDoWhatHeight(), 0);

        x = rect.right - BizTeleworkDimension::EditWorkDayTargetWidth() - BizTeleworkDimension::EditWorkDayTargetMarginRight();
        y = rect.bottom - BizTeleworkDimension::EditWorkDayTargetHeight() - BizTeleworkDimension::EditWorkDayTargetMarginBottom();
        m_EditWorkDayTarget.SetWindowPos(nullptr, x, y, BizTeleworkDimension::EditWorkDayTargetWidth(), BizTeleworkDimension::EditWorkDayTargetHeight(), 0);
    }
}

void BizTeleworkWorkDayTargetDialog::ApplyStyle()
{
    SetBackgroundColor(BizTeleworkColor::DialogBackground());
    ApplyButtonStyle(&m_ButtonSendWorkTarget);
    BizTeleworkDialogUtility::ApplyWeightFont(&m_StaticDoWhat, FW_HEAVY, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontSize(&m_StaticDoWhat, 13, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(&m_StaticDoWhat, L"Yu Gothic UI Bold", m_NewFont);
    BizTeleworkDialogUtility::ApplyStaticImageStyle(&m_StaticImage, &m_BitMapCleanup);
    ApplyEditWorkTargetStyle(&m_EditWorkDayTarget);
}

void BizTeleworkWorkDayTargetDialog::ApplyButtonStyle(CMFCButton* button)
{
    CMFCButton::EnableWindowsTheming(FALSE);
    button->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    button->m_bTransparent = FALSE;
    button->SetFaceColor(BizTeleworkColor::ButtonBackground(), true);
    button->SetTextColor(BizTeleworkColor::ButtonText());
    BizTeleworkDialogUtility::ApplyFontSize(button, 11, m_NewFont);
}

void BizTeleworkWorkDayTargetDialog::OnBnClickedButtonSendWorkTarget()
{
    int len = m_EditWorkDayTarget.GetWindowTextLengthW();
    std::wstring workTarget;
    workTarget.resize(len);
    if (m_EditWorkDayTarget.GetWindowTextW(&workTarget[0], len + 1) != len)
    {
        LOG_WARN("m_EditWorkDayTarget.GetWindowTextW() failed.");
    }

    if (!BizTeleworkDialogUtility::StartRequestWorkTargetAndControlItems(&m_ButtonSendWorkTarget, workTarget, this))
    {
        LOG_WARN("BizTeleworkDialogUtility::StartRequestWorkTargetAndControlItems(START) failed.");
    }
}

BOOL BizTeleworkWorkDayTargetDialog::PreTranslateMessage(MSG* pMsg)
{
    if ((pMsg->hwnd == this->m_hWnd && pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
    {
        return TRUE;
    }
    return CWnd::PreTranslateMessage(pMsg);
}

void BizTeleworkWorkDayTargetDialog::ApplyEditWorkTargetStyle(const CRichEditCtrl* edit) const
{
    edit->SendMessage(EM_LIMITTEXT, LIMT_CHARACTER_WORK_TARGET, 0);
}

void BizTeleworkWorkDayTargetDialog::InitTargetText()
{
    RestoreTargetFromTemporaryMemory();
    //khanhpqtest start
    wchar_t* content = L"Khanhpq";
    m_EditWorkDayTarget.SetWindowTextW(content);
    //khanhpqtest end
}

bool BizTeleworkWorkDayTargetDialog::SaveTargetToTemporaryMemory() const
{
    try
    {
        if (int len = m_EditWorkDayTarget.GetWindowTextLengthW(); len > 0)
        {
            TemporaryWorkDayTarget.workDayTarget.resize(len);
            if (m_EditWorkDayTarget.GetWindowTextW(&TemporaryWorkDayTarget.workDayTarget[0], len + 1) != len)
            {
                LOG_WARN("m_EditWorkDayTarget.GetWindowTextW() failed.");
            }
        }
        else
        {
            TemporaryWorkDayTarget.workDayTarget.clear();
        }
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_WARN("Failed by std::exception %s", e.what());
        return false;
    }
}

void BizTeleworkWorkDayTargetDialog::RestoreTargetFromTemporaryMemory()
{
    m_EditWorkDayTarget.SetWindowTextW(TemporaryWorkDayTarget.workDayTarget.c_str());
}

LRESULT BizTeleworkWorkDayTargetDialog::OnMessageWorkTargetResponse(WPARAM wParam, LPARAM lParam)
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

    if (auto succeeded = static_cast<BOOL>(wParam); !succeeded)
    {
        auto errorDialogType = static_cast<ErrorDialogMessageType>(lParam);
        BizTeleworkDialogUtility::DisplayErrorDialog(errorDialogType);
    }
    else if (!PostMessage(WM_CLOSE))
    {
        LOG_WARN("PostMessage(WM_CLOSE) failed.");
    }

    return 0;
}
