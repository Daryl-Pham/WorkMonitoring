#pragma once

#include <functional>
#include <Windows.h>
#pragma comment(lib, "Wtsapi32.lib")

class SessionEnumerator
{
public:
    SessionEnumerator() = default;
    virtual ~SessionEnumerator() = default;

    virtual void EnumerateSession(const std::function<void(DWORD)> &handler) const;

private:
    bool IsLoggedOnUserSession(DWORD sessionId) const;
};
