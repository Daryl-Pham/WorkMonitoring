#include "stdafx.h"
#include "AuthenticateOkieResponsive.h"
#include "Log.h"
#include "Utility.h"

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    const wchar_t* CompanyGUID = L"company_guid";
    const wchar_t* AgentGUID = L"agent_guid";

}

AuthenticateOkieResponsive::AuthenticateOkieResponsive(const std::wstring& value)
    : OkieResponsive(value)
{
}

bool AuthenticateOkieResponsive::ParseValue()
{
    std::wstring stringBody = this->GetBody();
    std::wstringstream s;
    s << stringBody;


    if (stringBody.find(CompanyGUID, 0) != std::string::npos && stringBody.find(AgentGUID, 0) != std::string::npos)
    {
        try
        {
            web::json::value valueJson = web::json::value::parse(s);
            m_CompanyGUID = valueJson.at(CompanyGUID).as_string();
            m_AgentGUID = valueJson.at(AgentGUID).as_string();
        }
        catch (const web::json::json_exception& jsonEx)
        {
            LOG_ERROR("Get companyGUID or agentGUID failed.%s", jsonEx.what());
            return false;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Get companyGUID or agentGUID failed.%s", e.what());
            return false;
        }
        return true;
    }

    return false;
}

const std::wstring AuthenticateOkieResponsive::GetCompanyGUID() const
{
    return m_CompanyGUID;
}
const std::wstring AuthenticateOkieResponsive::GetAgentGUID() const
{
    return m_AgentGUID;
}
