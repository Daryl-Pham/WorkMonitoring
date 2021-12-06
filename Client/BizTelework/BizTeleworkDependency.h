#pragma once

#include <string>
#include "Request.h"
#include <vector>
#include <ActiveInformation.h>
#include "Common.h"

class BizTeleworkDependency
{
public:
    BizTeleworkDependency() = default;
    virtual ~BizTeleworkDependency() = default;

    virtual std::wstring GetApiUrl() const;
    virtual DWORD GetTimerIntervalCheckAuthen() const;
    virtual DWORD GetTimerIntervalCheckLog() const;
    virtual bool IsActive(std::wstring* companyGUID, std::vector<BYTE>* agentGUID) const;
    virtual bool IsActive() const;
    virtual bool IsActive(const std::wstring& companyGUID, const std::vector<BYTE>& agentGUID) const;
    virtual std::unique_ptr<Request> GetRequest(const HttpMethod method = HttpMethod::POST);
    virtual std::unique_ptr<Request> GetRequestForActivation(const HttpMethod method = HttpMethod::POST, const std::wstring& apiURL = L"");
    virtual void ShowAuthenticationDialog(const ActiveInformation& activeInformation) const;
};
