#pragma once

#include <gmock/gmock.h>
#include <string>
#include <memory>
#include <AppConfigInterface.h>
#include <AppExecutor.h>
#include <AppExecutionCondition.h>
#include <FetchingTaskInterface.h>
#include <SessionEnumerator.h>
#include <UpdateChecker.h>
#include <UpdaterServiceInterface.h>
#include <UpdaterService.h>
#include <UpdateProcess.h>
#include <InstallerExecutor.h>
#include <InstallerFetcher.h>
#include <InstallerValidator.h>
#include <PaceMaker.h>

class MockAppConfig : public AppConfigInterface
{
public:
    MOCK_METHOD(bool, GetApiUrl, (std::wstring*));
    MOCK_METHOD(bool, GetDataDir, (std::wstring*));
    MOCK_METHOD(bool, GetUpdateUrl, (std::wstring*));
    MOCK_METHOD(bool, GetUpgradeCode, (std::wstring*));
    MOCK_METHOD(bool, GetVersion, (std::wstring*));
    MOCK_METHOD(bool, GetTimerIntervalGetActiveApplication, (DWORD*));
    MOCK_METHOD(bool, GetTimerIntervalCheckAuthen, (DWORD*));
    MOCK_METHOD(bool, GetSentryDSN, (std::string*));
    MOCK_METHOD(bool, GetReleaseEnvironment, (std::string*));
    MOCK_METHOD(bool, GetBizTeleworkUpdaterRPCEndpoint, (std::wstring*));
    MOCK_METHOD(bool, GetExecutedBizTelework, (bool*));
    MOCK_METHOD(bool, SetExecutedBizTelework, (bool));
    MOCK_METHOD(bool, GetTimeDisplayHealthStatusRemind, (int64_t*));
    MOCK_METHOD(bool, SetTimeDisplayHealthStatusRemind, (const int64_t));
    MOCK_METHOD(bool, GetTimerIntervalToRemidOnDutyDialog, (int64_t*));
    MOCK_METHOD(bool, GetTimeToShowOnDutyDialog, (int64_t*));
    MOCK_METHOD(bool, GetTimeToShowHealthStatusDialog, (int64_t*));
    MOCK_METHOD(bool, SetCopiedAuthentication, (bool));
    MOCK_METHOD(bool, GetCopiedAuthentication, (bool*));
    MOCK_METHOD(bool, DeleteTimeDisplayHealthStatusRemind, ());

};

class MockAppExecutor : public AppExecutor
{
public:
    MOCK_METHOD(void, ExecuteOnEachSession, ());
};

class MockAppExecutorForWin32 : public AppExecutor
{
public:
    MockAppExecutorForWin32(std::unique_ptr<AppExecutor::Container> container)
        : AppExecutor(std::move(container))
    {}
    MOCK_CONST_METHOD2(WTSQueryUserToken, BOOL(ULONG, PHANDLE));
    MOCK_CONST_METHOD3(CreateEnvironmentBlock, BOOL(LPVOID*, HANDLE, BOOL));
    MOCK_METHOD(BOOL, CreateProcessAsUserW,
                (HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
                 BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION),
                (const, override));
    MOCK_CONST_METHOD1(DestroyEnvironmentBlock, BOOL(LPVOID));
    MOCK_CONST_METHOD1(CloseHandle, BOOL(HANDLE));
};

class MockFetchingTask : public FetchingTaskInterface
{
public:
    MOCK_METHOD(bool, RequestFile, (const std::wstring&, const std::wstring&, HANDLE));
    MOCK_METHOD(bool, RequestBodyString, (const std::wstring&, std::wstring*, HANDLE));
};

class MockAppExecutorContainer : public AppExecutor::Container
{
public:
    MOCK_CONST_METHOD0(GetAppExecutionCondition, std::unique_ptr<AppExecutionCondition>());
    MOCK_CONST_METHOD0(GetSessionEnumerator, std::unique_ptr<SessionEnumerator>());
    MOCK_CONST_METHOD0(GetExePath, boost::filesystem::path());
};

