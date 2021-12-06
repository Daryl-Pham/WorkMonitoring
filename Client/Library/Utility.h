#pragma once

#include <string>
#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/chrono/system_clocks.hpp"

class Utility
{
public:
    static bool                     ConvertStringToUnixTime(int64_t *dst, const std::wstring &strTime);
    static std::wstring             ConvertToStringTimeUTC(DWORD unixSeconds);
    static int64_t                  ConvertTimePointToUnixTime(const boost::chrono::system_clock::time_point timePoint);
    static boost::filesystem::path  GetDefaultDataDirectory();
    static boost::filesystem::path  GetAppLogDirectory();
    static boost::filesystem::path  GetUpdaterServiceLogDirectory();
    static int64_t                  GetCurrentSecondInUnixTime();
    static boost::filesystem::path  CreateProgramDataDirectory();
};