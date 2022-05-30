
#include "stdafx.h"
#include "WMMainWindow.h"
#include <WindowsHook.h>
#include "WMMessage.h"

WMMainWindow::WMMainWindow()
{
	m_WindowsHook = std::make_unique<::WindowsHook>();
}

bool WMMainWindow::Init()
{
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
	return 0;
}

LRESULT WMMainWindow::OnMessageMouse(WPARAM wParam, LPARAM lParam)
{
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
