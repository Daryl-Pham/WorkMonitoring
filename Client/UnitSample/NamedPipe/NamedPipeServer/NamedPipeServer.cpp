#include <Windows.h>
#include <stdio.h>

// Copy https://s-kita.hatenablog.com/entry/20100216/1266318458

int main()
{
    HANDLE hPipe;
    wchar_t szBuff[256] = { 0 };
    DWORD dwNumberOfBytesRead;

    hPipe = CreateNamedPipe(L"\\\\.\\pipe\\mypipe", //lpName
	PIPE_ACCESS_DUPLEX,		       //dwOpenMode
	PIPE_TYPE_BYTE | PIPE_WAIT,	       //dwPipeMode
	3,				       //nMaxInstances
	0,				       //nOutBufferSize
	0,				       //nInBufferSize
	100,				       //nDefaultTimeOut
	NULL);				       //lpSecurityAttributes

    if (hPipe == INVALID_HANDLE_VALUE) {
	return 1;
    }

    if (!ConnectNamedPipe(hPipe, NULL)) {
	CloseHandle(hPipe);
	return 1;
    }

    while (1) {
	if (!ReadFile(hPipe,
	    szBuff,
	    sizeof(szBuff),
	    &dwNumberOfBytesRead, NULL)) {
	    break;
	}
	szBuff[dwNumberOfBytesRead] = L'\0';
	wprintf(L"PipeServer: %s", szBuff);
    }

    FlushFileBuffers(hPipe);

    DisconnectNamedPipe(hPipe);

    CloseHandle(hPipe);

    return 0;
}
