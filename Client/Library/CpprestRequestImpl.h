#pragma once

#include "Request.h"

class CpprestRequestImpl : public Request
{
public:
    explicit CpprestRequestImpl(const RequestConfig& requestConfig);
    virtual ~CpprestRequestImpl() = default;

    // For Request
    std::unique_ptr<CallContextForWindowsDeviceLogs> CallWindowsDeviceLogs(std::wstring &eventLog);
    std::unique_ptr<CallContextForWindowsWorkLogs> CallWindowsWorkLogs(RequestWorkLogsType requestWorkLogsType);
    std::unique_ptr<CallContextForTimeCardsToday> CallWindowsTimeCardsToday();
    std::unique_ptr<CallContextForWindowsUserConditionLogs> CallWindowsUserConditionLogs(RequestHealthStatusType requestHealthStatusType);
    std::unique_ptr<CallContextForWindowsDayOffRequests> CallWindowsDayOffRequests();
    std::unique_ptr<CallContextForWindowsApplicationLogs> CallWindowsApplicationLogs(const std::wstring& eventLog);
    std::unique_ptr<CallContextForWindowsAuthenticate> CallWindowsAuthenticate(const std::wstring& companyCode, const std::wstring& activationCode);
    std::unique_ptr<CallContextForWindowsWorkTarget> CallWindowsWorkTarget(const std::wstring& workTarget) override;

private:
    const RequestConfig m_RequestConfig;
};
