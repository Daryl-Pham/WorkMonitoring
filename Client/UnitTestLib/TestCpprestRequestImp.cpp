#include "stdafx.h"

#include "gtest/gtest.h"
#include "CpprestRequestImpl.h"
#include "RequestConfig.h"
#include "HttpMethod.h"

#include <cpprest/http_listener.h>

// テストサーバーの参考 https://github.com/microsoft/cpprestsdk/blob/master/Release/tests/functional/http/client/header_tests.cpp
// https://gist.github.com/SeanCline/6005128

using namespace web::http::experimental::listener;
using namespace web::http;
using namespace std::placeholders;

namespace
{

/* https://github.com/microsoft/cpprestsdk/blob/master/Release/tests/functional/http/client/http_client_tests.h
 * を参考に http://localhost:34568 を設定。テスト用なのでサーバーアドレスは何でも良い。 */

std::wstring apiUrl(L"http://localhost:34568/");
std::wstring companyGuid(L"COMPANY-GUID");
std::wstring agentGuid(L"AGENT-GUID");
std::wstring content = L"{\"logs\":[{\"event_at\":\"2020-04-01T00:00:00Z\",\"log_type\":\"logon\"}]}";
std::wstring contentActiveApp = L"{\"logs\":[{\"process_name\":\"chrome.exe\",\"event_at\":\"2020-04-01T00:00:00Z\"}]}";

class HandlerContext
{
public:
    HandlerContext()
    {
        Reset(L"");
    }

    ~HandlerContext() = default;

    void Reset(const std::wstring &r)
    {
        uri = L"";
        requestContentType = L"";
        requestContent = L"";
        requestXCompanyGuid = L"";
        requestXAgentGuid = L"";
        responseBody = r;
        called = false;
    }

    std::wstring uri;
    std::wstring requestContentType;
    std::wstring requestContent;
    std::wstring requestXCompanyGuid;
    std::wstring requestXAgentGuid;
    std::wstring responseBody = L"testbody";
    bool called;
};

void handler(http_request req, HandlerContext *context)
{
    context->uri = req.request_uri().to_string();
    context->requestContentType = req.headers().content_type();

    auto headers = req.headers();
    auto found = headers.find(L"X-Company-GUID");
    if (found != headers.end())
    {
        context->requestXCompanyGuid = found->second;
    }
    found = headers.find(L"X-Agent-GUID");
    if (found != headers.end())
    {
        context->requestXAgentGuid = found->second;
    }
    auto body = req.extract_string();
    context->requestContent = utility::conversions::to_utf16string(body.get());

    context->called = true;
}

void handlerPOST(http_request req, HandlerContext* context)
{
    handler(req, context);
    req.reply(status_codes::Created /* 201 */, context->responseBody);
}

void handlerGET(http_request req, HandlerContext* context)
{
    handler(req, context);
    req.reply(status_codes::OK /* 200 */, context->responseBody);
}

}  // namespace

TEST(testCpprestRequestImpl, CallWindowsDeviceLogs)
{
    RequestConfig config(apiUrl, companyGuid, agentGuid, HttpMethod::POST);
    CpprestRequestImpl request(config);

    HandlerContext handlerContext;
    handlerContext.Reset(L"testbody");

    http_listener listener(apiUrl);
    listener.open().wait();
    auto h = std::bind(handlerPOST, _1, &handlerContext);
    listener.support(methods::POST, h);

    std::unique_ptr<CallContextForWindowsDeviceLogs> context = request.CallWindowsDeviceLogs(content);

    bool check = context->Wait();

    EXPECT_TRUE(check);
    EXPECT_EQ(201, context->StatusCode());
    EXPECT_TRUE(handlerContext.called);
    EXPECT_EQ(L"/app/biz_telework/api/v1/windows/device_logs", handlerContext.uri);
    EXPECT_EQ(L"application/json", handlerContext.requestContentType);
    EXPECT_EQ(companyGuid, handlerContext.requestXCompanyGuid);
    EXPECT_EQ(agentGuid, handlerContext.requestXAgentGuid);
    EXPECT_EQ(handlerContext.requestContent, content);
    EXPECT_EQ(handlerContext.responseBody, context->Body());
    listener.close().wait();
}

