#include "stdafx.h"
#include <memory>
#include "CpprestRequestImpl.h"
#include "Log.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include "ResponsiveCreator.h"
#include "Responsive.h"
#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "crypt32.lib")

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

namespace
{

DEFINE_LOGGER_FILE_NAME;
const std::wstring BASE_PATH = L"/app/biz_telework/api/v1";
const wchar_t *ADDITIONAL_PATH_WINDOWS_DEVICE_LOGS = L"/windows/device_logs";
const wchar_t* ADDITIONAL_PATH_WINDOWS_WORK_LOGS = L"/windows/work_logs";
const wchar_t* ADDITIONAL_PATH_WINDOWS_TIMECARDS_TODAY = L"/windows/timecards/today";
const wchar_t* ADDITIONAL_PATH_WINDOWS_USER_CONDITION_LOGS = L"/windows/user_condition_logs";
const wchar_t* ADDITIONAL_PATH_WINDOWS_DAY_OFF_REQUESTS = L"/windows/day_off_requests";
const wchar_t* ADDITIONAL_PATH_WINDOWS_APPLICATION_LOGS = L"/windows/application_logs";
const wchar_t* ADDITIONAL_PATH_WINDOWS_AUTHENTICATE = L"/windows/authenticate";
const wchar_t* const ADDITIONAL_PATH_WINDOWS_WORK_TARGET = L"/windows/work_target";
const wchar_t *HEADER_X_COMPANY_GUID = L"X-Company-GUID";
const wchar_t *HEADER_X_AGENT_GUID = L"X-Agent-GUID";
const wchar_t* JSON_FIELD_WORK_STATUS = L"work_status";
const wchar_t* JSON_VALUE_START = L"start";
const wchar_t* JSON_VALUE_END = L"end";
const wchar_t* JSON_FIELD_CONDITION = L"condition";
const wchar_t* JSON_VALUE_GOOD_HEALTH = L"good";
const wchar_t* JSON_VALUE_NORMAL_HEALTH = L"normal";
const wchar_t* JSON_VALUE_BAD_HEALTH = L"bad";
const wchar_t* JSON_FIELD_COMPANY_CODE = L"company_code";
const wchar_t* JSON_FIELD_ACTIVATION_CODE = L"activation_code";
const wchar_t* const JSON_FIELD_WORK_TARGET = L"work_target";

template<class T>
class CallContextImplBase : public T
{
public:
    explicit CallContextImplBase(const RequestConfig& requestConfig)
        : m_CancellationToken(m_CTS.get_token())
        , m_RequestConfig(requestConfig)
    {
    }

    virtual ~CallContextImplBase() = default;

