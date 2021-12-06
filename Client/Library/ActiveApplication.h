#pragma once
#include <boost/filesystem.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <string>
#include "Logging.h"

class ActiveApplication
{
public:
    explicit ActiveApplication(const std::wstring& logFileName);
    ActiveApplication(const std::wstring& basePath, const std::wstring& logFileName);
    ~ActiveApplication();

    /**
     * Write a log. If the log file is not exist, then create log file and write log.
     * @return true if no error happend. false if error has occurred.
     */
    bool WriteLog(const boost::chrono::system_clock::time_point& current,
                  const std::wstring& processName);

    std::wstring GetLogFilePath() const { return m_LogFilePath.wstring(); }

    bool GetCurrentProcessName();
    std::wstring GetProcessName() const { return m_ProcessName; };

    BOOL ReadActiveAppLog(LogLines* logLines);

    void SendLog(const LogLines& logLines);

private:
    const boost::filesystem::path m_LogFilePath;
    std::wstring m_ProcessName = L"";
    DWORD m_ProcessID = 0;

    static BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam);
    bool SetProcessNameFromProcessID();
};
