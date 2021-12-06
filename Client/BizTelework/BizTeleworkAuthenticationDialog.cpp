#include "stdafx.h"
#include "resource.h"
#include "BizTeleworkMainWindow.h"
#include "BizTeleworkDialogDependency.h"
#include "BizTeleworkAuthenticationDialog.h"
#include "BizTeleworkDimension.h"
#include "BizTeleworkDialogUtility.h"
#include "BizTeleworkMessage.h"
#include "AppConfig.h"
#include "afxdialogex.h"
#include "CopyDataForAuthenticationUri.h"
#include <Windows.h>
#include <Language.h>
#include <iostream>
#include <map>
#include <memory>
#include <Log.h>
#include "RegistryHelper.h"
#include "AuthenHelper.h"
#include "BizTeleworkDependency.h"
#include <boost/timer/timer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "ResponsiveCreator.h"
#include "Utility.h"
#include <string>
#include "Environment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    const wchar_t* PrefixURLHttps = L"https://";

    const wchar_t* PrefixURLHttp = L"http://";

    const wchar_t* SlashURL = L"/";

    const wchar_t* WindowTitle = L"BizTelework";

    // This Temporary memories are required for realizing https://biztelework.atlassian.net/browse/BTC-723.
    struct {
        std::wstring companyCode;
        std::wstring activeCode;
        // Exclude URL filed. Becasue it is not effected the pre/production environment.
    } TemporaryUserInputs;

}  // namespace

bool IsHaveLastSlash(const std::wstring& URL)
{
    return (URL.back() == *SlashURL);
}

bool IsHavePrefixURLHttps(const std::wstring& URL)
{
    if (URL.size() < 8)
    {
        return false;
    }
    return(!URL.compare(0, 8, PrefixURLHttps));
}

bool IsHavePrefixURLHttp(const std::wstring& URL)
{
    if (URL.size() < 7)
    {
        return false;
    }
    return(!URL.compare(0, 7, PrefixURLHttp));
}

void TrimAPIURL(std::wstring* URL)
{
    std::wstring tmpURL = *URL;
    boost::algorithm::trim(tmpURL);
    if (IsHaveLastSlash(tmpURL))
    {
        boost::algorithm::erase_tail(tmpURL, 1);
    }
    if (IsHavePrefixURLHttps(tmpURL))
    {
        boost::algorithm::erase_head(tmpURL, 8);
    }
    if (IsHavePrefixURLHttp(tmpURL))
    {
        boost::algorithm::erase_head(tmpURL, 7);
    }
    *URL = tmpURL;
}

void AppendAPIURL(std::wstring* URL)
{
    std::wstring tmpURL = *URL;
    if (!IsHaveLastSlash(tmpURL))
    {
        tmpURL.append(SlashURL);
    }
    if (!IsHavePrefixURLHttps(tmpURL) && !IsHavePrefixURLHttp(tmpURL))
    {
        std::wstring tmp;
        tmp.append(PrefixURLHttps);
        tmp.append(tmpURL);
        tmpURL = tmp;
    }
    *URL = tmpURL;
}

BEGIN_MESSAGE_MAP(CVEdit, CEdit)
    // {{AFX_MSG_MAP(CVEdit)
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVEdit message handlers

void CVEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
    (void)bCalcValidRects;  // Unused

    CRect rectWnd;
    CRect rectClient;

    // Calculate client area height needed for a font
    CFont* pFont = GetFont();
    CRect rectText;
    rectText.SetRectEmpty();

    CDC* pDC = GetDC();

    CFont* pOld = pDC->SelectObject(pFont);
    pDC->DrawText(L"InitText", rectText, DT_CALCRECT | DT_LEFT);
    UINT uiVClientHeight = rectText.Height();

    pDC->SelectObject(pOld);
    ReleaseDC(pDC);

    // Calculate NC area to center text.
    GetClientRect(rectClient);
    GetWindowRect(rectWnd);
    ClientToScreen(rectClient);

    UINT uiCenterOffset = (rectClient.Height() - uiVClientHeight) / 2;
    UINT uiCY = (rectWnd.Height() - rectClient.Height()) / 2;
    UINT uiCX = (rectWnd.Width() - rectClient.Width()) / 2;

    rectWnd.OffsetRect(-rectWnd.left, -rectWnd.top);
    m_rectNCTop = rectWnd;

    m_rectNCTop.DeflateRect(uiCX, uiCY, uiCX, uiCenterOffset + uiVClientHeight + uiCY);
    m_rectNCBottom = rectWnd;
    m_rectNCBottom.DeflateRect(uiCX, uiCenterOffset + uiVClientHeight + uiCY, uiCX, uiCY);

    lpncsp->rgrc[0].top += uiCY;
    lpncsp->rgrc[0].bottom -= uiCY;
    lpncsp->rgrc[0].left += uiCX;
    lpncsp->rgrc[0].right -= uiCX;

    lpncsp->rgrc->top += 1;
    lpncsp->rgrc->bottom -= 1;
}

void CVEdit::OnNcPaint()
{
    CDC* pDC = GetWindowDC();

    // Work out the coordinates of the window rectangle,
    CRect rect;
    GetWindowRect(&rect);
    rect.OffsetRect(-rect.left, -rect.top);

    // Draw a single line around the outside
    CBrush brush(BizTeleworkColor::InputBorder());
    CBrush* pOldBrush = pDC->SelectObject(&brush);
    pDC->Rectangle(rect);
    pDC->FrameRect(&rect, &brush);
    pDC->SelectObject(pOldBrush);
    ReleaseDC(pDC);
}

