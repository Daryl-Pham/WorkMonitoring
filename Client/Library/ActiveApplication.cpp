#include "stdafx.h"

/* Fix for issue https://github.com/Microsoft/vcpkg/issues/836
 */
#define _TURN_OFF_PLATFORM_STRING

#include "Utility.h"
#include "Log.h"
#include "ActiveApplication.h"
#include "HttpStatusCode.h"
#include <boost/filesystem.hpp>
#include <cpprest/filestream.h>
#include <cpprest/asyncrt_utils.h>
#include <boost/chrono/io/time_point_io.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include "Common.h"
#include <boost/timer/timer.hpp>
#include "GettingRequest.h"
#pragma comment(lib, "Version.lib")

namespace bfs = boost::filesystem;
using AppList = std::vector<std::wstring>;

namespace
{
    DEFINE_LOGGER_FILE_NAME;
    const char TimeFormat[] = "%Y-%m-%dT%H:%M:%SZ";
    const std::wstring ApplicationFrameHost = L"ApplicationFrameHost.exe";
    // Definition label
    const char* EventAtLabel = "event_at";
    const char* ProcessNameLabel = "process_name";
    const AppList IgnoreListApp{ L"LockApp.exe" };
}  // namespace

ActiveApplication::ActiveApplication(const std::wstring& logFileName)
    : ActiveApplication(Utility::GetDefaultDataDirectory().wstring(), logFileName)
{
}

ActiveApplication::ActiveApplication(const std::wstring& basePath, const std::wstring& logFileName)
    : m_LogFilePath(bfs::path(basePath) / bfs::path(logFileName))
{
}

ActiveApplication::~ActiveApplication()
{
}

bool ActiveApplication::SetProcessNameFromProcessID()
{
    HANDLE hProcess;
    if (!(hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_ProcessID)))
    {
        LOG_WARN("OpenProcess() failed. 0x%08x", GetLastError());
        return false;
    }

    wchar_t executeFilePath[MAX_PATH];
    DWORD sizePath = MAX_PATH;
    if (!QueryFullProcessImageNameW(hProcess, 0, executeFilePath, &sizePath))
    {
        LOG_WARN("QueryFullProcessImageNameW() failed. 0x%08x", GetLastError());
        return false;
    }

    const boost::filesystem::wpath wpathExecuteFile(executeFilePath);
    try
    {
        m_ProcessName = wpathExecuteFile.filename().wstring();
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return false;
    }

    return true;
}

BOOL CALLBACK ActiveApplication::EnumChildProc(HWND hwndChild, LPARAM lParam)
{
    ActiveApplication* pThis = reinterpret_cast<ActiveApplication*>(lParam);

    DWORD processID = 0;
    if (!GetWindowThreadProcessId(hwndChild, &processID))
    {
        LOG_WARN("GetWindowThreadProcessId() failed.");
        return FALSE;
    }

    if (processID != pThis->m_ProcessID)
    {
        pThis->m_ProcessID = processID;
        pThis->SetProcessNameFromProcessID();
        return FALSE;
    }
    else
    {
        return TRUE;
    }

}

bool ActiveApplication::GetCurrentProcessName()
{
    HWND hwnd = NULL;
    if (!(hwnd = GetForegroundWindow()))
    {
        LOG_WARN("GetForegroundWindow() failed.");
        return false;
    }

    if (!GetWindowThreadProcessId(hwnd, &m_ProcessID))
    {
        LOG_ERROR("GetWindowThreadProcessId() failed.");
        return false;
    }

    // For case application get from windows store and always run in application frame host (BTC-442)
    if (SetProcessNameFromProcessID())
    {
        if (m_ProcessName == ApplicationFrameHost)
        {
            ::EnumChildWindows(hwnd, EnumChildProc, reinterpret_cast<LPARAM>(this));
        }

        AppList::const_iterator it = std::find(IgnoreListApp.begin(), IgnoreListApp.end(), m_ProcessName);
        return (it == IgnoreListApp.end());
    }
    else
    {
        return false;
    }
}

