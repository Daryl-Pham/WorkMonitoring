#pragma once
#include "stdafx.h"
#include <memory>
#include "BizTeleworkDialogDependency.h"
#include <ActiveInformation.h>
#include <LanguageResourceInterface.h>
#include <LanguageResource.h>
#include "BizTeleworkColor.h"
#include "AuthenHelper.h"
#include "Common.h"
#include "Environment.h"

class CVEdit : public CEdit
{
    // Construction
public:
    CVEdit() = default;
    virtual ~CVEdit() = default;

private:
    CRect m_rectNCBottom = CRect(0, 0, 0, 0);
    CRect m_rectNCTop = CRect(0, 0, 0, 0);

    // Generated message map functions
protected:
    // {{AFX_MSG(CVEdit)
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
    afx_msg void OnNcPaint();
    // }}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

class CURLStatic : public CStatic
{
public:
    CURLStatic();
    virtual ~CURLStatic();

    virtual void PreSubclassWindow();

protected:
    HCURSOR m_Cursor;
    CFont   m_Font;

    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg LRESULT OnNcHitTest(CPoint point);

    DECLARE_MESSAGE_MAP()
};

class BizTeleworkAuthenticationDialog : public CDialogEx
{
    DECLARE_DYNAMIC(BizTeleworkAuthenticationDialog)

public:
    explicit BizTeleworkAuthenticationDialog(const BizTeleworkDialogDependency& dependency,
                                             CWnd* pParent,
                                             const ActiveInformation& activeInformation);
    virtual ~BizTeleworkAuthenticationDialog();
    bool IsChangeAPIURL() const;

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_BIZTELEWORK_AUTHENTICATION_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    DECLARE_MESSAGE_MAP()

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedClose();
    afx_msg void OnBnClickedButtonLogin();
    afx_msg void OnBnClickedEnterURL();
    afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
    virtual void OnDestroy();

private:
    CMFCButton m_ButtonLogin;
    CStatic m_StaticError;
    CStatic m_StaticAuthSuccess;
    CStatic m_StaticImage;
    CURLStatic m_Close;
    CURLStatic m_StaticTextEnterURL;
    CURLStatic m_ExpandMore;
    CURLStatic m_ExpandLess;
    CVEdit m_EditCompanyCode;
    CVEdit m_EditActiveCode;
    CVEdit m_EditAPIURL;
    CBitmap m_BitMapClose;
    CBitmap m_BitMapCleanupLogo;
    CBitmap m_BitMapCleanupExpandMore;
    CBitmap m_BitMapCleanupExpandLess;
    CFont   m_NewFont;
    LanguageResourceInterface* m_LanguageResource;
    HBRUSH m_BrBack = CreateSolidBrush(BizTeleworkColor::InputBackground());
    BOOL m_IsChangeAPIURL = FALSE;
    ActiveInformation m_ActivationInformation;
    ReleaseEnvironment m_ReleaseEnvironment = ReleaseEnvironment::DEVELOPMENT;
    CStatic m_StaticAuthExplanation;

    void ApplyLanguageResource();
    void ApplyPositionDialogAndControl(const BOOL& isHaveUpdateURL);
    void ApplyPositionDialogAndControlMore();
    void ApplyPositionDialogAndControlLess();
    void ApplyStyle();
    void ApplyButtonStyle(CMFCButton* button);
    void ApplyInputTextStyle(CEdit* inputText);
    void ApplyLanguageResourceLabelEnterURL();
    void ApplyStaticEnterURLStyle(CStatic* labelEnterURL);
    void ApplyStaticExplanationStyle(CStatic* labelExplanation);
    void ApplyErrorTextStyle(CStatic* error);
    void ApplAuthSuccessTextStyle(CStatic* success);
    void ShowError(const std::wstring& errorText);
    void ShowAuthSuccess();
    bool StartRequestGetAuthenticateInforAndControlItems(const std::wstring& companyCode,
                                                         const std::wstring& activationCode);
    bool SaveAuthenticationInformation(const AGENT_INFOR& agentInfor);
    void InitInputText();
    void GetReleaseEnvironment();
    bool ShouldAuthenNow() const;
    bool HasActivationInformation() const;
    void SaveInputFieldsToTemporaryMemory() const;
    void RestoreImputFieldsFromTemporaryMemory();
    void SetKeyboardAndMouseHook();
    void SetContextMenu();
    void SetShowOnDutyDialogRemindTimer();
    void RegisterSessionNotification();
};
