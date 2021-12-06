#pragma once
#include "boost/chrono/system_clocks.hpp"
#include "boost/filesystem/path.hpp"

class LastActivityLog
{
public:
    LastActivityLog();
    explicit LastActivityLog(const std::wstring& fileName);
    LastActivityLog(const std::wstring& basePath, const std::wstring& fileName);
    ~LastActivityLog();

    bool GetLastTime(int64_t* lastTime) const;
    bool WriteLastTime(const int64_t lastTime) const;
    bool IsSendLog(const boost::chrono::system_clock::time_point tp) const;

private:
    const boost::filesystem::path m_FilePath;
};
