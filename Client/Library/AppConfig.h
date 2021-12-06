#pragma once

#include "AppConfigInterface.h"

class AppConfig : public AppConfigInterface
{
public:
    AppConfig() = default;
    ~AppConfig() = default;

    bool GetApiUrl(std::wstring *dst) override;
    bool GetDataDir(std::wstring* dst) override;
    bool GetUpdateUrl(std::wstring *dst) override;
    bool GetUpgradeCode(std::wstring* dst) override;
    bool GetVersion(std::wstring *dst) override;
    bool GetTimerIntervalGetActiveApplication(DWORD* timer) override;
    bool GetTimerIntervalCheckAuthen(DWORD* timer) override;
    bool GetSentryDSN(std::string* dsn) override;
    bool GetReleaseEnvironment(std::string* dst) override;
    bool GetBizTeleworkUpdaterRPCEndpoint(std::wstring* dst) override;
    bool GetExecutedBizTelework(bool* dst) override;
    bool SetExecutedBizTelework(bool value) override;
    bool GetTimeDisplayHealthStatusRemind(int64_t* time) override;
    bool SetTimeDisplayHealthStatusRemind(const int64_t time) override;
    bool DeleteTimeDisplayHealthStatusRemind() override;
    bool GetTimerIntervalToRemidOnDutyDialog(int64_t* dst) override;
    bool GetTimeToShowOnDutyDialog(int64_t* dst) override;
    bool GetTimeToShowHealthStatusDialog(int64_t* dst) override;
    bool GetCopiedAuthentication(bool* dst) override;
    bool SetCopiedAuthentication(bool value) override;
};
