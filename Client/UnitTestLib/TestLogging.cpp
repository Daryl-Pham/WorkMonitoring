#include "stdafx.h"

#include <gtest/gtest.h>
#include <atlstr.h>
#include <fstream>
#include <boost/filesystem.hpp>
#include "Logging.h"
#include "Common.h"
#include "Utility.h"

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

//Test function LogFile::WriteLog
TEST(testLogging, WriteLog)
{

    //Write log succesfully
    Logging logging = Logging(Utility::GetDefaultDataDirectory(), KeyboardLogFileName);
    std::vector<std::pair<std::wstring, std::wstring>> content;
    content.push_back(std::make_pair(L",keyboard_operation", L"2020-04-04T17:28:22.000Z"));
    content.push_back(std::make_pair(L",keyboard_operation", L"2020-04-04T16:33:59.000Z"));

    logging.WriteLog(content);

    auto contentCompare = GetContent(logging.GetFilePath());

    EXPECT_EQ(L"2020-04-04T17:28:22.000Z,keyboard_operation\n"
               "2020-04-04T16:33:59.000Z,keyboard_operation\n", contentCompare);


    //Don't get handle of file because have a other process is holding file.
    std::wstring fullpathFile = logging.GetFilePath();
    HANDLE hFile = ::CreateFile(fullpathFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    content.clear();
    content.push_back(std::make_pair(L",keyboard_operation", L"2020-04-04T17:28:22.000Z"));
    content.push_back(std::make_pair(L",keyboard_operation", L"2020-04-04T16:33:59.000Z"));
    BOOL resultCheck = logging.WriteLog(content);
    CloseHandle(hFile);

    EXPECT_FALSE(resultCheck);

    // Cleanup log file.
    if (boost::filesystem::exists(logging.GetFilePath()))
    {
        boost::filesystem::remove(logging.GetFilePath());
    }

}