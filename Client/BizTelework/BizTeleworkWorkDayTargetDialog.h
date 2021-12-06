#pragma once
#include "stdafx.h"
#include <memory>
#include "BizTeleworkColor.h"
#include "BizTeleworkDialogDependency.h"
#include <LanguageResourceInterface.h>
#include <LanguageResource.h>
#include <Request.h>

class BizTeleworkWorkDayTargetDialog : public CDialogEx
{
    DECLARE_DYNAMIC(BizTeleworkWorkDayTargetDialog)

    explicit BizTeleworkWorkDayTargetDialog(const BizTeleworkDialogDependency& dependency, CWnd* pParent);
    ~BizTeleworkWorkDayTargetDialog() override = default;

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_BIZTELEWORK_WORK_DAY_TARGET_DIALOG };
#endif

public:
    BOOL PreTranslateMessage(MSG* pMsg) override;

protected:
    BOOL OnInitDialog() override;
    void DoDataExchange(CDataExchange* pDX) override;

    DECLARE_MESSAGE_MAP()

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnBnClickedButtonSendWorkTarget();
    afx_msg LRESULT OnMessageWorkTargetResponse(WPARAM wParam, LPARAM lParam);

private:
    CMFCButton m_ButtonSendWorkTarget;
    CStatic m_StaticImage;
    CStatic m_StaticDoWhat;
    CBitmap m_BitMapCleanup;
    CFont   m_NewFont;
    LanguageResourceInterface* m_LanguageResource;
    CRichEditCtrl m_EditWorkDayTarget;
    HBRUSH m_BrBack = CreateSolidBrush(BizTeleworkColor::InputWorkDayTargetBackground());

    void ApplyLanguageResource();
    void ApplyPositionDialogAndControl();
    void ApplyStyle();
    void ApplyButtonStyle(CMFCButton* button);
    void ApplyEditWorkTargetStyle(const CRichEditCtrl* edit) const;
    void InitTargetText();
    bool SaveTargetToTemporaryMemory() const;
    void RestoreTargetFromTemporaryMemory();
};
