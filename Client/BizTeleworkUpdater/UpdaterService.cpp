#include "pch.h"
#include "UpdaterService.h"
#include <Log.h>

namespace
{
    DEFINE_LOGGER_FILE_NAME;
}

UpdaterService::Dependency::Dependency(FetchingTaskInterface* fetchingTask)
    : m_FetchingTask(fetchingTask) {}

std::unique_ptr<PaceMaker> UpdaterService::Dependency::GetPaceMaker() const
{
    return std::make_unique<PaceMaker>();
}

std::unique_ptr<UpdateProcess> UpdaterService::Dependency::GetUpdateProcess() const
{
    return std::make_unique<UpdateProcess>(std::make_unique<UpdateChecker>(UpdateChecker::Config::Default(), m_FetchingTask),
                                           std::make_unique<InstallerFetcher>(m_FetchingTask));
}

DWORD WINAPI UpdaterService::HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
    (void)dwEventType;  // Unused
    (void)lpEventData;  // Unused

    LOG_INFO("Get control. %lu", dwControl);

    UpdaterService *self = reinterpret_cast<UpdaterService*>(lpContext);
    if (!self)
    {
        LOG_WARN("The lpContext must not be null.");
        return NO_ERROR;
    }

    switch (dwControl)
    {
    case SERVICE_CONTROL_STOP:
        LOG_INFO("Get SERVICE_CONTROL_STOP control.");
        if (!self->OnStop())
        {
            LOG_WARN("self->OnStop() failed.");
        }
        break;

    case SERVICE_CONTROL_INTERROGATE:
        LOG_INFO("Get SERVICE_CONTROL_INTERROGATE control.");
        if (!self->OnInterrogate())
        {
            LOG_WARN("self->OnInterrogate() failed.");
        }
        break;

    default:
        break;
    }

    return NO_ERROR;
}

UpdaterService::UpdaterService(std::unique_ptr<UpdaterService::Dependency> dependency)
    : m_Dependency(std::move(dependency))
{
}

bool UpdaterService::PreRun(LPCWSTR serviceName)
{
    if (!(m_EventStopControl = CreateEvent(NULL, TRUE, FALSE, NULL)))
    {
        LOG_WARN("CreateEvent() failed. %d", GetLastError());
        // TODO(ichino): SetServiceStatus as SERVICE_STOPPED.
        return false;
    }

    if (!(m_ServiceStatusHandle = RegisterServiceCtrlHandlerEx(serviceName, HandlerEx, this)))
    {
        LOG_WARN("RegisterServiceCtrlHandlerEx() failed. %d", GetLastError());
        CloseHandle(m_EventStopControl);
        m_EventStopControl = nullptr;
        return false;
    }

    m_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_ServiceStatus.dwWin32ExitCode = NO_ERROR;
    m_ServiceStatus.dwServiceSpecificExitCode = 0;
    m_ServiceStatus.dwCheckPoint = 1;
    m_ServiceStatus.dwWaitHint = 30000;

    if (!SetServiceStatus(m_ServiceStatusHandle, &m_ServiceStatus))
    {
        LOG_WARN("SetServiceStatus() failed. %d", GetLastError());
        // TODO(ichino): SetServiceStatus as SERVICE_STOPPED.
        CloseHandle(m_EventStopControl);
        m_EventStopControl = nullptr;
        return false;
    }

    // Start initialization.
    // Finish initialization.

    m_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    m_ServiceStatus.dwCheckPoint = 0;
    m_ServiceStatus.dwWaitHint = 0;

    if (!SetServiceStatus(m_ServiceStatusHandle, &m_ServiceStatus))
    {
        LOG_WARN("SetServiceStatus() failed. %d", GetLastError());
        // TODO(ichino): SetServiceStatus as SERVICE_STOPPED.
        CloseHandle(m_EventStopControl);
        m_EventStopControl = nullptr;
        return false;
    }

    return true;
}

