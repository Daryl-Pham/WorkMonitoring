
#include "stdafx.h"
#include "WMMainWindow.h"
#include <WindowsHook.h>
#include "WMMessage.h"
#include <chrono>
#include "WritingData.h"

static const int64_t DURATION_SECONDS_TO_WRITE_LOG = 300;
LPCTSTR WINDOW_NAME = _T("BizTelework");

WMMainWindow::WMMainWindow()
{
	m_pWindowsHook = std::make_unique<::WindowsHook>();
	m_nLastTimeWriteMouseOperate = m_nLastTimeWriteKeyboardOperate
		= std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	m_pWritingData = std::make_unique<WritingData>();
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
	(void)lParam;  // Unused

	LRESULT result = ERROR_SUCCESS;

	int64_t writingUnixTime = (int64_t)wParam;
	auto timePoint = std::chrono::system_clock::from_time_t(writingUnixTime);

	if (result = m_pWritingData->WriteData(KEYBOARD_OPERATE, timePoint))
	{
		m_nLastTimeWriteKeyboardOperate = writingUnixTime;
	}

	return result;
}

LRESULT WMMainWindow::OnMessageMouse(WPARAM wParam, LPARAM lParam)
{
	(void)lParam;  // Unused

	LRESULT result = ERROR_SUCCESS;

	int64_t writingUnixTime = (int64_t)wParam;
	auto timePoint = std::chrono::system_clock::from_time_t(writingUnixTime);

	if (result = m_pWritingData->WriteData(MOUSE_OPERATE, timePoint))
	{
		m_nLastTimeWriteMouseOperate = writingUnixTime;
	}

	return result;
}

LRESULT WMMainWindow::OnMessageKeyboardAndMouseHook(WPARAM wParam, LPARAM lParam)
{
	(void)lParam;  // Unused

	BOOL isSetHook = static_cast<BOOL>(wParam);

	if (isSetHook)
	{
		return SetKeyboardAndMouseHook();
	}
	else
	{
		return UnSetKeyboardAndMouseHook();
	}
}

BOOL WMMainWindow::SetKeyboardAndMouseHook()
{
	HWND hWnd = GetSafeHwnd();
	if (hWnd == NULL)
	{
		return FALSE;
	}

	if (!m_pWindowsHook->SetHookForKeyboard(hWnd, WM_MESSAGE_KEYBOARD))
	{

	}

	if (!m_pWindowsHook->SetHookForMouse(hWnd, WM_MESSAGE_MOUSE))
	{

	}

	return TRUE;
}

BOOL WMMainWindow::UnSetKeyboardAndMouseHook()
{
	if (!m_pWindowsHook)
	{
		return FALSE;
	}

	if (!m_pWindowsHook->UnsetHookForKeyboard())
	{
		
	}

	if (!m_pWindowsHook->UnsetHookForMouse())
	{
		
	}

	return TRUE;
}
