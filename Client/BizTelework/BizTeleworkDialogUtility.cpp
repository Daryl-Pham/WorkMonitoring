#include "stdafx.h"
#include "BizTeleworkDialogUtility.h"
#include "BizTeleworkAuthenticationDialog.h"
#include "BizTeleworkMessage.h"
#include "BizTeleworkColor.h"
#include "HttpStatusCode.h"
#include "GettingRequest.h"
#include "ResponsiveCreator.h"
#include <boost/timer/timer.hpp>
#include "Log.h"
#include "Windows.h"
#include "resource.h"
#include <memory>
#include "Request.h"
#include <thread>
#include "BizTeleworkDependency.h"
#include "GettingRequest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    const double WINDOWS_BASE_DPI = 96;

}  // namespace

void BizTeleworkDialogUtility::ApplyWeightFont(CWnd* control, const LONG weight, CFont& newFont)
{
    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));

    CFont* pFont = GetCurrentFont(control);
    if (!pFont)
    {
        return;
    }

    if (!pFont->GetLogFont(&lf))
    {
        LOG_WARN("GetLogFont() failed.");
        lf = { -21, 0, 0, 0, 400, 0, 0, 0, 128, 0, 0, 0, 0, L"Yu Gothic UI Light" };
    }

    lf.lfWeight = weight;

    newFont.Detach();
    newFont.DeleteObject();
    newFont.CreateFontIndirect(&lf);

    control->SetFont(&newFont);
}

void BizTeleworkDialogUtility::ApplyFontFaceName(CWnd* control, const WCHAR* fontFaceName, CFont& newFont)
{
    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));

    CFont* pFont = GetCurrentFont(control);
    if (!pFont)
    {
        return;
    }

    if (!pFont->GetLogFont(&lf))
    {
        LOG_WARN("GetLogFont() failed.");
        lf = { -21, 0, 0, 0, 400, 0, 0, 0, 128, 0, 0, 0, 0, L"Yu Gothic UI Light" };
    }

    wmemcpy(lf.lfFaceName, fontFaceName, LF_FACESIZE);

    newFont.Detach();
    newFont.DeleteObject();
    newFont.CreateFontIndirect(&lf);

    control->SetFont(&newFont);
}

void BizTeleworkDialogUtility::ApplyFontSize(CWnd* control, const LONG sizeOnPixel, CFont& newFont)
{
    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));

    CFont* pFont = GetCurrentFont(control);
    if (!pFont)
    {
        return;
    }

    if (!pFont->GetLogFont(&lf))
    {
        LOG_WARN("GetLogFont() failed.");
        lf = { -21, 0, 0, 0, 400, 0, 0, 0, 128, 0, 0, 0, 0, L"Yu Gothic UI Light" };
    }

    const CDC* pDC = control->GetParent()->GetDC();

    double dpiScale = (pDC->GetDeviceCaps(LOGPIXELSY) / WINDOWS_BASE_DPI);  // Fallback for DPI adaption on older than Windows 10 1607.
    lf.lfHeight = -MulDiv(static_cast<int>(static_cast<double>(sizeOnPixel) / dpiScale), pDC->GetDeviceCaps(LOGPIXELSY), 72);
    ReleaseDC(control->GetParent()->GetSafeHwnd(), pDC->GetSafeHdc());

    newFont.Detach();
    newFont.DeleteObject();
    newFont.CreateFontIndirect(&lf);

    control->SetFont(&newFont);
}

void BizTeleworkDialogUtility::ApplyOpacity(CWnd* control, const COLORREF color, const BYTE alpha)
{
    LONG_PTR windowLongPtr = ::GetWindowLongPtr(control->GetSafeHwnd(), GWL_EXSTYLE);
    if (windowLongPtr == 0)
    {
        LOG_ERROR("GetWindowLongPtr() failed. 0x%08x", GetLastError());
        return;
    }

    if (SetWindowLongPtr(control->GetSafeHwnd(), GWL_EXSTYLE, windowLongPtr | WS_EX_LAYERED) == 0)
    {
        LOG_ERROR("SetWindowLongPtr() failed. 0x%08x", GetLastError());
        return;
    }

    if (!control->SetLayeredWindowAttributes(color, alpha, LWA_ALPHA))
    {
        LOG_ERROR("SetLayeredWindowAttributes() failed. 0x%08x", GetLastError());
        return;
    }

    if (!::RedrawWindow(control->GetSafeHwnd(), NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN))
    {
        LOG_ERROR("RedrawWindow() failed.");
    }
}

