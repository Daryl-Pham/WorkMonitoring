#include <gtest/gtest.h>
#include "AppExecutionCondition.h"
#include "UtilityForTest.h"
#include "Mock.h"

using ::testing::ByMove;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Return;
using ::testing::_;

TEST(testAppExecutionCondition, ShouldExecute)
{
    auto selfPath = UtilityForTest::GetSelfPath();
    ASSERT_TRUE(boost::filesystem::exists(selfPath));

    DWORD sessionId = 0;
    ASSERT_TRUE(ProcessIdToSessionId(GetCurrentProcessId(), &sessionId));

    AppExecutionCondition aec;
    ASSERT_TRUE(aec.Initialize(selfPath));
    EXPECT_FALSE(aec.ShouldExecute(sessionId));
    EXPECT_TRUE(aec.ShouldExecute(sessionId + 1));
}

TEST(testAppExecutionCondition, ShouldExecuteAfterInstalledOnce)
{
    MockAppConfig ac;
    EXPECT_CALL(ac, GetExecutedBizTelework(_))
        .WillOnce(Invoke([](bool* dst)
            {
                *dst = false;
                return true;
            }));
    EXPECT_CALL(ac, SetExecutedBizTelework(Matcher<bool>(Eq(true))))
        .WillOnce(Return(true));

    AppExecutionCondition aec;
    EXPECT_TRUE(aec.ShouldExecuteAfterInstalledOnce(&ac));
}

TEST(testAppExecutionCondition, ShouldExecuteAfterInstalledOnce_InAlreadyExecuted)
{
    MockAppConfig ac;
    EXPECT_CALL(ac, GetExecutedBizTelework(_))
        .WillOnce(Invoke([](bool* dst)
            {
                *dst = true;
                return true;
            }));

    AppExecutionCondition aec;
    EXPECT_FALSE(aec.ShouldExecuteAfterInstalledOnce(&ac));
}

TEST(testAppExecutionCondition, ShouldExecuteAfterInstalledOnce_InFailedToGetExecutedBizTelework)
{
    MockAppConfig ac;
    EXPECT_CALL(ac, GetExecutedBizTelework(_))
        .WillOnce(Invoke([](bool* dst)
            {
                return false;
            }));

    AppExecutionCondition aec;
    EXPECT_FALSE(aec.ShouldExecuteAfterInstalledOnce(&ac));
}
