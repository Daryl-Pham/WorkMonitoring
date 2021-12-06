#pragma once

#include <memory>
#include <thread>

#include "FetchingTaskInterface.h"

class BizTeleworkUpdaterRpcServer : public FetchingTaskInterface
{
public:
    BizTeleworkUpdaterRpcServer() = default;
    virtual ~BizTeleworkUpdaterRpcServer() = default;
    void Start();
    void Shutdown();

    // For FetchingTaskInterface
    bool RequestFile(const std::wstring& url, const std::wstring& dstFilePath, HANDLE hEventStopControl) override;
    bool RequestBodyString(const std::wstring& url, std::wstring* dstBodyString, HANDLE hEventStopControl) override;

private:
    std::unique_ptr<std::thread> m_ServerThread = nullptr;
};
