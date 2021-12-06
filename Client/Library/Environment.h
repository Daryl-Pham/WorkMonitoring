#pragma once
#include <string>
#include <mutex>
#include <boost/noncopyable.hpp>

enum class ReleaseEnvironment
{
    DEVELOPMENT,
    TESTING,
    STAGING,
    PRODUCTION,
};

class Environment : public boost::noncopyable
{
public:
    static Environment* GetInstance();

    BOOL         IsWindowsOS64();

private:
    static Environment* m_Instance;
    static std::once_flag m_InitInstanceFlag;
    BOOL m_Is64;

    static void InitSingleton();

    BOOL IsWow64Process();
};