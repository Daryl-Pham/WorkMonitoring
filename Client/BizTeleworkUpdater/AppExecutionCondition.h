#pragma once

#include <AppConfigInterface.h>
#include <boost/filesystem/path.hpp>
#include <set>
#include <Windows.h>

class AppExecutionCondition
{
public:
    AppExecutionCondition() = default;
    virtual ~AppExecutionCondition() = default;

    virtual bool Initialize(const boost::filesystem::path& exePath);
    virtual bool ShouldExecuteAfterInstalledOnce(AppConfigInterface* appConfig) const;
    virtual bool ShouldExecute(DWORD sessionId) const;

private:
    using SessionIdSet = std::set<DWORD>;
    SessionIdSet m_SessionIds;
};
