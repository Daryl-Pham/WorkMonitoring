#include "stdafx.h"
#include "BadRequestResponsive.h"
#include "HttpStatusCode.h"

namespace
{

    const wchar_t* WorkStatusStarted = L"Error::WorkStatus::USER_WORK_STARTED";

    const wchar_t* WorkStatusEnded = L"Error::WorkStatus::USER_WORK_ENDED";

    const wchar_t* DayOffRequestStarted = L"Error::DayOffRequest::USER_WORK_STARTED";

}  // namespace

BadRequestResponsive::BadRequestResponsive(const std::wstring& value)
    : Responsive(HTTP_STATUS_CODE::HTTP_STATUS_CODE_BAD_REQUEST, value)
{
}

bool BadRequestResponsive::ParseValue()
{
    std::wstring stringBody = this->GetBody();

    if (stringBody.find(WorkStatusStarted, 0) != std::string::npos)
    {
        m_Status = BadStatus::BAD_WORK_STATUS_STARTED;
        return true;
    }
    else if (stringBody.find(WorkStatusEnded, 0) != std::string::npos)
    {
        m_Status = BadStatus::BAD_WORK_STATUS_ENDED;
        return true;
    }
    else if (stringBody.find(DayOffRequestStarted, 0) != std::string::npos)
    {
        m_Status = BadStatus::BAD_DAY_OFF_REQUEST_STARTED;
        return true;
    }

    return false;
}

BadStatus BadRequestResponsive::GetStatus()
{
    return m_Status;
}