CFont* BizTeleworkDialogUtility::GetCurrentFont(const CWnd* control)
{
    CFont* pFont = control->GetFont();

    if (!pFont)
    {
        pFont = new CFont();
        if (!pFont->CreateFontW(-21, 0, 0, 0, 400, 0, 0, 0, 128, 0, 0, 0, 0, L"Yu Gothic UI Light"))
        {
            LOG_WARN("CreateFontW() failed.");
            if (pFont)
            {
                delete pFont;
                pFont = NULL;
            }
            return NULL;
        }
    }
    return pFont;
}

IStream* BizTeleworkDialogUtility::CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType)
{
    IStream* pIStream = NULL;
    HRESULT result;
    HRSRC hrsrc = ::FindResource(NULL, lpName, lpType);

    if (hrsrc == NULL)
    {
        LOG_ERROR("FindResource() failded. 0x%08x", GetLastError());
        return NULL;
    }

    DWORD dwResourceSize = ::SizeofResource(NULL, hrsrc);

    if (dwResourceSize == 0)
    {
        LOG_ERROR("SizeofResource() failded. 0x%08x", GetLastError());
        return NULL;
    }

    HGLOBAL hglbImage = ::LoadResource(NULL, hrsrc);
    if (hglbImage == NULL)
    {
        LOG_ERROR("LoadResource() failded. 0x%08x", GetLastError());
        return NULL;
    }

    const void* const pvSourceResourceData = LockResource(hglbImage);
    if (pvSourceResourceData == NULL)
    {
        LOG_ERROR("LockResource() failded.");
        return NULL;
    }

    HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
    if (hgblResourceData == NULL)
    {
        LOG_ERROR("GlobalAlloc() failded. 0x%08x", GetLastError());
        return NULL;
    }

    LPVOID pvResourceData = GlobalLock(hgblResourceData);

    if (pvResourceData == NULL)
    {
        LOG_ERROR("GlobalLock() failded. 0x%08x", GetLastError());
        GlobalFree(hgblResourceData);
        return NULL;
    }

    CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);

    if (!GlobalUnlock(hgblResourceData) && NO_ERROR != GetLastError())
    {
        LOG_ERROR("GlobalUnlock() failded. 0x%08x", GetLastError());
        GlobalFree(hgblResourceData);
        return NULL;
    }

    result = CreateStreamOnHGlobal(hgblResourceData, TRUE, &pIStream);
    if (!SUCCEEDED(result))
    {
        LOG_ERROR("CreateStreamOnHGlobal() failded: %d", result);
        GlobalFree(hgblResourceData);
        return NULL;
    }

    return pIStream;
}

void BizTeleworkDialogUtility::ApplyStaticImageStyle(CStatic* staticImage, CBitmap* bitmapCleanUp)
{
    CImage imgSource;
    IStream* pStream = BizTeleworkDialogUtility::CreateStreamOnResource(MAKEINTRESOURCE(staticImage->GetDlgCtrlID()), L"PNG");

    if (pStream != nullptr)
    {
        HRESULT result = imgSource.Load(pStream);
        if (!SUCCEEDED(result))
        {
            LOG_WARN("imgSource.Load(pStream) failed. Error: %d", result);
            return;
        }

        HBITMAP hbitmap = imgSource.Detach();
        HBITMAP holdbmp = staticImage->SetBitmap(hbitmap);

        if (holdbmp && !DeleteObject(holdbmp))
        {
            LOG_WARN("DeleteObject(holdbmp) failed.");
        }

        if (!bitmapCleanUp->Attach(hbitmap))
        {
            LOG_WARN("m_BitMapCleanup.Attach(hbitmap) failed.");
            if (hbitmap && !DeleteObject(hbitmap))
            {
                LOG_WARN("DeleteObject(hbitmap) failed.");
            }
            pStream->Release();
            return;
        }
        pStream->Release();
    }
    else
    {
        LOG_ERROR("BizTeleworkDialogUtility::CreateStreamOnResource() failded");
    }
}

