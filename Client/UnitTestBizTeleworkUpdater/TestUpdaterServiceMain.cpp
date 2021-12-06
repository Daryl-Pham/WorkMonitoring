#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <UpdaterServiceMain.h>
#include <UpdaterService.h>
#include "Mock.h"

using ::testing::Return;
using ::testing::Matcher;
using ::testing::Eq;


TEST(testUpdaterServiceMain, ServiceMain)
{
    std::unique_ptr<MockUpdaterServiceInterface> usi(new MockUpdaterServiceInterface());
    EXPECT_CALL(*usi, PreRun(Matcher<LPCWSTR>(Eq(L"OptimalBizTeleworkUpdater"))))
        .WillOnce(Return(true));
    EXPECT_CALL(*usi, Run())
        .WillOnce(Return(true));

    std::unique_ptr<MockAppExecutor> ae(new MockAppExecutor());
    EXPECT_CALL(*ae, ExecuteOnEachSession());

    UpdaterServiceMain::SetUpdaterService(std::move(usi));
    UpdaterServiceMain::SetAppExecutor(std::move(ae));

    UpdaterServiceMain::ServiceMain(0, nullptr);

    UpdaterServiceMain::SetUpdaterService(nullptr);
    UpdaterServiceMain::SetAppExecutor(nullptr);
}