bool UpdaterService::Run()
{
    if (!m_EventStopControl)
    {
        LOG_WARN("The m_EventStopControl must not be null. You need call PreRun().");
        return false;
    }

    std::unique_ptr<PaceMaker> paceMaker = m_Dependency->GetPaceMaker();
    if (!paceMaker)
    {
        LOG_WARN("The paceMaker must not be null.");
        return false;
    }

    bool runWhileLoop = true;
    while (runWhileLoop)
    {
        PaceMaker::WaitResult r = paceMaker->Wait(m_EventStopControl);
        LOG_INFO("paceMaker->Wait returns %d", r);

        switch (r)
        {
        case PaceMaker::FINISHED_TO_SLEEP:
            LOG_INFO("Finish sleeping.");
            if (CheckUpdating())
            {
                LOG_INFO("Updater exists.");
                runWhileLoop = false;
            }
            else
            {
                LOG_INFO("There is no updated.");
            }
            break;

        case PaceMaker::GOT_EVENT:
            LOG_INFO("The m_EventStopControl is signaled.");
            runWhileLoop = false;
            break;

        case PaceMaker::FAILED:
            LOG_WARN("paceMaker->Wait() failed.");
            runWhileLoop = false;
            break;

        default:
            LOG_WARN("paceMaker->Wait() returns unexpected code. %d", r);
            runWhileLoop = false;
            break;
        }
    }

    CloseHandle(m_EventStopControl);
    m_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    m_ServiceStatus.dwCheckPoint = 0;
    m_ServiceStatus.dwWaitHint = 0;
    if (!SetServiceStatus(m_ServiceStatusHandle, &m_ServiceStatus))
    {
        LOG_WARN("SetServiceStatus() failed. %d", GetLastError());
    }

    return true;
}


bool UpdaterService::OnStop()
{
    if (!m_ServiceStatusHandle)
    {
        LOG_WARN("The m_ServiceStatusHandle must not be null.");
        // Continue to stop sequence.
    }
    else
    {
        m_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        m_ServiceStatus.dwCheckPoint = 0;
        m_ServiceStatus.dwWaitHint = 50000;
        if (!SetServiceStatus(m_ServiceStatusHandle, &m_ServiceStatus))
        {
            LOG_WARN("SetServiceStatus() failed. %d", GetLastError());
            // Continue to stop sequence.
        }
    }

    if (!m_EventStopControl)
    {
        LOG_WARN("The m_EventStopControl must not be null.");
        return false;
    }
    else if (!SetEvent(m_EventStopControl))
    {
        LOG_WARN("SetEvent() failed. %d", GetLastError());
        // TODO(ichino): SetServiceStatus as SERVICE_STOPPED.
        return false;
    }

    return true;
}

bool UpdaterService::OnInterrogate()
{
    if (!m_ServiceStatusHandle)
    {
        LOG_WARN("The m_ServiceStatusHandle must not be null.");
        return false;
    }
    else
    {
        if (!SetServiceStatus(m_ServiceStatusHandle, &m_ServiceStatus))
        {
            LOG_WARN("SetServiceStatus() failed. %d", GetLastError());
        }
    }

    return true;
}

bool UpdaterService::CheckUpdating()
{
    std::unique_ptr<UpdateProcess> updateProcess = m_Dependency->GetUpdateProcess();
    if (!updateProcess)
    {
        LOG_WARN("The updateProcess must not be null.");
        return false;
    }
    else if (!m_EventStopControl)
    {
        LOG_WARN("The m_EventStopControl must not be null.");
        return false;
    }
    else if (!updateProcess->Execute(m_EventStopControl))
    {
        LOG_WARN("The updateProcess->Execute() failed.");
        return false;
    }

    return true;
}

SERVICE_STATUS_HANDLE UpdaterService::RegisterServiceCtrlHandlerExW(
    _In_  LPCWSTR lpServiceName,
    _In_ __callback LPHANDLER_FUNCTION_EX lpHandlerProc,
    _In_opt_ LPVOID lpContext) const
{
    return ::RegisterServiceCtrlHandlerExW(lpServiceName, lpHandlerProc, lpContext);
}

BOOL UpdaterService::SetServiceStatus(
    _In_ SERVICE_STATUS_HANDLE hServiceStatus,
    _In_ LPSERVICE_STATUS lpServiceStatus) const
{
    return ::SetServiceStatus(hServiceStatus, lpServiceStatus);
}

HANDLE UpdaterService::CreateEventW(
    _In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
    _In_ BOOL bManualReset,
    _In_ BOOL bInitialState,
    _In_opt_ LPCWSTR lpName) const
{
    return ::CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);
}

BOOL UpdaterService::SetEvent(_In_ HANDLE hEvent) const
{
    return ::SetEvent(hEvent);
}

BOOL UpdaterService::CloseHandle(
    _In_ _Post_ptr_invalid_ HANDLE hObject) const
{
    return ::CloseHandle(hObject);
}