    // CallContextForWindowsDeviceLogs
    bool Wait() override
    {
        // Create http_client to send the request.
        std::wstring apiUrl = m_RequestConfig.ApiUrl();
        if (apiUrl.empty())
        {
            m_Logs.push_front(GetStringType() + L" m_RequestConfig.ApiUrl() failed.");
            m_Logs.push_back(GetStringType() + L" Set Request URL Failed ");
            return false;
        }
        // Get Logs about Request URL
        m_Logs.push_back(GetStringType() + L" Request URL = " + apiUrl);
        try
        {
            http_client_config client_config;
            client_config.set_proxy(web::web_proxy(web::web_proxy::use_auto_discovery));
            client_config.set_nativehandle_options([=](web::http::client::native_handle handle) {
                WINHTTP_PROXY_INFO pi;
                DWORD dwSize = sizeof(pi);
                if (!WinHttpQueryOption(handle, WINHTTP_OPTION_PROXY, &pi, &dwSize))
                {
                    m_Logs.push_back(GetStringType() + L" DONT GET Information Proxy");
                }
                if (pi.dwAccessType == WINHTTP_ACCESS_TYPE_NO_PROXY)
                {
                    m_Logs.push_back(GetStringType() + L" Proxy Is NOT Used");
                }
                else
                {
                    m_Logs.push_back(GetStringType() + L" Proxy Is Used");
                    m_Logs.push_back(GetStringType() + L" Proxy URL: " + std::wstring(pi.lpszProxy));
                }
                });
            http_client client(apiUrl.c_str(), client_config);
            uri_builder builder(BASE_PATH);
            AppendPath(&builder);
            // Build request URI and start the request.
            http_request request(GetCppRestHttpMethod());
            request.headers().add(HEADER_X_COMPANY_GUID, m_RequestConfig.CompanyGUID());
            request.headers().add(HEADER_X_AGENT_GUID, m_RequestConfig.AgentGUID());
            request.set_request_uri(builder.to_uri());
            // Get Logs about Request URL, Absolute Uri & Request Method
            m_Logs.push_back(GetStringType() + L" Request Method " + request.method() + L" " + request.absolute_uri().path());
            if (!SetBody(&request))
            {
                return false;
            }
            auto task = client.request(request, m_CancellationToken)
                .then([this](http_response response)
                    {
                        m_StatusCode = response.status_code();
                        m_Body = response.extract_utf16string().get();
                    });
            auto status = task.wait();
            if (m_StatusCode != 201 && m_StatusCode != 200)
            {
                m_Logs.push_back(GetStringType() + L" Respond Status Code = " + std::to_wstring(m_StatusCode) + L", Response Body = " + m_Body);
                WriteLogRequest();
                return false;
            }
            if (status != pplx::completed)
            {
                m_Logs.push_front(GetStringType() + L" Client request task is failed. The status should be completed but " + std::to_wstring(status));
                m_Logs.push_back(GetStringType() + L" Respond Status Code = -1.");
                WriteLogRequest();
                return false;
            }
        }
        catch (std::invalid_argument& invalidException)
        {
            m_Logs.push_front(GetStringType() + L" ApiURL is wrong: " + utility::conversions::utf8_to_utf16(invalidException.what()) );
            m_Logs.push_back(GetStringType() + L" Respond Status Code = -1");
            WriteLogRequest();
            return false;
        }
        catch (std::logic_error& logicException)
        {
            m_Logs.push_front(GetStringType() + L" Client request task is failed: " + utility::conversions::utf8_to_utf16(logicException.what()));
            m_Logs.push_back(GetStringType() + L" Respond Status Code = -1");
            WriteLogRequest();
            return false;
        }
        catch (std::exception& exception)
        {
            m_Logs.push_front(GetStringType() + L" Client request task is failed.  " + utility::conversions::utf8_to_utf16(exception.what()));
            m_Logs.push_back(GetStringType() + L" Respond Status Code = -1");
            WriteLogRequest();
            return false;
        }
        return true;
    }

    void WriteLogRequest()
    {
        m_Logs.push_front(L"WARNING Request Is Not Secessfull " + GetStringType());
        for (auto it = m_Logs.begin(); it != m_Logs.end(); it++)
        {
            LOG_WARN("%S", it->c_str());
        }
        return;
    }

    void Cancel() override
    {
        m_CTS.cancel();
    }

    /**
        * HTTP Status Code. You can call this after Wait() is succeeded.
        * @return HTTP Status Code from server.
        */
    int StatusCode() override
    {
        return m_StatusCode;
    }

    std::wstring Body() override
    {
        return m_Body;
    }

protected:
    virtual void AppendPath(uri_builder* uriBuilder) = 0;
    virtual bool RequestBody(std::string* requestBody) = 0;

private:
    method GetCppRestHttpMethod()
    {
        method result;
        switch (m_RequestConfig.HttpMethodRequest())
        {
        case HttpMethod::POST:
            result = methods::POST;
            break;
        case HttpMethod::GET:
            result = methods::GET;
            break;
        default:
            result = methods::POST;
            break;
        }
        return result;
    }

