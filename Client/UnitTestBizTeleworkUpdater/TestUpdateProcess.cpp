#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <UpdateProcess.h>
#include "Mock.h"

using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Return;
using ::testing::_;

TEST(testUpdateProcess, Execute)
{
    std::wstring msiPath = L"dummy msi path";
    auto msiPathMatcher = Matcher<const std::wstring&>(Eq(msiPath));
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);
    auto eventStopControlMatcher = Matcher<HANDLE>(Eq(eventStopControl));

    std::unique_ptr<MockUpdateChecker> uc(new MockUpdateChecker());
    EXPECT_CALL(*uc, CheckUpdating(eventStopControlMatcher))
        .WillOnce(Return(true));
    MockFetchingTask ft;
    std::unique_ptr<MockInstallerFetcher> f(new MockInstallerFetcher(&ft));
    EXPECT_CALL(*f, Fetch(_, eventStopControlMatcher))
        .WillOnce(Invoke([&](std::wstring * dstMsiFilePath, HANDLE)
            {
                *dstMsiFilePath = msiPath;
                return true;
            }));
    std::unique_ptr<MockInstallerValidator> v(new MockInstallerValidator());
    EXPECT_CALL(*v, ValidateMsiFile(msiPathMatcher))
        .WillOnce(Return(true));
    EXPECT_CALL(*v, ValidateSignature(msiPathMatcher))
        .WillOnce(Return(true));
    EXPECT_CALL(*v, ValidateUpgradeCode(msiPathMatcher))
        .WillOnce(Return(true));
    EXPECT_CALL(*v, ValidateVersion(msiPathMatcher))
        .WillOnce(Return(true));

    std::unique_ptr<MockInstallerExecutor> ie(new MockInstallerExecutor());
    EXPECT_CALL(*ie, Execute(msiPathMatcher, eventStopControlMatcher))
        .WillOnce(Return(true));

    UpdateProcess up(std::move(uc),
                     std::move(f),
                     std::move(v),
                     std::move(ie));

    EXPECT_TRUE(up.Execute(eventStopControl));
}

TEST(testUpdateProcess, ExecuteWithouMemberValues)
{
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);

    {
        MockFetchingTask ft;
        std::unique_ptr<MockInstallerFetcher> f(new MockInstallerFetcher(&ft));
        std::unique_ptr<MockInstallerValidator> v(new MockInstallerValidator());
        std::unique_ptr<MockInstallerExecutor> ie(new MockInstallerExecutor());
        UpdateProcess up(nullptr, std::move(f), std::move(v), std::move(ie));
        EXPECT_FALSE(up.Execute(eventStopControl));
    }

    {
        std::unique_ptr<MockUpdateChecker> uc(new MockUpdateChecker());
        std::unique_ptr<MockInstallerValidator> v(new MockInstallerValidator());
        std::unique_ptr<MockInstallerExecutor> ie(new MockInstallerExecutor());
        UpdateProcess up(std::move(uc), nullptr, std::move(v), std::move(ie));
        EXPECT_FALSE(up.Execute(eventStopControl));
    }

    {
        std::unique_ptr<MockUpdateChecker> uc(new MockUpdateChecker());
        MockFetchingTask ft;
        std::unique_ptr<MockInstallerFetcher> f(new MockInstallerFetcher(&ft));
        std::unique_ptr<MockInstallerExecutor> ie(new MockInstallerExecutor());
        UpdateProcess up(std::move(uc), std::move(f), nullptr, std::move(ie));
        EXPECT_FALSE(up.Execute(eventStopControl));
    }

    {
        std::unique_ptr<MockUpdateChecker> uc(new MockUpdateChecker());
        MockFetchingTask ft;
        std::unique_ptr<MockInstallerFetcher> f(new MockInstallerFetcher(&ft));
        std::unique_ptr<MockInstallerValidator> v(new MockInstallerValidator());
        UpdateProcess up(std::move(uc), std::move(f), std::move(v), nullptr);
        EXPECT_FALSE(up.Execute(eventStopControl));
    }

    {
        UpdateProcess up(nullptr, nullptr, nullptr, nullptr);
        EXPECT_FALSE(up.Execute(eventStopControl));
    }
}

TEST(testUpdateProcess, ExecuteWithUpdateCheckerFailed)
{
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);
    auto eventStopControlMatcher = Matcher<HANDLE>(Eq(eventStopControl));
    std::unique_ptr<MockUpdateChecker> uc(new MockUpdateChecker());
    EXPECT_CALL(*uc, CheckUpdating(eventStopControlMatcher))
        .WillOnce(Return(false));
    MockFetchingTask ft;
    std::unique_ptr<MockInstallerFetcher> f(new MockInstallerFetcher(&ft));
    std::unique_ptr<MockInstallerValidator> v(new MockInstallerValidator());
    std::unique_ptr<MockInstallerExecutor> ie(new MockInstallerExecutor());

    UpdateProcess up(std::move(uc),
                     std::move(f),
                     std::move(v),
                     std::move(ie));

    EXPECT_FALSE(up.Execute(eventStopControl));
}

