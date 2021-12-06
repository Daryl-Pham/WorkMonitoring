#include <string.h>
#include <stdio.h>
#include <Windows.h>

// Copy https://s-kita.hatenablog.com/entry/20100216/1266318458

int main()
{
    HANDLE hPipe;
    wchar_t szBuff[32] = { 0 };
    DWORD dwNumberOfBytesRead;
    DWORD dwNumberofBytesWritten;

    hPipe = CreateFile(L"\\\\.\\pipe\\mypipe",
	GENERIC_READ | GENERIC_WRITE,
	0,
	NULL,
	OPEN_EXISTING,
	FILE_ATTRIBUTE_NORMAL,
	NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
	return 1;
    }

    while (1) {
	fgetws(szBuff, sizeof(szBuff) / sizeof(szBuff[0]), stdin);
	if (!WriteFile(hPipe, szBuff, wcslen(szBuff), &dwNumberofBytesWritten, NULL)) {
	    break;
	}
    }

    CloseHandle(hPipe);

    return 0;
}