TEST(testCpprestRequestImpl, CallWindowsDeviceLogsWithoutServer)
{
    RequestConfig config(apiUrl, companyGuid, agentGuid, HttpMethod::POST);
    CpprestRequestImpl request(config);

    std::unique_ptr<CallContextForWindowsDeviceLogs> context = request.CallWindowsDeviceLogs(content);

    bool check = context->Wait();

    EXPECT_FALSE(check);
    EXPECT_EQ(-1, context->StatusCode());
}



TEST(testCpprestRequestImpl, CallWindowsWorkLogs)
{
    RequestConfig config(apiUrl, companyGuid, agentGuid, HttpMethod::POST);
    CpprestRequestImpl request(config);

    HandlerContext handlerContext;
    handlerContext.Reset(L"testbody");

    http_listener listener(apiUrl);
    listener.open().wait();
    auto h = std::bind(handlerPOST, _1, &handlerContext);
    listener.support(methods::POST, h);

    // For REQUEST_WORK_LOGS_TYPE_START.
    {
        std::unique_ptr<CallContextForWindowsWorkLogs> context = request.CallWindowsWorkLogs(REQUEST_WORK_LOGS_TYPE_START);
        bool check = context->Wait();

        EXPECT_TRUE(check);
        EXPECT_EQ(201, context->StatusCode());
        EXPECT_TRUE(handlerContext.called);
        EXPECT_EQ(L"/app/biz_telework/api/v1/windows/work_logs", handlerContext.uri);
        EXPECT_EQ(L"application/json", handlerContext.requestContentType);
        EXPECT_EQ(companyGuid, handlerContext.requestXCompanyGuid);
        EXPECT_EQ(agentGuid, handlerContext.requestXAgentGuid);
        EXPECT_EQ(L"{\"work_status\":\"start\"}", handlerContext.requestContent);
        EXPECT_EQ(handlerContext.responseBody, context->Body());
    }

    // For REQUEST_WORK_LOGS_TYPE_END.
    {
        handlerContext.Reset(L"testbody");
        std::unique_ptr<CallContextForWindowsWorkLogs> context = request.CallWindowsWorkLogs(REQUEST_WORK_LOGS_TYPE_END);
        bool check = context->Wait();

        EXPECT_TRUE(check);
        EXPECT_EQ(201, context->StatusCode());
        EXPECT_TRUE(handlerContext.called);
        EXPECT_EQ(L"/app/biz_telework/api/v1/windows/work_logs", handlerContext.uri);
        EXPECT_EQ(L"application/json", handlerContext.requestContentType);
        EXPECT_EQ(companyGuid, handlerContext.requestXCompanyGuid);
        EXPECT_EQ(agentGuid, handlerContext.requestXAgentGuid);
        EXPECT_EQ(L"{\"work_status\":\"end\"}", handlerContext.requestContent);
        EXPECT_EQ(handlerContext.responseBody, context->Body());
    }

    // For unknown value.
    {
        handlerContext.Reset(L"testbody");
        std::unique_ptr<CallContextForWindowsWorkLogs> context = request.CallWindowsWorkLogs(static_cast<RequestWorkLogsType>(3));
        bool check = context->Wait();

        EXPECT_FALSE(check);
        EXPECT_EQ(-1, context->StatusCode());
        EXPECT_FALSE(handlerContext.called);
    }

    listener.close().wait();
}

TEST(testCpprestRequestImpl, CallWindowsWorkLogsWithoutServer)
{
    // CallWindowsWorkLogs method POST without server
    RequestConfig config(apiUrl, companyGuid, agentGuid, HttpMethod::POST);
    CpprestRequestImpl request(config);

    std::unique_ptr<CallContextForWindowsWorkLogs> context = request.CallWindowsWorkLogs(REQUEST_WORK_LOGS_TYPE_START);

    bool check = context->Wait();

    EXPECT_FALSE(check);
    EXPECT_EQ(-1, context->StatusCode());
}

