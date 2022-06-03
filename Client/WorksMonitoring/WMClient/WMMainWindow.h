#pragma once

#include <afxwin.h>
#include <memory>

class WindowsHookInterface;
class WritingData;

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
	std::unique_ptr<WindowsHookInterface> m_pWindowsHook;
	int64_t m_nLastTimeWriteKeyboardOperate;
	int64_t m_nLastTimeWriteMouseOperate;
	std::unique_ptr<WritingData> m_pWritingData;

	BOOL SetKeyboardAndMouseHook();
	BOOL UnSetKeyboardAndMouseHook();
};
