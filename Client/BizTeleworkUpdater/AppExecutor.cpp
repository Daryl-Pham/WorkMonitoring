#include "pch.h"
#include "AppExecutor.h"
#include "Log.h"
#include <AppConfig.h>
#include <UserEnv.h>
#include <WtsApi32.h>
#include <boost/dll.hpp>

#pragma comment(lib, "Userenv.lib")

namespace
{
    DEFINE_LOGGER_FILE_NAME;
    const wchar_t BIZ_TELEWORK_EXE[] = L"BizTelework.exe";
}  // namespace

std::unique_ptr<AppExecutionCondition> AppExecutor::Container::GetAppExecutionCondition() const
{
    return std::make_unique<AppExecutionCondition>();
}

std::unique_ptr<SessionEnumerator> AppExecutor::Container::GetSessionEnumerator() const
{
    return std::make_unique<SessionEnumerator>();
}

boost::filesystem::path AppExecutor::Container::GetExePath() const
{
    try
    {
        return boost::dll::program_location().parent_path() / BIZ_TELEWORK_EXE;
    }
    catch (const boost::system::system_error& e)
    {
        LOG_ERROR("Failed by boost::system::system_error %s", e.what());
        return L"";
    }
    catch (const std::bad_alloc& be)
    {
        LOG_ERROR("Failed by std::bad_alloc %s", be.what());
        return L"";
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return L"";
    }
}

AppExecutor::AppExecutor(std::unique_ptr<Container> container)
    : m_Container(std::move(container))
{
}

void AppExecutor::ExecuteOnEachSession()
{
    if (!m_Container)
    {
        LOG_WARN("The m_Container must not be null.");
        return;
    }

    auto conditon = m_Container->GetAppExecutionCondition();
    if (!conditon)
    {
        LOG_WARN("m_Container->GetAppExecutionCondition() failed");
        return;
    }

    AppConfig appConfig;
    if (!conditon->ShouldExecuteAfterInstalledOnce(&appConfig))
    {
        LOG_INFO("The app need not to execute.");
        return;
    }

    auto enumerator = m_Container->GetSessionEnumerator();
    if (!enumerator)
    {
        LOG_WARN("m_Container->GetSessionEnumerator() failed");
        return;
    }

    auto exePath = m_Container->GetExePath();
    LOG_INFO("exePath: \"%S\"", exePath.c_str());
    if (exePath.empty())
    {
        LOG_WARN("The exePath must not be empty.");
        return;
    }

    if (!conditon->Initialize(exePath))
    {
        LOG_WARN("conditon.Initialize() failed.");
        return;
    }

    enumerator->EnumerateSession([&conditon, &exePath, this](DWORD sessionId)
        {
            if (conditon->ShouldExecute(sessionId))
            {
                ExecuteApp(exePath, sessionId);
            }
        });
}

BOOL AppExecutor::WTSQueryUserToken(ULONG SessionId, PHANDLE phToken) const
{
    return ::WTSQueryUserToken(SessionId, phToken);
}

BOOL AppExecutor::CreateEnvironmentBlock(LPVOID* lpEnvironment, HANDLE hToken, BOOL bInherit) const
{
    return ::CreateEnvironmentBlock(lpEnvironment, hToken, bInherit);
}

BOOL AppExecutor::CreateProcessAsUserW(
    _In_opt_ HANDLE hToken,
    _In_opt_ LPCWSTR lpApplicationName,
    _Inout_opt_ LPWSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCWSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOW lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation) const
{
    return ::CreateProcessAsUserW(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags,
                                  lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

BOOL AppExecutor::DestroyEnvironmentBlock(LPVOID  lpEnvironment) const
{
    return ::DestroyEnvironmentBlock(lpEnvironment);
}

BOOL AppExecutor::CloseHandle(_In_ _Post_ptr_invalid_ HANDLE hObject) const
{
    return ::CloseHandle(hObject);
}

void AppExecutor::ExecuteApp(const boost::filesystem::path& exePath, DWORD sessionId) const
{
    // This method requires LocalService permission to success.
    HANDLE userToken = NULL;
    if (!WTSQueryUserToken(sessionId, &userToken))
    {
        LOG_WARN("WTSQueryUserToken() failed. %lu", GetLastError());
        return;
    }

    LPVOID environment = NULL;
    if (!CreateEnvironmentBlock(&environment, userToken, FALSE))
    {
        LOG_WARN("CreateEnvironmentBlock() failed. %lu", GetLastError());
        CloseHandle(userToken);
        return;
    }

    STARTUPINFO si = { 0 };
    si.cb = sizeof(STARTUPINFO);
    si.lpDesktop = L"Winsta0\\default";
    PROCESS_INFORMATION pi = { 0 };
    std::wstring ep = exePath.wstring();
    if (!CreateProcessAsUser(userToken, NULL, &ep[0], NULL, NULL, FALSE,
        NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, environment, NULL, &si, &pi))
    {
        LOG_WARN("CreateProcessAsUser() failed. %lu", GetLastError());
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    DestroyEnvironmentBlock(environment);
    CloseHandle(userToken);
}