class MockAppExecutionCondition : public AppExecutionCondition
{
public:
    MOCK_METHOD(bool, Initialize, (const boost::filesystem::path&));
    MOCK_CONST_METHOD1(ShouldExecuteAfterInstalledOnce, bool(AppConfigInterface* appConfig));
    MOCK_CONST_METHOD1(ShouldExecute, bool(DWORD));
};

class MockSessionEnumerator : public SessionEnumerator
{
public:
    MOCK_CONST_METHOD1(EnumerateSession, void(const std::function<void(DWORD)>&));
};

class MockUpdateChecker : public UpdateChecker
{
public:
    MockUpdateChecker() : UpdateChecker(nullptr, nullptr) {}
    MOCK_METHOD(bool, CheckUpdating, (HANDLE));
};

class MockUpdaterServiceInterface : public UpdaterServiceInterface
{
public:
    MOCK_METHOD(bool, PreRun, (LPCWSTR));
    MOCK_METHOD(bool, Run, ());
    MOCK_METHOD(bool, OnStop, ());
    MOCK_METHOD(bool, OnInterrogate, ());
};

class MockUpdaterServiceDependency : public UpdaterService::Dependency
{
public:
    MockUpdaterServiceDependency()
        : UpdaterService::Dependency(nullptr) {}
    MOCK_CONST_METHOD0(GetPaceMaker, std::unique_ptr<PaceMaker>());
    MOCK_CONST_METHOD0(GetUpdateProcess, std::unique_ptr<UpdateProcess>());
};

class MockUpdaterService : public UpdaterService
{
public:
    MockUpdaterService(std::unique_ptr<UpdaterService::Dependency> dependency)
        : UpdaterService(std::move(dependency)) {}
    MOCK_CONST_METHOD3(RegisterServiceCtrlHandlerExW, SERVICE_STATUS_HANDLE(LPCWSTR, LPHANDLER_FUNCTION_EX, LPVOID));
    MOCK_CONST_METHOD2(SetServiceStatus, BOOL(SERVICE_STATUS_HANDLE, LPSERVICE_STATUS));
    MOCK_CONST_METHOD4(CreateEventW, HANDLE(LPSECURITY_ATTRIBUTES, BOOL, BOOL, LPCWSTR));
    MOCK_CONST_METHOD1(SetEvent, BOOL(HANDLE));
    MOCK_CONST_METHOD1(CloseHandle, BOOL(HANDLE));
    MOCK_METHOD(bool, CheckUpdating, ());
};

class MockUpdateProcess : public UpdateProcess
{
public:
    MockUpdateProcess() : UpdateProcess(nullptr) {}
    MOCK_METHOD(bool, Execute, (HANDLE));
};

class MockInstallerExecutor : public InstallerExecutor
{
public:
    MOCK_CONST_METHOD2(Execute, bool(const std::wstring&, HANDLE));
};

class McokInstallerExecutorForWin32 : public InstallerExecutor
{
public:
    McokInstallerExecutorForWin32(const InstallerExecutor::Config& config)
        : InstallerExecutor(config)
    {}
    MOCK_CONST_METHOD1(ShellExecuteExW, BOOL(SHELLEXECUTEINFOW*));
    MOCK_CONST_METHOD4(WaitForMultipleObjects, DWORD(DWORD, const HANDLE*, BOOL, DWORD));
    MOCK_CONST_METHOD1(CloseHandle, BOOL(HANDLE));
};

class MockInstallerFetcher : public InstallerFetcher
{
public:
    MockInstallerFetcher(FetchingTaskInterface* fetchingTask) : InstallerFetcher(fetchingTask, InstallerFetcher::Config(L"", L"")) {}
    MOCK_METHOD(bool, Fetch, (std::wstring*, HANDLE hEventStopControl));
};

class MockInstallerValidator : public InstallerValidator
{
public:
    MOCK_CONST_METHOD1(ValidateMsiFile, bool(const std::wstring&));
    MOCK_CONST_METHOD1(ValidateSignature, bool(const std::wstring&));
    MOCK_CONST_METHOD1(ValidateUpgradeCode, bool(const std::wstring&));
    MOCK_CONST_METHOD1(ValidateVersion, bool(const std::wstring&));
};

class MockPaceMaker : public PaceMaker
{
public:
    MOCK_METHOD(PaceMaker::WaitResult, Wait, (HANDLE));
};
