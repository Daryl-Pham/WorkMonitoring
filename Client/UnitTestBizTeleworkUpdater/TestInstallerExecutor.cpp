#include <gtest/gtest.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <Windows.h>
#include "UtilityForTest.h"
#include "InstallerExecutor.h"
#include "Mock.h"

using ::testing::Eq;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Return;
using ::testing::_;

/*
 * Tests for InstallerExecutor
 */
TEST(testInstallerExecutor, Execute)
{
    auto msiFilePath = UtilityForTest::GetMsiForTestingPath();
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);
    HANDLE misExecProcess = reinterpret_cast<HANDLE>(2);
    
    SYSTEMTIME st;
    GetLocalTime(&st);
    auto logFile = (boost::wformat(L"C:\\ProgramData\\OPTiM\\BizTelework\\log\\updater\\%04d%02d%02d%02d%02d_installer.txt") % st.wYear % st.wMonth % st.wDay % st.wHour % st.wMinute).str();

    McokInstallerExecutorForWin32 mie(InstallerExecutor::Config::Default());
    EXPECT_CALL(mie, ShellExecuteExW(_))
        .WillOnce(Invoke([&logFile, &msiFilePath, &misExecProcess](SHELLEXECUTEINFOW* pExecInfo)
            {
                EXPECT_EQ(sizeof(SHELLEXECUTEINFOW), pExecInfo->cbSize);
                EXPECT_EQ(SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS, pExecInfo->fMask);
                EXPECT_EQ(std::wstring(L"c:\\windows\\system32\\msiexec.exe"), boost::algorithm::to_lower_copy(std::wstring(pExecInfo->lpFile)));

#if 0
                // Enable this test when https://biztelework.atlassian.net/browse/BTC-508 is resolved.
                // This test success if Biz agent is installed.
                EXPECT_EQ((boost::wformat(L"/i %s /qn /lvx* \"%s\" REBOOT=ReallySuppress") % msiFilePath % logFile).str(),
                          std::wstring(pExecInfo->lpParameters));
#endif

                pExecInfo->hProcess = misExecProcess;
                return TRUE;
            }));
    EXPECT_CALL(mie, WaitForMultipleObjects(_, _, _, _))
        .WillOnce(Invoke([&misExecProcess, &eventStopControl](DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds)
            {
                EXPECT_EQ(2, nCount);
                EXPECT_EQ(misExecProcess, lpHandles[0]);
                EXPECT_EQ(eventStopControl, lpHandles[1]);
                EXPECT_FALSE(bWaitAll);
                EXPECT_EQ(INFINITE, dwMilliseconds);

                return WAIT_OBJECT_0 + 1;
            }));
    EXPECT_CALL(mie, CloseHandle(Matcher<HANDLE>(Eq(misExecProcess))))
        .WillOnce(Return(TRUE));

    EXPECT_TRUE(mie.Execute(msiFilePath.wstring(), eventStopControl));
}

TEST(testInstallerExecutor, ExecuteWithWindowsApiFailed)
{
    auto msiFilePath = UtilityForTest::GetMsiForTestingPath();
    HANDLE eventStopControl = reinterpret_cast<HANDLE>(1);
    McokInstallerExecutorForWin32 mie(InstallerExecutor::Config::Default());

    {
        EXPECT_CALL(mie, ShellExecuteExW(_))
            .WillOnce(Return(FALSE));

        EXPECT_FALSE(mie.Execute(msiFilePath.wstring(), eventStopControl));
    }

    {
        EXPECT_CALL(mie, ShellExecuteExW(_))
            .WillOnce(Return(TRUE));
        EXPECT_CALL(mie, WaitForMultipleObjects(_, _, _, _))
            .WillOnce(Return(WAIT_OBJECT_0));

        EXPECT_FALSE(mie.Execute(msiFilePath.wstring(), eventStopControl));
    }

    {
        EXPECT_CALL(mie, ShellExecuteExW(_))
            .WillOnce(Return(TRUE));
        EXPECT_CALL(mie, WaitForMultipleObjects(_, _, _, _))
            .WillOnce(Return(WAIT_FAILED));

        EXPECT_FALSE(mie.Execute(msiFilePath.wstring(), eventStopControl));
    }
}

TEST(testInstallerExecutor, ExecuteWithBadArgument)
{
    InstallerExecutor ie;

    EXPECT_FALSE(ie.Execute(L"", 0));
}

TEST(testInstallerExecutor, DefaultConfig)
{
#if 0
    // Enable this test when https://biztelework.atlassian.net/browse/BTC-508 is resolved.
    // This test success if Biz agent is installed.
    InstallerExecutor::Config c = InstallerExecutor::Config::Default();

    EXPECT_EQ(boost::filesystem::path(L"C:\\ProgramData\\OPTiM\\BizTelework\\log\\updater"), c.logDirectory);
#endif
}
