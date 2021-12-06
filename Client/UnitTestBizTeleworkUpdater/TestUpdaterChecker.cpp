#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cpprest/http_listener.h>
#include <UpdateChecker.h>
#include "Mock.h"

using ::testing::ByMove;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Return;
using ::testing::_;
using namespace std::placeholders;
using namespace web::http::experimental::listener;
using namespace web::http;

namespace
{
}  // namespace

TEST(testUpdaterChecker, Check)
{
    const wchar_t nextVersion[] = L"1.3.4.2";
    const wchar_t currentVersion[] = L"1.2.3.4";
    std::wstring listeningUrl = L"http://localhost:34568/";
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);

    std::unique_ptr<AppVersion> appVersion = AppVersion::Parse(currentVersion);
    ASSERT_NE(nullptr, appVersion);
    auto config = std::make_unique<UpdateChecker::Config>(listeningUrl, *appVersion);

    MockFetchingTask ft;
    UpdateChecker updateChecker(std::move(config), &ft);

    // Succeeded to request
    EXPECT_CALL(ft, RequestBodyString(Matcher<const std::wstring&>(Eq(listeningUrl + L"windows/version.txt")), _, Matcher<HANDLE>(Eq(eventStopControl))))
        .WillOnce(Invoke([nextVersion](const std::wstring&, std::wstring* dstBodyString, HANDLE)
            {
                *dstBodyString = nextVersion;
                return true;
            }));
    EXPECT_TRUE(updateChecker.CheckUpdating(eventStopControl));

    // Succeeded to request but same version.
    EXPECT_CALL(ft, RequestBodyString(Matcher<const std::wstring&>(Eq(listeningUrl + L"windows/version.txt")), _, Matcher<HANDLE>(Eq(eventStopControl))))
        .WillOnce(Invoke([currentVersion](const std::wstring&, std::wstring* dstBodyString, HANDLE)
            {
                *dstBodyString = currentVersion;
                return true;
            }));
    EXPECT_FALSE(updateChecker.CheckUpdating(eventStopControl));

    // Failed to request.
    EXPECT_CALL(ft, RequestBodyString(Matcher<const std::wstring&>(Eq(listeningUrl + L"windows/version.txt")), _, Matcher<HANDLE>(Eq(eventStopControl))))
        .WillOnce(Return(false));
    EXPECT_FALSE(updateChecker.CheckUpdating(eventStopControl));
}
