#pragma once

#include "BizTeleworkUpdaterRpcClientInterface.h"
#include <rpc.h>
#include <string>

class BizTeleworkUpdaterRpcClient : public BizTeleworkUpdaterRpcClientInterface
{
public:
    BizTeleworkUpdaterRpcClient() = default;
    virtual ~BizTeleworkUpdaterRpcClient() = default;

    bool Bind() override;
    void Unbind() override;
    bool IsBound() override { return !!m_Binding; };

    HRESULT PollFetchingTask(std::wstring* url, std::wstring* dstFilePath) override;
    void PushFetchingResult(bool succeeded) override;

private:
    RPC_WSTR m_Binding = nullptr;
};
