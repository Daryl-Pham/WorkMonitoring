#include "stdafx.h"
#include "WorkLogOkieResponsive.h"
#include "Log.h"
#include "Utility.h"

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    const wchar_t* SelfManagementUrl = L"self_management_url";

}

WorkLogOkieResponsive::WorkLogOkieResponsive(const std::wstring& value)
    : OkieResponsive(value)
{
}

bool WorkLogOkieResponsive::ParseValue()
{
    std::wstring stringBody = this->GetBody();
    std::wstringstream s;
    s << stringBody;


    if (stringBody.find(SelfManagementUrl ,0) != std::string::npos)
    {
        try
        {
            web::json::value valueJson = web::json::value::parse(s);
            m_SelfManagementUrl = valueJson.at(SelfManagementUrl).as_string();
        }
        catch (const web::json::json_exception& jsonEx)
        {
            LOG_ERROR("Get SelfManagementUrl failed.%s" ,jsonEx.what());
            return false;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Get SelfManagementUrl failed.%s" ,e.what());
            return false;
        }
        return true;
    }

    return false;
}

const std::wstring WorkLogOkieResponsive::GetSelfManagementUrl() const
{
    return m_SelfManagementUrl;
}
