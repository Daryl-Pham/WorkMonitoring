#pragma once
#include "OkieResponsive.h"

class AuthenticateOkieResponsive : public OkieResponsive
{
public:
    explicit AuthenticateOkieResponsive(const std::wstring& value);
    const std::wstring GetCompanyGUID() const;
    const std::wstring GetAgentGUID() const;

    bool ParseValue();

private:
    std::wstring m_CompanyGUID = L"";
    std::wstring m_AgentGUID = L"";
};
