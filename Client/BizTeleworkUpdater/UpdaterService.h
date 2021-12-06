#pragma once

#include <memory>
#include "UpdaterServiceInterface.h"
#include "FetchingTaskInterface.h"
#include "PaceMaker.h"
#include "UpdateProcess.h"

class UpdaterService : public UpdaterServiceInterface
{
public:
    class Dependency
    {
    public:
        explicit Dependency(FetchingTaskInterface* fetchingTask);
        virtual ~Dependency() = default;
        virtual std::unique_ptr<PaceMaker> GetPaceMaker() const;
        virtual std::unique_ptr<UpdateProcess> GetUpdateProcess() const;
    private:
        FetchingTaskInterface *m_FetchingTask = nullptr;
    };

    explicit UpdaterService(std::unique_ptr<UpdaterService::Dependency> dependency);
    ~UpdaterService() = default;

    static DWORD WINAPI HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

    // For UpdaterServiceInterface
    bool PreRun(LPCWSTR serviceName) override;
    bool Run() override;
    bool OnStop() override;
    bool OnInterrogate() override;

    // Wrapper methods for test of calling Windows API.
    virtual SERVICE_STATUS_HANDLE RegisterServiceCtrlHandlerExW(
        _In_ LPCWSTR lpServiceName,
        _In_ __callback LPHANDLER_FUNCTION_EX lpHandlerProc,
        _In_opt_ LPVOID lpContext) const;
    virtual BOOL SetServiceStatus(
        _In_ SERVICE_STATUS_HANDLE   hServiceStatus,
        _In_ LPSERVICE_STATUS        lpServiceStatus) const;
    virtual HANDLE CreateEventW(
        _In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
        _In_ BOOL bManualReset,
        _In_ BOOL bInitialState,
        _In_opt_ LPCWSTR lpName) const;
    virtual BOOL SetEvent(_In_ HANDLE hEvent) const;
    virtual BOOL CloseHandle(
        _In_ _Post_ptr_invalid_ HANDLE hObject) const;

protected:
    virtual bool CheckUpdating();  // Make this method as protected for unit test.

private:
    HANDLE                m_EventStopControl = NULL;
    SERVICE_STATUS        m_ServiceStatus = { 0 };
    SERVICE_STATUS_HANDLE m_ServiceStatusHandle = NULL;
    std::shared_ptr<UpdaterService::Dependency> m_Dependency = nullptr;
};
