#include "stdafx.h"
#include "BizTeleworkUpdaterRpcClient.h"
#include <Log.h>
#include <cstdlib>
#include "AppConfig.h"
#include "BizTeleworkUpdaterRpc_h.h"
#pragma comment(lib, "Rpcrt4.lib")

namespace
{
    DEFINE_LOGGER_FILE_NAME;

namespace rpc
{
    HRESULT PollFetchingTask(
        wchar_t url[], unsigned int urlLen,
        wchar_t dstFilePath[], unsigned int dstFilePathLen)
    {
        RpcTryExcept
        {
            return BizTeleworkUpdaterRpc_PollFetchingTask(url, urlLen, dstFilePath, dstFilePathLen);
        }
            RpcExcept(EXCEPTION_EXECUTE_HANDLER)
        {
            LOG_ERROR("Caught exception: 0x%08x", RpcExceptionCode());
            return E_FAIL;
        }
        RpcEndExcept
    }

    void PushFetchingResult(boolean succeeded)
    {
        RpcTryExcept
        {
            BizTeleworkUpdaterRpc_PushFetchingResult(succeeded);
        }
            RpcExcept(EXCEPTION_EXECUTE_HANDLER)
        {
            LOG_ERROR("Caught exception: 0x%08x", RpcExceptionCode());
        }
        RpcEndExcept
    }
}  // namespace rpc
}  // namespace

// vvv Implementation of RPC vvv

void __RPC_FAR* __RPC_API midl_user_allocate(size_t len)
{
    return malloc(len);
}

void __RPC_API midl_user_free(void __RPC_FAR* ptr)
{
    free(ptr);
}

// ^^^ Implementation of RPC ^^^

bool BizTeleworkUpdaterRpcClient::Bind()
{
    RPC_STATUS status = RPC_S_OK;
    RPC_WSTR protocol = ((RPC_WSTR)L"ncalrpc");
    unsigned int maxInstance = RPC_C_LISTEN_MAX_CALLS_DEFAULT;
    RPC_WSTR binding = nullptr;

    std::wstring endpoint;
    AppConfig appConfig;
    if (!appConfig.GetBizTeleworkUpdaterRPCEndpoint(&endpoint))
    {
        LOG_ERROR("appConfig.GetBizTeleworkUpdaterRPCEndpoint() failed.");
        return false;
    }

    status = RpcStringBindingCompose(
        NULL, protocol, NULL, (RPC_WSTR)endpoint.c_str(), NULL, &binding);
    if (status != RPC_S_OK)
    {
        LOG_ERROR("RpcStringBindingCompose() failed. 0x%08x", status);
        return false;
    }

    status = RpcBindingFromStringBinding(binding, &BizTeleworkUpdaterRpc_IfHandle);
    if (status != RPC_S_OK)
    {
        LOG_ERROR("RpcBindingFromStringBinding() failed. (0x%08x)", status);
        RpcStringFree(&binding);
        return false;
    }

    m_Binding = binding;
    return true;
}

void BizTeleworkUpdaterRpcClient::Unbind()
{
    RPC_STATUS status = RPC_S_OK;

    status = RpcStringFree(&m_Binding);
    if (status != RPC_S_OK)
    {
        LOG_ERROR("RpcStringFree(). failed 0x%08x", status);
    }
    m_Binding = nullptr;

    status = RpcBindingFree(&BizTeleworkUpdaterRpc_IfHandle);
    if (status != RPC_S_OK)
    {
        LOG_ERROR("RpcBindingFree failed (0x%08x)", status);
    }
}

HRESULT BizTeleworkUpdaterRpcClient::PollFetchingTask(std::wstring* url, std::wstring* dstFilePath)
{
    if (!url)
    {
        LOG_ERROR("The url must not be null.");
        return E_FAIL;
    }
    if (!dstFilePath)
    {
        LOG_ERROR("The dstFilePath must not be null.");
        return E_FAIL;
    }

    try
    {
        HRESULT hr = S_OK;
        std::wstring u(4 * 1024, L'\0');
        std::wstring dfp(4 * 1024, L'\0');

        hr = rpc::PollFetchingTask(&u[0], u.size(), &dfp[0], dfp.size());

        if (hr == S_OK)
        {
            *url = u;
            *dstFilePath = dfp;
        }

        return hr;
    }
    catch (const std::bad_alloc& e)
    {
        LOG_ERROR("Failed by std::bad_alloc %s", e.what());
        return E_FAIL;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return E_FAIL;
    }
}

void BizTeleworkUpdaterRpcClient::PushFetchingResult(bool succeeded)
{
    boolean s = succeeded ? TRUE : FALSE;
    rpc::PushFetchingResult(s);
}
