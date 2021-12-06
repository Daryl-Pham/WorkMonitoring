#include "stdafx.h"
#include "BizTeleworkUpdaterRpcTaskRunner.h"
#include <Log.h>
#include <boost/filesystem.hpp>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>


namespace
{
    DEFINE_LOGGER_FILE_NAME;

    const DWORD CONSUMING_INTERVAL_MILLI_SECOND = 2 * 1000;
}  // namespace

bool BizTeleworkUpdaterRpcTaskRunner::Start()
{
    if (!m_RpcClient)
    {
        LOG_ERROR("The m_RpcClient must be not null.");
        return false;
    }
    else if (m_CosumerThread)
    {
        LOG_WARN("You already have started the consumer thread.");
        return false;
    }

    m_ContinueRunning = true;
    m_CosumerThread.reset(new std::thread([this]
        {
            while (m_ContinueRunning)
            {
                Sleep(CONSUMING_INTERVAL_MILLI_SECOND);

                if ((!m_RpcClient->IsBound()) && (!m_RpcClient->Bind()))
                {
                    // Waiting for binding to server.
                    continue;
                }

                std::wstring url;
                std::wstring dstFilePath;
                HRESULT hr = m_RpcClient->PollFetchingTask(&url, &dstFilePath);

                if (hr == S_OK)
                {
                    LOG_INFO("Get fetching task.");
                    LOG_INFO("url: %S", url.c_str());
                    LOG_INFO("dstFilePath: %S", dstFilePath.c_str());

                    bool succeeded = Fetch(url, dstFilePath);

                    m_RpcClient->PushFetchingResult(succeeded);
                }
            }

            m_RpcClient->Unbind();
        }));

    return true;
}

void BizTeleworkUpdaterRpcTaskRunner::Shutdown()
{
    if (m_CosumerThread && m_ContinueRunning)
    {
        m_ContinueRunning = false;
        LOG_INFO("Joining the m_CosumerThread...");
        m_CosumerThread->join();
        LOG_INFO("The m_CosumerThread is joined.");
    }
}


bool BizTeleworkUpdaterRpcTaskRunner::Fetch(const std::wstring& url, const std::wstring& dstFilePath) const
{
    try
    {
        using web::http::http_request;
        using web::http::http_response;
        using web::http::methods;
        using web::http::client::http_client;
        using web::http::client::http_client_config;
        using web::web_proxy;

        http_client_config client_config;
        client_config.set_proxy(web_proxy(web_proxy::use_auto_discovery));

        http_client client(url.c_str(), client_config);

        // Build request URI and start the request.
        http_request request(methods::GET);

        int statusCode;
        auto fileStream = std::make_shared<concurrency::streams::ostream>();

        // Open stream to output file.
        pplx::task<void> requestTask = concurrency::streams::fstream::open_ostream(dstFilePath, std::ios::out | std::ios::binary | std::ios::trunc)
            .then([&fileStream, &client](concurrency::streams::ostream outFile)
                {
                    *fileStream = outFile;

                    return client.request(methods::GET);
                })
            .then([&statusCode, &fileStream](http_response response)
                {
                    LOG_INFO("Got response.");
                    statusCode = response.status_code();

                    return response.body().read_to_end(fileStream->streambuf());
                })
            .then([&fileStream](size_t)
                {
                    LOG_INFO("Finished to write response.");
                    return fileStream->close();
                });

        auto status = requestTask.wait();
        if (status != pplx::completed)
        {
            LOG_ERROR("client request task is failed. The status shuold be completed but %d.", status);
            return false;
        }

        if (statusCode != 200)
        {
            LOG_WARN("The statusCode must be 200 but %d.", statusCode);
            return false;
        }

        if (!boost::filesystem::exists(dstFilePath))
        {
            LOG_ERROR("The installer file does not exist.");
            return false;
        }

        return true;
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("client request task is failed. %s", e.what());
        return false;
    }
}
