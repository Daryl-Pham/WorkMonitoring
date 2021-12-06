#pragma once
#include "OkieResponsive.h"

class WorkLogOkieResponsive: public OkieResponsive
{
public:
    explicit WorkLogOkieResponsive(const std::wstring& value);
    const std::wstring GetSelfManagementUrl() const;

    bool ParseValue();

private:
    std::wstring m_SelfManagementUrl = L"";
};