bool ActiveApplication::WriteLog(const boost::chrono::system_clock::time_point& current,
                                 const std::wstring& processName)
{
    // If data directory not exist then create this directory
    boost::system::error_code ec;
    if (!bfs::exists(m_LogFilePath, ec) && !bfs::create_directories(m_LogFilePath.parent_path(), ec) && ec)
    {
        LOG_WARN("bfs::create_directories(%s) failed. %d(%s)", m_LogFilePath.parent_path().string().c_str(), ec.value(), ec.message());
        return false;
    }

    // There is no error by ec value if parentPath already exists.

    try
    {
        std::fstream file(m_LogFilePath.wstring(), std::ios::in | std::ios::out | std::ios::app);
        if (!file)
        {
            LOG_ERROR("std::fstream(%s) failed().", m_LogFilePath.string().c_str());
            return false;
        }
        file.seekg(std::ios::beg);
        file.seekp(std::ios::beg);
        file.clear();
        file.seekg(std::ios::end);

        // Write the timestamp and process name.
        file << EventAtLabel << ":"
             << boost::chrono::time_fmt(boost::chrono::timezone::utc, TimeFormat) << current
             << "\t"
             << ProcessNameLabel << ":"
             << utility::conversions::to_utf8string(processName) << std::endl;
        if (file.fail())
        {
            LOG_WARN("Failed to write string to file stream.");
            file.close();
            return false;
        }

        file.close();
        return true;
    }
    catch (const boost::exception& e)
    {
        LOG_ERROR("Failed by boost::exception %s", boost::diagnostic_information_what(e));
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("std::exception occured. %s", e.what());
        return false;
    }
}

BOOL ActiveApplication::ReadActiveAppLog(LogLines* logLines)
{
    Logging activeAppLogFile(Utility::GetDefaultDataDirectory(), ActiveAppLogFileName);

    return activeAppLogFile.ReadLogActiveApp(logLines);
}

void ActiveApplication::SendLog(const LogLines& logLines)
{
    auto thr = std::thread([logLines]()
    {
        std::wstring jsonStrigify;
        if (!Logging::ActiveAppLogStringifyAsJsonBody(logLines, &jsonStrigify))
        {
            LOG_WARN("Logging::ActiveAppLogStringifyAsJsonBody() failed.");
            return;
        }

        boost::timer::cpu_timer timer;

        std::unique_ptr<Request> request = GettingRequest::GetRequest();
        if (!request)
        {
            return;
        }
        auto callContext = request->CallWindowsApplicationLogs(jsonStrigify);
        if (callContext == nullptr)
        {
            LOG_WARN("request.CallWindowsApplicationLogs() failed.");
            return;
        }
        else if (!callContext->Wait())
        {
            LOG_ERROR("callContext->Wait() takes %s[sec].", timer.format(2, "%w").c_str());
            LOG_ERROR("callContext->Wait() failed.");
            LOG_WARN("Requesting jsonStrigify = %S", jsonStrigify.c_str());
            return;
        }
        else if (callContext->StatusCode() != HTTP_STATUS_CODE_CREATED)
        {
            LOG_WARN("The request is failed. StatusCode: %d", callContext->StatusCode());
            LOG_WARN("Requesting jsonStrigify = %S", jsonStrigify.c_str());
            return;
        }
        else
        {
            // "%w" means wall clock time.
            LOG_INFO("callContext->Wait() takes %s[sec].", timer.format(2, "%w").c_str());
            LOG_INFO("Succeeded to upload an event log.");
            Logging applicationLogFile = Logging(Utility::GetDefaultDataDirectory(), ActiveAppLogFileName);
            if (!applicationLogFile.DeleteLog())
            {
                LOG_WARN("applicationLogFile.DeleteLog() failed.");
            }
        }
    });
    thr.detach();
}