TEST(testCpprestRequestImpl, CallWindowsTimeCardsToday)
{
    RequestConfig config(apiUrl, companyGuid, agentGuid, HttpMethod::GET);
    CpprestRequestImpl request(config);

    HandlerContext handlerContext;
    handlerContext.Reset(L"testbody");

    http_listener listener(apiUrl);
    listener.open().wait();
    auto h = std::bind(handlerGET, _1, &handlerContext);
    listener.support(methods::GET, h);

    std::unique_ptr<CallContextForTimeCardsToday> context = request.CallWindowsTimeCardsToday();

    bool check = context->Wait();

    EXPECT_TRUE(check);
    EXPECT_EQ(200, context->StatusCode());
    EXPECT_TRUE(handlerContext.called);
    EXPECT_EQ(L"/app/biz_telework/api/v1/windows/timecards/today", handlerContext.uri);
    EXPECT_EQ(L"", handlerContext.requestContentType);
    EXPECT_EQ(companyGuid, handlerContext.requestXCompanyGuid);
    EXPECT_EQ(agentGuid, handlerContext.requestXAgentGuid);
    EXPECT_EQ(handlerContext.requestContent, L"");
    EXPECT_EQ(handlerContext.responseBody, context->Body());
    listener.close().wait();
}

TEST(testCpprestRequestImpl, CallWindowsTimeCardsTodayWithoutServer)
{
    // CallWindowsTimeCardsToday method GET without server
    RequestConfig configGetMethod(apiUrl, companyGuid, agentGuid, HttpMethod::GET);
    CpprestRequestImpl requestGetMethod(configGetMethod);
    std::unique_ptr<CallContextForTimeCardsToday> contextGetMethod = requestGetMethod.CallWindowsTimeCardsToday();
    bool check = true;
    check = contextGetMethod->Wait();
    EXPECT_FALSE(check);
    EXPECT_EQ(-1, contextGetMethod->StatusCode());
}

TEST(testCpprestRequestImpl, CallWindowsUserConditionLogs)
{
    RequestConfig config(apiUrl, companyGuid, agentGuid, HttpMethod::POST);
    CpprestRequestImpl request(config);

    HandlerContext handlerContext;
    handlerContext.Reset(L"testbody");

    http_listener listener(apiUrl);
    listener.open().wait();
    auto h = std::bind(handlerPOST, _1, &handlerContext);
    listener.support(methods::POST, h);

    std::unique_ptr<CallContextForWindowsUserConditionLogs> context = request.CallWindowsUserConditionLogs(RequestHealthStatusType::REQUEST_HEALTH_STATUS_TYPE_GOOD);

    bool check = context->Wait();

    EXPECT_TRUE(check);
    EXPECT_EQ(201, context->StatusCode());
    EXPECT_TRUE(handlerContext.called);
    EXPECT_EQ(L"/app/biz_telework/api/v1/windows/user_condition_logs", handlerContext.uri);
    EXPECT_EQ(L"application/json", handlerContext.requestContentType);
    EXPECT_EQ(companyGuid, handlerContext.requestXCompanyGuid);
    EXPECT_EQ(agentGuid, handlerContext.requestXAgentGuid);
    EXPECT_EQ(L"{\"condition\":\"good\"}", handlerContext.requestContent);
    EXPECT_EQ(handlerContext.responseBody, context->Body());
    listener.close().wait();
}

TEST(testCpprestRequestImpl, CallWindowsUserConditionLogsWithoutServer)
{
    // CallWindowsUserConditionLogs method POST without server
    RequestConfig config(apiUrl, companyGuid, agentGuid, HttpMethod::POST);
    CpprestRequestImpl request(config);
    std::unique_ptr<CallContextForWindowsUserConditionLogs> contextGetMethod = request.CallWindowsUserConditionLogs(RequestHealthStatusType::REQUEST_HEALTH_STATUS_TYPE_GOOD);
    bool check = true;
    check = contextGetMethod->Wait();
    EXPECT_FALSE(check);
    EXPECT_EQ(-1, contextGetMethod->StatusCode());
}

