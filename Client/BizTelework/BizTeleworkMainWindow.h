
// BizTeleworkDlg.h : header file
//

#pragma once

#include <memory>
#include <string>
#include "HttpStatusCode.h"
#include "BizTeleworkMainWindowDependency.h"
#include "LastActivityLog.h"
#include "Request.h"
#pragma comment(lib, "Wtsapi32.lib")

enum class WhereCallShowDialog
{
    NOTIFY_ICON,
    ON_COPY_DATA,
    SESSION_UNLOCK,
    ON_TIMER,
    ON_INIT,
    ON_TOAST_NOTIFICATION_HANDLER,
    ON_DEAUTHENTICATION,
};

class BizTeleworkMainWindow : public CWnd
{
// Construction
public:
    explicit BizTeleworkMainWindow(const BizTeleworkMainWindowDependency& dependency);

    bool Init(const BizTeleworkMainWindowDependency& dependency);
    std::wstring GetApiUrl() const;


// Implementation
protected:

    // Generated message map functions
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg LRESULT OnMessageKeyboard(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMessageMouse(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMessageNotifyIcon(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
    afx_msg LRESULT OnMessageKeyboardAndMouseHook(WPARAM wParam = NULL, LPARAM lParam = NULL);
    afx_msg LRESULT OnMessageContextMenu(WPARAM wParam = NULL, LPARAM lParam = NULL);
    afx_msg LRESULT OnSetShowOnDutyDialogRemindTimer(WPARAM wParam = NULL, LPARAM lParam = NULL);
    afx_msg LRESULT OnRegisterSessionNotification(WPARAM wParam = NULL, LPARAM lParam = NULL);
    afx_msg LRESULT OnWtsSessionChange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMessageToastNotificationHandler(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

    bool ShowNotifyIcon(const BizTeleworkMainWindowDependency &dependency);
    void HideNotifyIcon();

private:
    void SendLogIfNeccessary(const boost::chrono::system_clock::time_point tp,
                             const bool isKeyboardLog,
                             const bool isMouseLog);
    void ShowDialog(WhereCallShowDialog whereCall);

    void SetGettingActiveAppTimer();
    BOOL KillGettingActiveAppTimer();
    void WriteCurrentActiveAppLog();
    BOOL SetKeyboardAndMouseHook();
    BOOL UnSetKeyboardAndMouseHook();
    bool SetContextMenu();
    bool UnSetContextMenu();
    void ProcessMenuItemSelected(const UINT itemSelected);
    bool SetMenuDeleteCredential(UINT* itemSelected);
    void DeleteCredential();
    BOOL SetShowOnDutyDialogRemindTimer();
    BOOL KillShowOnDutyDialogRemindTimer();
    bool IsShowOnDutyDialogRemindFromTimer(const int64_t unixStartedTime);
    bool UnRegisterSessionNotification();
    bool IsShowHealthStatusDialogRemindFromUnlocking();
    bool BringWindowToTop();
    bool CopyAuthenticationToCurrentUser();
    void SetTimeDisplayHealthStatusRemindToCurrentUser();
    bool SetToastNotificationHandler();
    bool ShowToastNotification(const std::wstring& title, const std::wstring& content);

    HICON m_hIcon;
    std::unique_ptr<NotifyIconInterface> m_NotifyIcon;
    std::unique_ptr<WindowsHookInterface> m_WindowsHook;
    const std::wstring m_ApiUrl;
    const LastActivityLog m_LastActivityLog;
    UINT_PTR m_GettingActiveAppTimer = 0;
    std::unique_ptr<ContextMenuInterface> m_ContextMenu;
    UINT_PTR m_ShowOnDutyDialogRemindTimer = 0;
    std::unique_ptr<Request> m_Request;
    std::unique_ptr<AppConfigInterface> m_AppConfig;
    std::unique_ptr<AuthenHelper> m_AuthenHelper;
    std::unique_ptr<ToastNotificationHandlerInterface> m_ToastNotificationHandler;
};
