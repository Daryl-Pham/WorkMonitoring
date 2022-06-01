
#include "stdafx.h"
#include "WMMainWindow.h"
#include <WindowsHook.h>
#include "WMMessage.h"
#include <chrono>

static const int64_t DURATION_SECONDS_TO_WRITE_LOG = 300;
LPCTSTR WINDOW_NAME = _T("BizTelework");

WMMainWindow::WMMainWindow()
{
	m_WindowsHook = std::make_unique<::WindowsHook>();
	m_LastTimeWriteMouseOperate = m_LastTimeWriteKeyboardOperate
		= std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

bool WMMainWindow::Init()
{
	CString wcn = ::AfxRegisterWndClass(NULL);
	if (!this->CreateEx(0, wcn, WINDOW_NAME, 0, 0, 0, 0, 0, HWND_MESSAGE, 0))
	{
		//LOG_WARN("CreateEx() failed.");
	}

	SetKeyboardAndMouseHook();
	return true;
}

BEGIN_MESSAGE_MAP(WMMainWindow, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_MESSAGE_KEYBOARD, &WMMainWindow::OnMessageKeyboard)
	ON_MESSAGE(WM_MESSAGE_MOUSE, &WMMainWindow::OnMessageMouse)
END_MESSAGE_MAP()

int WMMainWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}

void WMMainWindow::OnDestroy()
{

}

LRESULT WMMainWindow::OnMessageKeyboard(WPARAM wParam, LPARAM lParam)
{
	(void)wParam;  // Unused
	(void)lParam;  // Unused

	//khanhpqtest - Check exist log files or not

	auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if (now - m_LastTimeWriteKeyboardOperate >= DURATION_SECONDS_TO_WRITE_LOG)
	{
		//khanhpqtest - write log to file
	}

	return 0;
}

LRESULT WMMainWindow::OnMessageMouse(WPARAM wParam, LPARAM lParam)
{
	(void)wParam;  // Unused
	(void)lParam;  // Unused



	return 0;
}

LRESULT WMMainWindow::OnMessageKeyboardAndMouseHook(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BOOL WMMainWindow::SetKeyboardAndMouseHook()
{
	HWND hWnd = GetSafeHwnd();
	if (hWnd == NULL)
	{
		return FALSE;
	}

	if (!m_WindowsHook->SetHookForKeyboard(hWnd, WM_MESSAGE_KEYBOARD))
	{

	}

	if (!m_WindowsHook->SetHookForMouse(hWnd, WM_MESSAGE_MOUSE))
	{

	}

	return TRUE;
}

BOOL WMMainWindow::UnSetKeyboardAndMouseHook()
{
	if (!m_WindowsHook)
	{
		return FALSE;
	}

	if (!m_WindowsHook->UnsetHookForKeyboard())
	{
		
	}

	if (!m_WindowsHook->UnsetHookForMouse())
	{
		
	}

	return TRUE;
}
