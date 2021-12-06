#pragma once

#include <string>
#include "HttpMethod.h"

class RequestConfig
{
public:
    /**
     * @param addr contains scheme, host and port.
     *        (eg. http://example.com:8080/ , https://example.com/ or, http://127.0.0.1/ , etc...
     * @param companyGUID Company GUID for HTTP API.
     * @param agentGUID Agent GUID for HTTP API.
     */
    RequestConfig(const std::wstring &apiUrl, const std::wstring &companyGUID, const std::wstring &agentGUID, const HttpMethod &method)
        : m_ApiUrl(apiUrl)
        , m_CompanyGUID(companyGUID)
        , m_AgentGUID(agentGUID)
        , m_HttpMethod(method)
        // TODO(ichino): set proxy server and its authentication.
    {}
    ~RequestConfig() {}

    std::wstring ApiUrl() const { return m_ApiUrl; }
    std::wstring CompanyGUID() const { return m_CompanyGUID; }
    std::wstring AgentGUID() const { return m_AgentGUID; }
    HttpMethod   HttpMethodRequest() const { return m_HttpMethod; }


private:
    std::wstring m_ApiUrl;
    std::wstring m_CompanyGUID;
    std::wstring m_AgentGUID;
    HttpMethod   m_HttpMethod;
};
