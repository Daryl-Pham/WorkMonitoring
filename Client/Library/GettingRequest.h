#pragma once
#include "Request.h"
#include <vector>

class GettingRequest
{
public:
    static std::unique_ptr<Request> GetRequest(const std::wstring& companyGUID, const std::vector<BYTE>& agentGUID, const HttpMethod method = HttpMethod::POST);
    static std::unique_ptr<Request> GetRequest(const HttpMethod method = HttpMethod::POST);
    static std::unique_ptr<Request> GetRequestForActivation(const HttpMethod method = HttpMethod::POST, const std::wstring& apiURL = L"");
};