    bool SetBody(http_request* request)
    {
        if (m_RequestConfig.HttpMethodRequest() == HttpMethod::GET)
        {
            return true;
        }
        else
        {
            std::string body;
            if (!RequestBody(&body))
            {
                m_Logs.push_back(GetStringType() + L" RequestBody() failed.");
                return false;
            }
            // Get log of Request Body
            m_Logs.push_back(GetStringType() + L" Request Body = " + utility::conversions::utf8_to_utf16(body));
            // Set Encrypted Request Body
            request->set_body(body, "application/json");
            return true;
        }
    }

    pplx::cancellation_token_source m_CTS;
    const pplx::cancellation_token m_CancellationToken;
    const RequestConfig m_RequestConfig;
    int m_StatusCode = -1;
    std::wstring m_Body;
    std::list<std::wstring> m_Logs;
};

class CallContextForWindowsDeviceLogsImpl :  public CallContextImplBase<CallContextForWindowsDeviceLogs>
{
private:
    const std::wstring &m_EventLog;

public:
    CallContextForWindowsDeviceLogsImpl(const RequestConfig &requestConfig, const std::wstring &eventLog)
        : CallContextImplBase(requestConfig)
        , m_EventLog(eventLog)
    {
    }

    virtual ~CallContextForWindowsDeviceLogsImpl() = default;

    void AppendPath(uri_builder* uriBuilder) override
    {
        uriBuilder->append_path(ADDITIONAL_PATH_WINDOWS_DEVICE_LOGS);
    }

    bool RequestBody(std::string* requestBody) override
    {
        *requestBody = utility::conversions::to_utf8string(m_EventLog);
        return true;
    }
};

class CallContextForWindowsWorkLogsImpl : public CallContextImplBase<CallContextForWindowsWorkLogs>
{
private:
    const RequestWorkLogsType m_RequestWorkLogsType;

public:
    CallContextForWindowsWorkLogsImpl(const RequestConfig& requestConfig, RequestWorkLogsType requestWorkLogsType)
        : CallContextImplBase(requestConfig)
        , m_RequestWorkLogsType(requestWorkLogsType)
    {
    }

    virtual ~CallContextForWindowsWorkLogsImpl() = default;

    void AppendPath(uri_builder* uriBuilder) override
    {
        uriBuilder->append_path(ADDITIONAL_PATH_WINDOWS_WORK_LOGS);
    }

    bool RequestBody(std::string* requestBody) override
    {
        web::json::value body;
        switch (m_RequestWorkLogsType)
        {
        case REQUEST_WORK_LOGS_TYPE_START:
            body[JSON_FIELD_WORK_STATUS] = web::json::value::string(JSON_VALUE_START);
            break;
        case REQUEST_WORK_LOGS_TYPE_END:
            body[JSON_FIELD_WORK_STATUS] = web::json::value::string(JSON_VALUE_END);
            break;
        default:
            LOG_WARN("The m_RequestWorkLogsType is unexpected value %d.", m_RequestWorkLogsType);
            return false;
        }

        *requestBody = utility::conversions::to_utf8string(body.serialize().c_str());
        return true;
    }
};

class CallContextForTimeCardsTodayImpl : public CallContextImplBase<CallContextForTimeCardsToday>
{
public:
    explicit CallContextForTimeCardsTodayImpl(const RequestConfig& requestConfig)
        : CallContextImplBase(requestConfig)
    {
    }

    virtual ~CallContextForTimeCardsTodayImpl() = default;

    void AppendPath(uri_builder* uriBuilder) override
    {
        uriBuilder->append_path(ADDITIONAL_PATH_WINDOWS_TIMECARDS_TODAY);
    }

    bool RequestBody(std::string* requestBody) override
    {
        (void)requestBody;  // Unused

        return true;
    }
};

class CallContextForWindowsUserConditionLogsImpl : public CallContextImplBase<CallContextForWindowsUserConditionLogs>
{
private:
    const RequestHealthStatusType m_RequestHealthStatusType;

public:
    CallContextForWindowsUserConditionLogsImpl(const RequestConfig& requestConfig, RequestHealthStatusType requestHealthStatusType)
        : CallContextImplBase(requestConfig)
        , m_RequestHealthStatusType(requestHealthStatusType)
    {
    }

