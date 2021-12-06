#pragma once
#include <string>
#include <Windows.h>

class BizTeleworkUpdaterRpcClientInterface
{
public:
    virtual bool Bind() = 0;
    virtual void Unbind() = 0;
    virtual bool IsBound() = 0;
    virtual HRESULT PollFetchingTask(std::wstring* url, std::wstring* dstFilePath) = 0;
    virtual void PushFetchingResult(bool succeeded) = 0;
    virtual ~BizTeleworkUpdaterRpcClientInterface() = default;
};
