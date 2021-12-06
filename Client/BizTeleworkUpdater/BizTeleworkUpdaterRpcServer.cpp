#include "pch.h"
#include "BizTeleworkUpdaterRpcServer.h"
#include "BizTeleworkUpdaterRpc_h.h"
#include "ConfigUtility.h"
#include <Log.h>
#include <mutex>
#include <stdlib.h>
#include <boost/filesystem.hpp>


#pragma comment(lib, "Rpcrt4.lib")

namespace
{
    DEFINE_LOGGER_FILE_NAME;

    const DWORD WAIT_INTERVAL_MILLI_SECOND =   3 * 1000;
    const DWORD WAIT_TIMEOUT_MILLI_SECOND  = 180 * 1000;

    // Queing for one task.
    struct {
        std::recursive_mutex mutex;
        bool empty = true;
        std::wstring url;
        std::wstring dstFilePath;
        bool succeeded = false;
        bool done = false;
    } fetchingTaskContainer;
}  // namespace

// vvv Implementation of RPC vvv

HRESULT BizTeleworkUpdaterRpc_PollFetchingTask(
    /* [size_is][string][out][in] */ wchar_t Url[],
    /* [in] */ unsigned int UrlLen,
    /* [size_is][string][out][in] */ wchar_t DstFilePath[],
    /* [in] */ unsigned int DstFilePathLen)
{
    try
    {
        std::lock_guard<std::recursive_mutex> lock(fetchingTaskContainer.mutex);

        if (fetchingTaskContainer.empty)
        {
            return ERROR_EMPTY;
        }

        if ((UrlLen < fetchingTaskContainer.url.size()) || (DstFilePathLen < fetchingTaskContainer.dstFilePath.size()))
        {
            return E_INVALIDARG;
        }

        errno_t e;
        if ((e = wcscpy_s(Url, UrlLen, fetchingTaskContainer.url.c_str())))
        {
            LOG_WARN("wcscpy_s(Url) failed. %d", e);
            return E_FAIL;
        }
        else if ((e = wcscpy_s(DstFilePath, DstFilePathLen, fetchingTaskContainer.dstFilePath.c_str())))
        {
            LOG_WARN("wcscpy_s(DstFilePathLen) failed. %d", e);
            return E_FAIL;
        }

        fetchingTaskContainer.url.clear();
        fetchingTaskContainer.dstFilePath.clear();
        fetchingTaskContainer.empty = true;

        return S_OK;
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


void BizTeleworkUpdaterRpc_PushFetchingResult(
    /* [in] */ boolean succeeded)
{
    try
    {
        std::lock_guard<std::recursive_mutex> lock(fetchingTaskContainer.mutex);

        fetchingTaskContainer.succeeded = (succeeded == TRUE) ? true : false;
        fetchingTaskContainer.done = true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());

        // Fallbacking. It may be succeeded to set values.
        fetchingTaskContainer.succeeded = (succeeded == TRUE) ? true : false;
        fetchingTaskContainer.done = true;
    }
}

void __RPC_FAR* __RPC_API midl_user_allocate(size_t len)
{
    return malloc(len);
}

void __RPC_API midl_user_free(void __RPC_FAR* ptr)
{
    free(ptr);
}

// ^^^ Implementation of RPC ^^^

void BizTeleworkUpdaterRpcServer::Start()
{
    if (m_ServerThread)
    {
        Shutdown();
    }

    m_ServerThread.reset(new std::thread([this]
        {
            RPC_STATUS status = RPC_S_OK;
            RPC_WSTR protocol = ((RPC_WSTR)L"ncalrpc");
            unsigned int maxInstance = RPC_C_LISTEN_MAX_CALLS_DEFAULT;
            std::wstring endpoint = ConfigUtility::GetBizTeleworkUpdaterRPCEndpoint();

            LOG_INFO("RPC server is starting.");

            if (endpoint.empty())
            {
                LOG_WARN("The endpoint must not be empty.");
                return;
            }

            status = RpcServerUseProtseqEp(protocol, maxInstance, (RPC_WSTR)endpoint.c_str(), NULL);
            if (status != RPC_S_OK)
            {
                LOG_WARN("RpcServerUseProtseqEp() failed. 0x%08x", status);
                return;
            }

            status = RpcServerRegisterIf(BizTeleworkUpdaterRpc_v1_0_s_ifspec, NULL, NULL);
            if (status != RPC_S_OK)
            {
                LOG_WARN("RpcServerRegisterIf() failed. 0x%08x", status);
                return;
            }

            LOG_INFO("RPC server is listening.");
            status = RpcServerListen(1, maxInstance, 0);
            if (status != RPC_S_OK)
            {
                LOG_INFO("RpcServerListen() return 0x%08x", status);

                status = RpcServerUnregisterIf(NULL, NULL, FALSE);
                if (status != RPC_S_OK)
                {
                    LOG_WARN("RpcServerUnregisterIf() failed. 0x%08x", status);
                }
            }

            LOG_INFO("Done.");
        }));
}

void BizTeleworkUpdaterRpcServer::Shutdown()
{
    if (m_ServerThread)
    {
        try
        {
            RPC_STATUS status = RpcMgmtStopServerListening(NULL);
            if (status != RPC_S_OK)
            {
                LOG_WARN("RpcMgmtStopServerListening() failed. %d", status);
            }

            LOG_INFO("Joining the m_ServerThread...");
            m_ServerThread->join();
            LOG_INFO("The m_ServerThread is joined.");

            m_ServerThread = nullptr;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Failed by std::exception %s", e.what());
        }
    }
}

// For FetchingTaskInterface
bool BizTeleworkUpdaterRpcServer::RequestFile(const std::wstring& url, const std::wstring& dstFilePath, HANDLE hEventStopControl)
{
    LOG_INFO("RequestFile() called.");

    try
    {
        {
            std::lock_guard<std::recursive_mutex> lock(fetchingTaskContainer.mutex);

            // This removing code need for resolving file access permission.
            // If the user process does not have write permission, it will be failed to over-write dstFilePath.
            if (boost::filesystem::exists(dstFilePath) && !boost::filesystem::remove(dstFilePath))
            {
                LOG_WARN("boost::filesystem::remove() failed.");
            }

            fetchingTaskContainer.url = url;
            fetchingTaskContainer.dstFilePath = dstFilePath;
            fetchingTaskContainer.empty = false;
            fetchingTaskContainer.succeeded = false;
            fetchingTaskContainer.done = false;
        }

        DWORD elapsed = 0;
        while ((!fetchingTaskContainer.done) && (elapsed < WAIT_TIMEOUT_MILLI_SECOND))
        {
            DWORD r = WaitForSingleObject(hEventStopControl, WAIT_INTERVAL_MILLI_SECOND);
            if (r == WAIT_OBJECT_0)
            {
                LOG_INFO("Got hEventStopControl.");
                return false;
            }

            elapsed += WAIT_INTERVAL_MILLI_SECOND;
        }

        LOG_INFO("RequestFile() called.");  // DEBUG.
        return fetchingTaskContainer.succeeded;
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return false;
    }
    catch (const std::bad_alloc& e)
    {
        LOG_ERROR("Failed by std::bad_alloc %s", e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return false;
    }
}

bool BizTeleworkUpdaterRpcServer::RequestBodyString(const std::wstring& url, std::wstring* dstBodyString, HANDLE hEventStopControl)
{
    LOG_INFO("RequestBodyString() called.");

    if (!dstBodyString)
    {
        LOG_WARN("The dstBodyString must not be null.");
        return false;
    }

    try
    {
        auto dir = ConfigUtility::GetDataDirectory();
        if (dir.empty())
        {
            LOG_WARN("ConfigUtility::GetDataDirectory() failed.");
        }
        boost::filesystem::path dstFilePath(dir);
        dstFilePath /= "response.bin";

        if (!RequestFile(url, dstFilePath.wstring(), hEventStopControl))
        {
            return false;
        }

        std::wstring content;
        std::wifstream stream(dstFilePath.string());
        stream >> content;
        stream.close();

        if (!boost::filesystem::remove(dstFilePath))
        {
            LOG_INFO("boost::filesystem::remove() failed.");
        }

        *dstBodyString = content;
        return true;
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return false;
    }
    catch (const std::bad_alloc& e)
    {
        LOG_ERROR("Failed by std::bad_alloc %s", e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return false;
    }
}