TEST(testCpprestRequestImpl, CallWindowsDayOffRequests)
{
    RequestConfig config(apiUrl, companyGuid, agentGuid, HttpMethod::POST);
    CpprestRequestImpl request(config);

    HandlerContext handlerContext;
    handlerContext.Reset(L"testbody");

    http_listener listener(apiUrl);
    listener.open().wait();
    auto h = std::bind(handlerPOST, _1, &handlerContext);
    listener.support(methods::POST, h);

    std::unique_ptr<CallContextForWindowsDayOffRequests> context = request.CallWindowsDayOffRequests();

    bool check = context->Wait();

    EXPECT_TRUE(check);
    EXPECT_EQ(201, context->StatusCode());
    EXPECT_TRUE(handlerContext.called);
    EXPECT_EQ(L"/app/biz_telework/api/v1/windows/day_off_requests", handlerContext.uri);
    EXPECT_EQ(L"application/json", handlerContext.requestContentType);
    EXPECT_EQ(companyGuid, handlerContext.requestXCompanyGuid);
    EXPECT_EQ(agentGuid, handlerContext.requestXAgentGuid);
    EXPECT_EQ(L"", handlerContext.requestContent);
    EXPECT_EQ(handlerContext.responseBody, context->Body());
    listener.close().wait();
}

TEST(testCpprestRequestImpl, CallWindowsDayOffRequestsWithoutServer)
{
    // CallWindowsDayOffRequests method POST without server
    RequestConfig config(apiUrl, companyGuid, agentGuid, HttpMethod::POST);
    CpprestRequestImpl request(config);
    std::unique_ptr<CallContextForWindowsDayOffRequests> contextGetMethod = request.CallWindowsDayOffRequests();
    bool check = true;
    check = contextGetMethod->Wait();
    EXPECT_FALSE(check);
    EXPECT_EQ(-1, contextGetMethod->StatusCode());
}

TEST(testCpprestRequestImpl, CallWindowsApplicationLogs)
{
    RequestConfig config(apiUrl, companyGuid, agentGuid, HttpMethod::POST);
    CpprestRequestImpl request(config);

    HandlerContext handlerContext;
    handlerContext.Reset(L"testbody");

    http_listener listener(apiUrl);
    listener.open().wait();
    auto h = std::bind(handlerPOST, _1, &handlerContext);
    listener.support(methods::POST, h);

    std::unique_ptr<CallContextForWindowsApplicationLogs> context = request.CallWindowsApplicationLogs(contentActiveApp);

    bool check = context->Wait();

    EXPECT_TRUE(check);
    EXPECT_EQ(201, context->StatusCode());
    EXPECT_TRUE(handlerContext.called);
    EXPECT_EQ(L"/app/biz_telework/api/v1/windows/application_logs", handlerContext.uri);
    EXPECT_EQ(L"application/json", handlerContext.requestContentType);
    EXPECT_EQ(companyGuid, handlerContext.requestXCompanyGuid);
    EXPECT_EQ(agentGuid, handlerContext.requestXAgentGuid);
    EXPECT_EQ(handlerContext.requestContent, contentActiveApp);
    EXPECT_EQ(handlerContext.responseBody, context->Body());
    listener.close().wait();
}

TEST(testCpprestRequestImpl, CallWindowsApplicationLogsWithoutServer)
{
    // CallWindowsApplicationLogs method POST without server
    RequestConfig config(apiUrl, companyGuid, agentGuid, HttpMethod::POST);
    CpprestRequestImpl request(config);
    std::unique_ptr<CallContextForWindowsApplicationLogs> contextGetMethod = request.CallWindowsApplicationLogs(contentActiveApp);
    bool check = true;
    check = contextGetMethod->Wait();
    EXPECT_FALSE(check);
    EXPECT_EQ(-1, contextGetMethod->StatusCode());
}
