#include "stdafx.h"
#include "gtest/gtest.h"
#include "LastActivityLog.h"
#include "Utility.h"
#include "boost/chrono.hpp"
#include "boost/chrono/duration.hpp"

TEST(testLastActivityLog, SentLog)
{
    LastActivityLog lastActivityLog(L"last_get_log_test.txt");
    auto now = boost::chrono::system_clock::now();
    auto duration = now - boost::chrono::seconds(1800);// Duration equals 30 minutes.
    lastActivityLog.WriteLastTime(Utility::ConvertTimePointToUnixTime(duration));
    EXPECT_TRUE(lastActivityLog.IsSendLog(now));

    duration = now - boost::chrono::seconds(1801); // Duration more 30 minutes.
    lastActivityLog.WriteLastTime(Utility::ConvertTimePointToUnixTime(duration));
    EXPECT_TRUE(lastActivityLog.IsSendLog(now));
}

TEST(testLastActivityLog, NotSenLog)
{
    LastActivityLog lastActivityLog(L"last_get_log_test.txt");
    auto now = boost::chrono::system_clock::now();
    auto duration = now - boost::chrono::seconds(1799); // Duration less than 30 minutes.
    lastActivityLog.WriteLastTime(Utility::ConvertTimePointToUnixTime(duration));
    EXPECT_FALSE(lastActivityLog.IsSendLog(now));
}