    virtual ~CallContextForWindowsUserConditionLogsImpl() = default;

    void AppendPath(uri_builder * uriBuilder) override
    {
        uriBuilder->append_path(ADDITIONAL_PATH_WINDOWS_USER_CONDITION_LOGS);
    }

    bool RequestBody(std::string * requestBody) override
    {
        web::json::value body;
        switch (m_RequestHealthStatusType)
        {
        case RequestHealthStatusType::REQUEST_HEALTH_STATUS_TYPE_GOOD:
            body[JSON_FIELD_CONDITION] = web::json::value::string(JSON_VALUE_GOOD_HEALTH);
            break;
        case RequestHealthStatusType::REQUEST_HEALTH_STATUS_TYPE_NORMAL:
            body[JSON_FIELD_CONDITION] = web::json::value::string(JSON_VALUE_NORMAL_HEALTH);
            break;
        case RequestHealthStatusType::REQUEST_HEALTH_STATUS_TYPE_BAD:
            body[JSON_FIELD_CONDITION] = web::json::value::string(JSON_VALUE_BAD_HEALTH);
            break;
        default:
            LOG_WARN("The m_RequestHealthStatusType is unexpected value %d.", m_RequestHealthStatusType);
            return false;
        }

        *requestBody = utility::conversions::to_utf8string(body.serialize().c_str());
        return true;
    }
};

class CallContextForWindowsDayOffRequestsImpl : public CallContextImplBase<CallContextForWindowsDayOffRequests>
{
public:
    explicit CallContextForWindowsDayOffRequestsImpl(const RequestConfig& requestConfig)
        : CallContextImplBase(requestConfig)
    {
    }

    virtual ~CallContextForWindowsDayOffRequestsImpl() = default;

    void AppendPath(uri_builder* uriBuilder) override
    {
        uriBuilder->append_path(ADDITIONAL_PATH_WINDOWS_DAY_OFF_REQUESTS);
    }

    bool RequestBody(std::string* requestBody) override
    {
        (void)requestBody;  // Unused

        return true;
    }
};

class CallContextForWindowsApplicationLogsImpl : public CallContextImplBase<CallContextForWindowsApplicationLogs>
{
private:
    const std::wstring& m_EventLog;

public:
    CallContextForWindowsApplicationLogsImpl(const RequestConfig& requestConfig, const std::wstring& eventLog)
        : CallContextImplBase(requestConfig)
        , m_EventLog(eventLog)
    {
    }

    virtual ~CallContextForWindowsApplicationLogsImpl() = default;

    void AppendPath(uri_builder* uriBuilder) override
    {
        uriBuilder->append_path(ADDITIONAL_PATH_WINDOWS_APPLICATION_LOGS);
    }

    bool RequestBody(std::string* requestBody) override
    {
        *requestBody = utility::conversions::to_utf8string(m_EventLog);
        return true;
    }
};

class CallContextForWindowsAuthenticateImpl : public CallContextImplBase<CallContextForWindowsAuthenticate>
{
private:
    const std::wstring& m_CompanyCode;
    const std::wstring& m_ActivationCode;

public:
    CallContextForWindowsAuthenticateImpl(const RequestConfig& requestConfig, const std::wstring& companyCode, const std::wstring& activationCode)
        : CallContextImplBase(requestConfig)
        , m_CompanyCode(companyCode)
        , m_ActivationCode(activationCode)
    {
    }

    virtual ~CallContextForWindowsAuthenticateImpl() = default;

    void AppendPath(uri_builder* uriBuilder) override
    {
        uriBuilder->append_path(ADDITIONAL_PATH_WINDOWS_AUTHENTICATE);
    }

