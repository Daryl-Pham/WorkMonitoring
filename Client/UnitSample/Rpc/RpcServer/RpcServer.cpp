// RpcServer.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <Psapi.h>
#include <process.h>
#include <rpc.h>
#include "Control_h.h"

#pragma comment(lib, "RpcRT4.lib")

// Reference https://msmania.wordpress.com/2011/07/10/win32-c-local-rpc-over-named-pipe-and-lpc/

void Hello(
    handle_t IDL_handle,
    const wchar_t* String)
{
    std::wcout << L"Hello called: " << String << std::endl;

    RPC_STATUS Status;
    RPC_CALL_ATTRIBUTES CallAttributes = { 0 };
    CallAttributes.Version = RPC_CALL_ATTRIBUTES_VERSION;
    CallAttributes.Flags = RPC_QUERY_CLIENT_PID;
    Status = RpcServerInqCallAttributes(IDL_handle, &CallAttributes);
    if (Status != RPC_S_OK)
    {
        wprintf(L"RpcServerInqCallAttributes() failed (0x%08x)\n", Status);
    }
    else
    {
        DWORD pid = reinterpret_cast<DWORD>(CallAttributes.ClientPID);
        wprintf(L"ClientPID is %d\n", pid);
    }
}

void Hello2(
    /* [in] */ handle_t IDL_handle,
    /* [size_is][string][out][in] */ wchar_t Destination[])
{
    std::wcout << L"Hello2 called: " << std::endl;
    Destination[0] = L'H';
    Destination[1] = L'i';
    Destination[2] = L'!';
    Destination[3] = L'\0';
}

void __RPC_FAR* __RPC_API midl_user_allocate(size_t len)
{
    return malloc(len);
}

void __RPC_API midl_user_free(void __RPC_FAR* ptr)
{
    free(ptr);
}

int main()
{
    wprintf(L"Server PID %d\n", _getpid());

    RPC_STATUS Status = RPC_S_OK;
    RPC_WSTR Protocol = ((RPC_WSTR)L"ncalrpc");
    unsigned int MaxInstance = RPC_C_LISTEN_MAX_CALLS_DEFAULT;
    RPC_WSTR Endpoint = (RPC_WSTR)L"mypipes";

    Status = RpcServerUseProtseqEp(Protocol, MaxInstance, Endpoint, NULL);
    if (Status != RPC_S_OK)
    {
        wprintf(L"RpcServerUseProtseqEp() failed (0x%08x)\n", Status);
        exit(Status);
    }

    Status = RpcServerRegisterIf(Control_v1_0_s_ifspec, NULL, NULL);
    if (Status != RPC_S_OK)
    {
        wprintf(L"RpcServerRegisterIf failed (0x%08x)\n", Status);
        exit(Status);
    }

    Status = RpcServerListen(1, MaxInstance, 0);
    if (Status != RPC_S_OK)
    {
        wprintf(L"RpcServerListen failed (0x%08x)\n", Status);

        Status = RpcServerUnregisterIf(NULL, NULL, FALSE);
        if (Status != RPC_S_OK)
            wprintf(L"RpcServerUnregisterIf failed (0x%08x)\n", Status);

        exit(Status);
    }
}
