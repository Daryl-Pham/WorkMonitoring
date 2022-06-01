#include "WMHook.h"
#include <chrono>
#include "SafeQueue.h"

#pragma data_seg("BTH_DATA")
HHOOK bthKeyboardHook = NULL;
HHOOK bthMouseHook = NULL;
HWND bthWndKeyboard = NULL;
HWND bthWndMouse = NULL;
UINT bthMsgKeyboard = NULL;
UINT bthMsgMouse = NULL;
HINSTANCE hDllInst = NULL;
#pragma data_seg()

struct MessageOperate
{
	int typeOperate = -1;
	int64_t timeOperate = 0;
	WPARAM wParamOperate = 0;
	LPARAM lParamOperate = 0;

	MessageOperate(int type, int64_t time, WPARAM wParam, LPARAM lParam)
	{
		typeOperate = type;
		timeOperate = time;
		wParamOperate = wParam;
		lParamOperate = lParam;
	}
};

static int64_t timePointLastKeyboardOperate = 0;
static int64_t timePointLastMouseOperate = 0;
static const int DURATION_SECONDS_TO_WRITE_LOG = 3;
SafeQueue<MessageOperate> messageQueue;
UINT_PTR timerID = 0;

VOID CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	MessageOperate item = messageQueue.dequeue();
	if (item.typeOperate != -1)
	{
		HWND handle = item.typeOperate == 0 ? bthWndKeyboard : bthWndMouse;
		UINT msg = item.typeOperate == 0 ? bthMsgKeyboard : bthMsgMouse;
		PostMessage(handle, msg, item.wParamOperate, item.lParamOperate);
	}
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wp, LPARAM lp)
{
	int64_t now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	int64_t periodTime = now - timePointLastKeyboardOperate;

	if ((nCode >= 0) && bthWndKeyboard && bthMsgKeyboard && periodTime >= DURATION_SECONDS_TO_WRITE_LOG)
	{
		timePointLastKeyboardOperate = now;
		messageQueue.enqueue(MessageOperate(0, now, wp, lp));
		//Add to queue, then from queue post message
		//PostMessage(bthWndKeyboard, bthMsgKeyboard, wp, lp);  // TODO(ichino) Logging if failed.
	}

	return CallNextHookEx(bthKeyboardHook, nCode, wp, lp);
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wp, LPARAM lp)
{
	int64_t now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	int64_t periodTime = now - timePointLastMouseOperate;

	if ((nCode >= 0) && bthWndMouse && bthMsgMouse && periodTime >= DURATION_SECONDS_TO_WRITE_LOG)
	{
		timePointLastMouseOperate = now;
		messageQueue.enqueue(MessageOperate(1, now, wp, lp));
		//Add to queue, then from queue post message
		//PostMessage(bthWndMouse, bthMsgMouse, wp, lp);  // TODO(ichino) Logging if failed.
	}

	return CallNextHookEx(bthMouseHook, nCode, wp, lp);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	(void)lpReserved;  // Unused

	timerID = SetTimer(NULL, 10010, 1000, (TIMERPROC)&TimerProc);

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
	if (timerID)
	{
		KillTimer(NULL, timerID);
	}

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
	if (timerID)
	{
		KillTimer(NULL, timerID);
	}

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
