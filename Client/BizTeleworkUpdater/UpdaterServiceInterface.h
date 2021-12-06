#pragma once

#include <Windows.h>

class UpdaterServiceInterface
{
public:
    virtual bool PreRun(LPCWSTR serviceName) = 0;
    virtual bool Run() = 0;
    virtual bool OnStop() = 0;
    virtual bool OnInterrogate() = 0;

    virtual ~UpdaterServiceInterface() = default;
};
