#pragma once
#include "stdafx.h"
#include <memory>
#include "BizTeleworkDialogDependency.h"
#include <LanguageResourceInterface.h>
#include <LanguageResource.h>
#include "BizTeleworkFailedCallApiDialog.h"
#include "Request.h"

class BizTeleworkDayOffVerifyDialog : public CDialogEx
{
    DECLARE_DYNAMIC(BizTeleworkDayOffVerifyDialog)

    explicit BizTeleworkDayOffVerifyDialog(const BizTeleworkDialogDependency& dependency, CWnd* pParent);
    virtual ~BizTeleworkDayOffVerifyDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_BIZTELEWORK_DAY_OFF_VERIFICATION_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnBnClickedButtonOffDay();
    afx_msg void OnBnClickedButtonDoWorking();
    afx_msg LRESULT OnMessageDayOffRequestsResponse(WPARAM wParam, LPARAM lParam);

private:
    CMFCButton m_ButtonOffDay;
    CMFCButton m_ButtonDoWorking;
    CStatic m_StaticImage;
    CStatic m_StaticTakeBreakQuestion;
    CBitmap m_BitMapCleanup;
    CFont   m_NewFont;
    LanguageResourceInterface* m_LanguageResource;

    void ApplyLanguageResource();
    void ApplyPositionDialogAndControl();
    void ApplyStyle();
    void ApplyButtonStyle(CMFCButton* button);
};
