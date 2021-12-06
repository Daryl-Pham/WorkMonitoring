// RpcClient.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <rpc.h>
#include "Control_h.h"
#include <process.h>

#pragma comment(lib, "RpcRT4.lib")

void __RPC_FAR* __RPC_API midl_user_allocate(size_t len) {
    return malloc(len);
}

void __RPC_API midl_user_free(void __RPC_FAR* ptr) {
    free(ptr);
}

int main()
{
    wprintf(L"Client PID %d\n", _getpid());

    RPC_STATUS Status = RPC_S_OK;
    RPC_WSTR Protocol = ((RPC_WSTR)L"ncalrpc");
    unsigned int MaxInstance = RPC_C_LISTEN_MAX_CALLS_DEFAULT;
    RPC_WSTR Endpoint = (RPC_WSTR)L"mypipes";

    RPC_WSTR Binding = NULL;

    Status = RpcStringBindingCompose(
        NULL, Protocol, NULL, Endpoint, NULL, &Binding);
    if (Status != RPC_S_OK)
    {
        wprintf(L"RpcStringBindingCompose failed (0x%08x)\n", Status);
        return Status;
    }

    Status = RpcBindingFromStringBinding(Binding, &control_IfHandle);
    if (Status != RPC_S_OK)
    {
        wprintf(L"RpcBindingFromStringBinding failed (0x%08x)\n", Status);
        return Status;
    }

    RpcTryExcept
    {
        Hello(L"hoge");
    }
    RpcExcept(EXCEPTION_EXECUTE_HANDLER)
    {
        printf("Caught exception: 0x%08x\n", RpcExceptionCode());
    }
    RpcEndExcept

        RpcTryExcept
    {
        wchar_t dst[1024] = { 0 };
        Hello2(dst);
        wprintf(L"Hello2() returns: %s", dst);
    }
        RpcExcept(EXCEPTION_EXECUTE_HANDLER)
    {
        printf("Caught exception: 0x%08x\n", RpcExceptionCode());
    }
    RpcEndExcept

    Status = RpcStringFree(&Binding);
    if (Status != RPC_S_OK)
    {
        wprintf(L"RpcStringFree failed (0x%08x)\n", Status);
    }

    Status = RpcBindingFree(&control_IfHandle);
    if (Status != RPC_S_OK)
    {
        wprintf(L"RpcBindingFree failed (0x%08x)\n", Status);
    }

    Sleep(20 * 1000);

    return 0;
}
