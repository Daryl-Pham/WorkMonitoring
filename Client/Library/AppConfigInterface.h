#pragma once

#include <string>
#include <Windows.h>

class AppConfigInterface
{
public:
    virtual bool GetApiUrl(std::wstring *dst) = 0;
    virtual bool GetDataDir(std::wstring* dst) = 0;
    virtual bool GetUpdateUrl(std::wstring *dst) = 0;
    virtual bool GetUpgradeCode(std::wstring* dst) = 0;
    virtual bool GetVersion(std::wstring *dst) = 0;
    virtual bool GetTimerIntervalGetActiveApplication(DWORD* timer) = 0;
    virtual bool GetTimerIntervalCheckAuthen(DWORD* timer) = 0;
    virtual bool GetSentryDSN(std::string* dsn) = 0;
    virtual bool GetReleaseEnvironment(std::string* dst) = 0;
    virtual bool GetBizTeleworkUpdaterRPCEndpoint(std::wstring* dst) = 0;
    virtual bool GetExecutedBizTelework(bool* dst) = 0;
    virtual bool SetExecutedBizTelework(bool value) = 0;
    virtual bool GetTimeDisplayHealthStatusRemind(int64_t* time) = 0;
    virtual bool SetTimeDisplayHealthStatusRemind(const int64_t time) = 0;
    virtual bool DeleteTimeDisplayHealthStatusRemind() = 0;
    virtual bool GetTimerIntervalToRemidOnDutyDialog(int64_t* dst) = 0;
    virtual bool GetTimeToShowOnDutyDialog(int64_t* dst) = 0;
    virtual bool GetTimeToShowHealthStatusDialog(int64_t* dst) = 0;
    virtual bool SetCopiedAuthentication(bool value) = 0;
    virtual bool GetCopiedAuthentication(bool* dst) = 0;
    virtual ~AppConfigInterface() = default;
};
