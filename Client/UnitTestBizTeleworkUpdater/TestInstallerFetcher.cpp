#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <cpprest/http_listener.h>
#include <Windows.h>
#include <ConfigUtility.h>
#include <InstallerFetcher.h>
#include "Mock.h"

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
    std::wstring listeningUrl(L"http://localhost:34569/");
}  // namespace

TEST(testInstallerFetcher, Fetch)
{
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);
    wchar_t tmp[MAX_PATH] = { 0 };

    ASSERT_NE(0, GetModuleFileName(NULL, tmp, sizeof(tmp) / sizeof(tmp[0])));
    auto dstDir = boost::filesystem::path(tmp).parent_path();
    auto msiPath = dstDir / L"BizTelework.msi";

    MockFetchingTask ft;
    EXPECT_CALL(ft, RequestFile(Matcher<const std::wstring&>(Eq(listeningUrl + L"BizTelework.msi")),
                                Matcher<const std::wstring&>(Eq(msiPath.wstring())),
                                Matcher<HANDLE>(Eq(eventStopControl))))
        .WillOnce(Invoke([](const std::wstring& _, const std::wstring& dstFilePaht, HANDLE) {
            // Create empty file.
            std::ofstream f(dstFilePaht);
            f.close();

            return true;
        }));
    InstallerFetcher::Config config(dstDir.wstring(), listeningUrl);
    InstallerFetcher fetcher(&ft, config);

    std::wstring msFilePath;
    EXPECT_TRUE(fetcher.Fetch(&msFilePath, eventStopControl));
}

TEST(testInstallerFetcher, FetchButMsiDoesNotExist)
{
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);
    wchar_t tmp[MAX_PATH] = { 0 };

    ASSERT_NE(0, GetModuleFileName(NULL, tmp, sizeof(tmp) / sizeof(tmp[0])));
    auto dstDir = boost::filesystem::path(tmp).parent_path() / "un-exist";
    auto msiPath = dstDir / L"BizTelework.msi";

    MockFetchingTask ft;
    EXPECT_CALL(ft, RequestFile(Matcher<const std::wstring&>(Eq(listeningUrl + L"BizTelework.msi")),
                                Matcher<const std::wstring&>(Eq(msiPath.wstring())),
                                Matcher<HANDLE>(Eq(eventStopControl))))
        .WillOnce(Return(true));
    InstallerFetcher::Config config(dstDir.wstring(), listeningUrl);
    InstallerFetcher fetcher(&ft, config);

    std::wstring msFilePath;
    EXPECT_FALSE(fetcher.Fetch(&msFilePath, eventStopControl));
}

TEST(testInstallerFetcher, FetchWithBadArgument)
{
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);
    MockFetchingTask ft;
    InstallerFetcher::Config config(L"", L"");
    InstallerFetcher fetcher(&ft, config);
    std::wstring msFilePath;

    EXPECT_FALSE(fetcher.Fetch(nullptr, eventStopControl));
    EXPECT_FALSE(fetcher.Fetch(&msFilePath, nullptr));
}
