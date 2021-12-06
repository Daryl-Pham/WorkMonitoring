#include "stdafx.h"

#include "gtest/gtest.h"
#include "LogFile.h"
#include <sstream>
#include <string>

TEST(testLogFile, ReadLogToJsonStringify)
{
    LogFile logFile;

    std::wstringbuf content(L"2020-04-01T00:00:00Z,logon");
    std::wistream logStream(&content);
    LogLines logLines;

    BOOL check = logFile.ReadLog(logStream, logLines);
    EXPECT_TRUE(check);

    LogLines expected = { {L"2020-04-01T00:00:00Z", L"logon"} };
    EXPECT_EQ(expected, logLines);
}

TEST(testLogFile, ReadLogToJsonStringifyWithEmpty)
{
    LogFile logFile;

    std::wstringbuf content(L"");
    std::wistream logStream(&content);
    LogLines logLines;

    BOOL check = logFile.ReadLog(logStream, logLines);
    EXPECT_TRUE(check);
    EXPECT_EQ(0, logLines.size());
}

TEST(testLogFile, StringifyAsJsonBody)
{
    LogLines logLines = {
        {L"2020-04-01T00:00:00Z", L"logon"},
    };

    std::wstring out;
    BOOL check = LogFile::StringifyAsJsonBody(logLines, out);
    EXPECT_TRUE(check);
    EXPECT_EQ(L"{\"logs\":[{\"event_at\":\"2020-04-01T00:00:00Z\",\"log_type\":\"logon\"}]}", out);

    out = L"";
    logLines = {};
    check = LogFile::StringifyAsJsonBody(logLines, out);
    EXPECT_TRUE(check);
}
