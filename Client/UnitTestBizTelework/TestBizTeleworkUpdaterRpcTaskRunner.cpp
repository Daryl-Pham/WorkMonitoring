#include "pch.h"
#include <boost/dll.hpp>
#include <cpprest/http_listener.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <BizTeleworkUpdaterRpcTaskRunner.h>
#include "Mock.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ByMove;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Eq;
using namespace web::http::experimental::listener;
using namespace web::http;

namespace
{
    std::wstring apiUrl(L"http://localhost:34568/");
} // namespace

TEST(testBizTeleworkUpdaterRpcTaskRunner, Start)
{
    auto dstPath = boost::dll::program_location().parent_path() / L"dst.msi";
    auto bturc = new MockBizTeleworkUpdaterRpcClient();
    EXPECT_CALL(*bturc, IsBound())
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*bturc, Bind())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*bturc, PollFetchingTask(_, _))
        .WillOnce(Invoke([&dstPath](std::wstring* url, std::wstring* dstFilePath) {
                *url = apiUrl + L"/dst.msi";
                *dstFilePath = dstPath.wstring();
                return S_OK;
            }));
    EXPECT_CALL(*bturc, PushFetchingResult(Matcher<bool>(Eq(true))));
    
    EXPECT_CALL(*bturc, Unbind());

    std::wstring fileContent = L"dummy_content";
    http_listener listener(apiUrl);
    listener.support(methods::GET, [&fileContent](http_request req) {
        req.reply(status_codes::OK /* 200 */, fileContent);
    });
    listener.open().wait();

    std::unique_ptr<MockBizTeleworkUpdaterRpcClient> client(bturc);
    BizTeleworkUpdaterRpcTaskRunner runner(std::move(client));
    EXPECT_TRUE(runner.Start());

    Sleep(1000);

    runner.Shutdown();
    listener.close().wait();

    std::wstring c;
    std::wifstream stream(dstPath.c_str());
    stream >> c;
    stream.close();
    EXPECT_EQ(c, fileContent);
}

TEST(testBizTeleworkUpdaterRpcTaskRunner, BadArgument)
{
    BizTeleworkUpdaterRpcTaskRunner runner(nullptr);
    EXPECT_FALSE(runner.Start());
}

// This test case for https://biztelework.atlassian.net/browse/BTC-769.
TEST(testBizTeleworkUpdaterRpcTaskRunner, TruncateDestinationFile)
{
    auto dstPath = boost::dll::program_location().parent_path() / L"dst.msi";
    auto bturc = new MockBizTeleworkUpdaterRpcClient();
    EXPECT_CALL(*bturc, IsBound())
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*bturc, Bind())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*bturc, PollFetchingTask(_, _))
        .WillOnce(Invoke([&dstPath](std::wstring* url, std::wstring* dstFilePath) {
                *url = apiUrl + L"/dst.msi";
                *dstFilePath = dstPath.wstring();
                return S_OK;
            }));
    EXPECT_CALL(*bturc, PushFetchingResult(Matcher<bool>(Eq(true))));

    EXPECT_CALL(*bturc, Unbind());

    std::wfstream largeFile(dstPath.wstring(), std::ios_base::out);
    largeFile << L"LargeLargeLargeContent";
    largeFile.close();

    std::wstring fileContent = L"small_content";
    http_listener listener(apiUrl);
    listener.support(methods::GET, [&fileContent](http_request req) {
        req.reply(status_codes::OK /* 200 */, fileContent);
    });
    listener.open().wait();

    std::unique_ptr<MockBizTeleworkUpdaterRpcClient> client(bturc);
    BizTeleworkUpdaterRpcTaskRunner runner(std::move(client));
    EXPECT_TRUE(runner.Start());

    Sleep(1000);

    runner.Shutdown();
    listener.close().wait();

    std::wstring c;
    std::wifstream stream(dstPath.c_str());
    stream >> c;
    stream.close();
    EXPECT_EQ(c, fileContent);
}