bool BizTeleworkDialogUtility::GetMonitorInfor(const CDialogEx* dialog, MONITORINFOEX* monitorInfor)
{
    // Move dialog to right bottom.
    RECT wndRect;
    dialog->GetWindowRect(&wndRect);
    LONG wndWidth = std::abs(wndRect.right - wndRect.left);
    LONG wndHeight = std::abs(wndRect.bottom - wndRect.top);
    if (wndWidth < 1)
    {
        LOG_ERROR("The wndWidth should be 1 or more: %ld", wndWidth);
        return false;
    }
    else if (wndHeight < 1)
    {
        LOG_ERROR("The wndHeight should be 1 or more: %ld", wndHeight);
        return false;
    }
    POINT cursorPoint = { 0 };
    HMONITOR hMonitor = NULL;
    monitorInfor->cbSize = sizeof(MONITORINFOEX);
    if (!(hMonitor = MonitorFromPoint(cursorPoint, MONITOR_DEFAULTTOPRIMARY)))
    {
        LOG_ERROR("MonitorFromPoint() failed. 0x%08x", GetLastError());
        return false;
    }
    else if (!GetMonitorInfo(hMonitor, monitorInfor))
    {
        LOG_ERROR("GetMonitorInfo() failed. 0x%08x", GetLastError());
        return false;
    }

    return true;
}

bool BizTeleworkDialogUtility::SetCursorWait(bool wait, const CWnd* dialog)
{
    HCURSOR hCursor = NULL;
    LPCTSTR cursorName = wait ? IDC_WAIT : IDC_ARROW;
    HWND hWnd = NULL;

    if (!(hWnd = dialog->GetSafeHwnd()))
    {
        LOG_ERROR("GetSafeHwnd() failed.");
        return false;
    }
    else if (!(hCursor = LoadCursor(NULL, cursorName)))
    {
        LOG_ERROR("LoadCursor(%d) failed. 0x%08x", wait, GetLastError());
        return false;
    }
    else if (!SetClassLongPtr(hWnd, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(hCursor)))
    {
        LOG_ERROR("SetClassLong(%d) failed. 0x%08x", wait, GetLastError());

        if (!DestroyCursor(hCursor))
        {
            LOG_ERROR("DestroyCursor() failed. 0x%08x", GetLastError());
        }

        return false;
    }
    else if (!SetCursor(hCursor))
    {
        LOG_ERROR("SetCursor() failed.");
        return false;
    }

    LOG_INFO("SetClassLong suceeded. %d", wait);

    return true;
}

bool BizTeleworkDialogUtility::StartRequestWorkLogsAndControlItems(CMFCButton* button,
                                                                   const RequestWorkLogsType workLogsType,
                                                                   CWnd* dialog)
{
    if (!button)
    {
        LOG_ERROR("The button must not be null.");
        return false;
    }

    button->EnableWindow(FALSE);
    if (!BizTeleworkDialogUtility::SetCursorWait(true, dialog))
    {
        LOG_WARN("BizTeleworkDialogUtility::SetCursorWait() failed.");
    }

    auto thr = std::thread([workLogsType, dialog]()
    {
        std::unique_ptr<Request> request = GettingRequest::GetRequest();
        if (!request)
        {
            return;
        }
        std::unique_ptr<CallContext> callContext = request->CallWindowsWorkLogs(workLogsType);
        if (!CheckCallContext(callContext.get(), dialog))
        {
            LOG_WARN("Requesting workLogsType = %d", workLogsType);
        }
    });
    thr.detach();

    return true;
}

bool BizTeleworkDialogUtility::StartRequestUserConditionLogsAndControlItems(const CMFCButton* button,
                                                                            const RequestHealthStatusType healthStatusType,
                                                                            CWnd* dialog)
{
    if (!button)
    {
        LOG_ERROR("The button must not be null.");
        return false;
    }
    if (!BizTeleworkDialogUtility::SetCursorWait(true, dialog))
    {
        LOG_WARN("BizTeleworkDialogUtility::SetCursorWait() failed.");
    }

    auto thr = std::thread([healthStatusType, dialog]()
    {
        std::unique_ptr<Request> request = GettingRequest::GetRequest();
        if (!request)
        {
            return;
        }
        std::unique_ptr<CallContext> callContext = request->CallWindowsUserConditionLogs(healthStatusType);
        auto checkCallContext = CheckCallContext(callContext.get(), dialog);
        if (checkCallContext
            && !::PostMessage(*dialog, BIZ_TELEWORK_MESSAGE_USER_CONDITION_LOGS_RESPONSIVE, TRUE, static_cast<LPARAM>(healthStatusType)))
        {
            LOG_WARN("PostMessage() failed. 0x%08x", GetLastError());
        }
        else if (!checkCallContext)
        {
            LOG_WARN("Requesting healthStatusType = %d", healthStatusType);
        }
    });
    thr.detach();

    return true;
}

