#pragma once
#include "stdafx.h"
#include <memory>
#include "BizTeleworkDialogDependency.h"
#include <LanguageResourceInterface.h>
#include <LanguageResource.h>
#include "BizTeleworkFailedCallApiDialog.h"
#include "Request.h"
#include "GettingRequest.h"

enum class TypeOperatingContent
{
    NORMAL_TYPE,
    PAST_TYPE,
    TWENTY_FOUR_TYPE,
};

class BizTeleworkOnDutyDialog : public CDialogEx
{
    DECLARE_DYNAMIC(BizTeleworkOnDutyDialog)

    explicit BizTeleworkOnDutyDialog(const BizTeleworkDialogDependency& dependency,
                                     const int64_t unixStartedTime,
                                     CWnd* pParent);
    virtual ~BizTeleworkOnDutyDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_BIZTELEWORK_ON_DUTY_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnBnClickedButtonFinishWorking();

private:
    CMFCButton m_ButtonEndWork;
    CStatic m_StaticImage;
    CBitmap m_BitMapCleanup;
    CFont   m_NewFont;
    LanguageResourceInterface* m_LanguageResource;
    UINT_PTR m_CalculateWorkingTimeTimer = 0;
    int64_t m_UnixStartTime = 0;
    CStatic m_OperatingTimeLabel;
    CStatic m_OperatingTimeContent;
    CStatic m_NotificationTerminalTimeIncorrect;

    void ApplyLanguageResource();
    void ApplyPositionDialogAndControl();
    void ApplyStyle();
    void ApplyButtonStyle(CMFCButton* button);
    void SetTimersDialog();
    void SetOperatingTimeContent();
    void OpenEmployeeDashboard(const std::wstring& url) const;
    BOOL KillCalculateTimer();
    BOOL ProcessDeviceLog(Request* request);
    BOOL ProcessActiveAppLog(Request* request);
    BOOL ProcessWorkLog(Request* request);
    BOOL CheckCallContext(CallContext* callContext) const;

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnClose();

    TypeOperatingContent GetOperatingTimeContentType(const int64_t secondWorkingTime);
    bool GetOperatingTimeContent(const TypeOperatingContent type,
                                 const int64_t secondWorkingTime,
                                 std::wstring* operatingContent);
    int64_t GetUnixStartTime() const;
};
