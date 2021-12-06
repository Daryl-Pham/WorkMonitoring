#include "pch.h"
#include "AppExecutionCondition.h"
#include "Log.h"
#include <AppConfig.h>
#include <Psapi.h>

namespace
{
    DEFINE_LOGGER_FILE_NAME;

    using ProcessIdList = std::vector<DWORD>;

    const size_t MAX_PROC = 1024 * 16;

    bool EnumAllProcesses(ProcessIdList* dst)
    {
        try
        {
            if (!dst)
            {
                LOG_WARN("The dst must not be null.");
                return false;
            }

            for (size_t len = 1024; len < MAX_PROC; len *= 2)
            {
                DWORD cbNeeded = 0;
                dst->resize(len);

                if (!EnumProcesses(&(*dst)[0], dst->size() * sizeof(DWORD), &cbNeeded))
                {
                    LOG_WARN("EnumProcesses() failed. %lu", GetLastError());
                    return false;
                }

                DWORD cProcesses = cbNeeded / sizeof(DWORD);
                if (cProcesses < len)
                {
                    dst->resize(cProcesses);
                    return true;
                }
            }

            // Reach here when maximum process and *dst is full.
            return true;
        }
        catch (const std::bad_alloc& e)
        {
            LOG_ERROR("Failed by std::bad_alloc %s", e.what());
            return false;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Failed by std::exception %s", e.what());
            return false;
        }
    }
}  // namespace

bool AppExecutionCondition::Initialize(const boost::filesystem::path& exePath)
{
    try
    {
        ProcessIdList processIds;
        if (!EnumAllProcesses(&processIds))
        {
            LOG_WARN("EnumAllProcesses() failed.");
            return false;
        }

        LOG_INFO("Got %lu processe IDs", processIds.size());

        for (DWORD processId : processIds)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
            if (!hProcess)
            {
                LOG_DEBUG("OpenProcess() failed. %lu", GetLastError());
                continue;
            }

            HMODULE hModule[1024] = { 0 };
            DWORD dwSize = 0;
            if (!EnumProcessModules(hProcess, hModule, sizeof(hModule), &dwSize))
            {
                LOG_DEBUG("EnumProcessModules() failed. %lu", GetLastError());
                CloseHandle(hProcess);
                continue;
            }

            wchar_t processPath[MAX_PATH] = { L'\0' };
            if (!GetModuleFileNameEx(hProcess, hModule[0], processPath, sizeof(processPath) / sizeof(TCHAR)))
            {
                LOG_WARN("GetModuleFileNameEx() failed. %lu", GetLastError());
                CloseHandle(hProcess);
                continue;
            }

            LOG_DEBUG("processPath: %S", processPath);

            if (exePath != processPath)
            {
                CloseHandle(hProcess);
                continue;
            }

            DWORD sessionId = 0;
            if (!ProcessIdToSessionId(processId, &sessionId))
            {
                LOG_WARN("ProcessIdToSessionId() failed. %lu", GetLastError());
                CloseHandle(hProcess);
                continue;
            }

            CloseHandle(hProcess);

            LOG_INFO("The sessionId: %lu, %lu", processId, sessionId);
            m_SessionIds.insert(sessionId);
        }

        return true;
    }
    catch (const std::bad_alloc& e)
    {
        LOG_ERROR("Failed by std::bad_alloc %s", e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return false;
    }
}

bool AppExecutionCondition::ShouldExecuteAfterInstalledOnce(AppConfigInterface* appConfig) const
{
    if (!appConfig)
    {
        LOG_WARN("The appConfig must not be null.");
        return false;
    }

    bool executedBizTelework = false;
    if (!appConfig->GetExecutedBizTelework(&executedBizTelework))
    {
        // Reach here Windows registry items has some problems.
        LOG_WARN("appConfig.GetExecutedBizTelework() failed.");
        return false;
    }

    LOG_INFO("The executedBizTelework is %d", executedBizTelework);

    if (executedBizTelework)
    {
        return false;
    }

    if (!appConfig->SetExecutedBizTelework(true))
    {
        LOG_WARN("appConfig.SetExecuteBizTelework() failed.");
    }

    return true;
}

bool AppExecutionCondition::ShouldExecute(DWORD sessionId) const
{
    try
    {
        SessionIdSet::const_iterator it = std::find(m_SessionIds.begin(), m_SessionIds.end(), sessionId);
        return it == m_SessionIds.end();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return false;
    }
}
