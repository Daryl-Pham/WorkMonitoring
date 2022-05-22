#include "WMHook.h"

#pragma data_seg("BTH_DATA")
HHOOK bthKeyboardHook = NULL;
HHOOK bthMouseHook = NULL;
HWND bthWndKeyboard = NULL;
HWND bthWndMouse = NULL;
UINT bthMsgKeyboard = NULL;
UINT bthMsgMouse = NULL;
HINSTANCE hDllInst = NULL;
#pragma data_seg()

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wp, LPARAM lp)
{
	if ((nCode >= 0) && bthWndKeyboard && bthMsgKeyboard)
	{
		PostMessage(bthWndKeyboard, bthMsgKeyboard, wp, lp);  // TODO(ichino) Logging if failed.
	}

	return CallNextHookEx(bthKeyboardHook, nCode, wp, lp);
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wp, LPARAM lp)
{
	if ((nCode >= 0) && bthWndMouse && bthMsgMouse)
	{
		PostMessage(bthWndMouse, bthMsgMouse, wp, lp);  // TODO(ichino) Logging if failed.
	}

	return CallNextHookEx(bthMouseHook, nCode, wp, lp);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	(void)lpReserved;  // Unused

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// LOG_OUTPUT("Process Attached")
		hDllInst = hModule;
		break;
	case DLL_PROCESS_DETACH:
		// LOG_OUTPUT("Process Detached")
		hDllInst = NULL;
		break;
	default:
		break;
	}
	return TRUE;
}

BOOL BthSetHookForKeyboard(HWND hWnd, UINT keyboardMessage)
{
	bthKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, hDllInst, 0);
	if (bthKeyboardHook == NULL)
	{
		// LOG_ERROR("keyboard, SetWindowsHookEx() failed. 0x%08d", GetLastError())
		return FALSE;
	}

	// LOG_DEBUG("Succeeded to set hook for keyboard.")
	bthWndKeyboard = hWnd;
	bthMsgKeyboard = keyboardMessage;

	return TRUE;
}

BOOL BthSetHookForMouse(HWND hWnd, UINT mouseMessage)
{
	bthMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, hDllInst, 0);
	if (bthMouseHook == NULL)
	{
		// LOG_ERROR("mouse, SetWindowsHookEx() failed. 0x%08d", GetLastError())
		return FALSE;
	}

	// LOG_DEBUG("Succeeded to set hook for mouse.")
	bthWndMouse = hWnd;
	bthMsgMouse = mouseMessage;

	return TRUE;
}

BOOL BthUnsetHookForKeyboard()
{
	bthWndKeyboard = NULL;
	bthMsgKeyboard = 0;

	if (!UnhookWindowsHookEx(bthKeyboardHook))
	{
		// LOG_ERROR("keyboard, SetWindowsHookEx() failed. 0x%08d", GetLastError())
		bthKeyboardHook = NULL;

		return FALSE;
	}

	// LOG_DEBUG("Succeeded to un-set hook for keyboard.")
	bthKeyboardHook = NULL;

	return TRUE;
}

BOOL BthUnsetHookForMouse()
{
	bthWndMouse = NULL;
	bthMsgMouse = 0;

	if (!UnhookWindowsHookEx(bthMouseHook))
	{
		// LOG_ERROR("mouse, SetWindowsHookEx() failed. 0x%08d", GetLastError())
		bthMouseHook = NULL;

		return FALSE;
	}

	// LOG_DEBUG("Succeeded to un-set hook for mouse.")
	bthMouseHook = NULL;

	return TRUE;
}
