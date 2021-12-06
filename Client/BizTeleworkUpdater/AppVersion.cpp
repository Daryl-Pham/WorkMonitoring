#include "pch.h"
#include <regex>
#include <Log.h>
#include "AppVersion.h"

namespace
{
    DEFINE_LOGGER_FILE_NAME;
}  // namespace

AppVersion::AppVersion(unsigned int major_, unsigned int minor_, unsigned int patch_, unsigned int build_)
    : major(major_)
    , minor(minor_)
    , patch(patch_)
    , build(build_)
{
}

std::unique_ptr<AppVersion> AppVersion::Parse(const std::wstring& versionString)
{
    try
    {
        std::wregex versionPattern(L"(\\d+).(\\d+).(\\d+).(\\d+)");
        std::wsmatch m;

        if (!std::regex_match(versionString, m, versionPattern))
        {
            return nullptr;
        }
        else if (m.empty())
        {
            return nullptr;
        }
        else if (m.size() != 5)
        {
            return nullptr;
        }
        else
        {
            return std::unique_ptr<AppVersion>(new AppVersion(
                _wtoi(m.str(1).c_str()),
                _wtoi(m.str(2).c_str()),
                _wtoi(m.str(3).c_str()),
                _wtoi(m.str(4).c_str())));
        }
    }
    catch (const std::regex_error& e)
    {
        LOG_ERROR("std::regex_error has occurred. %s", e.what());
        return nullptr;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("std::exception has occurred. %s", e.what());
        return nullptr;
    }
}

bool AppVersion::IsNewerThan(const AppVersion& other) const
{
    if (major != other.major)
    {
        return (major > other.major);
    }
    else if (minor != other.minor)
    {
        return (minor > other.minor);
    }
    else if (patch != other.patch)
    {
        return (patch > other.patch);
    }
    else if (build != other.build)
    {
        return (build > other.build);
    }

    return false;
}

bool AppVersion::IsEqual(const AppVersion& other) const
{
    return (major == other.major) &&
           (minor == other.minor) &&
           (patch == other.patch) &&
           (build == other.build);
}
