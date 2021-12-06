#pragma once
#include "stdafx.h"
#include <memory>
#include "BizTeleworkDialogDependency.h"
#include <LanguageResourceInterface.h>
#include <LanguageResource.h>
#include <Request.h>

class BizTeleworkOffDutyDialog : public CDialogEx
{
    DECLARE_DYNAMIC(BizTeleworkOffDutyDialog)

    explicit BizTeleworkOffDutyDialog(const BizTeleworkDialogDependency& dependency, CWnd* pParent);
    virtual ~BizTeleworkOffDutyDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_BIZTELEWORK_OFF_DUTY_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnBnClickedButtonStartWorking();
    afx_msg void OnBnClickedButtonNotYetWorking();
    afx_msg LRESULT OnMessageWorkLogsResponse(WPARAM wParam, LPARAM lParam);

private:
    CMFCButton m_ButtonStartWorking;
    CMFCButton m_ButtonNotYet;
    CStatic m_StaticNotification;
    CStatic m_StaticImage;
    CBitmap m_BitMapCleanup;
    CFont   m_NewFont;
    LanguageResourceInterface* m_LanguageResource;

    void ApplyLanguageResource();
    void ApplyPositionDialogAndControl();
    void ApplyStyle();
    void ApplyButtonStyle(CMFCButton* button);
    bool SendLogs();
};
