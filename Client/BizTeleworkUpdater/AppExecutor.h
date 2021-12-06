#pragma once

#include <boost/filesystem/path.hpp>
#include <memory>
#include <Windows.h>
#include "AppExecutionCondition.h"
#include "SessionEnumerator.h"
#pragma comment(lib, "Wtsapi32.lib")

class AppExecutor
{
public:
    // DI Container class.
    class Container
    {
    public:
        Container() = default;
        virtual ~Container() = default;
        virtual std::unique_ptr<AppExecutionCondition> GetAppExecutionCondition() const;
        virtual std::unique_ptr<SessionEnumerator> GetSessionEnumerator() const;
        virtual boost::filesystem::path GetExePath() const;
    };

    explicit AppExecutor(std::unique_ptr<Container> container = std::make_unique<Container>());
    virtual ~AppExecutor() = default;

    virtual void ExecuteOnEachSession();

protected:
    // Wrapper methods for test of calling Windows API.
    virtual BOOL WTSQueryUserToken(ULONG SessionId, PHANDLE phToken) const;
    virtual BOOL CreateEnvironmentBlock(LPVOID* lpEnvironment, HANDLE hToken, BOOL bInherit) const;
    virtual BOOL CreateProcessAsUserW(
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
            _Out_ LPPROCESS_INFORMATION lpProcessInformation) const;
    virtual BOOL DestroyEnvironmentBlock(LPVOID  lpEnvironment) const;
    virtual BOOL CloseHandle(_In_ _Post_ptr_invalid_ HANDLE hObject) const;

private:
    void ExecuteApp(const boost::filesystem::path& exePath, DWORD sessionId) const;
    const std::unique_ptr<Container> m_Container;
};
