#pragma once

#include <afxwin.h>
#include <memory>

class WindowsHookInterface;

class WMMainWindow : public CWnd
{
public:
	explicit WMMainWindow();
	bool Init();

protected:
	// Generated message map functions
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnMessageKeyboard(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageMouse(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageKeyboardAndMouseHook(WPARAM wParam = NULL, LPARAM lParam = NULL);

	DECLARE_MESSAGE_MAP()

private:
	std::unique_ptr<WindowsHookInterface> m_WindowsHook;
	int64_t m_LastTimeWriteKeyboardOperate;
	int64_t m_LastTimeWriteMouseOperate;

	BOOL SetKeyboardAndMouseHook();
	BOOL UnSetKeyboardAndMouseHook();
};