BOOL BizTeleworkDialogUtility::CheckCallContext(CallContext* callContext, const CWnd* dialog)
{
    HWND hWnd;
    if (!(hWnd = dialog->GetSafeHwnd()))
    {
        LOG_ERROR("GetSafeHwnd() failed.");
        return FALSE;
    }
    UINT message = GetBizTeleworkMessage(callContext->GetTypeCallContext());
    if (!callContext->Wait())
    {
        LOG_WARN("callContext->Wait() failed.");
        if (!::PostMessage(hWnd, message, FALSE, static_cast<LPARAM>(ErrorDialogMessageType::COMMUNICATION_ERROR_TYPE)))
        {
            LOG_WARN("PostMessage() failed. 0x%08x", GetLastError());
        }
        return FALSE;
    }
    boost::timer::cpu_timer timer;  // To know duration of post. For debug Internet connectivity condition.
    LOG_INFO("callContext->Wait() takes %s[sec]. ", timer.format(2, "%w").c_str());  // "%w" means wall clock time.

    if (!::IsWindowVisible(hWnd))
    {
        LOG_WARN("This windows is visbled or closed. 0x%08x", GetLastError());
        return FALSE;
    }

    BOOL succeeded = callContext->StatusCode() == HTTP_STATUS_CODE_CREATED;
    if (succeeded)
    {
        LOG_INFO("Succeeded to send request.");
        if (callContext->GetTypeCallContext() != TypeCallContext::WINDOWS_USER_CONDITION_LOGS
            && !::PostMessage(hWnd, message, TRUE, static_cast<LPARAM>(0)))
        {
            LOG_WARN("PostMessage() failed. 0x%08x", GetLastError());
        }
    }
    else
    {
        LOG_WARN("The request is failed. StatusCode: %d", callContext->StatusCode());
        ErrorDialogMessageType errorType = BizTeleworkDialogUtility::GetErrorDialogType(callContext->StatusCode(), callContext->Body());
        if (!::PostMessage(hWnd, message, FALSE, static_cast<LPARAM>(errorType)))
        {
            LOG_WARN("PostMessage() failed. 0x%08x", GetLastError());
        }
    }

    return succeeded;
}

ErrorDialogMessageType BizTeleworkDialogUtility::GetErrorDialogType(const unsigned int statusCode, const std::wstring& body)
{
    ErrorDialogMessageType type = ErrorDialogMessageType::COMMUNICATION_ERROR_TYPE;
    std::unique_ptr<Responsive> res = ResponsiveCreator::GetResponsive((HTTP_STATUS_CODE)statusCode, body);
    switch (statusCode)
    {
    case HTTP_STATUS_CODE_BAD_REQUEST:
        try
        {
            BadRequestResponsive& badRes = dynamic_cast<BadRequestResponsive&>(*res);
            if (!badRes.ParseValue())
            {
                LOG_WARN("badRes.ParseValue() failed.");
                return type;
            }
            if (badRes.GetStatus() == BadStatus::BAD_WORK_STATUS_STARTED
                || badRes.GetStatus() == BadStatus::BAD_DAY_OFF_REQUEST_STARTED)
            {
                type = ErrorDialogMessageType::BAD_REQUEST_STARTED_ERROR_TYPE;
            }
            else if (badRes.GetStatus() == BadStatus::BAD_WORK_STATUS_ENDED)
            {
                type = ErrorDialogMessageType::BAD_REQUEST_ENDED_ERROR_TYPE;
            }
        }
        catch (const std::bad_cast& e)
        {
            LOG_ERROR("Dynamic cast BadRequestResponsive failed %s.", e.what());
        }
        break;
    case HTTP_STATUS_CODE_UNAUTHORIZED:
        type = ErrorDialogMessageType::UNAUTHORIZED_ERROR_TYPE;
        break;
    default:
        break;
    }

    return type;
}

