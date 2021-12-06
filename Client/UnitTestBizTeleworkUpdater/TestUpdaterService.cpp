#include <gtest/gtest.h>
#include <UpdaterService.h>
#include "Mock.h"

using ::testing::ByMove;
using ::testing::Return;
using ::testing::Matcher;
using ::testing::Eq;
using ::testing::IsNull;
using ::testing::_;

TEST(testUpdaterService, HandlerEx)
{
    MockUpdaterServiceInterface us;
    EXPECT_CALL(us, OnStop())
        .WillOnce(Return(true));
    UpdaterService::HandlerEx(SERVICE_CONTROL_STOP, 0, nullptr, static_cast<LPVOID>(&us));

    EXPECT_CALL(us, OnInterrogate())
        .WillOnce(Return(true));
    UpdaterService::HandlerEx(SERVICE_CONTROL_INTERROGATE, 0, nullptr, static_cast<LPVOID>(&us));
}

TEST(testUpdaterService, PreRunAndRun_GOT_EVENT)
{
    LPCWSTR serviceName = L"TEST_SERVICE";
    HANDLE stopEventHandler = reinterpret_cast<HANDLE>(1);
    SERVICE_STATUS_HANDLE serviceStatusHandle = reinterpret_cast<SERVICE_STATUS_HANDLE>(2);

    MockPaceMaker *mpm = new MockPaceMaker();  // Delete by pm object.
    std::unique_ptr<MockPaceMaker> pm(mpm);

    MockUpdaterServiceDependency *musd = new MockUpdaterServiceDependency();
    std::unique_ptr<MockUpdaterServiceDependency> usd(musd);
    EXPECT_CALL(*musd, GetPaceMaker())
        .WillOnce(Return(ByMove(std::move(pm))));

    MockUpdaterService mus(std::move(usd));

    // For PreRun()
    EXPECT_CALL(mus, CreateEventW(Matcher<LPSECURITY_ATTRIBUTES>(IsNull()), Matcher<BOOL>(Eq(TRUE)), Matcher<BOOL>(Eq(FALSE)), Matcher<LPCWSTR>(IsNull())))
        .WillOnce(Return(stopEventHandler));
    EXPECT_CALL(mus, RegisterServiceCtrlHandlerExW(
                        Matcher<LPCWSTR>(Eq(serviceName)),
                        Matcher<LPHANDLER_FUNCTION_EX>(Eq(UpdaterService::HandlerEx)), 
                        Matcher<LPVOID>(Eq(&mus))))
        .WillOnce(Return(serviceStatusHandle));
    EXPECT_CALL(mus, SetServiceStatus(Matcher<SERVICE_STATUS_HANDLE>(Eq(serviceStatusHandle)), _))
        .WillRepeatedly(Return(TRUE));
    EXPECT_TRUE(mus.PreRun(serviceName));

    // For Run(), on GOT_EVENT
    EXPECT_CALL(*mpm, Wait(_))
        .WillOnce(Return(PaceMaker::GOT_EVENT));
    EXPECT_CALL(mus, CloseHandle(Matcher<HANDLE>(Eq(stopEventHandler))))
        .WillOnce(Return(TRUE));
    EXPECT_TRUE(mus.Run());
}

