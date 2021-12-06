#include <gtest/gtest.h>
#include "Mock.h"
#include <AppConfig.h>

using ::testing::ByMove;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Return;
using ::testing::_;

TEST(testAppExecutor, Execute)
{
    using bfs = boost::filesystem::path;
    bfs exePath(L"hoge.exe");
    DWORD sid1 = 1;
    DWORD sid2 = 2;
    LPVOID env = reinterpret_cast<LPVOID>(3);

    std::unique_ptr<MockAppExecutionCondition> conditoin(new MockAppExecutionCondition());
    EXPECT_CALL(*conditoin, ShouldExecuteAfterInstalledOnce(_))
        .WillOnce(Return(true));
    EXPECT_CALL(*conditoin, Initialize(Matcher<const bfs&>(Eq(exePath))))
        .WillOnce(Return(true));
    EXPECT_CALL(*conditoin, ShouldExecute(Matcher<DWORD>(Eq(sid1))))
        .WillOnce(Return(false));
    EXPECT_CALL(*conditoin, ShouldExecute(Matcher<DWORD>(Eq(sid2))))
        .WillOnce(Return(true));

    std::unique_ptr<MockSessionEnumerator> enumerator(new MockSessionEnumerator());
    EXPECT_CALL(*enumerator, EnumerateSession(_))
        .WillOnce(Invoke([sid1, sid2](const std::function<void(DWORD)>& handler)
            {
                handler(sid1);
                handler(sid2);
            }));

    std::unique_ptr<MockAppExecutorContainer> aec(new MockAppExecutorContainer());
    EXPECT_CALL(*aec, GetAppExecutionCondition())
        .WillOnce(Return(ByMove(std::move(conditoin))));
    EXPECT_CALL(*aec, GetSessionEnumerator())
        .WillOnce(Return(ByMove(std::move(enumerator))));
    EXPECT_CALL(*aec, GetExePath())
        .WillOnce(Return(exePath));

    MockAppExecutorForWin32 ae(std::move(aec));
    EXPECT_CALL(ae, WTSQueryUserToken(Matcher<DWORD>(Eq(sid2)), _))
        .WillOnce(Return(TRUE));
    EXPECT_CALL(ae, CreateEnvironmentBlock(_, _, Matcher<BOOL>(Eq(FALSE))))
        .WillOnce(Invoke([&env](LPVOID* lpEnvironment, HANDLE hToken, BOOL bInherit)
            {
                *lpEnvironment = env;
                EXPECT_FALSE(bInherit);
                return TRUE;
            }));
    EXPECT_CALL(ae, CreateProcessAsUser(_, _, _, _, _, _, _, _, _, _, _))
        .WillOnce(Invoke([&exePath](HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
            LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
            LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
            {
                EXPECT_EQ(exePath.wstring(), std::wstring(lpCommandLine));
                return TRUE;
            }));
    EXPECT_CALL(ae, DestroyEnvironmentBlock(Matcher<LPVOID>(Eq(env))));
    EXPECT_CALL(ae, CloseHandle(_))
        .Times(3)
        .WillRepeatedly(Return(TRUE));

    ae.ExecuteOnEachSession();
}

TEST(testAppExecutor, ShouldNotExecuteAfterInstalledOnce)
{
    std::unique_ptr<MockAppExecutionCondition> conditoin(new MockAppExecutionCondition());
    EXPECT_CALL(*conditoin, ShouldExecuteAfterInstalledOnce(_))
        .WillOnce(Return(false));

    std::unique_ptr<MockAppExecutorContainer> aec(new MockAppExecutorContainer());
    EXPECT_CALL(*aec, GetAppExecutionCondition())
        .WillOnce(Return(ByMove(std::move(conditoin))));

    MockAppExecutorForWin32 ae(std::move(aec));

    ae.ExecuteOnEachSession();
}
