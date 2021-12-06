#include "stdafx.h"
#include "LastActivityLog.h"
#include "Common.h"
#include "Log.h"
#include <boost/chrono/system_clocks.hpp>
#include "Utility.h"
#include <iostream>
#include <sstream>

namespace bfs = boost::filesystem;

namespace
{
    DEFINE_LOGGER_FILE_NAME;
    static const auto DURATION_TO_SEND_LOG = boost::chrono::minutes(30);
}

LastActivityLog::LastActivityLog()
    : LastActivityLog(LastLogTimeFileName)
{
}

LastActivityLog::LastActivityLog(const std::wstring& fileName)
    : LastActivityLog(Utility::GetDefaultDataDirectory().wstring(), fileName)
{
}

LastActivityLog::LastActivityLog(const std::wstring& basePath, const std::wstring& fileName)
    : m_FilePath(bfs::path(basePath) / bfs::path(fileName))
{
}

LastActivityLog::~LastActivityLog()
{
}

bool LastActivityLog::GetLastTime(int64_t* lastTime) const
{
    boost::system::error_code ec;
    if (!bfs::exists(m_FilePath, ec) && !bfs::create_directories(m_FilePath.parent_path(), ec) && ec)
    {
        LOG_WARN("bfs::create_directories(%s) failed. %d(%s)", m_FilePath.parent_path().c_str(), ec.value(), ec.message());
        return false;
    }

    std::ifstream lastTimeStream(m_FilePath.string());
    try
    {
        if (!lastTimeStream.is_open())
        {
            lastTime = 0;
            LOG_WARN("lastTimeFile.is_open() faield.");
            return false;
        }

        std::string line;
        std::getline(lastTimeStream, line);
        std::stringstream sStream(line);
        *lastTime = 0;
        sStream >> *lastTime;

        lastTimeStream.close();
        return true;
    }
    catch (const std::ifstream::failure& e)
    {
        lastTimeStream.close();
        LOG_ERROR("Failed by exception %s", e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        lastTimeStream.close();
        LOG_ERROR("Failed by exception %s", e.what());
        return false;
    }
}

bool LastActivityLog::WriteLastTime(const int64_t lastTime) const
{
    boost::system::error_code ec;
    if (!bfs::exists(m_FilePath, ec) && !bfs::create_directories(m_FilePath.parent_path(), ec) && ec)
    {
        LOG_WARN("bfs::create_directories(%s) failed. %d(%s)", m_FilePath.parent_path().c_str(), ec.value(), ec.message());
        return false;
    }

    std::ofstream file(m_FilePath.string());
    try
    {
        if (!file)
        {
            LOG_WARN("std::ofstream(%s) failed().", m_FilePath.string().c_str());
            return false;
        }
        file << lastTime;

        if (file.fail())
        {
            LOG_WARN("Failed to write last time to file stream.");
            file.close();
            return false;
        }

        file.close();
        return true;
    }
    catch (const std::ofstream::failure& e)
    {
        file.close();
        LOG_ERROR("Failed by exception %s", e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        file.close();
        LOG_ERROR("std::exception occured. %s", e.what());
        return false;
    }
}

bool LastActivityLog::IsSendLog(const boost::chrono::system_clock::time_point tp) const
{
    int64_t lastTime;
    GetLastTime(&lastTime);
    return (tp - boost::chrono::system_clock::from_time_t(lastTime)
        >= DURATION_TO_SEND_LOG);
}
