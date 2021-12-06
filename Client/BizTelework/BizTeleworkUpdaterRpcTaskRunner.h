#pragma once

#include "BizTeleworkUpdaterRpcClient.h"
#include <memory>
#include <thread>

class BizTeleworkUpdaterRpcTaskRunner
{
public:
    explicit BizTeleworkUpdaterRpcTaskRunner(std::unique_ptr<BizTeleworkUpdaterRpcClientInterface> rpcClient)
        : m_RpcClient(std::move(rpcClient)) {}
    virtual ~BizTeleworkUpdaterRpcTaskRunner() = default;

    bool Start();
    void Shutdown();

private:
    bool Fetch(const std::wstring& url, const std::wstring& dstFilePath) const;

    std::unique_ptr<BizTeleworkUpdaterRpcClientInterface> m_RpcClient = nullptr;
    std::unique_ptr<std::thread> m_CosumerThread = nullptr;
    bool m_ContinueRunning = false;
};
