#include "stdafx.h"
#include "BizTeleworkDependency.h"
#include "AppConfig.h"
#include "Common.h"
#include "Log.h"
#include "BizTeleworkAuthenticationDialog.h"
#include "AuthenHelper.h"
#include "GettingRequest.h"

namespace
{

DEFINE_LOGGER_FILE_NAME;

}

std::wstring  BizTeleworkDependency::GetApiUrl() const
{
    std::wstring apiUrl;
    AppConfig appConfig;

    if (!appConfig.GetApiUrl(&apiUrl))
    {
        LOG_ERROR("appConfig.GetApiUrl() failed.");
        return L"";
    }

    return apiUrl;
}

DWORD BizTeleworkDependency::GetTimerIntervalCheckAuthen() const
{
    DWORD timerInterval;
    AppConfig appConfig;

    if (!appConfig.GetTimerIntervalCheckAuthen(&timerInterval))
    {
        LOG_WARN("appConfig.GetTimerIntervalCheckAuthen() failed.");
        timerInterval = TimerIntervalCheckAuthen;
    }
    else if (timerInterval == 0)
    {
        LOG_INFO("The timerInterval is zero and set default value.");
        timerInterval = TimerIntervalCheckAuthen;
    }

    return timerInterval;
}

DWORD BizTeleworkDependency::GetTimerIntervalCheckLog() const
{
    return TimerIntervalCheckLog;
}

bool BizTeleworkDependency::IsActive(std::wstring* companyGUID, std::vector<BYTE>* agentGUID) const
{
    AuthenHelper authenHelper;
    *companyGUID = authenHelper.GetCompanyGUIDFromRegistry();
    *agentGUID = authenHelper.GetAgentGUIDFromRegistry();
    return IsActive(*companyGUID, *agentGUID);
}

void BizTeleworkDependency::ShowAuthenticationDialog(const ActiveInformation& activeInformation) const
{
    BizTeleworkDialogDependency dependency;
    std::unique_ptr<CDialogEx> dialog = std::make_unique<BizTeleworkAuthenticationDialog>(dependency, nullptr, activeInformation);
    dialog->DoModal();
}

std::unique_ptr<Request> BizTeleworkDependency::GetRequest(const HttpMethod method)
{
    std::vector<BYTE> agentGUID(0);
    std::wstring companyGUID = L"";
    if (!IsActive(&companyGUID, &agentGUID))
    {
        ShowAuthenticationDialog(ActiveInformation());
        return nullptr;
    }
    return GettingRequest::GetRequest(companyGUID, agentGUID, method);
}

std::unique_ptr<Request> BizTeleworkDependency::GetRequestForActivation(const HttpMethod method, const std::wstring& apiURL)
{
    return GettingRequest::GetRequestForActivation(method, apiURL);
}

bool BizTeleworkDependency::IsActive() const
{
    std::wstring companyGUID;
    std::vector<BYTE> agentGUID;
    AuthenHelper authenHelper;
    companyGUID = authenHelper.GetCompanyGUIDFromRegistry();
    agentGUID = authenHelper.GetAgentGUIDFromRegistry();
    return IsActive(companyGUID, agentGUID);
}

bool BizTeleworkDependency::IsActive(const std::wstring& companyGUID, const std::vector<BYTE>& agentGUID) const
{
    return (!companyGUID.empty() && !agentGUID.empty());
}