BEGIN_MESSAGE_MAP(CURLStatic, CStatic)
    // {{AFX_MSG_MAP(CURLStaticText)
    ON_WM_NCHITTEST()
    ON_WM_LBUTTONUP()
    ON_WM_SETCURSOR()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

CURLStatic::CURLStatic()
{
    m_Cursor = NULL;
}

CURLStatic::~CURLStatic()
{
    ::DeleteObject(m_Cursor);
    ::DeleteObject(m_Font);
}

BOOL CURLStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (m_Cursor)
    {
        ::SetCursor(m_Cursor);
        return TRUE;
    }
    return FALSE;
}

void CURLStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
    GetParent()->SendMessage(WM_COMMAND, MAKELONG(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
}

void CURLStatic::PreSubclassWindow()
{
    LPCTSTR cursorName = IDC_HAND;
    HWND hWnd = NULL;
    if (!(hWnd = this->GetSafeHwnd()))
    {
        LOG_ERROR("CURLStaticText::GetSafeHwnd() failed.");
        return;
    }

    if (m_Cursor == NULL && !(m_Cursor = LoadCursor(NULL, cursorName)))
    {
        LOG_ERROR("CURLStaticText LoadCursor failed.");
        return;
    }

    if (!SetClassLongPtr(hWnd, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(m_Cursor)))
    {
        LOG_ERROR("CURLStaticText SetClassLong failed.");
        if (!DestroyCursor(m_Cursor))
        {
            LOG_ERROR("DestroyCursor() failed. 0x%08x", GetLastError());
        }
        return;
    }
}

LRESULT CURLStatic::OnNcHitTest(CPoint point)
{
    return HTCLIENT;
}

// For set cue banner
// https://docs.microsoft.com/en-us/windows/win32/controls/em-setcuebanner
// https://docs.microsoft.com/en-us/windows/win32/controls/cookbook-overview

IMPLEMENT_DYNAMIC(BizTeleworkAuthenticationDialog, CDialogEx)

BizTeleworkAuthenticationDialog::BizTeleworkAuthenticationDialog(const BizTeleworkDialogDependency& dependency,
                                                                 CWnd* pParent,
                                                                 const ActiveInformation& activeInformation)
    : CDialogEx(IDD_BIZTELEWORK_AUTHENTICATION_DIALOG, pParent)
    , m_LanguageResource(dependency.GetLanguageResource())
    , m_ActivationInformation(activeInformation)
{
}

BizTeleworkAuthenticationDialog::~BizTeleworkAuthenticationDialog()
{
}

BOOL BizTeleworkAuthenticationDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetWindowText(WindowTitle);
    ApplyStyle();
    GetReleaseEnvironment();
    ApplyPositionDialogAndControl(FALSE);
    ApplyLanguageResource();
    InitInputText();

    if (!SetForegroundWindow())
    {
        LOG_WARN("SetForegroundWindow() failed.");
    }

    if (ShouldAuthenNow())
    {
        OnBnClickedButtonLogin();
    }

    return TRUE;
}

void BizTeleworkAuthenticationDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDB_STATIC_IMAGE, m_StaticImage);
    DDX_Control(pDX, IDC_STATIC_ACTIVE_ERROR, m_StaticError);
    DDX_Control(pDX, IDC_STATIC_AUTH_SUCCESS, m_StaticAuthSuccess);
    DDX_Control(pDX, IDC_EDIT_COMPANY_CODE, m_EditCompanyCode);
    DDX_Control(pDX, IDC_EDIT_ACTIVE_CODE, m_EditActiveCode);
    DDX_Control(pDX, IDB_CLOSE_IMAGE, m_Close);
    DDX_Control(pDX, IDC_STATIC_TEXT_ENTER_URL, m_StaticTextEnterURL);
    DDX_Control(pDX, IDB_EXPAND_MORE_IMAGE, m_ExpandMore);
    DDX_Control(pDX, IDB_EXPAND_LESS_IMAGE, m_ExpandLess);
    DDX_Control(pDX, IDC_EDIT_ENTER_UPDATE_URL, m_EditAPIURL);
    DDX_Control(pDX, IDC_BUTTON_LOGIN, m_ButtonLogin);
    DDX_Control(pDX, IDC_STATIC_EXPLANATION, m_StaticAuthExplanation);
}

BEGIN_MESSAGE_MAP(BizTeleworkAuthenticationDialog, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDB_CLOSE_IMAGE, &BizTeleworkAuthenticationDialog::OnBnClickedClose)
    ON_BN_CLICKED(IDC_BUTTON_LOGIN, &BizTeleworkAuthenticationDialog::OnBnClickedButtonLogin)
    ON_BN_CLICKED(IDC_STATIC_TEXT_ENTER_URL, &BizTeleworkAuthenticationDialog::OnBnClickedEnterURL)
    ON_BN_CLICKED(IDB_EXPAND_MORE_IMAGE, &BizTeleworkAuthenticationDialog::OnBnClickedEnterURL)
    ON_BN_CLICKED(IDB_EXPAND_LESS_IMAGE, &BizTeleworkAuthenticationDialog::OnBnClickedEnterURL)
    ON_WM_COPYDATA()
    ON_WM_NCHITTEST()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// BizTeleworkAuthenticationDialog Message Handler
HBRUSH BizTeleworkAuthenticationDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hBrush = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
    if (pWnd->GetDlgCtrlID() == IDC_EDIT_COMPANY_CODE
        || pWnd->GetDlgCtrlID() == IDC_EDIT_ACTIVE_CODE
        || pWnd->GetDlgCtrlID() == IDC_EDIT_ENTER_UPDATE_URL)
    {
        pDC->SetTextColor(BizTeleworkColor::InputText());
        pDC->SetBkColor(BizTeleworkColor::InputBackground());
        hBrush = m_BrBack;
    }
    else if (pWnd->GetDlgCtrlID() == IDC_STATIC_TEXT_ENTER_URL)
    {
        pDC->SetTextColor(BizTeleworkColor::ButtonText());
    }
    else if (pWnd->GetDlgCtrlID() == IDC_STATIC_ACTIVE_ERROR)
    {
        pDC->SetTextColor(BizTeleworkColor::ErrorText());
    }
    else if (pWnd->GetDlgCtrlID() == IDC_STATIC_AUTH_SUCCESS)
    {
        pDC->SetTextColor(BizTeleworkColor::AuthSuccessfulText());
    }
    else if (pWnd->GetDlgCtrlID() == IDC_STATIC_EXPLANATION)
    {
        pDC->SetTextColor(BizTeleworkColor::ButtonText());
    }
    return hBrush;
}

void BizTeleworkAuthenticationDialog::ApplyLanguageResource()
{
    std::wstring str(L"");
    if (!m_LanguageResource->GetString(IDS_BUTTON_LOGIN, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_BUTTON_LOGIN) failed.");
        m_ButtonLogin.SetWindowTextW(L"");
    }
    else
    {
        m_ButtonLogin.SetWindowTextW(str.c_str());
    }

    if (!m_LanguageResource->GetString(IDS_CUE_BANNER_COMPANY_CODE, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_CUE_BANNER_COMPANY_CODE) failed.");
        m_EditCompanyCode.SetCueBanner(L"", TRUE);
    }
    else
    {
        m_EditCompanyCode.SetCueBanner(str.c_str(), TRUE);
    }

    if (!m_LanguageResource->GetString(IDS_CUE_BANNER_ACTIVE_CODE, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_CUE_BANNER_ACTIVE_CODE) failed.");
        m_EditActiveCode.SetCueBanner(L"", TRUE);
    }
    else
    {
        m_EditActiveCode.SetCueBanner(str.c_str(), TRUE);
    }

    AppConfig appConfig;
    if (!appConfig.GetApiUrl(&str))
    {
        LOG_WARN("appConfig.GetApiUrl() failed.");
        m_EditAPIURL.SetCueBanner(L"", TRUE);
    }
    else
    {
        TrimAPIURL(&str);
        m_EditAPIURL.SetCueBanner(str.c_str(), TRUE);
    }

    if (!m_LanguageResource->GetString(IDS_STATIC_AUTH_EXPLANATION, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_STATIC_AUTH_EXPLANATION) failed.");
        m_StaticAuthExplanation.SetWindowTextW(L"");
    }
    else
    {
        m_StaticAuthExplanation.SetWindowTextW(str.c_str());
    }

    if (!m_LanguageResource->GetString(IDS_STATIC_AUTH_SUCCESSFUL, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_STATIC_AUTH_SUCCESSFUL) failed.");
        m_StaticAuthSuccess.SetWindowTextW(L"");
    }
    else
    {
        m_StaticAuthSuccess.SetWindowTextW(str.c_str());
    }

    ApplyLanguageResourceLabelEnterURL();
}

void BizTeleworkAuthenticationDialog::ApplyPositionDialogAndControl(const BOOL& isHaveAPIURL)
{
    if (isHaveAPIURL)
    {
        ApplyPositionDialogAndControlMore();
    }
    else
    {
        ApplyPositionDialogAndControlLess();
    }
    Invalidate();
}

void BizTeleworkAuthenticationDialog::ApplyStyle()
{
    SetBackgroundColor(BizTeleworkColor::DialogBackground());
    CMFCButton::EnableWindowsTheming(FALSE);
    ApplyButtonStyle(&m_ButtonLogin);
    ApplyInputTextStyle(&m_EditActiveCode);
    ApplyInputTextStyle(&m_EditCompanyCode);
    ApplyInputTextStyle(&m_EditAPIURL);
    BizTeleworkDialogUtility::ApplyStaticImageStyle(&m_Close, &m_BitMapClose);
    BizTeleworkDialogUtility::ApplyStaticImageStyle(&m_StaticImage, &m_BitMapCleanupLogo);
    ApplyErrorTextStyle(&m_StaticError);
    ApplAuthSuccessTextStyle(&m_StaticAuthSuccess);
    BizTeleworkDialogUtility::ApplyStaticImageStyle(&m_ExpandMore, &m_BitMapCleanupExpandMore);
    BizTeleworkDialogUtility::ApplyStaticImageStyle(&m_ExpandLess, &m_BitMapCleanupExpandLess);
    ApplyStaticEnterURLStyle(&m_StaticTextEnterURL);
    ApplyStaticExplanationStyle(&m_StaticAuthExplanation);
}

void BizTeleworkAuthenticationDialog::ApplyButtonStyle(CMFCButton* button)
{
    button->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    button->m_bTransparent = FALSE;
    button->SetFaceColor(BizTeleworkColor::ButtonBackground(), true);
    button->SetTextColor(BizTeleworkColor::ButtonText());
    BizTeleworkDialogUtility::ApplyFontSize(button, 13, m_NewFont);
}