TEST(testUpdateProcess, ExecuteWithInstallerFetcherFailed)
{
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);
    auto eventStopControlMatcher = Matcher<HANDLE>(Eq(eventStopControl));
    std::unique_ptr<MockUpdateChecker> uc(new MockUpdateChecker());
    EXPECT_CALL(*uc, CheckUpdating(eventStopControl))
        .WillOnce(Return(true));
    MockFetchingTask ft;
    std::unique_ptr<MockInstallerFetcher> f(new MockInstallerFetcher(&ft));
    EXPECT_CALL(*f, Fetch(_, Matcher<HANDLE>(Eq(eventStopControl))))
        .WillOnce(Return(false));
    std::unique_ptr<MockInstallerValidator> v(new MockInstallerValidator());
    std::unique_ptr<MockInstallerExecutor> ie(new MockInstallerExecutor());

    UpdateProcess up(std::move(uc),
        std::move(f),
        std::move(v),
        std::move(ie));

    EXPECT_FALSE(up.Execute(eventStopControl));
}

TEST(testUpdateProcess, ExecuteWithInstallerValidatorFailed)
{
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);
    auto eventStopControlMatcher = Matcher<HANDLE>(Eq(eventStopControl));
    std::unique_ptr<MockUpdateChecker> uc(new MockUpdateChecker());
    EXPECT_CALL(*uc, CheckUpdating(eventStopControlMatcher))
        .WillRepeatedly(Return(true));
    MockFetchingTask ft;
    std::unique_ptr<MockInstallerFetcher> f(new MockInstallerFetcher(&ft));
    EXPECT_CALL(*f, Fetch(_, Matcher<HANDLE>(Eq(eventStopControl))))
        .WillRepeatedly(Return(true));

    MockInstallerValidator* miv = new MockInstallerValidator();  // Deleted by v object.
    std::unique_ptr<MockInstallerValidator> v(miv);

    std::unique_ptr<MockInstallerExecutor> ie(new MockInstallerExecutor());

    UpdateProcess up(std::move(uc),
        std::move(f),
        std::move(v),
        std::move(ie));


    EXPECT_CALL(*miv, ValidateMsiFile(_))
        .WillOnce(Return(false));
    EXPECT_FALSE(up.Execute(eventStopControl));

    EXPECT_CALL(*miv, ValidateMsiFile(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*miv, ValidateSignature(_))
        .WillOnce(Return(false));
    EXPECT_FALSE(up.Execute(eventStopControl));

    EXPECT_CALL(*miv, ValidateMsiFile(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*miv, ValidateSignature(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*miv, ValidateUpgradeCode(_))
        .WillOnce(Return(false));
    EXPECT_FALSE(up.Execute(eventStopControl));

    EXPECT_CALL(*miv, ValidateMsiFile(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*miv, ValidateSignature(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*miv, ValidateUpgradeCode(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*miv, ValidateVersion(_))
        .WillOnce(Return(false));
    EXPECT_FALSE(up.Execute(eventStopControl));
}

TEST(testUpdateProcess, ExecuteWithInstallerExecutorFailed)
{
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);
    auto eventStopControlMatcher = Matcher<HANDLE>(Eq(eventStopControl));
    std::unique_ptr<MockUpdateChecker> uc(new MockUpdateChecker());
    EXPECT_CALL(*uc, CheckUpdating(eventStopControlMatcher))
        .WillOnce(Return(true));

    MockFetchingTask ft;
    std::unique_ptr<MockInstallerFetcher> f(new MockInstallerFetcher(&ft));
    EXPECT_CALL(*f, Fetch(_, eventStopControlMatcher))
        .WillOnce(Return(true));

    std::unique_ptr<MockInstallerValidator> v(new MockInstallerValidator());
    EXPECT_CALL(*v, ValidateMsiFile(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*v, ValidateSignature(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*v, ValidateUpgradeCode(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*v, ValidateVersion(_))
        .WillOnce(Return(true));

    std::unique_ptr<MockInstallerExecutor> ie(new MockInstallerExecutor());
    EXPECT_CALL(*ie, Execute(_, eventStopControlMatcher))
        .WillOnce(Return(false));

    UpdateProcess up(std::move(uc),
        std::move(f),
        std::move(v),
        std::move(ie));

    EXPECT_FALSE(up.Execute(eventStopControl));
}
