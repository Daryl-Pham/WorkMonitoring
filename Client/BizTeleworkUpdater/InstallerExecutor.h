#pragma once

#include <string>
#include <boost/filesystem.hpp>
#include <shellapi.h>
#include <Windows.h>

class InstallerExecutor
{
public:
    class Config
    {
    public:
        static Config Default();
        explicit Config(const boost::filesystem::path &logDirectory);
        virtual ~Config() = default;

        const boost::filesystem::path logDirectory;
    };

    explicit InstallerExecutor(const InstallerExecutor::Config &config = InstallerExecutor::Config::Default());
    virtual ~InstallerExecutor() = default;

    virtual bool Execute(const std::wstring& msiFilePath, HANDLE hEventStopControl) const;

    static void Initialize();
    static void Finalize();

protected:
    // Wrapper methods for test of calling Windows API.
    virtual BOOL ShellExecuteExW(_Inout_ SHELLEXECUTEINFOW* pExecInfo) const;
    virtual DWORD WaitForMultipleObjects(
        _In_ DWORD nCount,
        _In_reads_(nCount) CONST HANDLE* lpHandles,
        _In_ BOOL bWaitAll,
        _In_ DWORD dwMilliseconds) const;
    virtual BOOL CloseHandle(_In_ _Post_ptr_invalid_ HANDLE hObject) const;

private:
    const InstallerExecutor::Config m_Config;
};