void BizTeleworkDialogUtility::DisplayErrorDialog(const ErrorDialogMessageType messageType)
{
    std::unique_ptr<BizTeleworkFailedCallApiDialog> errorDialog = std::make_unique<BizTeleworkFailedCallApiDialog>(messageType, nullptr);
    errorDialog->DoModal();
}

bool BizTeleworkDialogUtility::CheckVisibleDialog(const CWnd* dialog)
{
    HWND hWnd = NULL;
    if (!(hWnd = dialog->GetSafeHwnd()))
    {
        LOG_ERROR("GetSafeHwnd() failed.");
        return false;
    }
    else if (!::IsWindowVisible(hWnd))
    {
        LOG_WARN("This windows is visbled or closed. 0x%08x", GetLastError());
        return false;
    }
    return true;
}

bool BizTeleworkDialogUtility::CheckRequest(const Request* request)
{
    if (!request)
    {
        BizTeleworkDialogUtility::DisplayErrorDialog(ErrorDialogMessageType::COMMUNICATION_ERROR_TYPE);
        return false;
    }
    return true;
}

bool BizTeleworkDialogUtility::StartRequestDayOffRequestsAndControlItems(CMFCButton* button, CWnd* dialog)
{
    if (!button)
    {
        LOG_ERROR("The button must not be null.");
        return false;
    }

    button->EnableWindow(FALSE);
    button->SetFaceColor(BizTeleworkColor::ButtonDisableColor(), true);

    if (!BizTeleworkDialogUtility::SetCursorWait(true, dialog))
    {
        LOG_WARN("BizTeleworkDialogUtility::SetCursorWait() failed.");
    }

    auto thr = std::thread([dialog]()
    {
        std::unique_ptr<Request> request = GettingRequest::GetRequest();
        if (!request)
        {
            return;
        }
        std::unique_ptr<CallContext> callContext = request->CallWindowsDayOffRequests();
        CheckCallContext(callContext.get(), dialog);
    });
    thr.detach();

    return true;
}

UINT BizTeleworkDialogUtility::GetBizTeleworkMessage(const TypeCallContext typeCallContext)
{
    UINT message = 0;

    switch (typeCallContext)
    {
    case TypeCallContext::WINDOWS_WORK_LOGS:
        message = BIZ_TELEWORK_MESSAGE_WORK_LOGS_RESPONSE;
        break;
    case TypeCallContext::WINDOWS_USER_CONDITION_LOGS:
        message = BIZ_TELEWORK_MESSAGE_USER_CONDITION_LOGS_RESPONSIVE;
        break;
    case TypeCallContext::WINDOWS_DAY_OFF_REQUESTS:
        message = BIZ_TELEWORK_MESSAGE_DAY_OFF_REQUESTS_RESPONSIVE;
        break;
    case TypeCallContext::WINDOWS_AUTHENTICATE:
        message = BIZ_TELEWORK_MESSAGE_AUTHENTICATE_RESPONSIVE;
        break;
    case TypeCallContext::WINDOWS_WORK_TARGET:
        message = BIZ_TELEWORK_MESSAGE_WORK_TARGET;
        break;
    default:
        break;
    }

    return message;
}

void BizTeleworkDialogUtility::DisplayActiveDialog()
{
    auto thr = std::thread([]()
    {
        BizTeleworkDialogDependency dependency;
        std::unique_ptr<CDialogEx> dialog = std::make_unique<BizTeleworkAuthenticationDialog>(dependency, nullptr, ActiveInformation());
        dialog->DoModal();
    });
    thr.detach();
}

bool BizTeleworkDialogUtility::StartRequestWorkTargetAndControlItems(CMFCButton* button,
                                                                     const std::wstring& workTarget,
                                                                     const CWnd* dialog)
{
    if (!button)
    {
        LOG_WARN("The button must not be null.");
        return false;
    }

    button->EnableWindow(FALSE);
    button->SetFaceColor(BizTeleworkColor::ButtonDisableColor(), true);

    if (!BizTeleworkDialogUtility::SetCursorWait(true, dialog))
    {
        LOG_WARN("BizTeleworkDialogUtility::SetCursorWait() failed.");
    }

    std::unique_ptr<Request> request = GettingRequest::GetRequest();
    if (!request)
    {
        return false;
    }
    std::unique_ptr<CallContext> callContext = request->CallWindowsWorkTarget(workTarget);
    CheckCallContext(callContext.get(), dialog);

    return true;
}
