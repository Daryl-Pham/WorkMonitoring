#include "stdafx.h"

/* Fix for issue https://github.com/Microsoft/vcpkg/issues/836
 */
#define _TURN_OFF_PLATFORM_STRING

#include "GettingRequest.h"
#include "AuthenHelper.h"
#include "Log.h"
#include "CpprestRequestImpl.h"
#include "AppConfig.h"
#include "HttpMethod.h"
#include <thread>

namespace
{

    DEFINE_LOGGER_FILE_NAME;

    BOOL validateAgentCredential(const AgentCredential& agentCredential)
    {
        if (!agentCredential.company_guid.has_value())
        {
            LOG_WARN("The agentCredential.company_guid should be inserted a value but null.");
            return FALSE;
        }
        else if (agentCredential.company_guid.get().empty())
        {
            LOG_WARN("The agentCredential.company_guid should be not be empty.");
            return FALSE;
        }
        else if (!agentCredential.agent_guid.has_value())
        {
            LOG_WARN("The agentCredential.agent_guid should be inserted a value but null.");
            return FALSE;
        }
        else if (agentCredential.agent_guid.get().empty())
        {
            LOG_WARN("The agentCredential.agent_guid should be not be empty.");
            return FALSE;
        }

        return TRUE;
    }

}  // namespace

std::unique_ptr<Request> GettingRequest::GetRequest(const std::wstring& companyGUID, const std::vector<BYTE>& agentGUID, const HttpMethod method)
{
    AuthenHelper authen;
    AgentCredential agentCredential;

    if (companyGUID.empty() || agentGUID.empty())
    {
        LOG_INFO("BizTelework is not active.");
        return nullptr;
    }

    if (!authen.CheckAuthenTelework(companyGUID, agentGUID, &agentCredential))
    {
        LOG_WARN("authen.CheckAuthenTelework() failed.");
        return nullptr;
    }
    else if (!validateAgentCredential(agentCredential))
    {
        LOG_WARN("validateAgentCredential() failed.");
        return nullptr;
    }

    std::wstring apiUrl;
    AppConfig appConfig;

    if (!appConfig.GetApiUrl(&apiUrl))
    {
        LOG_WARN("appConfig.GetApiUrl() failed.");
        return nullptr;
    }

    RequestConfig config(apiUrl, *agentCredential.company_guid, *agentCredential.agent_guid, method);
    return std::make_unique<CpprestRequestImpl>(config);
}

std::unique_ptr<Request> GettingRequest::GetRequest(const HttpMethod method)
{
    AuthenHelper authen;
    AgentCredential agentCredential;
    std::vector<BYTE> agentGUID = authen.GetAgentGUIDFromRegistry();
    std::wstring companyGUID = authen.GetCompanyGUIDFromRegistry();

    if (agentGUID.empty() || companyGUID.empty())
    {
        LOG_INFO("BizTelework is not active.");
        return nullptr;
    }
    if (!authen.CheckAuthenTelework(companyGUID, agentGUID, &agentCredential))
    {
        LOG_WARN("authen.CheckAuthenTelework() failed.");
        return nullptr;
    }
    else if (!validateAgentCredential(agentCredential))
    {
        LOG_WARN("validateAgentCredential() failed.");
        return nullptr;
    }

    std::wstring apiUrl;
    AppConfig appConfig;

    if (!appConfig.GetApiUrl(&apiUrl))
    {
        LOG_WARN("appConfig.GetApiUrl() failed.");
        return nullptr;
    }

    RequestConfig config(apiUrl, *agentCredential.company_guid, *agentCredential.agent_guid, method);
    return std::make_unique<CpprestRequestImpl>(config);
}

std::unique_ptr<Request> GettingRequest::GetRequestForActivation(const HttpMethod method, const std::wstring& apiURL)
{
    std::wstring configAPIURL;
    AppConfig appConfig;
    if (!apiURL.empty())
    {
        configAPIURL = apiURL;
    }
    else if (!appConfig.GetApiUrl(&configAPIURL))
    {
        LOG_WARN("appConfig.GetApiUrl() failed.");
        return nullptr;
    }

    RequestConfig config(configAPIURL, L"", L"", method);
    return std::make_unique<CpprestRequestImpl>(config);
}