TEST(testUpdaterService, PreRunAndRun_FAILED)
{
    LPCWSTR serviceName = L"TEST_SERVICE";
    HANDLE stopEventHandler = reinterpret_cast<HANDLE>(1);
    SERVICE_STATUS_HANDLE serviceStatusHandle = reinterpret_cast<SERVICE_STATUS_HANDLE>(2);

    MockPaceMaker* mpm = new MockPaceMaker();  // Delete by pm object.
    std::unique_ptr<MockPaceMaker> pm(mpm);

    MockUpdaterServiceDependency* musd = new MockUpdaterServiceDependency();
    std::unique_ptr<MockUpdaterServiceDependency> usd(musd);
    EXPECT_CALL(*musd, GetPaceMaker())
        .WillRepeatedly(Return(ByMove(std::move(pm))));

    MockUpdaterService mus(std::move(usd));

    // For PreRun()
    EXPECT_CALL(mus, CreateEventW(Matcher<LPSECURITY_ATTRIBUTES>(IsNull()), Matcher<BOOL>(Eq(TRUE)), Matcher<BOOL>(Eq(FALSE)), Matcher<LPCWSTR>(IsNull())))
        .WillOnce(Return(stopEventHandler));
    EXPECT_CALL(mus, RegisterServiceCtrlHandlerExW(
        Matcher<LPCWSTR>(Eq(serviceName)),
        Matcher<LPHANDLER_FUNCTION_EX>(Eq(UpdaterService::HandlerEx)),
        Matcher<LPVOID>(Eq(&mus))))
        .WillOnce(Return(serviceStatusHandle));
    EXPECT_CALL(mus, SetServiceStatus(Matcher<SERVICE_STATUS_HANDLE>(Eq(serviceStatusHandle)), _))
        .WillRepeatedly(Return(TRUE));
    EXPECT_TRUE(mus.PreRun(serviceName));

    // For Run(), on FAILED
    EXPECT_CALL(*mpm, Wait(Matcher<HANDLE>(Eq(stopEventHandler))))
        .WillRepeatedly(Return(PaceMaker::FAILED));
    EXPECT_CALL(mus, CloseHandle(Matcher<HANDLE>(Eq(stopEventHandler))))
        .WillOnce(Return(TRUE));
    EXPECT_TRUE(mus.Run());
}

TEST(testUpdaterService, PreRunAndRun_UnexpectedValue)
{
    LPCWSTR serviceName = L"TEST_SERVICE";
    HANDLE stopEventHandler = reinterpret_cast<HANDLE>(1);
    SERVICE_STATUS_HANDLE serviceStatusHandle = reinterpret_cast<SERVICE_STATUS_HANDLE>(2);

    MockPaceMaker* mpm = new MockPaceMaker();  // Delete by pm object.
    std::unique_ptr<MockPaceMaker> pm(mpm);

    MockUpdaterServiceDependency* musd = new MockUpdaterServiceDependency();
    std::unique_ptr<MockUpdaterServiceDependency> usd(musd);
    EXPECT_CALL(*musd, GetPaceMaker())
        .WillRepeatedly(Return(ByMove(std::move(pm))));

    MockUpdaterService mus(std::move(usd));

    // For PreRun()
    EXPECT_CALL(mus, CreateEventW(Matcher<LPSECURITY_ATTRIBUTES>(IsNull()), Matcher<BOOL>(Eq(TRUE)), Matcher<BOOL>(Eq(FALSE)), Matcher<LPCWSTR>(IsNull())))
        .WillOnce(Return(stopEventHandler));
    EXPECT_CALL(mus, RegisterServiceCtrlHandlerExW(
        Matcher<LPCWSTR>(Eq(serviceName)),
        Matcher<LPHANDLER_FUNCTION_EX>(Eq(UpdaterService::HandlerEx)),
        Matcher<LPVOID>(Eq(&mus))))
        .WillOnce(Return(serviceStatusHandle));
    EXPECT_CALL(mus, SetServiceStatus(Matcher<SERVICE_STATUS_HANDLE>(Eq(serviceStatusHandle)), _))
        .WillRepeatedly(Return(TRUE));
    EXPECT_TRUE(mus.PreRun(serviceName));

    // For Run(), on Unexpected value
    DWORD unexpectedValue = 1000;
    EXPECT_CALL(*mpm, Wait(Matcher<HANDLE>(Eq(stopEventHandler))))
        .WillRepeatedly(Return(static_cast<PaceMaker::WaitResult>(unexpectedValue)));
    EXPECT_CALL(mus, CloseHandle(Matcher<HANDLE>(Eq(stopEventHandler))))
        .WillOnce(Return(TRUE));
    EXPECT_TRUE(mus.Run());
}

