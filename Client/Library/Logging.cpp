#include "stdafx.h"
#include "Common.h"
#include "Environment.h"
#include "Utility.h"
#include "Logging.h"
#include "RegistryHelper.h"
#include "Log.h"
#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <iostream>

namespace
{

    DEFINE_LOGGER_FILE_NAME;
    const size_t LENGTH_LABEL_EVENT_AT = 9;
    const size_t LENGTH_LABEL_PROCESS_NAME = 13;

}  // namespace

Logging::Logging(const boost::filesystem::path& path, const std::wstring& fileName)
    : m_DataDirectory(path)
    , m_FileName(fileName)
{
    try
    {
        boost::filesystem::create_directories(m_DataDirectory);
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
    }
}

BOOL Logging::WriteLog(LogLines content)
{

    if (CreateDirectory(m_DataDirectory.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
    {
        boost::filesystem::wpath fullPathFile (m_DataDirectory);
        fullPathFile /= (m_FileName);

        HANDLE hFile = ::CreateFileW(fullPathFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_DELETE,
            NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            CloseHandle(hFile);
            return FALSE;
        }
        else
        {
            std::string contentWriteFile;
            for (auto c : content)
            {
                contentWriteFile
                    .append(::utility::conversions::utf16_to_utf8(c.second))
                    .append(::utility::conversions::utf16_to_utf8(c.first))
                    .append("\r\n");
            }
            DWORD byteWritten;
            if (contentWriteFile.size() > DWORD_MAX)
            {
                LOG_WARN("The contentWriteFile() failed.");
                return FALSE;
            }
            if (!::WriteFile(hFile, contentWriteFile.c_str(), static_cast<DWORD>(contentWriteFile.size()), &byteWritten, NULL))
            {
                CloseHandle(hFile);
                return FALSE;
            }
        }

        CloseHandle(hFile);
        return TRUE;
    }

    return FALSE;

}

BOOL Logging::ReadLog(std::istream &logStream, LogLines &logLines)
{
    try
    {
        std::string line;
        while (std::getline(logStream, line))
        {
            size_t numberComma = 0;
            numberComma = std::count(line.begin(), line.end(), ',');
            if (numberComma == 1)
            {
                boost::char_separator<char> separator(",");
                boost::tokenizer<boost::char_separator<char>, std::string::const_iterator, std::string> tokens(line, separator);
                auto cur = tokens.begin();
                std::string eventAt = *cur;  // content
                cur++;
                std::string logType = *cur;  // type

                logLines.push_back(std::make_pair(::utility::conversions::utf8_to_utf16(eventAt), ::utility::conversions::utf8_to_utf16(logType)));
            }
        }

        return TRUE;
    }
    catch (const boost::exception& e)
    {
        LOG_ERROR("Failed by boost::exception %s", boost::diagnostic_information_what(e));
        return FALSE;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return FALSE;
    }
}

BOOL Logging::ReadLog(LogLines &logLines)
{
    boost::filesystem::wpath fullPathLogFile = boost::filesystem::wpath(m_DataDirectory).append(m_FileName);

    try
    {
        std::ifstream logFile(fullPathLogFile.wstring());
        if (!logFile.is_open())
        {
            LOG_WARN("logFile.is_open() faield.");
            return FALSE;
        }

        if (!ReadLog(logFile, logLines)) {
            LOG_ERROR("ReadLog() faield.");
            logFile.close();
            return FALSE;
        }
        logFile.close();
        return TRUE;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by exception %s", e.what());
        return FALSE;
    }
}

BOOL Logging::ReadLog(LogLines* logLines, BOOL* existFile)
{
    boost::filesystem::wpath fullPathLogFile = boost::filesystem::wpath(m_DataDirectory).append(m_FileName);

    boost::system::error_code ec;
    if (!boost::filesystem::exists(fullPathLogFile, ec))
    {
        *existFile = FALSE;
        return TRUE;
    }

    try
    {
        *existFile = TRUE;
        std::ifstream logFile(fullPathLogFile.wstring());
        if (!logFile.is_open())
        {
            LOG_ERROR("logFile.is_open() faield.");
            return FALSE;
        }

        if (!ReadLog(logFile, *logLines)) {
            LOG_ERROR("ReadLog() faield.");
            logFile.close();
            return FALSE;
        }
        logFile.close();
        return TRUE;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by exception %s", e.what());
        return FALSE;
    }
}

BOOL Logging::DeviceLogStringifyAsJsonBody(const LogLines& logLines, std::wstring* jsonStrigify)
{
    /*
    content value is a JSON object like the below.
    {
        "logs": [
            {
                "event_at": "2020-04-01T00:00:00Z",
                "log_type" : "logon"
            },
            ...
        ]
    }
    */
    web::json::value content;

    int i = 0;
    for (auto line : logLines)
    {
        web::json::value item;
        item[L"event_at"] = web::json::value::string(line.first);
        item[L"log_type"] = web::json::value::string(line.second);
        content[L"logs"][i] = item;
        i++;
    }

    if (content.is_null())
    {
        LOG_DEBUG("The content is null.");
        return TRUE;
    }

    *jsonStrigify = content.serialize().c_str();

    return TRUE;
}

BOOL Logging::DeleteLog()
{
    boost::filesystem::wpath fullPathLogFile = boost::filesystem::wpath(m_DataDirectory).append(m_FileName);

    if (!boost::filesystem::exists(fullPathLogFile))
    {
        return TRUE;
    }

    try
    {
        return boost::filesystem::remove(fullPathLogFile);
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        LOG_ERROR("Failed by boost::filesystem::filesystem_error %s", e.what());
        return FALSE;
    }
}

std::wstring Logging::GetFilePath() const
{
    return boost::filesystem::wpath(m_DataDirectory).append(m_FileName).wstring();
}

BOOL Logging::ReadLogActiveApp(std::istream& logStream, LogLines* jsonStrigify) const
{
    try
    {
        std::string line;
        while (std::getline(logStream, line))
        {
            boost::char_separator<char> separator("\t");
            boost::tokenizer<boost::char_separator<char>, std::string::const_iterator, std::string> tokens(line, separator);
            auto cur = tokens.begin();
            std::string eventAt = *cur;
            eventAt.erase(0, LENGTH_LABEL_EVENT_AT);
            cur++;
            std::string processName = *cur;
            processName.erase(0, LENGTH_LABEL_PROCESS_NAME);

            jsonStrigify->push_back(std::make_pair(::utility::conversions::utf8_to_utf16(eventAt), ::utility::conversions::utf8_to_utf16(processName)));
        }

        return TRUE;
    }
    catch (const boost::exception& e)
    {
        LOG_ERROR("Failed by boost::exception %s", boost::diagnostic_information_what(e));
        return FALSE;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by std::exception %s", e.what());
        return FALSE;
    }
}

BOOL Logging::ReadLogActiveApp(LogLines* jsonStrigify)
{
    boost::filesystem::wpath fullPathLogFile = boost::filesystem::wpath(m_DataDirectory).append(m_FileName);

    boost::system::error_code ec;
    if (!boost::filesystem::exists(fullPathLogFile, ec))
    {
        return FALSE;
    }

    try
    {
        std::ifstream logFile(fullPathLogFile.wstring());
        if (!logFile.is_open())
        {
            LOG_ERROR("logFile.is_open() failed.");
            return FALSE;
        }

        if (!ReadLogActiveApp(logFile, jsonStrigify)) {
            LOG_ERROR("ReadLogActiveApp() failed.");
            logFile.close();
            return FALSE;
        }
        logFile.close();
        return TRUE;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed by exception %s", e.what());
        return FALSE;
    }
}

BOOL Logging::ActiveAppLogStringifyAsJsonBody(const LogLines& logLines, std::wstring* jsonStrigify)
{
    /*
    content value is a JSON object like the below.
    {
        "logs": [
            {
                "process_name": "chrome.exe",
                "event_at" : "2020-04-01T00:00:00Z"
            },
            ...
        ]
    }
    */
    web::json::value content;

    int i = 0;
    for (auto line : logLines)
    {
        web::json::value item;
        item[L"process_name"] = web::json::value::string(line.second);
        item[L"event_at"] = web::json::value::string(line.first);
        content[L"logs"][i] = item;
        i++;
    }

    if (content.is_null())
    {
        LOG_DEBUG("The content is null.");
        return TRUE;
    }

    *jsonStrigify = content.serialize().c_str();

    return TRUE;
}
