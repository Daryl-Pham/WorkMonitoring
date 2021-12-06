#include "pch.h"
#include "SessionEnumerator.h"
#include "Log.h"
#include <WtsApi32.h>


namespace
{
    DEFINE_LOGGER_FILE_NAME;
}  // namespace

// Reference https://gitlab.tokyo.optim.co.jp/bizwindows/optimal_biz_windows_agent/-/blob/master/BizLib/src/WtsApi32.cpp#L143
bool SessionEnumerator::IsLoggedOnUserSession(DWORD sessionId) const
{
    LPWSTR p = nullptr;
    DWORD bytes = 0;
    if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, sessionId, WTSUserName, &p, &bytes))
    {
        LOG_WARN("WTSQuerySessionInformation failed. %u", GetLastError());
        return false;
    }

    std::wstring userName(p, bytes);
    if (p) WTSFreeMemory(p);

    LOG_INFO("userName: %S, size: %lu, bytes: %lu", userName.c_str(), userName.size(), bytes);
    if (bytes == 2)
    {
        LOG_INFO("userName is empty.");
        return false;
    }

    return !userName.empty();
}

void SessionEnumerator::EnumerateSession(const std::function<void(DWORD)>& handler) const
{
    if (!handler)
    {
        LOG_WARN("The handler must not be null.");
        return;
    }

    PWTS_SESSION_INFO sessionInfo = NULL;
    DWORD count = 0;
    if (!WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &sessionInfo, &count))
    {
        LOG_WARN("WTSEnumerateSessions failed. %u", GetLastError());
        return;
    }

    LOG_INFO("sessionInfo: %p, count: %lu", sessionInfo, count);

    for (DWORD i = 0; i < count; i++)
    {
        auto si = sessionInfo[i];
        LOG_INFO("sessionInfo[%u] = {%u, %d, \"%S\"}", i, si.SessionId, si.State, si.pWinStationName);

        if (IsLoggedOnUserSession(si.SessionId))
        {
            LOG_INFO("The SessionId is logged-on user session.");

            handler(si.SessionId);
        }
    }

    if (sessionInfo)
    {
        WTSFreeMemory(sessionInfo);
    }
}
