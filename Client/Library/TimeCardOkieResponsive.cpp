#include "stdafx.h"
#include "HttpStatusCode.h"
#include "TimeCardOkieResponsive.h"
#include "Log.h"
#include "Utility.h"

namespace
{

    DEFINE_LOGGER_FILE_NAME;
    const wchar_t* OnDuty = L"on_duty";
    const wchar_t* OffDuty = L"off_duty";
    const wchar_t* const WorkTarget = L"work_target";

}

TimeCardOkieResponsive::TimeCardOkieResponsive(const std::wstring& value)
    : OkieResponsive(value)
{
}

TimeCardOkieStatus TimeCardOkieResponsive::GetStatus() const
{
    return m_Status;
}

int64_t TimeCardOkieResponsive::GetUnixTimeStartedAt() const
{
    return m_UnixTimeStartedAt;
}

bool TimeCardOkieResponsive::ParseValue()
{
    std::wstring stringBody = this->GetBody();

    if (stringBody.find(OnDuty, 0) != std::string::npos)
    {
        m_Status = TimeCardOkieStatus::ON_DUTY;
        std::wstring startedAt = L"";
        std::wstring stringWorkTarget = L"";
        GetWorkTargetFromBody(&stringWorkTarget, stringBody);
        if (GetStartedAtFromBody(&startedAt, stringBody)
            && Utility::ConvertStringToUnixTime(&m_UnixTimeStartedAt, startedAt))
        {
            return true;
        }
    }
    else if (stringBody.find(OffDuty, 0) != std::string::npos)
    {
        m_Status = TimeCardOkieStatus::OFF_DUTY;
        return true;
    }

    return false;
}

bool TimeCardOkieResponsive::GetStartedAtFromBody(std::wstring* startedAt, const std::wstring& body)
{
    std::wstringstream s;
    s << body;

    try
    {
        web::json::value valueJson = web::json::value::parse(s);
        *startedAt = valueJson.at(U("started_at")).as_string();
    }
    catch (const web::json::json_exception& jsonEx)
    {
        LOG_ERROR("Get time started failed.%s", jsonEx.what());
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Get time started failed.%s", e.what());
        return false;
    }

    return true;
}

bool TimeCardOkieResponsive::GetWorkTargetFromBody(std::wstring* workDayTarget, const std::wstring_view& body)
{
    std::wstringstream s;
    s << body;

    try
    {
        web::json::value valueJson = web::json::value::parse(s);
        *workDayTarget = valueJson.at(std::wstring(WorkTarget)).as_string();
        if (ClassifyWorkTarget(*workDayTarget))
        {
            return true;
        }
    }
    catch (const web::json::json_exception& jsonEx)
    {
        LOG_WARN("Get time started failed.%s", jsonEx.what());
        m_WorkTarget.targetStatus = WorkDayTargetStatus::NULL_STATUS;
        m_WorkTarget.content = L"";
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_WARN("Get time started failed.%s", e.what());
        return false;
    }

    return true;
}

bool TimeCardOkieResponsive::ClassifyWorkTarget(const std::wstring_view& workTargetString)
{
    if (!workTargetString.compare(L"null"))
    {
        m_WorkTarget.targetStatus = WorkDayTargetStatus::NULL_STATUS;
        m_WorkTarget.content = L"";
        return true;
    }

    if (!workTargetString.compare(L""))
    {
        m_WorkTarget.targetStatus = WorkDayTargetStatus::BLANK_STATUS;
        m_WorkTarget.content = L"";
        return true;
    }

    if (workTargetString.size() > 0)
    {
        m_WorkTarget.targetStatus = WorkDayTargetStatus::VARIABLE_STATUS;
        m_WorkTarget.content = workTargetString;
        return true;
    }

    return false;
}

WorkDayTarget TimeCardOkieResponsive::GetWorkDayTarget() const
{
    return m_WorkTarget;
}
