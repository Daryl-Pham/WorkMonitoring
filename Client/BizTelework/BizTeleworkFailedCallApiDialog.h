#pragma once
#include "stdafx.h"
#include <memory>
#include <LanguageResourceInterface.h>
#include <LanguageResource.h>

enum class ErrorDialogMessageType
{
    BAD_REQUEST_STARTED_ERROR_TYPE,
    BAD_REQUEST_ENDED_ERROR_TYPE,
    UNAUTHORIZED_ERROR_TYPE,
    COMMUNICATION_ERROR_TYPE,
    DEFAULT_ERROR_TYPE,
};

class BizTeleworkFailedCallApiDialog : public CDialogEx
{
    DECLARE_DYNAMIC(BizTeleworkFailedCallApiDialog)

    BizTeleworkFailedCallApiDialog(const ErrorDialogMessageType errorType, CWnd* pParent);
    explicit BizTeleworkFailedCallApiDialog(CWnd* pParent);
    virtual ~BizTeleworkFailedCallApiDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_BIZTELEWORK_FAILED_CALL_API_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnBnClickedButtonReturn();

private:
    LanguageResourceInterface* m_LanguageResource = LanguageResource::Singleton();
    CMFCButton m_ButtonRetry;
    CStatic m_StaticNotification;
    CStatic m_StaticImage;
    CBitmap m_BitMapCleanup;
    CFont   m_NewFont;
    ErrorDialogMessageType m_MessageType = ErrorDialogMessageType::DEFAULT_ERROR_TYPE;

    void ApplyLanguageResource();
    void ApplyPositionDialogAndControl();
    void ApplyStyle();
    void ApplyButtonStyle(CMFCButton* button);
};