TEST(testUpdaterService, PreRunAndRun_GotEventAdnCheckUpdatingFailed)
{
    LPCWSTR serviceName = L"TEST_SERVICE";
    HANDLE stopEventHandler = reinterpret_cast<HANDLE>(1);
    SERVICE_STATUS_HANDLE serviceStatusHandle = reinterpret_cast<SERVICE_STATUS_HANDLE>(2);

    MockPaceMaker* mpm = new MockPaceMaker();  // Delete by pm object.
    std::unique_ptr<MockPaceMaker> pm(mpm);

    MockUpdaterServiceDependency* musd = new MockUpdaterServiceDependency();
    std::unique_ptr<MockUpdaterServiceDependency> usd(musd);
    EXPECT_CALL(*musd, GetPaceMaker())
        .WillRepeatedly(Return(ByMove(std::move(pm))));

    MockUpdaterService mus(std::move(usd));

    // For PreRun()
    EXPECT_CALL(mus, CreateEventW(Matcher<LPSECURITY_ATTRIBUTES>(IsNull()), Matcher<BOOL>(Eq(TRUE)), Matcher<BOOL>(Eq(FALSE)), Matcher<LPCWSTR>(IsNull())))
        .WillOnce(Return(stopEventHandler));
    EXPECT_CALL(mus, RegisterServiceCtrlHandlerExW(
        Matcher<LPCWSTR>(Eq(serviceName)),
        Matcher<LPHANDLER_FUNCTION_EX>(Eq(UpdaterService::HandlerEx)),
        Matcher<LPVOID>(Eq(&mus))))
        .WillOnce(Return(serviceStatusHandle));
    EXPECT_CALL(mus, SetServiceStatus(Matcher<SERVICE_STATUS_HANDLE>(Eq(serviceStatusHandle)), _))
        .WillRepeatedly(Return(TRUE));
    EXPECT_TRUE(mus.PreRun(serviceName));

    // For Run(), on GOT_EVENT and CheckUpdating() failed.
    EXPECT_CALL(*mpm, Wait(Matcher<HANDLE>(Eq(stopEventHandler))))
        .Times(2)
        .WillOnce(Return(PaceMaker::FINISHED_TO_SLEEP))
        .WillOnce(Return(PaceMaker::GOT_EVENT));
    EXPECT_CALL(mus, CheckUpdating())
        .WillOnce(Return(false));
    EXPECT_CALL(mus, CloseHandle(Matcher<HANDLE>(Eq(stopEventHandler))))
        .WillOnce(Return(TRUE));
    EXPECT_TRUE(mus.Run());
}


TEST(testUpdaterService, PreRunAndRun_GotEventAndCheckUpdatingSucceeded)
{
    LPCWSTR serviceName = L"TEST_SERVICE";
    HANDLE stopEventHandler = reinterpret_cast<HANDLE>(1);
    SERVICE_STATUS_HANDLE serviceStatusHandle = reinterpret_cast<SERVICE_STATUS_HANDLE>(2);

    MockPaceMaker* mpm = new MockPaceMaker();  // Delete by pm object.
    std::unique_ptr<MockPaceMaker> pm(mpm);

    MockUpdaterServiceDependency* musd = new MockUpdaterServiceDependency();
    std::unique_ptr<MockUpdaterServiceDependency> usd(musd);
    EXPECT_CALL(*musd, GetPaceMaker())
        .WillRepeatedly(Return(ByMove(std::move(pm))));

    MockUpdaterService mus(std::move(usd));

    // For PreRun()
    EXPECT_CALL(mus, CreateEventW(Matcher<LPSECURITY_ATTRIBUTES>(IsNull()), Matcher<BOOL>(Eq(TRUE)), Matcher<BOOL>(Eq(FALSE)), Matcher<LPCWSTR>(IsNull())))
        .WillOnce(Return(stopEventHandler));
    EXPECT_CALL(mus, RegisterServiceCtrlHandlerExW(
        Matcher<LPCWSTR>(Eq(serviceName)),
        Matcher<LPHANDLER_FUNCTION_EX>(Eq(UpdaterService::HandlerEx)),
        Matcher<LPVOID>(Eq(&mus))))
        .WillOnce(Return(serviceStatusHandle));
    EXPECT_CALL(mus, SetServiceStatus(Matcher<SERVICE_STATUS_HANDLE>(Eq(serviceStatusHandle)), _))
        .WillRepeatedly(Return(TRUE));
    EXPECT_TRUE(mus.PreRun(serviceName));

    // For Run(), on GOT_EVENT and CheckUpdating() succeeded.
    EXPECT_CALL(*mpm, Wait(Matcher<HANDLE>(Eq(stopEventHandler))))
        .WillOnce(Return(PaceMaker::FINISHED_TO_SLEEP));
    EXPECT_CALL(mus, CheckUpdating())
        .WillOnce(Return(true));
    EXPECT_CALL(mus, CloseHandle(Matcher<HANDLE>(Eq(stopEventHandler))))
        .WillOnce(Return(TRUE));
    EXPECT_TRUE(mus.Run());
}

