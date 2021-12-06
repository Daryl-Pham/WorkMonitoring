#include "stdafx.h"
#include "Environment.h"
#include "Utility.h"
#include "RegistryHelper.h"
#include "Log.h"

namespace
{
    DEFINE_LOGGER_FILE_NAME;
}  // namespace

Environment* Environment::m_Instance = NULL;
std::once_flag Environment::m_InitInstanceFlag;

void Environment::InitSingleton()
{
    m_Instance = new Environment;
    m_Instance->m_Is64 = m_Instance->IsWow64Process();
}

Environment* Environment::GetInstance()
{
    std::call_once(m_InitInstanceFlag, &Environment::InitSingleton);

    return m_Instance;
}

BOOL Environment::IsWow64Process()
{
#ifdef _WIN64
    return TRUE;
#elif defined _WIN32
    BOOL bIsWow64 = FALSE;

    typedef BOOL(WINAPI *FnISWow64Process) (HANDLE, PBOOL);
    FnISWow64Process fnIsWow64Process;

    fnIsWow64Process = (FnISWow64Process)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
    if (!fnIsWow64Process)
    {
        LOG_WARN("GetProcAddress() failed.");
        return FALSE;
    }
    else if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
    {
        LOG_WARN("GetProcAddress() failed. %lu", GetLastError());
        return FALSE;
    }

    return bIsWow64;
#else
    assert(0);
    return FALSE;
#endif
}

/*
* If target build is x64, then have exist macro _WIN64 and OS is Windows 64 bits.
* Else, if target buld is x86 and IsWow64Process function return TRUE, then OS is Windows 64 bits. Else OS is Windows 64 bits.
*/
BOOL Environment::IsWindowsOS64()
{
    return m_Is64;
}
