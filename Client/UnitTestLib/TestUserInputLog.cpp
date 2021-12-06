#include "stdafx.h"

#include "gtest/gtest.h"
#include "UserInputLog.h"
#include "Common.h"

namespace
{

std::wstring GetContent(std::wstring path)
{
    std::wifstream file(path);
    EXPECT_TRUE(file.is_open());
    std::wstringstream buf;
    buf << file.rdbuf();

    file.close();

    return buf.str();
}

}

TEST(testUserInputLog, Constructor)
{
    {
        UserInputLog uil(KeyboardLogFileName, LogTypeKeyboard);
        EXPECT_EQ(L"C:\\ProgramData\\OPTiM\\BizTelework\\keyboard_logs.csv", uil.GetFilePath());
    }

    {
        UserInputLog uil(MouseLogFileName, LogTypeMouse);
        EXPECT_EQ(L"C:\\ProgramData\\OPTiM\\BizTelework\\mouse_logs.csv", uil.GetFilePath());
    }
}

TEST(testUserInputLog, WriteLogOnTimeCondition)
{
    wchar_t tmp[MAX_PATH];

    ASSERT_NE(0, GetModuleFileName(NULL, tmp, sizeof(tmp) / sizeof(tmp[0])));
    auto currentPath = boost::filesystem::path(tmp).parent_path();
    currentPath /= L"test_folder";
    std::wstring fileName(L"hoge.csv");

    // Setup
    if (boost::filesystem::exists(currentPath))
    {
        boost::filesystem::remove_all(currentPath);
    }

    UserInputLog uil(currentPath.wstring(), fileName, LogTypeKeyboard);

    // Write the first line.
    auto zero = boost::chrono::system_clock::from_time_t(0);
    EXPECT_TRUE(uil.WriteLogOnTimeCondition(zero));
    auto content = GetContent(uil.GetFilePath());
    EXPECT_EQ(L"1970-01-01T00:00:00Z,keyboard_operation\n", content);

    // There is no new line because the specified time has not passed.
    EXPECT_TRUE(uil.WriteLogOnTimeCondition(zero));
    content = GetContent(uil.GetFilePath());
    EXPECT_EQ(L"1970-01-01T00:00:00Z,keyboard_operation\n", content);

    // There are 2 lines because the specified time has passed.
    auto passed5Minutes = boost::chrono::system_clock::from_time_t(5 * 60);
    EXPECT_TRUE(uil.WriteLogOnTimeCondition(passed5Minutes));
    content = GetContent(uil.GetFilePath());
    EXPECT_EQ(L"1970-01-01T00:00:00Z,keyboard_operation\n"
              L"1970-01-01T00:05:00Z,keyboard_operation\n", content);

    // Cleanup directory.
    if (boost::filesystem::exists(currentPath))
    {
        boost::filesystem::remove_all(currentPath);
    }
}
