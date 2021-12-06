#include "stdafx.h"
#include "BizTeleworkInterval.h"
#include "ActiveApplication.h"
#include "BizTeleworkDependency.h"

namespace
{

    DEFINE_LOGGER_FILE_NAME;

}

void BizTeleworkInterval::callback_CheckAuthen(int secCheckAuthen)
{
    AuthenHelper authen;
    AgentCredential agentCredential;
    // Check authen
    BizTeleworkDependency dependency;
    std::vector<BYTE> agentGUID(0);
    std::wstring companyGUID = L"";
    if (!dependency.IsActive(&companyGUID, &agentGUID))
    {
        boost::mutex::scoped_lock(m_MutexForLogFile);
        LOG_INFO("BizTelework is not active.");
    }
    else if (authen.CheckAuthenTelework(companyGUID, agentGUID, &agentCredential))
    {
        // sendLogsAndDeleteLogFilesIfSucceeded is a heavy routine therefore post it to avoid stopping m_Strant routines.
        m_Strand.post(boost::bind(&BizTeleworkInterval::sendLogsAndDeleteLogFilesIfSucceeded, this));
    }

    timer_CheckAuthen.expires_at(timer_CheckAuthen.expires_at() + boost::posix_time::seconds(secCheckAuthen));
    timer_CheckAuthen.async_wait(m_Strand.wrap(boost::bind(&BizTeleworkInterval::callback_CheckAuthen, this, secCheckAuthen)));
}

void BizTeleworkInterval::callback_ReadLogSecurity(int secReadLogSecurity)
{
    timer_ReadLogSecurity.expires_at(timer_ReadLogSecurity.expires_at() + boost::posix_time::seconds(secReadLogSecurity));
    timer_ReadLogSecurity.async_wait(m_Strand.wrap(boost::bind(&BizTeleworkInterval::callback_ReadLogSecurity, this, secReadLogSecurity)));
}

BOOL BizTeleworkInterval::sendLogsAndDeleteLogFilesIfSucceeded()
{
    std::unique_ptr<Request> request = GettingRequest::GetRequest();
    if (!request)
    {
        return FALSE;
    }

    ProcessDeviceLog(request.get());

    ProcessActiveAppLog(request.get());

    return TRUE;
}

BOOL BizTeleworkInterval::validateAgentCredential(const AgentCredential& agentCredential)
{
    if (!agentCredential.company_guid.has_value())
    {
        LOG_ERROR("The agentCredential.company_guid should be inserted a value but null.");
        return FALSE;
    }
    else if (agentCredential.company_guid.get().empty())
    {
        LOG_ERROR("The agentCredential.company_guid should be not be empty.");
        return FALSE;
    }
    else if (!agentCredential.agent_guid.has_value())
    {
        LOG_ERROR("The agentCredential.agent_guid should be inserted a value but null.");
        return FALSE;
    }
    else if (agentCredential.agent_guid.get().empty())
    {
        LOG_ERROR("The agentCredential.agent_guid should be not be empty.");
        return FALSE;
    }

    return TRUE;
}

BOOL BizTeleworkInterval::ProcessDeviceLog(Request* request)
{
    DeviceLog deviceLog;
    bool keyboardLogFileExists = false;
    bool mouseLogFileExists = false;

    std::wstring jsonStrigify;
    LogLines logLines;
    deviceLog.ReadDeviceLog(&logLines, &keyboardLogFileExists, &mouseLogFileExists);
    if (!Logging::DeviceLogStringifyAsJsonBody(logLines, &jsonStrigify))
    {
        LOG_ERROR("LogFile::StringifyAsJsonBody() failed.");
        return FALSE;
    }

    boost::timer::cpu_timer timer;  // To know duration of post. For debug Internet connectivity condition.

    std::unique_ptr<CallContext> callContext = request->CallWindowsDeviceLogs(jsonStrigify);
    if (CheckCallContext(callContext.get()))
    {
        // "%w" means wall clock time.
        LOG_INFO("callContext->Wait() takes %s[sec].", timer.format(2, "%w").c_str());
        LOG_INFO("Succeeded to upload an event log.");
        deviceLog.DeleteDeviceLog(keyboardLogFileExists, mouseLogFileExists);
        return TRUE;
    }
    else
    {
        LOG_WARN("Requesting jsonStrigify = %S", jsonStrigify.c_str());
    }
    return FALSE;
}
BOOL BizTeleworkInterval::ProcessActiveAppLog(Request* request)
{
    std::wstring jsonStrigify;
    LogLines logLines;
    ActiveApplication activeApp(ActiveAppLogFileName);
    if (!activeApp.ReadActiveAppLog(&logLines))
    {
        return FALSE;
    }

    if (!Logging::ActiveAppLogStringifyAsJsonBody(logLines, &jsonStrigify))
    {
        LOG_ERROR("LogFile::ActiveAppLogStringifyAsJsonBody() failed.");
        return FALSE;
    }

    boost::timer::cpu_timer timer;  // To know duration of post. For debug Internet connectivity condition.

    std::unique_ptr<CallContext> callContext  = request->CallWindowsApplicationLogs(jsonStrigify);
    if (CheckCallContext(callContext.get()))
    {
        // "%w" means wall clock time.
        LOG_INFO("callContext->Wait() takes %s[sec].", timer.format(2, "%w").c_str());
        LOG_INFO("Succeeded to upload an application log.");
        Logging applicationLogFile = Logging(Utility::GetDefaultDataDirectory(), ActiveAppLogFileName);
        if (!applicationLogFile.DeleteLog())
        {
            LOG_WARN("applicationLogFile.DeleteLog() failed.");
        }
        return TRUE;
    }
    else
    {
        LOG_WARN("Requesting jsonStrigify = %S", jsonStrigify.c_str());
    }
    return FALSE;
}

BOOL BizTeleworkInterval::CheckCallContext(CallContext* callContext) const
{
    boost::timer::cpu_timer timer;  // To know duration of post. For debug Internet connectivity condition.
    if (callContext == nullptr)
    {
        LOG_WARN("Call API failed.");
        return FALSE;
    }
    else if (!callContext->Wait())
    {
        LOG_INFO("callContext->Wait() takes %s[sec].", timer.format(2, "%w").c_str());
        LOG_WARN("callContext->Wait() failed.");
        return FALSE;
    }
    else if (callContext->StatusCode() != HTTP_STATUS_CODE_CREATED)
    {
        LOG_WARN("The request is failed. StatusCode: %d", callContext->StatusCode());
        return FALSE;
    }
    return TRUE;
}
