#include "stdafx.h"
#include "UserInputLog.h"
#include "Utility.h"
#include "Common.h"
#include "Log.h"
#include <boost/chrono/io/time_point_io.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <cpprest/filestream.h>
#include <iostream>
#include <chrono>
#include <string.h>

namespace bfs = boost::filesystem;

// Definition log type
const std::wstring LogTypeKeyboard = L"keyboard_operation";
const std::wstring LogTypeMouse = L"mouse_operation";

namespace
{

DEFINE_LOGGER_FILE_NAME;
const char TimeFormat[] = "%Y-%m-%dT%H:%M:%SZ";

// Definition time duraiton for writing log
static const auto DURATION_TO_WRITE_LOG = boost::chrono::minutes(5);

} // namespace

UserInputLog::UserInputLog(const std::wstring& fileName, const std::wstring& inputType)
    : UserInputLog(Utility::GetDefaultDataDirectory().wstring(), fileName, inputType)
{
}

UserInputLog::UserInputLog(const std::wstring& basePath, const std::wstring& fileName, const std::wstring& inputType)
    : m_FilePath(bfs::path(basePath) / bfs::path(fileName))
    , m_InputType(inputType)
{
}

UserInputLog::~UserInputLog()
{
}

BOOL UserInputLog::WriteLogOnTimeCondition(const boost::chrono::system_clock::time_point &current)
{
    boost::system::error_code ec;
    if (!bfs::exists(m_FilePath, ec))
    {
        // This condition means that there is not the file.
        // Therefore, create the file and write current time.
        if (!CreateFileAndWriteLog(current))
        {
            LOG_WARN("CreateFileAndWriteLog() failed.");
            return FALSE;
        }

        return TRUE;
    }
    else
    {
        // This condition means that threre is the file.
        // Therefore, read the file and compare the last time and current time.
        // If duration of the last time and current time is 5 or more, write current time.
        if (!WriteLogIfNeccessary(current))
        {
            LOG_WARN("WriteLogIfNeccessary() failed.");
            return FALSE;
        }

        return TRUE;
    }
}


BOOL UserInputLog::CreateFileAndWriteLog(const boost::chrono::system_clock::time_point &current)
{
    boost::system::error_code ec;
    auto parentPath = m_FilePath.parent_path();

    if ((!bfs::create_directories(parentPath, ec)) && ec)
    {
        LOG_WARN("bfs::create_directories(%S) failed. %d(%s)", parentPath.wstring().c_str(), ec.value(), ec.message().c_str());
        return FALSE;
    }

    try
    {
        std::ofstream file(m_FilePath.wstring());
        if (!file)
        {
            LOG_WARN("std::wofstream(%s) failed().", m_FilePath.string().c_str());
            return FALSE;
        }
        file << boost::chrono::time_fmt(boost::chrono::timezone::utc, TimeFormat)
            << current << "," << ::utility::conversions::utf16_to_utf8(m_InputType) << std::endl;
        if (file.fail())
        {
            LOG_WARN("Failed to write string to file stream.");
            file.close();
            return FALSE;
        }

        file.close();
        return TRUE;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("std::exception occured. %s", e.what());
        return FALSE;
    }
}

BOOL UserInputLog::WriteLogIfNeccessary(const boost::chrono::system_clock::time_point &current)
{
    try
    {
        std::fstream file(m_FilePath.string(), std::ios::in | std::ios::out | std::ios::app);
        if (!file)
        {
            char buffer[DCE_C_ERROR_STRING_LEN] = { 0 };
            if (!_strerror_s(&buffer[0], DCE_C_ERROR_STRING_LEN, nullptr))
            {
                LOG_WARN("std::fstream(%s) failed: %s.", m_FilePath.string().c_str(), buffer);
            }
            else
            {
                LOG_WARN("std::fstream(%s) failed().", m_FilePath.string().c_str());
            }
            return FALSE;
        }
        file.seekg(std::ios::beg);
        file.seekp(std::ios::beg);
        std::string theLastLine;
        std::string line;
        while (std::getline(file, line))
        {
            if (!line.empty())
            {
                theLastLine = line;
            }
        }

        file.clear();

        size_t found = theLastLine.find(",");
        if (found == std::string::npos)
        {
            file.close();
            LOG_WARN("theLastLine should include a comma.");
            DeleteWrongLastLine(theLastLine.size());
            return FALSE;
        }
        std::string dateTime = theLastLine.substr(0, found);
        int64_t t = 0;
        if (!Utility::ConvertStringToUnixTime(&t, ::utility::conversions::utf8_to_utf16(dateTime)))
        {
            LOG_WARN("Utility::ConvertStringToUnixTime() failed.");
            return FALSE;
        }

        auto theLastTime = boost::chrono::system_clock::from_time_t(t);

        auto duration = (current - theLastTime);
        if (duration >= DURATION_TO_WRITE_LOG)
        {
            file.seekg(std::ios::end);

            // Write the timestamp.
            file << boost::chrono::time_fmt(boost::chrono::timezone::utc, TimeFormat)
                << current << "," << ::utility::conversions::utf16_to_utf8(m_InputType) << std::endl;
            if (file.fail())
            {
                LOG_WARN("Failed to write string to file stream.");
                file.close();
                return FALSE;
            }

            file.close();
            return TRUE;
        }

        file.close();
        return TRUE;  // Not need to write a log.
    }
    catch (const boost::exception& e)
    {
        LOG_ERROR("Failed by boost::exception %s", boost::diagnostic_information_what(e));
        return FALSE;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("std::exception occured. %s", e.what());
        return FALSE;
    }
}

bool UserInputLog::DeleteWrongLastLine(const size_t size)
{
    // Delete file if file empty
    std::ifstream file(m_FilePath.c_str());
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        file.close();
        try
        {
            return boost::filesystem::remove(m_FilePath);
        }
        catch (const boost::filesystem::filesystem_error& e)
        {
            LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
            return FALSE;
        }
    }
    file.close();

    // Delete line wrong format
    INT64 distance = -1;
    if (size != 0)
    {
        distance = (INT64)size * (-1);
    }
    HANDLE hFile = CreateFile(m_FilePath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW | OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD dwError = GetLastError();
    if (dwError == ERROR_SUCCESS)
    {
        LARGE_INTEGER large{ 0 };
        large.QuadPart = distance;
        if (!SetFilePointerEx(hFile, large, nullptr, FILE_END))
        {
            CloseHandle(hFile);
            LOG_WARN("SetFilePointerEx() failed.");
            return false;
        }
        if (!SetEndOfFile(hFile))
        {
            CloseHandle(hFile);
            LOG_WARN("SetEndOfFile() failed.");
            return false;
        }
        CloseHandle(hFile);
    }
    else
    {
        LOG_WARN("CreateFile() failed. Code error: 0x%08x", dwError);
        return false;
    }

    return true;
}
