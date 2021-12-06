#pragma once

#include "RequestConfig.h"
#include "CallContext.h"
#include "RequestWorkLogsType.h"
#include "RequestHealthStatusType.h"
#include <memory>
#include <istream>
#include <string>

/**
 * Interface for request of /app/biz_telework/api/v1 .
 */
class Request
{
public:

    /**
     * Interface to call POST /windows/device_logs
     * @param eventLog An stream of event log content. (eg. std::ifstream("file.log") or std::istringstream(std::string("event log content"))
     * @return Returns CallContext object. Returns nullptr if calling is failed.
     */
    virtual std::unique_ptr<CallContextForWindowsDeviceLogs> CallWindowsDeviceLogs(std::wstring &eventLog) = 0;


    /**
     * Interface to call POST /windows/work_logs
     * @param requestWorkLogsType Specify log type
     * @return Returns CallContext object. Returns nullptr if calling is failed.
     */
    virtual std::unique_ptr<CallContextForWindowsWorkLogs> CallWindowsWorkLogs(RequestWorkLogsType requestWorkLogsType) = 0;


    /**
     * Interface to call GET /windows/timecards/today
     * @return Returns CallContext object. Returns nullptr if calling is failed.
     */
    virtual std::unique_ptr<CallContextForTimeCardsToday> CallWindowsTimeCardsToday() = 0;

    /**
     * Interface to call POST /windows/user_condition_logs
     * @return Returns CallContext object. Returns nullptr if calling is failed.
     */
    virtual std::unique_ptr<CallContextForWindowsUserConditionLogs> CallWindowsUserConditionLogs(RequestHealthStatusType requestHealthStatusType) = 0;

    /**
     * Interface to call POST /windows/day_off_requests
     * @return Returns CallContext object. Returns nullptr if calling is failed.
     */
    virtual std::unique_ptr<CallContextForWindowsDayOffRequests> CallWindowsDayOffRequests() = 0;

    /**
     * Interface to call POST /windows/application_logs
     * @param eventLog An stream of event log content. (eg. std::ifstream("file.log") or std::istringstream(std::string("event log content"))
     * @return Returns CallContext object. Returns nullptr if calling is failed.
     */
    virtual std::unique_ptr<CallContextForWindowsApplicationLogs> CallWindowsApplicationLogs(const std::wstring& eventLog) = 0;

    /**
     * Interface to call POST /windows/authenticate
     * @param eventLog An stream of event log content. (eg. std::ifstream("file.log") or std::istringstream(std::string("event log content"))
     * @return Returns CallContext object. Returns nullptr if calling is failed.
     */
    virtual std::unique_ptr<CallContextForWindowsAuthenticate> CallWindowsAuthenticate(const std::wstring& companyCode,
                                                                                       const std::wstring& activationCode) = 0;

    /**
     * Interface to call POST /windows/work_target
     * @param workTarget work target of user.
     * @return Returns CallContext object. Returns nullptr if calling is failed.
     */
    virtual std::unique_ptr<CallContextForWindowsWorkTarget> CallWindowsWorkTarget(const std::wstring& workTarget) = 0;

    virtual ~Request() = default;
};

