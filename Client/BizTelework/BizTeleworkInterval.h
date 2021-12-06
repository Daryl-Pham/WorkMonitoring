// BizTeleworkInterval.h : main header file for the PROJECT_NAME application
//

#pragma once
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/timer/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "AuthenHelper.h"
#include "Logging.h"
#include "CpprestRequestImpl.h"
#include "HttpStatusCode.h"
#include "Common.h"
#include "Utility.h"
#include "Log.h"
#include "DeviceLog.h"
#include "GettingRequest.h"

class BizTeleworkInterval
{
public:
    BizTeleworkInterval(boost::asio::io_service *io, int secCheckAuthen, int secReadLogSecurity, const std::wstring& apiUrl)
        : m_Strand(*io),
        timer_CheckAuthen(*io, boost::posix_time::seconds(0)),
        timer_ReadLogSecurity(*io, boost::posix_time::seconds(secReadLogSecurity)),
        m_ApiUrl(apiUrl)
    {
        timer_CheckAuthen.async_wait(m_Strand.wrap(boost::bind(&BizTeleworkInterval::callback_CheckAuthen, this, secCheckAuthen)));
        timer_ReadLogSecurity.async_wait(m_Strand.wrap(boost::bind(&BizTeleworkInterval::callback_ReadLogSecurity, this, secReadLogSecurity)));
    }

    ~BizTeleworkInterval()
    {
    }

    void callback_CheckAuthen(int secCheckAuthen);
    void callback_ReadLogSecurity(int secReadLogSecurity);

protected:
    BOOL sendLogsAndDeleteLogFilesIfSucceeded();
    BOOL validateAgentCredential(const AgentCredential& agentCredential);

private:
    boost::asio::io_service::strand m_Strand;
    boost::asio::deadline_timer     timer_CheckAuthen;
    boost::asio::deadline_timer     timer_ReadLogSecurity;

    const std::wstring m_ApiUrl;

    BOOL ProcessDeviceLog(Request* request);
    BOOL ProcessActiveAppLog(Request* request);
    BOOL CheckCallContext(CallContext* callContext) const;
};
