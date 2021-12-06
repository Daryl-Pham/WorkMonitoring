#pragma once
#include <string>
#include <cpprest/json.h>

class Responsive
{
public:

    Responsive(const int code, const std::wstring& value)
        : m_Code(code),
        m_StringBody(value)
    {
    }

    virtual ~Responsive() = default;

    int GetCode() const;
    std::wstring GetBody() const;

private:
    int m_Code = 0;
    std::wstring m_StringBody = L"";
};
