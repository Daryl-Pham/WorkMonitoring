#pragma once
#include <string>

class FetchingTaskInterface
{
public:
    virtual bool RequestFile(const std::wstring& url, const std::wstring& dstFilePath, HANDLE hEventStopControl) = 0;
    virtual bool RequestBodyString(const std::wstring& url, std::wstring* dstBodyString, HANDLE hEventStopControl) = 0;
    virtual ~FetchingTaskInterface() = default;
};