afx_msg void BizTeleworkAuthenticationDialog::OnBnClickedClose()
{
    EndDialog(IDCANCEL);
}

void BizTeleworkAuthenticationDialog::OnBnClickedButtonLogin()
{
    CString companyCode = L"";
    CString activationCode = L"";
    m_EditCompanyCode.GetWindowText(companyCode);
    m_EditActiveCode.GetWindowText(activationCode);

    if (companyCode.IsEmpty() || activationCode.IsEmpty())
    {
        std::wstring str(L"");
        if (!m_LanguageResource->GetString(IDS_PLEASE_ENTER_ERROR_MESSAGE, &str))
        {
            ShowError(L"");
        }
        else
        {
            ShowError(str);
        }
        return;
    }

    if (!StartRequestGetAuthenticateInforAndControlItems(std::wstring(companyCode), std::wstring(activationCode)))
    {
        LOG_WARN("StartRequestGetAuthenticateInforAndControlItems() failed.");
    }

    if (!BizTeleworkDialogUtility::SetCursorWait(false, this))
    {
        LOG_WARN("BizTeleworkDialogUtility::SetCursorWait() failed.");
    }
}

void BizTeleworkAuthenticationDialog::ApplyInputTextStyle(CEdit* inputText)
{
    BizTeleworkDialogUtility::ApplyFontSize(inputText, 16, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(inputText, L"Yu Gothic UI Normal", m_NewFont);
    ::SendMessage(inputText->GetSafeHwnd(), EM_SETMARGINS, EC_LEFTMARGIN, 10);
}

void BizTeleworkAuthenticationDialog::ShowError(const std::wstring& errorText)
{
    m_StaticError.ShowWindow(SW_SHOW);
    m_StaticError.SetWindowTextW(errorText.c_str());
}

void BizTeleworkAuthenticationDialog::ShowAuthSuccess()
{
    m_StaticAuthSuccess.ShowWindow(SW_SHOW);
    m_EditActiveCode.EnableWindow(FALSE);
    m_EditCompanyCode.EnableWindow(FALSE);
    m_ButtonLogin.EnableWindow(FALSE);
    Sleep(1500);
}

bool BizTeleworkAuthenticationDialog::StartRequestGetAuthenticateInforAndControlItems(const std::wstring& companyCode,
                                                                                      const std::wstring& activationCode)
{
    if (!&m_ButtonLogin)
    {
        LOG_ERROR("The button must not be null.");
        return false;
    }

    if (!BizTeleworkDialogUtility::SetCursorWait(true, this))
    {
        LOG_WARN("BizTeleworkDialogUtility::SetCursorWait() failed.");
    }

    auto thr = std::thread([this, companyCode, activationCode]()
    {
        boost::timer::cpu_timer timer;
        CString apiURL = L"";
        std::wstring wstringApiUrl = L"";
        BizTeleworkDependency dependency;
        if (this->IsChangeAPIURL())
        {
            m_EditAPIURL.GetWindowTextW(apiURL);
            wstringApiUrl = apiURL.GetString();
            AppendAPIURL(&wstringApiUrl);
        }
        std::unique_ptr<Request> request = dependency.GetRequestForActivation(HttpMethod::POST, wstringApiUrl);
        if (!request)
        {
            return;
        }
        std::unique_ptr<CallContext> callContext = request->CallWindowsAuthenticate(companyCode, activationCode);

        if (callContext == nullptr)
        {
            LOG_ERROR("request->CallWindowsAuthenticate() failed.");
            return;
        }
        else if (!callContext->Wait())
        {
            LOG_WARN("callContext->Wait() takes %s[sec].", timer.format(2, "%w").c_str());
            LOG_WARN("callContext->Wait() failed.");
            LOG_WARN("Requesting companyCode = %S, activationCode = %S", companyCode.c_str(), activationCode.c_str());
            std::wstring str(L"");
            m_LanguageResource->GetString(IDS_COMMUNICATION_ERROR_MESSAGE, &str);
            ShowError(str);
            if (!BizTeleworkDialogUtility::SetCursorWait(false, this))
            {
                LOG_WARN("BizTeleworkDialogUtility::SetCursorWait() failed.");
            }
            return;
        }

        if (callContext->StatusCode() == HTTP_STATUS_CODE_OK)
        {
            if (!BizTeleworkDialogUtility::SetCursorWait(false, this))
            {
                LOG_WARN("BizTeleworkDialogUtility::SetCursorWait() failed.");
            }
            try
            {
                std::unique_ptr<Responsive> res = ResponsiveCreator::GetResponsive((HTTP_STATUS_CODE)callContext->StatusCode(), callContext->Body());
                AuthenticateOkieResponsive& authenResponsive = dynamic_cast<AuthenticateOkieResponsive&>(*res);
                if (!authenResponsive.ParseValue())
                {
                    LOG_WARN("authenRes.ParseValue() failed.");
                    return;
                }

                AGENT_INFOR agentInfor = {};
                agentInfor.companyGUID = authenResponsive.GetCompanyGUID();
                agentInfor.agentGUID = authenResponsive.GetAgentGUID();
                agentInfor.apiUrl = wstringApiUrl;
                if (!agentInfor.companyGUID.empty() && !agentInfor.agentGUID.empty() && SaveAuthenticationInformation(agentInfor))
                {
                    ShowAuthSuccess();
                    SetKeyboardAndMouseHook();
                    RegisterSessionNotification();
                    SetContextMenu();
                    SetShowOnDutyDialogRemindTimer();
                }

                if (dependency.IsActive() && !this->PostMessage(WM_CLOSE))
                {
                    LOG_WARN("BizTeleworkAuthenticationDialog.PostMessage(WM_CLOSE) failed.");
                }
            }
            catch (const std::bad_cast& e)
            {
                LOG_ERROR("Dynamic cast AuthenticateOkieResponsive failed %s.", e.what());
            }
        }
        else
        {
            std::wstring str(L"");
            m_LanguageResource->GetString(IDS_AUTHENTICATION_ERROR_MESSAGE, &str);
            LOG_WARN("Requesting companyCode = %S, activationCode = %S", companyCode.c_str(), activationCode.c_str());
            ShowError(str);
            if (!BizTeleworkDialogUtility::SetCursorWait(false, this))
            {
                LOG_WARN("BizTeleworkDialogUtility::SetCursorWait() failed.");
            }
        }
    });
    thr.detach();

    return true;
}

void BizTeleworkAuthenticationDialog::ApplyErrorTextStyle(CStatic* error)
{
    BizTeleworkDialogUtility::ApplyFontSize(error, 9, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(error, L"Yu Gothic UI Normal", m_NewFont);
}

void BizTeleworkAuthenticationDialog::ApplAuthSuccessTextStyle(CStatic* success)
{
    BizTeleworkDialogUtility::ApplyFontSize(success, 9, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(success, L"Yu Gothic UI Normal", m_NewFont);
}

void BizTeleworkAuthenticationDialog::ApplyStaticEnterURLStyle(CStatic* labelEnterURL)
{
    BizTeleworkDialogUtility::ApplyFontSize(labelEnterURL, 12, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(labelEnterURL, L"Yu Gothic UI Normal", m_NewFont);
}

void BizTeleworkAuthenticationDialog::ApplyStaticExplanationStyle(CStatic* labelExplanation)
{
    BizTeleworkDialogUtility::ApplyFontSize(labelExplanation, 14, m_NewFont);
    BizTeleworkDialogUtility::ApplyFontFaceName(labelExplanation, L"Yu Gothic UI Normal", m_NewFont);
}

void BizTeleworkAuthenticationDialog::OnBnClickedEnterURL()
{
    m_IsChangeAPIURL = !m_IsChangeAPIURL;
    ApplyPositionDialogAndControl(m_IsChangeAPIURL);
}

afx_msg BOOL BizTeleworkAuthenticationDialog::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
    CopyData* cp = reinterpret_cast<CopyData*>(pCopyDataStruct->lpData);

    if (pCopyDataStruct->dwData != COPYDATA_TYPE_1)
    {
        LOG_WARN("Unknow copy data.");
        return FALSE;
    }

    if (pCopyDataStruct->cbData < sizeof(GUID))
    {
        LOG_WARN("Unknow copy data.");
        return FALSE;
    }

    if (!memcmp(&Signature, &cp->signature, sizeof(GUID)) == 0)
    {
        LOG_WARN("Unknow copy data.");
        return FALSE;
    }

    m_ActivationInformation = cp->infor;
    InitInputText();
    if (ShouldAuthenNow())
    {
        OnBnClickedButtonLogin();
    }

    return TRUE;
}

void BizTeleworkAuthenticationDialog::OnDestroy()
{
    SaveInputFieldsToTemporaryMemory();
    CDialogEx::OnDestroy();
}

void BizTeleworkAuthenticationDialog::ApplyPositionDialogAndControlMore()
{
    MONITORINFOEX monitorInfo;
    if (BizTeleworkDialogUtility::GetMonitorInfor(this, &monitorInfo))
    {
        LONG x = monitorInfo.rcWork.right - monitorInfo.rcWork.right / 2 - BizTeleworkDimension::ActiveDialogWidth() / 2;
        LONG y = monitorInfo.rcWork.bottom - monitorInfo.rcWork.bottom / 2 - BizTeleworkDimension::ActiveDialogHeightMore() / 2;

        SetWindowPos(&CWnd::wndTop, x, y, BizTeleworkDimension::ActiveDialogWidth(), BizTeleworkDimension::ActiveDialogHeightMore(), 0);

        RECT rect;
        this->GetClientRect(&rect);

        x = rect.right - BizTeleworkDimension::CloseWidth() - BizTeleworkDimension::CloseMarginRight();
        y = BizTeleworkDimension::CloseMarginTop();
        m_Close.SetWindowPos(NULL, x, y, BizTeleworkDimension::CloseWidth(), BizTeleworkDimension::CloseHeight(), SWP_NOZORDER);

        x = rect.right - BizTeleworkDimension::ImageDimension() - BizTeleworkDimension::ImageMarginRight();
        y = rect.bottom - BizTeleworkDimension::ImageDimension() - BizTeleworkDimension::ImageMarginBottomMore();
        m_StaticImage.SetWindowPos(NULL, x, y, BizTeleworkDimension::ImageDimension(), BizTeleworkDimension::ImageDimension(), 0);

        x = rect.right - BizTeleworkDimension::StaticAuthExplanationWidth() - BizTeleworkDimension::StaticAuthExplanationMarginRight();
        y = rect.bottom - BizTeleworkDimension::StaticAuthExplanationHeight() - BizTeleworkDimension::StaticAuthExplanationnBottomMore();
        m_StaticAuthExplanation.SetWindowPos(NULL, x, y, BizTeleworkDimension::StaticAuthExplanationWidth(), BizTeleworkDimension::StaticAuthExplanationHeight(), SWP_NOZORDER);

        x = rect.right - BizTeleworkDimension::EditTextWidth() - BizTeleworkDimension::EditTextMarginRight();
        y = rect.bottom - BizTeleworkDimension::EditTextHeight() - BizTeleworkDimension::EditCompanyCodeMarginBottomMore();
        m_EditCompanyCode.SetWindowPos(NULL, x, y, BizTeleworkDimension::EditTextWidth(), BizTeleworkDimension::EditTextHeight(), SWP_NOZORDER);

        x = rect.right - BizTeleworkDimension::EditTextWidth() - BizTeleworkDimension::EditTextMarginRight();
        y = rect.bottom - BizTeleworkDimension::EditTextHeight() - BizTeleworkDimension::EditActiveCodeMarginBottomMore();
        m_EditActiveCode.SetWindowPos(NULL, x, y, BizTeleworkDimension::EditTextWidth(), BizTeleworkDimension::EditTextHeight(), SWP_NOZORDER);

        x = rect.right - BizTeleworkDimension::ErrorTextWidth() - BizTeleworkDimension::EditTextMarginRight();
        y = rect.bottom - BizTeleworkDimension::ErrorTextHeight() - BizTeleworkDimension::ErrorTextMarginBottomMore();
        m_StaticError.SetWindowPos(NULL, x, y, BizTeleworkDimension::ErrorTextWidth(), BizTeleworkDimension::ErrorTextHeight(), 0);

        m_StaticAuthSuccess.SetWindowPos(NULL, x, y, BizTeleworkDimension::ErrorTextWidth(), BizTeleworkDimension::ErrorTextHeight(), 0);

        x = rect.right - BizTeleworkDimension::StaticEnterURLWidth() - BizTeleworkDimension::StaticEnterURLMarginRight();
        y = rect.bottom - BizTeleworkDimension::StaticEnterURLHeight() - BizTeleworkDimension::StaticEnterURLMarginBottomMore();
        m_StaticTextEnterURL.SetWindowPos(NULL, x, y, BizTeleworkDimension::StaticEnterURLWidth(), BizTeleworkDimension::StaticEnterURLHeight(), 0);

        m_ExpandLess.ShowWindow(SW_HIDE);
        m_ExpandMore.ShowWindow(SW_SHOW);
        x = rect.right - BizTeleworkDimension::ExpandArrowDimension() - BizTeleworkDimension::ExpandArrowMarginRight();
        y = rect.bottom - BizTeleworkDimension::ExpandArrowDimension() - BizTeleworkDimension::ExpandArrowMarginBottomMore();
        m_ExpandMore.SetWindowPos(NULL, x, y, BizTeleworkDimension::ExpandArrowDimension(), BizTeleworkDimension::ExpandArrowDimension(), 0);

        m_EditAPIURL.ShowWindow(SW_SHOW);
        x = rect.right - BizTeleworkDimension::EditTextWidth() - BizTeleworkDimension::EditTextMarginRight();
        y = rect.bottom - BizTeleworkDimension::EditTextHeight() - BizTeleworkDimension::EditUpdateURLMarginBottomMore();
        m_EditAPIURL.SetWindowPos(NULL, x, y, BizTeleworkDimension::EditTextWidth(), BizTeleworkDimension::EditTextHeight(), SWP_NOZORDER);

        x = rect.right - BizTeleworkDimension::LoginButtonWidth() - BizTeleworkDimension::LoginButtonMarginRight();
        y = rect.bottom - BizTeleworkDimension::LoginButtonHeight() - BizTeleworkDimension::LoginButtonMarginBottom();
        m_ButtonLogin.SetWindowPos(NULL, x, y, BizTeleworkDimension::LoginButtonWidth(), BizTeleworkDimension::LoginButtonHeight(), SWP_NOZORDER);
    }
}

bool BizTeleworkAuthenticationDialog::SaveAuthenticationInformation(const AGENT_INFOR& agentInfor)
{
    AuthenHelper authenHelper;
    return authenHelper.SaveAuthentication(agentInfor);
}

BOOL BizTeleworkAuthenticationDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        this->OnBnClickedButtonLogin();
        return FALSE;
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void BizTeleworkAuthenticationDialog::InitInputText()
{
    if (HasActivationInformation())
    {
        m_EditCompanyCode.SetWindowTextW(m_ActivationInformation.companyCode);
        m_EditActiveCode.SetWindowTextW(m_ActivationInformation.activationCode);
    }
    else
    {
        RestoreImputFieldsFromTemporaryMemory();
    }
}

void BizTeleworkAuthenticationDialog::ApplyPositionDialogAndControlLess()
{
    MONITORINFOEX monitorInfo;
    if (BizTeleworkDialogUtility::GetMonitorInfor(this, &monitorInfo))
    {
        LONG x = monitorInfo.rcWork.right - monitorInfo.rcWork.right / 2 - BizTeleworkDimension::ActiveDialogWidth() / 2;
        LONG y = monitorInfo.rcWork.bottom - monitorInfo.rcWork.bottom / 2 - BizTeleworkDimension::ActiveDialogHeightLess() / 2;

        SetWindowPos(&CWnd::wndTop, x, y, BizTeleworkDimension::ActiveDialogWidth(), BizTeleworkDimension::ActiveDialogHeightLess(), 0);

        RECT rect;
        this->GetClientRect(&rect);

        m_StaticError.ShowWindow(SW_HIDE);
        m_StaticAuthSuccess.ShowWindow(SW_HIDE);

        x = rect.right - BizTeleworkDimension::CloseWidth() - BizTeleworkDimension::CloseMarginRight();
        y = BizTeleworkDimension::CloseMarginTop();
        m_Close.SetWindowPos(NULL, x, y, BizTeleworkDimension::CloseWidth(), BizTeleworkDimension::CloseHeight(), SWP_NOZORDER);

        x = rect.right - BizTeleworkDimension::ImageDimension() - BizTeleworkDimension::ImageMarginRight();
        y = rect.bottom - BizTeleworkDimension::ImageDimension() - BizTeleworkDimension::ImageMarginBottomLess();
        m_StaticImage.SetWindowPos(NULL, x, y, BizTeleworkDimension::ImageDimension(), BizTeleworkDimension::ImageDimension(), 0);

        x = rect.right - BizTeleworkDimension::StaticAuthExplanationWidth() - BizTeleworkDimension::StaticAuthExplanationMarginRight();
        y = rect.bottom - BizTeleworkDimension::StaticAuthExplanationHeight() - BizTeleworkDimension::StaticAuthExplanationnBottomLess();
        m_StaticAuthExplanation.SetWindowPos(NULL, x, y, BizTeleworkDimension::StaticAuthExplanationWidth(), BizTeleworkDimension::StaticAuthExplanationHeight(), 0);

        x = rect.right - BizTeleworkDimension::EditTextWidth() - BizTeleworkDimension::EditTextMarginRight();
        y = rect.bottom - BizTeleworkDimension::EditTextHeight() - BizTeleworkDimension::EditCompanyCodeMarginBottomLess();
        m_EditCompanyCode.SetWindowPos(NULL, x, y, BizTeleworkDimension::EditTextWidth(), BizTeleworkDimension::EditTextHeight(), SWP_NOZORDER);

        x = rect.right - BizTeleworkDimension::EditTextWidth() - BizTeleworkDimension::EditTextMarginRight();
        y = rect.bottom - BizTeleworkDimension::EditTextHeight() - BizTeleworkDimension::EditActiveCodeMarginBottomLess();
        m_EditActiveCode.SetWindowPos(NULL, x, y, BizTeleworkDimension::EditTextWidth(), BizTeleworkDimension::EditTextHeight(), SWP_NOZORDER);

        x = rect.right - BizTeleworkDimension::ErrorTextWidth() - BizTeleworkDimension::EditTextMarginRight();
        y = rect.bottom - BizTeleworkDimension::ErrorTextHeight() - BizTeleworkDimension::ErrorTextMarginBottomLess();
        m_StaticError.SetWindowPos(NULL, x, y, BizTeleworkDimension::ErrorTextWidth(), BizTeleworkDimension::ErrorTextHeight(), 0);

        m_StaticAuthSuccess.SetWindowPos(NULL, x, y, BizTeleworkDimension::ErrorTextWidth(), BizTeleworkDimension::ErrorTextHeight(), 0);

        /* Reference ticket https://biztelework.atlassian.net/browse/BTC-698
        * Don't display Login URL on Staging/Production.
        */
        if (m_ReleaseEnvironment == ReleaseEnvironment::STAGING
            || m_ReleaseEnvironment == ReleaseEnvironment::PRODUCTION)
        {
            m_StaticTextEnterURL.ShowWindow(SW_HIDE);
            m_ExpandLess.ShowWindow(SW_HIDE);
        }
        else
        {
            x = rect.right - BizTeleworkDimension::StaticEnterURLWidth() - BizTeleworkDimension::StaticEnterURLMarginRight();
            y = rect.bottom - BizTeleworkDimension::StaticEnterURLHeight() - BizTeleworkDimension::StaticEnterURLMarginBottomLess();
            m_StaticTextEnterURL.SetWindowPos(NULL, x, y, BizTeleworkDimension::StaticEnterURLWidth(), BizTeleworkDimension::StaticEnterURLHeight(), 0);

            m_ExpandLess.ShowWindow(SW_SHOW);
            x = rect.right - BizTeleworkDimension::ExpandArrowDimension() - BizTeleworkDimension::ExpandArrowMarginRight();
            y = rect.bottom - BizTeleworkDimension::ExpandArrowDimension() - BizTeleworkDimension::ExpandArrowMarginBottomLess();
            m_ExpandLess.SetWindowPos(NULL, x, y, BizTeleworkDimension::ExpandArrowDimension(), BizTeleworkDimension::ExpandArrowDimension(), 0);
        }

        m_ExpandMore.ShowWindow(SW_HIDE);
        m_EditAPIURL.ShowWindow(SW_HIDE);

        x = rect.right - BizTeleworkDimension::LoginButtonWidth() - BizTeleworkDimension::LoginButtonMarginRight();
        y = rect.bottom - BizTeleworkDimension::LoginButtonHeight() - BizTeleworkDimension::LoginButtonMarginBottom();
        m_ButtonLogin.SetWindowPos(NULL, x, y, BizTeleworkDimension::LoginButtonWidth(), BizTeleworkDimension::LoginButtonHeight(), SWP_NOZORDER);
    }
}

void BizTeleworkAuthenticationDialog::ApplyLanguageResourceLabelEnterURL()
{
    std::wstring str = L"";
    if (!m_LanguageResource->GetString(IDS_STATIC_ENTER_URL, &str))
    {
        LOG_WARN("m_LanguageResource->GetString(IDS_STATIC_ENTER_URL_SHOW) failed.");
        m_StaticTextEnterURL.SetWindowTextW(L"");
    }
    else
    {
        m_StaticTextEnterURL.SetWindowTextW(str.c_str());
    }
}

bool BizTeleworkAuthenticationDialog::IsChangeAPIURL() const
{
    return m_IsChangeAPIURL;
}

void BizTeleworkAuthenticationDialog::GetReleaseEnvironment()
{
    AppConfig appConfig;
    std::string releaseEnvironment;
    if (appConfig.GetReleaseEnvironment(&releaseEnvironment))
    {
        const std::map< ReleaseEnvironment, const char*> releaseEnvironmentsMapToString =
            {
                {ReleaseEnvironment::DEVELOPMENT, "Development"},
                {ReleaseEnvironment::TESTING, "Testing"},
                {ReleaseEnvironment::STAGING, "Staging"},
                {ReleaseEnvironment::PRODUCTION, "Production"},
            };
        for (auto& iterator : releaseEnvironmentsMapToString)
        {
            if (std::string(iterator.second) == releaseEnvironment)
            {
                m_ReleaseEnvironment = iterator.first;
                break;
            }
        }
    }
}

bool BizTeleworkAuthenticationDialog::ShouldAuthenNow() const
{
    return (m_ActivationInformation.companyCode[0] != L'\0') && (m_ActivationInformation.activationCode[0] != L'\0');
}

bool BizTeleworkAuthenticationDialog::HasActivationInformation() const
{
    return (m_ActivationInformation.companyCode[0] != L'\0') || (m_ActivationInformation.activationCode[0] != L'\0');
}

void BizTeleworkAuthenticationDialog::SaveInputFieldsToTemporaryMemory() const
{
    try
    {
        AuthenHelper authenHelper;
        if ((!authenHelper.GetCompanyGUIDFromRegistry().empty()) && (!authenHelper.GetAgentGUIDFromRegistry().empty()))
        {
            // Reach here when authentication is succceeded.
            TemporaryUserInputs.companyCode.clear();
            TemporaryUserInputs.activeCode.clear();
            return;
        }

        int len = m_EditCompanyCode.GetWindowTextLengthW();
        if (len > 0)
        {
            TemporaryUserInputs.companyCode.resize(len);
            if (m_EditCompanyCode.GetWindowTextW(&TemporaryUserInputs.companyCode[0], len + 1) != len)
            {
                LOG_WARN("m_EditCompanyCode.GetWindowTextW() failed.");
            }
        }
        else
        {
            TemporaryUserInputs.companyCode.clear();
        }

        len = m_EditActiveCode.GetWindowTextLengthW();
        if (len > 0)
        {
            TemporaryUserInputs.activeCode.resize(len);
            if (m_EditActiveCode.GetWindowTextW(&TemporaryUserInputs.activeCode[0], len + 1) != len)
            {
                LOG_WARN("m_EditCompanyCode.GetWindowTextW() failed.");
            }
        }
        else
        {
            TemporaryUserInputs.activeCode.clear();
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
    }
}

void BizTeleworkAuthenticationDialog::RestoreImputFieldsFromTemporaryMemory()
{
    m_EditCompanyCode.SetWindowTextW(TemporaryUserInputs.companyCode.c_str());
    m_EditActiveCode.SetWindowTextW(TemporaryUserInputs.activeCode.c_str());
}

void BizTeleworkAuthenticationDialog::SetKeyboardAndMouseHook()
{
    BOOL setHook = TRUE;
    if (AfxGetApp()->m_pMainWnd
        && !::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), BIZ_TELEWORK_MESSAGE_KEYBOAD_AND_MOUSE_HOOK, setHook, (LPARAM)NULL))
    {
        LOG_ERROR("PostMessage() BIZ_TELEWORK_MESSAGE_KEYBOAD_AND_MOUSE_HOOK failed.");
    }
}

void BizTeleworkAuthenticationDialog::SetContextMenu()
{
    BOOL setContextMenu = TRUE;
    if (AfxGetApp()->m_pMainWnd
        && !::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), BIZ_TELEWORK_MESSAGE_CONTEXT_MENU, setContextMenu, (LPARAM)NULL))
    {
        LOG_ERROR("PostMessage() BIZ_TELEWORK_MESSAGE_CONTEXT_MENU failed.");
    }
}

void BizTeleworkAuthenticationDialog::SetShowOnDutyDialogRemindTimer()
{
    if (AfxGetApp()->m_pMainWnd
        && !::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), BIZ_TELEWORK_MESSAGE_SET_SHOWING_ON_DUTY_DIALOG_REMIND_TIMER, (WPARAM)NULL, (LPARAM)NULL))
    {
        LOG_ERROR("PostMessage() BIZ_TELEWORK_MESSAGE_SET_SHOWING_ON_DUTY_DIALOG_REMIND_TIMER failed.");
    }
}

void BizTeleworkAuthenticationDialog::RegisterSessionNotification()
{
    if (AfxGetApp()->m_pMainWnd
        && !::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), BIZ_TELEWORK_MESSAGE_REGISTER_SESSION_NOTIFICATION, (WPARAM)NULL, (LPARAM)NULL))
    {
        LOG_ERROR("PostMessage() BIZ_TELEWORK_MESSAGE_REGISTER_SESSION_NOTIFICATION failed.");
    }
}

