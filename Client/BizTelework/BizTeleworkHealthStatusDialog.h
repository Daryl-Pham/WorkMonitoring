#pragma once
#include "stdafx.h"
#include <memory>
#include "BizTeleworkDialogDependency.h"
#include <LanguageResourceInterface.h>
#include <LanguageResource.h>
#include "BizTeleworkFailedCallApiDialog.h"
#include "Request.h"

class BizTeleworkHealthStatusDialog : public CDialogEx
{
    DECLARE_DYNAMIC(BizTeleworkHealthStatusDialog)

    explicit BizTeleworkHealthStatusDialog(const BizTeleworkDialogDependency& dependency, CWnd* pParent);
    virtual ~BizTeleworkHealthStatusDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_BIZTELEWORK_HEALTH_STATUS_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnBnClickedButtonGoodHealth();
    afx_msg void OnBnClickedButtonNormalHealth();
    afx_msg void OnBnClickedButtonBadHealth();
    afx_msg LRESULT OnMessageUserConditionLogsResponse(WPARAM wParam, LPARAM lParam);

private:
    CMFCButton m_ButtonGoodHealth;
    CMFCButton m_ButtonNormalHealth;
    CMFCButton m_ButtonBadHealth;
    CStatic m_StaticImage;
    CBitmap m_BitMapCleanup;
    CFont   m_NewFont;
    LanguageResourceInterface* m_LanguageResource;
    CStatic m_FeelingQuestion;

    void ApplyLanguageResource();
    void ApplyPositionDialogAndControl();
    void ApplyStyle();
    void ApplyButtonStyle(CMFCButton* button);
};
