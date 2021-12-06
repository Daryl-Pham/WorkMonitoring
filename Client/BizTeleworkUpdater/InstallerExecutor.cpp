#include "pch.h"
#include "InstallerExecutor.h"
#include "ConfigUtility.h"
#include <combaseapi.h>
#include <Msi.h>
#include <shellapi.h>
#include <Log.h>
#include <boost/format.hpp>


namespace
{
    DEFINE_LOGGER_FILE_NAME;

    const wchar_t MSI_EXEC_EXE[] = L"msiexec.exe";
    const wchar_t DEFAULT_MSI_EXEC_EXE_PATH[] = L"C:\\WINDOWS\\system32\\msiexec.exe";

    boost::filesystem::path GetMsiExecPath()
    {
        try
        {
            std::wstring buf(MAX_PATH, L'\0');

            DWORD len = GetSystemDirectory(&buf[0], buf.size());
            if (len == 0)
            {
                LOG_WARN("GetSystemDirectory() failed. %lu", GetLastError());
                return boost::filesystem::path(DEFAULT_MSI_EXEC_EXE_PATH);
            }
            else if (len > buf.size())
            {
                buf.resize(len);

                len = GetSystemDirectory(&buf[0], buf.size());
                if (len == 0)
                {
                    LOG_WARN("GetSystemDirectory() failed. %lu", GetLastError());
                    return boost::filesystem::path(DEFAULT_MSI_EXEC_EXE_PATH);
                }
                else if (len > buf.size())
                {
                    LOG_WARN("GetSystemDirectory() failed.");
                    return boost::filesystem::path(DEFAULT_MSI_EXEC_EXE_PATH);
                }
            }

            buf.resize(len);

            LOG_INFO("GetSystemDirectory returns \"%S\"", buf.c_str());
            return boost::filesystem::path(buf) / MSI_EXEC_EXE;
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
            return boost::filesystem::path(DEFAULT_MSI_EXEC_EXE_PATH);
        }
    }
}  // namespace

InstallerExecutor::Config InstallerExecutor::Config::Default()
{
    auto dir = ConfigUtility::GetLogDirectory();
    if (dir.empty())
    {
        LOG_WARN("ConfigUtility::GetDataDirectory() failed.");
    }

    return InstallerExecutor::Config(dir);
}

InstallerExecutor::Config::Config(const boost::filesystem::path& logDirectory_)
    : logDirectory(logDirectory_)
{
}

InstallerExecutor::InstallerExecutor(const InstallerExecutor::Config &config)
    : m_Config(config)
{
}

bool InstallerExecutor::Execute(const std::wstring& msiFilePath, HANDLE hEventStopControl) const
{
    if (!hEventStopControl)
    {
        LOG_WARN("The hEventStopControl must not be null.");
        return false;
    }

    try
    {
        UINT r = 0;
        auto logDir = m_Config.logDirectory;
        LOG_INFO("logDir is %S", logDir.wstring().c_str());

        SYSTEMTIME st;
        GetLocalTime(&st);
        auto installerLogFile = logDir / (boost::format("%04d%02d%02d%02d%02d_installer.txt") % st.wYear % st.wMonth % st.wDay  % st.wHour % st.wMinute).str();
        LOG_INFO("installerLogFile is %S", installerLogFile.wstring().c_str());

        std::wstring params = (boost::wformat(L"/i \"%s\" /qn /lvx* \"%s\" REBOOT=ReallySuppress")
            % msiFilePath.c_str() % installerLogFile.c_str()).str();

        if (logDir.empty())
        {
            LOG_WARN("ConfigUtility::GetLogDirectory() failed.");
        }
        else if ((!boost::filesystem::exists(logDir)) && (!boost::filesystem::create_directories(logDir)))
        {
            LOG_WARN("The logDir() does not exist.");
        }

        auto msiExecPath = GetMsiExecPath().wstring();

        SHELLEXECUTEINFO info = { 0 };
        info.cbSize = sizeof(info);
        info.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
        info.lpFile = msiExecPath.c_str();
        info.lpParameters = params.c_str();

        LOG_INFO("lpFile = %S", info.lpFile);
        LOG_INFO("lpParameters = %S", info.lpParameters);

        if (!ShellExecuteEx(&info))
        {
            LOG_WARN("ShellExecuteEx() failed. %d", GetLastError());
            return false;
        }

        LOG_INFO("Executing installer.");

        HANDLE handlers[2] = { info.hProcess, hEventStopControl };
        DWORD dr = WaitForMultipleObjects(2, handlers, FALSE, INFINITE);
        LOG_INFO("Finished to install %u", r);
        CloseHandle(info.hProcess);

        if (dr == WAIT_OBJECT_0)
        {
            // For info.hProcess
            LOG_WARN("Executing installer is failed.");
            DWORD exitCode = 0;
            if (!GetExitCodeProcess(info.hProcess, &exitCode))
            {
                LOG_WARN("GetExitCodeProcess() failed. %lu", GetLastError());
            }
            LOG_INFO("The installer returns exit code. %lu", exitCode);
            return false;
        }
        else if (dr == WAIT_OBJECT_0 + 1)
        {
            // For hEventStopControl
            LOG_INFO("Stop control event is happened.");
            return true;
        }

        LOG_WARN("WaitForMultipleObjects() failed. %lu", GetLastError());
        return false;
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return false;
    }
}

void InstallerExecutor::Initialize()
{
    // https://docs.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecuteexw
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        LOG_WARN("CoInitializeEx() failed. 0x%08x", hr);
    }
}

void InstallerExecutor::Finalize()
{
    CoUninitialize();
}

BOOL InstallerExecutor::ShellExecuteExW(_Inout_ SHELLEXECUTEINFOW* pExecInfo) const
{
    return ::ShellExecuteExW(pExecInfo);
}

DWORD InstallerExecutor::WaitForMultipleObjects(
    _In_ DWORD nCount,
    _In_reads_(nCount) CONST HANDLE* lpHandles,
    _In_ BOOL bWaitAll,
    _In_ DWORD dwMilliseconds) const
{
    return ::WaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds);
}

BOOL InstallerExecutor::CloseHandle(_In_ _Post_ptr_invalid_ HANDLE hObject) const
{
    return ::CloseHandle(hObject);
}