    bool RequestBody(std::string* requestBody) override
    {
        if (m_CompanyCode.empty() || m_ActivationCode.empty())
        {
            return false;
        }
        else
        {
            web::json::value body;
            body[JSON_FIELD_COMPANY_CODE] = web::json::value::string(m_CompanyCode);
            body[JSON_FIELD_ACTIVATION_CODE] = web::json::value::string(m_ActivationCode);

            *requestBody = utility::conversions::to_utf8string(body.serialize().c_str());
            return true;
        }
    }
};

class CallContextForWindowsWorkTargetImpl : public CallContextImplBase<CallContextForWindowsWorkTarget>
{
private:
    const std::wstring& m_WorkTarget;

public:
    CallContextForWindowsWorkTargetImpl(const RequestConfig& requestConfig, const std::wstring& workTarget)
        : CallContextImplBase(requestConfig)
        , m_WorkTarget(workTarget)
    {
    }

    ~CallContextForWindowsWorkTargetImpl() override = default;

    void AppendPath(uri_builder* uriBuilder) override
    {
        uriBuilder->append_path(ADDITIONAL_PATH_WINDOWS_WORK_TARGET);
    }

    bool RequestBody(std::string* requestBody) override
    {
        web::json::value body;
        body[JSON_FIELD_WORK_TARGET] = web::json::value::string(m_WorkTarget);
        *requestBody = utility::conversions::to_utf8string(body.serialize().c_str());
        return true;
    }
};

}  // namespace

CpprestRequestImpl::CpprestRequestImpl(const RequestConfig& requestConfig)
    : m_RequestConfig(requestConfig)
{
}

// For Request
std::unique_ptr<CallContextForWindowsDeviceLogs> CpprestRequestImpl::CallWindowsDeviceLogs(std::wstring &eventLog)
{
    auto ctx = std::make_unique<CallContextForWindowsDeviceLogsImpl>(m_RequestConfig, eventLog);
    return std::move(ctx);
}

std::unique_ptr<CallContextForWindowsWorkLogs> CpprestRequestImpl::CallWindowsWorkLogs(RequestWorkLogsType requestWorkLogsType)
{
    auto ctx = std::make_unique<CallContextForWindowsWorkLogsImpl>(m_RequestConfig, requestWorkLogsType);
    return std::move(ctx);
}

std::unique_ptr<CallContextForTimeCardsToday> CpprestRequestImpl::CallWindowsTimeCardsToday()
{
    auto ctx = std::make_unique<CallContextForTimeCardsTodayImpl>(m_RequestConfig);
    return std::move(ctx);
}

std::unique_ptr<CallContextForWindowsUserConditionLogs> CpprestRequestImpl::CallWindowsUserConditionLogs(RequestHealthStatusType requestHealthStatusType)
{
    auto ctx = std::make_unique<CallContextForWindowsUserConditionLogsImpl>(m_RequestConfig, requestHealthStatusType);
    return std::move(ctx);
}

std::unique_ptr<CallContextForWindowsDayOffRequests> CpprestRequestImpl::CallWindowsDayOffRequests()
{
    auto ctx = std::make_unique<CallContextForWindowsDayOffRequestsImpl>(m_RequestConfig);
    return std::move(ctx);
}

std::unique_ptr<CallContextForWindowsApplicationLogs> CpprestRequestImpl::CallWindowsApplicationLogs(const std::wstring& eventLog)
{
    auto ctx = std::make_unique<CallContextForWindowsApplicationLogsImpl>(m_RequestConfig, eventLog);
    return std::move(ctx);
}

std::unique_ptr<CallContextForWindowsAuthenticate> CpprestRequestImpl::CallWindowsAuthenticate(const std::wstring& companyCode,
                                                                                               const std::wstring& activationCode)
{
    auto ctx = std::make_unique<CallContextForWindowsAuthenticateImpl>(m_RequestConfig, companyCode, activationCode);
    return std::move(ctx);
}

std::unique_ptr<CallContextForWindowsWorkTarget> CpprestRequestImpl::CallWindowsWorkTarget(const std::wstring& workTarget)
{
    auto ctx = std::make_unique<CallContextForWindowsWorkTargetImpl>(m_RequestConfig, workTarget);
    return std::move(ctx);
}
