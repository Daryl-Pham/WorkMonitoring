#include "stdafx.h"
#include <fstream>
#include "AppConfig.h"
#include "Common.h"
#include "Environment.h"
#include "Log.h"
#include "Utility.h"
#include <objbase.h>
#include <ShlObj_core.h>
#include <Windows.h>
#include <tchar.h>
#include <boost/exception/diagnostic_information.hpp>
#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"
#include <boost/algorithm/string/replace.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time/local_time.hpp"
#include "boost/date_time/date.hpp"
#include "boost/date_time/c_local_time_adjustor.hpp"
#include "boost/date_time/posix_time/posix_time_io.hpp"


namespace
{

    DEFINE_LOGGER_FILE_NAME;
    const wchar_t LOG_UPDATER_DIRECTORY[] = L"log\\updater";
    const wchar_t LOG_APP_DIRECTORY[] = L"log\\app";
    const wchar_t PROGRAM_DATA_DIRECTORY[] = L"C:\\ProgramData\\OPTiM\\BizTelework";  // For fallback directory.

}  // namespace

bool Utility::ConvertStringToUnixTime(int64_t* dst, const std::wstring& strTime)
{
    if (!dst)
    {
        LOG_WARN("The dst must not be null.");
        return false;
    }

    try
    {
        boost::posix_time::wtime_input_facet *tif = new boost::posix_time::wtime_input_facet;
        tif->set_iso_extended_format();

        std::wistringstream iss(strTime);
        iss.imbue(std::locale(std::locale::classic(), tif));

        boost::posix_time::ptime abs_time;
        iss >> abs_time;

        boost::posix_time::ptime time_t_epoch(boost::gregorian::date(1970, 1, 1));

        *dst = (abs_time - time_t_epoch).total_seconds();

        return true;
    }
    catch (const std::out_of_range& e)
    {
        LOG_ERROR("Failed by std::out_of_range %s", e.what());
        return false;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return false;
    }
    catch (...)
    {
        LOG_ERROR("Failed by an unknown exception");
        return false;
    }
}

std::wstring Utility::ConvertToStringTimeUTC(DWORD seconds)
{

    try
    {
        boost::posix_time::ptime pTime = boost::posix_time::from_time_t(seconds);

        return boost::posix_time::to_iso_extended_wstring(pTime);
    }
    catch (const std::out_of_range& e)
    {
        LOG_ERROR("Failed by std::out_of_range %s", e.what());
        return L"";
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return L"";
    }
    catch (...)
    {
        LOG_ERROR("Failed by an unknown exception");
        return L"";
    }
}

boost::filesystem::path Utility::GetDefaultDataDirectory()
{
    try
    {
        AppConfig appConfig;

        std::wstring dataDir;
        if (!appConfig.GetDataDir(&dataDir))
        {
            LOG_WARN("appConfig.GetDataDir() failed. And returns fallback value \"%S\"", PROGRAM_DATA_DIRECTORY);
            return CreateProgramDataDirectory();
        }

        boost::filesystem::path ret(dataDir);
        boost::filesystem::create_directories(ret);
        return ret;
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return CreateProgramDataDirectory();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return CreateProgramDataDirectory();
    }
    catch (...)
    {
        LOG_ERROR("Failed by an unknown exception");
        return CreateProgramDataDirectory();
    }
}

boost::filesystem::path Utility::GetAppLogDirectory()
{
    try
    {
        auto dir = GetDefaultDataDirectory();
        if (dir.empty())
        {
            LOG_WARN("The dir must not be empty.");
            return boost::filesystem::path();
        }

        dir /= LOG_APP_DIRECTORY;

        return dir;
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return boost::filesystem::path();
    }
    catch (...)
    {
        LOG_ERROR("Failed by an unknown exception");
        return boost::filesystem::path();
    }
}

boost::filesystem::path Utility::GetUpdaterServiceLogDirectory()
{
    try
    {
        auto dir = CreateProgramDataDirectory();
        if (dir.empty())
        {
            LOG_WARN("The dir must not be empty.");
            return boost::filesystem::path();
        }

        dir /= LOG_UPDATER_DIRECTORY;

        return dir;
    }
    catch (const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return boost::filesystem::path();
    }
    catch (...)
    {
        LOG_ERROR("Failed by an unknown exception");
        return boost::filesystem::path();
    }
}

int64_t Utility::ConvertTimePointToUnixTime(const boost::chrono::system_clock::time_point timePoint)
{
    int64_t unixTime = 0;
    boost::chrono::system_clock::duration duration = timePoint.time_since_epoch();
    unixTime = boost::chrono::duration_cast<boost::chrono::seconds>(duration).count();
    return unixTime;
}

int64_t Utility::GetCurrentSecondInUnixTime()
{
    auto now = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
    return now.time_since_epoch().count();
}

boost::filesystem::path Utility::CreateProgramDataDirectory()
{
    boost::system::error_code ec;
    boost::filesystem::path path(PROGRAM_DATA_DIRECTORY);
    if (!boost::filesystem::create_directories(path, ec) && ec.failed())
    {
        LOG_WARN("boost::filesystem::create_directories() failed. %d(%s)", ec.value(), ec.message().c_str());
        return boost::filesystem::path();  // There is no fallback to do.
    }

    return path;
}
