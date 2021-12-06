#pragma once

#include "stdafx.h"
#include "RequestWorkLogsType.h"
#include "RequestHealthStatusType.h"
#include "BizTeleworkFailedCallApiDialog.h"
#include "Request.h"

class BizTeleworkDialogUtility
{
public:
    static void ApplyWeightFont(CWnd* control, const LONG weight, CFont& newFont);
    static void ApplyFontFaceName(CWnd* control, const WCHAR* fontFaceName, CFont& newFont);
    static void ApplyFontSize(CWnd* control, const LONG sizeOnPixel, CFont& newFont);
    static void ApplyOpacity(CWnd* control, const COLORREF color, const BYTE alpha);
    static CFont* GetCurrentFont(const CWnd* control);
    static IStream* CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType);
    static void ApplyStaticImageStyle(CStatic* staticImage, CBitmap* bitmapCleanUp);
    static bool GetMonitorInfor(const CDialogEx* dialog, MONITORINFOEX* monitorInfor);
    static bool SetCursorWait(bool wait, const CWnd* cwnd);
    static bool StartRequestWorkLogsAndControlItems(CMFCButton* button,
                                                    const RequestWorkLogsType workLogsType,
                                                    CWnd* dialog);
    static bool StartRequestUserConditionLogsAndControlItems(const CMFCButton* button,
                                                             const RequestHealthStatusType healthStatusType,
                                                             CWnd* dialog);
    static ErrorDialogMessageType GetErrorDialogType(const unsigned int statusCode, const std::wstring& body);
    static void DisplayErrorDialog(const ErrorDialogMessageType messageType);
    static bool CheckVisibleDialog(const CWnd* dialog);
    static bool CheckRequest(const Request* request);
    static BOOL CheckCallContext(CallContext* callContext, const CWnd* dialog);
    static bool StartRequestDayOffRequestsAndControlItems(CMFCButton* button, CWnd* dialog);
    static UINT GetBizTeleworkMessage(const TypeCallContext typeCallContext);
    static void DisplayActiveDialog();
    static bool StartRequestWorkTargetAndControlItems(CMFCButton* button,
                                                      const std::wstring& workTarget,
                                                      const CWnd* dialog);
};