TEST(testUpdaterService, OnStop)
{
    LPCWSTR serviceName = L"TEST_SERVICE";
    HANDLE stopEventHandler = reinterpret_cast<HANDLE>(1);
    SERVICE_STATUS_HANDLE serviceStatusHandle = reinterpret_cast<SERVICE_STATUS_HANDLE>(2);
    MockUpdaterService mus(nullptr);

    // Execute PreRun for obtaining m_ServiceStatusHandle.
    EXPECT_CALL(mus, CreateEventW(Matcher<LPSECURITY_ATTRIBUTES>(IsNull()), Matcher<BOOL>(Eq(TRUE)), Matcher<BOOL>(Eq(FALSE)), Matcher<LPCWSTR>(IsNull())))
        .WillOnce(Return(stopEventHandler));
    EXPECT_CALL(mus, RegisterServiceCtrlHandlerExW(
        Matcher<LPCWSTR>(Eq(serviceName)),
        Matcher<LPHANDLER_FUNCTION_EX>(Eq(UpdaterService::HandlerEx)),
        Matcher<LPVOID>(Eq(&mus))))
        .WillOnce(Return(serviceStatusHandle));
    EXPECT_CALL(mus, SetServiceStatus(Matcher<SERVICE_STATUS_HANDLE>(Eq(serviceStatusHandle)), _))
        .WillRepeatedly(Return(TRUE));
    EXPECT_TRUE(mus.PreRun(serviceName));

    // For OnStop().
    EXPECT_CALL(mus, SetServiceStatus(Matcher<SERVICE_STATUS_HANDLE>(Eq(serviceStatusHandle)), _))
        .WillOnce(Return(TRUE));
    EXPECT_CALL(mus, SetEvent(Matcher<HANDLE>(Eq(stopEventHandler))))
        .WillOnce(Return(TRUE));
    EXPECT_TRUE(mus.OnStop());
}

TEST(testUpdaterService, OnInterrogate)
{
    LPCWSTR serviceName = L"TEST_SERVICE";
    HANDLE stopEventHandler = reinterpret_cast<HANDLE>(1);
    SERVICE_STATUS_HANDLE serviceStatusHandle = reinterpret_cast<SERVICE_STATUS_HANDLE>(2);
    MockUpdaterService mus(nullptr);

    // Execute PreRun for obtaining m_ServiceStatusHandle.
    EXPECT_CALL(mus, CreateEventW(Matcher<LPSECURITY_ATTRIBUTES>(IsNull()), Matcher<BOOL>(Eq(TRUE)), Matcher<BOOL>(Eq(FALSE)), Matcher<LPCWSTR>(IsNull())))
        .WillOnce(Return(stopEventHandler));
    EXPECT_CALL(mus, RegisterServiceCtrlHandlerExW(
        Matcher<LPCWSTR>(Eq(serviceName)),
        Matcher<LPHANDLER_FUNCTION_EX>(Eq(UpdaterService::HandlerEx)),
        Matcher<LPVOID>(Eq(&mus))))
        .WillOnce(Return(serviceStatusHandle));
    EXPECT_CALL(mus, SetServiceStatus(Matcher<SERVICE_STATUS_HANDLE>(Eq(serviceStatusHandle)), _))
        .WillRepeatedly(Return(TRUE));
    EXPECT_TRUE(mus.PreRun(serviceName));

    // For OnInterrogate().
    EXPECT_CALL(mus, SetServiceStatus(Matcher<SERVICE_STATUS_HANDLE>(Eq(serviceStatusHandle)), _))
        .WillOnce(Return(TRUE));
    EXPECT_TRUE(mus.OnInterrogate());
}
