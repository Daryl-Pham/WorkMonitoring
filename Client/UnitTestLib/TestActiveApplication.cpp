#include "stdafx.h"

#include "gtest/gtest.h"
#include "ActiveApplication.h"
#include "Common.h"
#include <cpprest/asyncrt_utils.h>

namespace
{

    std::string GetContent(std::string path)
    {
        std::ifstream file(path);
        EXPECT_TRUE(file.is_open());
        std::stringstream buf;
        buf << file.rdbuf();
        file.close();
        return buf.str();
    }
}

TEST(testActiveApplication_NotRunOnCI, Constructor)
{
    ActiveApplication activeApp(L"application_info.ltsv");
    EXPECT_EQ(L"C:\\ProgramData\\OPTiM\\BizTelework\\application_info.ltsv", activeApp.GetLogFilePath());
}

TEST(testActiveApplication, WriteLog)
{
    wchar_t tmp[MAX_PATH];

    ASSERT_NE(0, GetModuleFileName(NULL, tmp, sizeof(tmp) / sizeof(tmp[0])));
    auto currentPath = boost::filesystem::path(tmp).parent_path();
    currentPath /= L"test_folder";
    std::wstring fileName(L"application_info.ltsv");

    // Setup
    if (boost::filesystem::exists(currentPath))
    {
        boost::filesystem::remove_all(currentPath);
    }

    ActiveApplication activeApp(currentPath.wstring(), fileName);

    // Write log
    auto zero = boost::chrono::system_clock::from_time_t(0);
    EXPECT_TRUE(activeApp.WriteLog(zero, L"chrome.exe"));
    EXPECT_TRUE(activeApp.WriteLog(zero, L"QR作成ツール.exe"));
    auto content = GetContent(utility::conversions::to_utf8string(activeApp.GetLogFilePath()));
    EXPECT_EQ("event_at:1970-01-01T00:00:00Z\tprocess_name:chrome.exe\nevent_at:1970-01-01T00:00:00Z\tprocess_name:QR作成ツール.exe\n", content);

    // Cleanup directory.
    if (boost::filesystem::exists(currentPath))
    {
        boost::filesystem::remove_all(currentPath);
    }
}
